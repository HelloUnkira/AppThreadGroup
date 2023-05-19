/*实现目标:
 *    设计一个精简协议数据传输接口
 *    它向下适配驱动接口并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"

/*@brief  传输分包包大小
 *        保证底层至少能刚好接收该包并打包发送
 *@retval 传输分包包大小最大值
 */
static inline uint32_t app_module_transfer_max_size(void)
{
    return 20;
}

/*@brief     接收协议数据并拆包发送
 *@param[in] data 编码数据流
 *@param[in] size 编码数据流大小
 */
bool app_module_transfer_notify(uint8_t *data, uint32_t size)
{
    app_module_transfer_t *transfer = NULL;
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
    uint32_t length = app_module_transfer_max_size();
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
        transfer->head = offset == 0 ? true : false;
        offset += transfer->size;
        transfer->tail = offset == size ? true : false;
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

/*@brief     接收协议数据并组包解析
 *@param[in] data 编码数据流
 *@param[in] size 编码数据流大小
 */
void app_module_transfer_respond(app_module_transfer_t *transfer)
{
    /* 本地缓冲区 */
    static uint32_t offset = 0;
    static uint32_t length = APP_MODULE_TRANSFER_BLOCK;
    static uint8_t *stream = NULL;
    /* 接收到第一个缓冲块 */
    if (transfer->head) {
        /* 现在检查旧的传输队列 */
        if (stream != NULL) {
            APP_SYS_LOG_ERROR("catch leak package:%u", offset);
            #if APP_SYS_LOG_PROTOCOL_CHECK
            for (uint32_t idx = 0; idx < offset; idx++)
                APP_SYS_LOG_ERROR_RAW("%02x ", stream[idx]);
            APP_SYS_LOG_ERROR_RAW(APP_SYS_LOG_LINE);
            #endif
            app_mem_free(stream);
        }
        /* 开始了新一次的传输 */
        offset = 0;
        stream = app_mem_alloc(length);
        memset(stream, 0, length);
    }
    /* 防御:出现意外的响应时 */
    if (stream == NULL) {
        APP_SYS_LOG_ERROR("catch unknown package");
        #if APP_SYS_LOG_PROTOCOL_CHECK
        APP_SYS_LOG_ERROR("package head:%u", transfer->head);
        APP_SYS_LOG_ERROR("package tail:%u", transfer->tail);
        for (uint32_t idx = 0; idx < transfer->size; idx++)
            APP_SYS_LOG_ERROR_RAW("%02x ", transfer->data[idx]);
        APP_SYS_LOG_ERROR_RAW(APP_SYS_LOG_LINE);
        #endif
        app_mem_free(stream);
        offset = 0;
        stream = NULL;
        return;
    }
    /* 防御:传输数据包超限 */
    if (offset + transfer->size > length) {
        APP_SYS_LOG_ERROR("catch overflow package:%u", offset);
        #if APP_SYS_LOG_PROTOCOL_CHECK
        for (uint32_t idx = 0; idx < offset; idx++)
            APP_SYS_LOG_ERROR_RAW("%02x ", stream[idx]);
        APP_SYS_LOG_ERROR_RAW(APP_SYS_LOG_LINE);
        #endif
        return;
    }
    /* 将数据包进行流式存储 */
    memcpy(stream + offset, transfer->data, transfer->size);
    offset += transfer->size;
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("catch current package:%u", transfer->size);
    APP_SYS_LOG_INFO("package head:%u", transfer->head);
    APP_SYS_LOG_INFO("package tail:%u", transfer->tail);
    for (uint32_t idx = 0; idx < transfer->size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", transfer->data[idx]);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 接收到最后一个缓冲块 */
    if (transfer->tail) {
        /* 现在打包新的传输队列 */
        #if APP_SYS_LOG_PROTOCOL_CHECK
        APP_SYS_LOG_INFO("catch fully package:%u", offset);
        for (uint32_t idx = 0; idx < offset; idx++)
            APP_SYS_LOG_INFO_RAW("%02x ", stream[idx]);
        APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
        #endif
        /* 传输到目标 */
        app_module_protocol_t protocol = {
            .respond.data    = stream,
            .respond.size    = offset,
            .respond.dynamic = true,
        };
        app_module_protocol_respond(&protocol);
        offset = 0;
        stream = NULL;
    }
}
