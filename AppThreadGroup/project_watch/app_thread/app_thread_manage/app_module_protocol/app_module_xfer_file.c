/*实现目标:
 *    通用文件传输引擎(与序列化协议解耦)
 *    发送:描述符->顺序分包(等ack推进)->结束(等完成ack);分包失败经xfer断点(offset)续传
 *    接收:描述符/分包/结束校验,成功回ack,失败发xfer断点告知续传位置
 *    打包/解析由协议层经ops注入,本模组只做状态机/校验/应答决策
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 对端反馈类型 */
enum {
    APP_XFER_FB_NONE  = 0,
    APP_XFER_FB_ACK_OK,     /* 对端ack成功 */
    APP_XFER_FB_ACK_FAIL,   /* 对端ack失败(重发当前包) */
    APP_XFER_FB_BREAK,      /* 对端xfer断点 */
    APP_XFER_FB_STATE,      /* 对端状态同步(期望分包) */
};

/* 文件发送/接收引擎实例 */
static app_module_xfer_file_send_t app_module_xfer_send = {0};
static app_module_xfer_file_recv_t app_module_xfer_recv = {0};

static app_module_xfer_file_ops_t    app_module_xfer_ops  = {0}; /* 序列化协议原语 */
static void (*app_module_xfer_done)(bool) = NULL;               /* 发送完成回调 */

/*@brief 文件分包总数 */
static uint32_t app_module_xfer_package_count(void)
{
    return (app_module_xfer_send.file_size + app_module_xfer_ops.chunk - 1) / app_module_xfer_ops.chunk;
}

/*@brief 发送文件描述符 */
static void app_module_xfer_send_descriptor(const char *name)
{
    app_module_xfer_ops.send_des(app_module_xfer_send.ftype, name, app_module_xfer_send.file_size,
                                 app_module_xfer_send.file_crc8, app_module_xfer_send.file_cks8);
    APP_SYS_LOG_INFO("file send des type:%u size:%u crc8:%02x cks8:%02x",
                     app_module_xfer_send.ftype, app_module_xfer_send.file_size,
                     app_module_xfer_send.file_crc8, app_module_xfer_send.file_cks8);
}

/*@brief 发送文件分包 */
static void app_module_xfer_send_package(uint32_t index)
{
    uint16_t chunk = app_module_xfer_ops.chunk;
    uint32_t base  = index * chunk;
    if (base >= app_module_xfer_send.file_size)
        return;
    uint32_t amount = app_module_xfer_send.file_size - base;
    if (amount > chunk)
        amount = chunk;
    app_module_xfer_ops.send_pkg(index, base, app_module_xfer_send.image + base, amount);
    APP_SYS_LOG_INFO("file send pkg index:%u base:%u size:%u", index, base, amount);
}

/*@brief 发送文件传输结束(xfer END) */
static void app_module_xfer_send_end(void)
{
    app_module_xfer_ops.send_xfer(APP_MODULE_XFER_FILE_STAGE_END, APP_MODULE_XFER_FILE_RESULT_OK, 0);
    APP_SYS_LOG_INFO("file send end size:%u", app_module_xfer_send.file_size);
}

/*@brief 按当前阶段重发(ack失败/异常时)
 */
static void app_module_xfer_send_resend(void)
{
    switch (app_module_xfer_send.phase) {
    case app_module_xfer_file_phase_start:
        app_module_xfer_send_descriptor(app_module_xfer_send.fname);
        break;
    case app_module_xfer_file_phase_pkg:
        app_module_xfer_send_package(app_module_xfer_send.next_pkg);
        break;
    case app_module_xfer_file_phase_end:
        app_module_xfer_send_end();
        break;
    default:
        break;
    }
}

/*@brief ack轮询定时器回调 */
static void app_module_xfer_timer_handler(void *timer)
{
    if (!app_module_xfer_send.active)
        return;
    app_module_xfer_ops.post_step();
}

/*@brief 结束发送流程并通报完成 */
static void app_module_xfer_send_finish(bool ok)
{
    app_module_xfer_send.phase  = app_module_xfer_file_phase_done;
    app_module_xfer_send.active = false;
    app_sys_timer_stop(&app_module_xfer_send.timer);
    if (app_module_xfer_done)
        app_module_xfer_done(ok);
}

/*@brief 注入文件传输引擎原语 */
void app_module_xfer_file_config(const app_module_xfer_file_ops_t *ops)
{
    app_module_xfer_ops = *ops;
    if (app_module_xfer_ops.chunk == 0)
        app_module_xfer_ops.chunk = APP_MODULE_XFER_FILE_CHUNK_SIZE;
}

/*@brief 设置发送完成回调 */
void app_module_xfer_file_set_done(void (*done)(bool))
{
    app_module_xfer_done = done;
}

/*@brief 启动文件发送 */
bool app_module_xfer_file_notify(const char *name, app_module_xfer_file_type_t type)
{
    if (app_module_xfer_send.active)
        return false;
    app_module_xfer_send.file_size  = app_module_xfer_file_build(app_module_xfer_send.image, sizeof(app_module_xfer_send.image));
    app_module_xfer_send.file_crc8  = app_sys_crc8(app_module_xfer_send.image, app_module_xfer_send.file_size);
    app_module_xfer_send.file_cks8  = app_sys_checksum8(app_module_xfer_send.image, app_module_xfer_send.file_size);
    app_module_xfer_send.phase       = app_module_xfer_file_phase_start;
    app_module_xfer_send.ftype       = (uint8_t)type;
    app_module_xfer_send.fname       = name;
    app_module_xfer_send.next_pkg    = 0;
    app_module_xfer_send.retry       = 0;
    app_module_xfer_send.fb          = APP_XFER_FB_NONE;
    app_module_xfer_send.poll_count  = 0;
    app_module_xfer_send.active      = true;
    app_module_xfer_send_descriptor(name);
    app_module_xfer_send.timer.expired = app_module_xfer_timer_handler;
    app_module_xfer_send.timer.peroid  = APP_MODULE_XFER_FILE_PERIOD_SEND;
    app_module_xfer_send.timer.reload  = 1;
    app_sys_timer_start(&app_module_xfer_send.timer);
    return true;
}

/*@brief 发送轮询步进:消费对端反馈或超时退出 */
void app_module_xfer_file_step(void)
{
    if (!app_module_xfer_send.active) {
        app_sys_timer_stop(&app_module_xfer_send.timer);
        return;
    }
    /* 无反馈则计数,超时退出 */
    if (app_module_xfer_send.fb == APP_XFER_FB_NONE) {
        if (++app_module_xfer_send.poll_count >= APP_MODULE_XFER_FILE_LIMIT_POLL) {
            APP_SYS_LOG_ERROR("file ack timeout phase:%u", app_module_xfer_send.phase);
            app_module_xfer_send_finish(false);
        }
        return;
    }
    uint32_t fb     = app_module_xfer_send.fb;
    uint16_t offset = app_module_xfer_send.fb_offset;
    app_module_xfer_send.fb         = APP_XFER_FB_NONE;
    app_module_xfer_send.poll_count = 0;
    switch (fb) {
    case APP_XFER_FB_ACK_OK:
        /* 描述符确认后进分包; 分包确认后推下一包; 结束确认则成功 */
        switch (app_module_xfer_send.phase) {
        case app_module_xfer_file_phase_start:
            app_module_xfer_send.phase = app_module_xfer_file_phase_pkg;
            app_module_xfer_send_package(0);
            break;
        case app_module_xfer_file_phase_pkg:
            app_module_xfer_send.next_pkg++;
            if (app_module_xfer_send.next_pkg >= app_module_xfer_package_count()) {
                app_module_xfer_send.phase = app_module_xfer_file_phase_end;
                app_module_xfer_send_end();
            } else {
                app_module_xfer_send_package(app_module_xfer_send.next_pkg);
            }
            break;
        case app_module_xfer_file_phase_end:
            app_module_xfer_send_finish(true);
            break;
        default:
            break;
        }
        break;
    case APP_XFER_FB_ACK_FAIL:
        /* 对端确认失败:重发当前包,限次后退出 */
        if (app_module_xfer_send.retry++ < APP_MODULE_XFER_FILE_LIMIT_POLL)
            app_module_xfer_send_resend();
        else
            app_module_xfer_send_finish(false);
        break;
    case APP_XFER_FB_BREAK:
        /* 从断点续传(过时路径,保留兼容) */
        app_module_xfer_send.phase    = app_module_xfer_file_phase_pkg;
        app_module_xfer_send.next_pkg = offset / app_module_xfer_ops.chunk;
        app_module_xfer_send.retry    = 0;
        if (app_module_xfer_send.next_pkg < app_module_xfer_package_count())
            app_module_xfer_send_package(app_module_xfer_send.next_pkg);
        break;
    case APP_XFER_FB_STATE:
        /* 对端状态同步:按对端期望续传位置续发 */
        app_module_xfer_send.phase    = app_module_xfer_file_phase_pkg;
        app_module_xfer_send.next_pkg = offset / app_module_xfer_ops.chunk;
        app_module_xfer_send.retry    = 0;
        if (app_module_xfer_send.next_pkg < app_module_xfer_package_count())
            app_module_xfer_send_package(app_module_xfer_send.next_pkg);
        break;
    default:
        break;
    }
}

/*@brief 注入对端ack(发送状态机推进) */
void app_module_xfer_file_ack_respond(uint8_t result)
{
    if (!app_module_xfer_send.active || app_module_xfer_send.fb != APP_XFER_FB_NONE)
        return;
    app_module_xfer_send.fb = (result == APP_MODULE_XFER_FILE_RESULT_OK) ? APP_XFER_FB_ACK_OK : APP_XFER_FB_ACK_FAIL;
}

/*@brief 注入对端断点(发送状态机续传) */
void app_module_xfer_file_xfer_respond(uint16_t offset)
{
    if (!app_module_xfer_send.active || app_module_xfer_send.fb != APP_XFER_FB_NONE)
        return;
    app_module_xfer_send.fb        = APP_XFER_FB_BREAK;
    app_module_xfer_send.fb_offset = offset;
}

/*@brief 注入对端状态同步(发送状态机按期望续传位置续发) */
void app_module_xfer_file_state_respond(uint16_t offset)
{
    if (!app_module_xfer_send.active || app_module_xfer_send.fb != APP_XFER_FB_NONE)
        return;
    app_module_xfer_send.fb        = APP_XFER_FB_STATE;
    app_module_xfer_send.fb_offset = offset;
}

/*@brief 接收描述符:记录整体校验目标 */
bool app_module_xfer_file_recv_start(uint8_t type, const char *name, uint32_t size, uint8_t crc8, uint8_t cks8)
{
    app_module_xfer_recv.active       = true;
    app_module_xfer_recv.offset       = 0;
    app_module_xfer_recv.last_index   = 0;
    app_module_xfer_recv.expect_size  = size;
    app_module_xfer_recv.expect_crc8  = crc8;
    app_module_xfer_recv.expect_cks8  = cks8;
    APP_SYS_LOG_INFO("file recv start type:%u name:%s size:%u crc8:%02x cks8:%02x", type, name, size, crc8, cks8);
    app_module_xfer_ops.send_ack(APP_MODULE_XFER_FILE_RESULT_OK, APP_MODULE_XFER_FILE_SUB_DES, false);
    return true;
}

/*@brief 接收分包:校验并组包 */
bool app_module_xfer_file_recv_pkg(uint16_t index, uint32_t base, const uint8_t *data, uint16_t size)
{
    if (!app_module_xfer_recv.active) {
        app_module_xfer_ops.send_ack(APP_MODULE_XFER_FILE_RESULT_OTHER, APP_MODULE_XFER_FILE_SUB_PKG, false);
        return false;
    }
    /* 乱序/缺失/越界/CRC:发状态同步告知对端期望续传位置 */
    if (base != app_module_xfer_recv.offset || index != app_module_xfer_recv.last_index ||
        base + size > app_sys_arr_len(app_module_xfer_recv.buffer)) {
        app_module_xfer_ops.send_state(app_module_xfer_recv.offset);
        APP_SYS_LOG_WARN("file recv pkg skip want:%u got:%u", app_module_xfer_recv.last_index, index);
        return false;
    }
    memcpy(app_module_xfer_recv.buffer + base, data, size);
    app_module_xfer_recv.offset += size;
    app_module_xfer_recv.last_index++;
    APP_SYS_LOG_INFO("file recv pkg index:%u base:%u size:%u", index, base, size);
    app_module_xfer_ops.send_ack(APP_MODULE_XFER_FILE_RESULT_OK, APP_MODULE_XFER_FILE_SUB_PKG, false);
    return true;
}

/*@brief 接收传输字(结束):整体校验 */
bool app_module_xfer_file_recv_xfer(uint8_t stage)
{
    if (stage != APP_MODULE_XFER_FILE_STAGE_END || !app_module_xfer_recv.active)
        return false;
    app_module_xfer_recv.active = false;
    bool ok = (app_module_xfer_recv.offset == app_module_xfer_recv.expect_size) &&
              (app_sys_crc8(app_module_xfer_recv.buffer, app_module_xfer_recv.offset) == app_module_xfer_recv.expect_crc8) &&
              (app_sys_checksum8(app_module_xfer_recv.buffer, app_module_xfer_recv.offset) == app_module_xfer_recv.expect_cks8);
    if (ok)
        APP_SYS_LOG_INFO("file recv end ok size:%u", app_module_xfer_recv.offset);
    else
        APP_SYS_LOG_ERROR("file recv end fail size:%u/%u", app_module_xfer_recv.offset, app_module_xfer_recv.expect_size);
    app_module_xfer_ops.send_ack(ok ? APP_MODULE_XFER_FILE_RESULT_OK : APP_MODULE_XFER_FILE_RESULT_CHK_FAIL,
                                 APP_MODULE_XFER_FILE_SUB_XFER, !ok);
    return ok;
}

/*@brief 组装文件内容:日志条目换行拼接 */
uint32_t app_module_xfer_file_build(uint8_t *image, uint32_t image_size)
{
    uint32_t file_size = 0;
    app_sys_log_text_peek_reset();
    while (file_size < image_size) {
        char item[APP_SYS_LOG_TEXT_LIMIT + 1] = {0};
        if (!app_sys_log_text_peek(item) || item[0] == '\0')
            break;
        uint32_t item_len = strlen(item);
        if (file_size + item_len + 1 >= image_size)
            break;
        memcpy(image + file_size, item, item_len);
        file_size += item_len;
        image[file_size++] = '\n';
    }
    if (file_size == 0) {
#if APP_MODULE_PROTOCOL_TEST_ENABLE
        for (uint32_t idx = 0; idx < image_size; idx++)
            image[idx] = (uint8_t)(idx + (idx / 7));
        file_size = image_size;
#else
        /* 等待系统适配:填充真实文件内容 */
#endif
    }
    return file_size;
}