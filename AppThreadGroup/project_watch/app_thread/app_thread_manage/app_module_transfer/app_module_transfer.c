/*实现目标:
 *    设计一个精简协议数据传输接口
 *    它向下适配驱动接口并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 注意:因为写权限高于读权限,且读写分离,该变量无需额外保护 */
static bool app_module_transfer_respond_timeout_status[app_module_transfer_chan_num] = {false};
static app_sys_timer_t app_module_transfer_respond_timeout_timer[app_module_transfer_chan_num] = {0};

/*@brief 传输分包接收定时器回调
 */
static void app_module_transfer_respond_timeout_timer_handler(void *timer)
{
    uintptr_t channel = (uintptr_t)((app_sys_timer_t *)timer)->user_data;
    APP_SYS_LOG_ERROR("respond channel %u timeout", channel);
    /* 设置超时标记 */
    app_module_transfer_respond_timeout_status[channel] = true;
    /* 使用一个空的结束包促使接收流程结束 */
    app_module_transfer_t transfer_none = {.chan = channel, .tail = true};
    app_module_transfer_respond(&transfer_none);
}

/*@brief 传输分包包大小
 *       保证底层至少能刚好接收该包并打包发送
 *@retval 传输分包包大小最大值
 */
static inline uint32_t app_module_transfer_max_size(app_module_transfer_chan_t channel)
{
    if (channel == app_module_transfer_chan_low)
        return 20;
    if (channel == app_module_transfer_chan_high)
        return 255;
}

/*@brief 接收协议数据并拆包发送
 *@param chan 传输信道
 *@param data 编码数据流
 *@param size 编码数据流大小
 */
bool app_module_transfer_notify(app_module_transfer_chan_t channel, uint8_t *data, uint32_t size)
{
    app_module_transfer_t *transfer = NULL;
    /* 容错检查 */
    if (channel >= app_module_transfer_chan_num) {
        APP_SYS_LOG_WARN("transfer channel is invalid :%u", channel);
        return false;
    }
    /* 容错检查 */
    if (data == NULL || size == 0) {
        APP_SYS_LOG_WARN("transfer stream is empty");
        return false;
    }
    /* 数据块过大断言 */
    if (size >= APP_MODULE_TRANSFER_BLOCK) {
        APP_SYS_LOG_ERROR("transfer stream is too long");
        return false;
    }
    /* 构建本地缓冲区 */
    uint32_t offset = 0;
    uint32_t length = app_module_transfer_max_size(channel);
    uint8_t *stream = app_mem_alloc(length);
    transfer = (void *)stream;
    /* 迭代拆包并投掷出去 */
    while (offset < size) {
        /* 清除缓冲块 */
        memset(stream, 0, length);
        /* 当前块可携带载荷 */
        if ((transfer->size = size - offset) > length - sizeof(app_module_transfer_t))
             transfer->size = length - sizeof(app_module_transfer_t);
        /* 数据转移到传输包 */
        memcpy(transfer->data, data + offset, transfer->size);
        /* 迭代数据流 */
        transfer->chan = channel;
        transfer->head = offset == 0 ? true : false;
        offset += transfer->size;
        transfer->tail = offset == size ? true : false;
        /* 检查数据流 */
        APP_SYS_LOG_INFO("send package:%u", transfer->size);
        APP_SYS_LOG_INFO("package chan:%u", transfer->chan);
        APP_SYS_LOG_INFO("package head:%u", transfer->head);
        APP_SYS_LOG_INFO("package tail:%u", transfer->tail);
        for (uint32_t idx = 0; idx < transfer->size; idx++)
            APP_SYS_LOG_INFO_RAW("%02x ", transfer->data[idx]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
        /* 传输缓冲块 */
        // app_module_transfer_throw(transfer);
        /* 本地回环 */
        #if APP_MODULE_PROTOCOL_LOCAL_LOOPBACK
        app_module_transfer_respond(transfer);
        #endif
    }
    /* 回收本地缓冲区 */
    app_mem_free(stream);
    return true;
}

/*@brief 接收协议数据并组包解析
 *@param data 编码数据流
 *@param size 编码数据流大小
 */
void app_module_transfer_respond(app_module_transfer_t *transfer)
{
    /* 本地缓冲区 */
    static struct {
        uint32_t offset;
        uint32_t length;
        uint8_t *stream;
    } zone[app_module_transfer_chan_num] = {
          [app_module_transfer_chan_low] = {
              .offset = 0,
              .length = APP_MODULE_TRANSFER_BLOCK,
              .stream = NULL,
          },
          [app_module_transfer_chan_high] = {
              .offset = 0,
              .length = APP_MODULE_TRANSFER_BLOCK,
              .stream = NULL,
          },
    };
    /* 容错检查 */
    if (transfer->chan >= app_module_transfer_chan_num) {
        APP_SYS_LOG_WARN("transfer channel is invalid :%u", transfer->chan);
        return;
    }
    /* 接收到第一个缓冲块 */
    if (transfer->head) {
        /* 现在检查旧的传输队列 */
        if (zone[transfer->chan].stream != NULL) {
            APP_SYS_LOG_ERROR("catch leak package:%u", zone[transfer->chan].offset);
            for (uint32_t idx = 0; idx < zone[transfer->chan].offset; idx++)
                APP_SYS_LOG_ERROR_RAW("%02x ", zone[transfer->chan].stream[idx]);
                APP_SYS_LOG_ERROR_RAW(app_sys_log_line());
            app_mem_free(zone[transfer->chan].stream);
        }
        /* 开始了新一次的传输 */
        zone[transfer->chan].offset = 0;
        zone[transfer->chan].stream = app_mem_alloc(zone[transfer->chan].length);
        memset(zone[transfer->chan].stream, 0, zone[transfer->chan].length);
        /* 就绪并启动超时定时器监控 */
        app_module_transfer_respond_timeout_timer[transfer->chan].expired   = app_module_transfer_respond_timeout_timer_handler;
        app_module_transfer_respond_timeout_timer[transfer->chan].peroid    = APP_MODULE_TRANSFER_RESPOND_TIMEOUT;
        app_module_transfer_respond_timeout_timer[transfer->chan].user_data = (void *)(uintptr_t)transfer->chan;
        app_sys_timer_start(&app_module_transfer_respond_timeout_timer[transfer->chan]);
    }
    /* 防御:出现意外的响应时 */
    if (zone[transfer->chan].stream == NULL) {
        APP_SYS_LOG_ERROR("catch unknown package");
        APP_SYS_LOG_ERROR("package chan:%u", transfer->chan);
        APP_SYS_LOG_ERROR("package head:%u", transfer->head);
        APP_SYS_LOG_ERROR("package tail:%u", transfer->tail);
        for (uint32_t idx = 0; idx < transfer->size; idx++)
            APP_SYS_LOG_ERROR_RAW("%02x ", transfer->data[idx]);
            APP_SYS_LOG_ERROR_RAW(app_sys_log_line());
        app_mem_free(zone[transfer->chan].stream);
        zone[transfer->chan].offset = 0;
        zone[transfer->chan].stream = NULL;
        return;
    }
    /* 防御:传输数据包超限 */
    if (zone[transfer->chan].offset + transfer->size > zone[transfer->chan].length) {
        APP_SYS_LOG_ERROR("catch overflow package:%u", zone[transfer->chan].offset);
        for (uint32_t idx = 0; idx < zone[transfer->chan].offset; idx++)
            APP_SYS_LOG_ERROR_RAW("%02x ", zone[transfer->chan].stream[idx]);
            APP_SYS_LOG_ERROR_RAW(app_sys_log_line());
        return;
    }
    /* 将数据包进行流式存储 */
    memcpy(zone[transfer->chan].stream + zone[transfer->chan].offset, transfer->data, transfer->size);
    zone[transfer->chan].offset += transfer->size;
    /* 检查数据流 */
    APP_SYS_LOG_INFO("recv package:%u", transfer->size);
    APP_SYS_LOG_INFO("package chan:%u", transfer->chan);
    APP_SYS_LOG_INFO("package head:%u", transfer->head);
    APP_SYS_LOG_INFO("package tail:%u", transfer->tail);
    for (uint32_t idx = 0; idx < transfer->size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", transfer->data[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    /* 更新超时监控 */
    app_sys_timer_stop(&app_module_transfer_respond_timeout_timer[transfer->chan]);
    app_sys_timer_start(&app_module_transfer_respond_timeout_timer[transfer->chan]);
    /* 接收到最后一个缓冲块 */
    if (transfer->tail) {
        /* 触发超时结束 */
        if (app_module_transfer_respond_timeout_status[transfer->chan]) {
            app_module_transfer_respond_timeout_status[transfer->chan] = false;
            app_mem_free(zone[transfer->chan].stream);
        } else {
            /* 现在打包新的传输队列 */
            APP_SYS_LOG_INFO("catch fully package:%u", zone[transfer->chan].offset);
            for (uint32_t idx = 0; idx < zone[transfer->chan].offset; idx++)
                APP_SYS_LOG_INFO_RAW("%02x ", zone[transfer->chan].stream[idx]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            /* 传输到目标 */
            app_module_protocol_t protocol = {
                .respond.data    = zone[transfer->chan].stream,
                .respond.size    = zone[transfer->chan].offset,
                .respond.dynamic = true,
            };
            app_module_protocol_respond(&protocol);
        }
        zone[transfer->chan].offset = 0;
        zone[transfer->chan].stream = NULL;
        app_sys_timer_stop(&app_module_transfer_respond_timeout_timer[transfer->chan]);
    }
}
