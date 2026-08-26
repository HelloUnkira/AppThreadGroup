/*实现目标:
 *    nanopb文件传输
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 文件传输实例 */
static app_nanopb_xfer_file_t app_nanopb_xfer_file = {0};

/*@brief 分包总数 */
static uint32_t app_nanopb_xfer_file_pkg_count(void)
{
    return (app_nanopb_xfer_file.send.file_size + APP_MODULE_XFER_FILE_CHUNK_SIZE - 1)
           / APP_MODULE_XFER_FILE_CHUNK_SIZE;
}

/*@brief 发送文件描述符 */
static void app_nanopb_xfer_file_send_des(void)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file.which_payload = AppPB_File_des_tag,
    };
    AppPB_FileDes *des = &msg.payload.file.payload.des;
    snprintf(des->name, sizeof(des->name), "%s", (char *)app_nanopb_xfer_file.send.fname);
    des->type = (AppPB_FileDes_TYPE)app_nanopb_xfer_file.send.ftype;
    des->size = app_nanopb_xfer_file.send.file_size;
    des->crc8 = app_nanopb_xfer_file.send.file_crc8;
    des->cks8 = app_nanopb_xfer_file.send.file_cks8;
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_high, &msg);
}

/*@brief 发送文件分包 */
static void app_nanopb_xfer_file_send_pkg(uint32_t idx)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file.which_payload = AppPB_File_pkg_tag,
    };
    AppPB_FilePkg *pkg = &msg.payload.file.payload.pkg;
    pkg->index = idx;
    pkg->base  = idx * APP_MODULE_XFER_FILE_CHUNK_SIZE;
    uint32_t remain = app_nanopb_xfer_file.send.file_size - pkg->base;
    uint16_t amount  = (remain > APP_MODULE_XFER_FILE_CHUNK_SIZE)
                       ? APP_MODULE_XFER_FILE_CHUNK_SIZE : (uint16_t)remain;
    pkg->size = amount;
    memcpy(pkg->data.bytes, app_nanopb_xfer_file.send.buffer + pkg->base, amount);
    pkg->data.size = amount;
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_high, &msg);
}

/*@brief 发送文件结束 */
static void app_nanopb_xfer_file_send_end(void)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file.which_payload = AppPB_File_xfer_tag,
    };
    AppPB_FileXFer *xfer = &msg.payload.file.payload.xfer;
    xfer->type   = AppPB_FileXFer_Type_FILE_END;
    xfer->code   = AppPB_FileXFer_Code_XFER_OK;
    xfer->offset = 0;
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_high, &msg);
}

/*@brief 发送状态同步(接收方:期望续传位置) */
static void app_nanopb_xfer_file_send_state(uint32_t offset)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file.which_payload = AppPB_File_xfer_tag,
    };
    AppPB_FileXFer *xfer = &msg.payload.file.payload.xfer;
    xfer->type   = AppPB_FileXFer_Type_FILE_STATE;
    xfer->code   = AppPB_FileXFer_Code_XFER_OK;
    xfer->offset = offset;
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_high, &msg);
}

/*@brief 发送文件应答ack(接收方回送) */
static void app_nanopb_xfer_file_send_ack(uint8_t result, uint8_t sub, bool repeat)
{
    AppPB_ACK_Code code = (result == APP_MODULE_XFER_FILE_RESULT_OK)        ? AppPB_ACK_Code_SUCCEED
                        : (result == APP_MODULE_XFER_FILE_RESULT_CHK_FAIL) ? AppPB_ACK_Code_CRC_FAILED
                        : AppPB_ACK_Code_PKG_INVALID;
    AppPB_ACK_Info info = repeat ? AppPB_ACK_Info_REQUEST_REPEAT
                                : AppPB_ACK_Info_NO_QUESTION;
    uint16_t tag = (sub == APP_MODULE_XFER_FILE_SUB_DES)  ? AppPB_File_des_tag
                 : (sub == APP_MODULE_XFER_FILE_SUB_PKG)  ? AppPB_File_pkg_tag
                 :                                          AppPB_File_xfer_tag;
    app_nanopb_xfer_notify_ack_async(code, info, tag);
}

/*@brief 入队file事件:由handler dispatch发下一个子notify */
static void app_nanopb_xfer_file_step(void)
{
    app_module_protocol_t node = {.type = app_module_protocol_file };
    app_module_protocol_notify(&node, APP_MODULE_PROTOCOL_PRIO_FILE);
}

/*@brief 结束文件传输 */
static void app_nanopb_xfer_file_finish(bool ok)
{
    app_nanopb_xfer_file.send.phase  = APP_NANOPB_XFER_FILE_PHASE_DONE;
    app_nanopb_xfer_file.send.active = false;
    APP_SYS_LOG_INFO("file transfer %s", ok ? "done" : "fail");
}

/*@brief 组装文件内容:日志条目换行拼接 */
static uint32_t app_nanopb_xfer_file_build(uint8_t *image, uint32_t image_size)
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

/*@brief 续传:按断点/状态同步位置转分包阶段并继续 */
static bool app_nanopb_xfer_file_resend(uint32_t offset)
{
    uint32_t pkg_idx = offset / APP_MODULE_XFER_FILE_CHUNK_SIZE;
    if (app_nanopb_xfer_file.send.active && pkg_idx < app_nanopb_xfer_file_pkg_count()) {
        app_nanopb_xfer_file.send.phase    = APP_NANOPB_XFER_FILE_PHASE_PKG;
        app_nanopb_xfer_file.send.pkg_next = pkg_idx;
        app_nanopb_xfer_file_step();
    }
    return true;
}

/*@brief 文件ack超时:直接判定失败 */
void app_nanopb_xfer_file_timeout(void)
{
    if (!app_nanopb_xfer_file.send.active)
        return;
    APP_SYS_LOG_ERROR("file ack timeout phase:%u", app_nanopb_xfer_file.send.phase);
    app_nanopb_xfer_file_finish(false);
}

/*@brief 文件ack到达:推进状态机,仍活跃则入队file继续 */
void app_nanopb_xfer_file_ack(bool ok)
{
    if (!app_nanopb_xfer_file.send.active || !ok) {
        if (!ok && app_nanopb_xfer_file.send.active)
            app_nanopb_xfer_file_finish(false);
        return;
    }
    switch (app_nanopb_xfer_file.send.phase) {
    case APP_NANOPB_XFER_FILE_PHASE_DES:
        app_nanopb_xfer_file.send.phase = APP_NANOPB_XFER_FILE_PHASE_PKG;
        break;
    case APP_NANOPB_XFER_FILE_PHASE_PKG:
        if (++app_nanopb_xfer_file.send.pkg_next >= app_nanopb_xfer_file_pkg_count())
            app_nanopb_xfer_file.send.phase = APP_NANOPB_XFER_FILE_PHASE_END;
        break;
    case APP_NANOPB_XFER_FILE_PHASE_END:
        app_nanopb_xfer_file_finish(true);
        break;
    default:
        break;
    }
    /* 仍活跃:入队file让handler dispatch发下一个子notify */
    if (app_nanopb_xfer_file.send.active)
        app_nanopb_xfer_file_step();
}

/*@brief 文件发送应答
 *@retval ack等待标记
 */
bool app_nanopb_xfer_notify_file(void)
{
    /* 首次:构建内容+启动引擎+发描述符 */
    if (app_nanopb_xfer_file.send.phase == APP_NANOPB_XFER_FILE_PHASE_IDLE) {
#if APP_MODULE_PROTOCOL_TEST_ENABLE
        static const char file_name[] = "loopback.log";
#else
        static const char file_name[] = "app.log";
#endif
        app_nanopb_xfer_file.send.file_size = app_nanopb_xfer_file_build(app_nanopb_xfer_file.send.buffer,
                                                                        sizeof(app_nanopb_xfer_file.send.buffer));
        app_nanopb_xfer_file.send.file_crc8 = app_sys_crc8(app_nanopb_xfer_file.send.buffer,
                                                          app_nanopb_xfer_file.send.file_size);
        app_nanopb_xfer_file.send.file_cks8 = app_sys_checksum8(app_nanopb_xfer_file.send.buffer,
                                                               app_nanopb_xfer_file.send.file_size);
        app_nanopb_xfer_file.send.ftype     = APP_MODULE_XFER_FILE_TYPE_LOG;
        app_nanopb_xfer_file.send.fname     = (uint8_t *)file_name;
        app_nanopb_xfer_file.send.pkg_next   = 0;
        app_nanopb_xfer_file.send.active     = true;
        app_nanopb_xfer_file.send.phase           = APP_NANOPB_XFER_FILE_PHASE_DES;
        app_nanopb_xfer_file_send_des();
        return true;
    }
    /* 后续:按状态机发当前步骤子notify */
    if (!app_nanopb_xfer_file.send.active)
        return false;
    switch (app_nanopb_xfer_file.send.phase) {
    case APP_NANOPB_XFER_FILE_PHASE_PKG:
        app_nanopb_xfer_file_send_pkg(app_nanopb_xfer_file.send.pkg_next);
        return true;
    case APP_NANOPB_XFER_FILE_PHASE_END:
        app_nanopb_xfer_file_send_end();
        return true;
    default:
        return false;
    }
}

/*@brief 传输接收应答
 *@param message 响应消息
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message)
{
    AppPB_File *file = &message->payload.file;
    switch (file->which_payload) {
    case AppPB_File_des_tag: {
        AppPB_FileDes *des = &file->payload.des;
        app_nanopb_xfer_file.recv.active    = true;
        app_nanopb_xfer_file.recv.offset    = 0;
        app_nanopb_xfer_file.recv.idx_last  = 0;
        app_nanopb_xfer_file.recv.exp_size  = des->size;
        app_nanopb_xfer_file.recv.exp_crc8  = des->crc8;
        app_nanopb_xfer_file.recv.exp_cks8  = des->cks8;
        APP_SYS_LOG_INFO("file recv start name:%s size:%u", des->name, des->size);
        app_nanopb_xfer_file_send_ack(APP_MODULE_XFER_FILE_RESULT_OK, APP_MODULE_XFER_FILE_SUB_DES, false);
        return true;
    }
    case AppPB_File_pkg_tag: {
        AppPB_FilePkg *pkg = &file->payload.pkg;
        if (!app_nanopb_xfer_file.recv.active) {
            app_nanopb_xfer_file_send_ack(APP_MODULE_XFER_FILE_RESULT_OTHER, APP_MODULE_XFER_FILE_SUB_PKG, false);
            return false;
        }
        if (pkg->base != app_nanopb_xfer_file.recv.offset || pkg->index != app_nanopb_xfer_file.recv.idx_last ||
            pkg->base + pkg->size > sizeof(app_nanopb_xfer_file.recv.buffer)) {
            app_nanopb_xfer_file_send_state(app_nanopb_xfer_file.recv.offset);
            APP_SYS_LOG_WARN("file recv pkg skip want:%u got:%u",
                            app_nanopb_xfer_file.recv.idx_last, pkg->index);
            return false;
        }
        memcpy(app_nanopb_xfer_file.recv.buffer + pkg->base, pkg->data.bytes, pkg->size);
        app_nanopb_xfer_file.recv.offset    += pkg->size;
        app_nanopb_xfer_file.recv.idx_last++;
        app_nanopb_xfer_file_send_ack(APP_MODULE_XFER_FILE_RESULT_OK, APP_MODULE_XFER_FILE_SUB_PKG, false);
        return true;
    }
    case AppPB_File_xfer_tag: {
        AppPB_FileXFer *xfer = &file->payload.xfer;
        /* 发送方:状态同步/断点反馈 -> 续传 */
        if (xfer->type == AppPB_FileXFer_Type_FILE_STATE)
            return app_nanopb_xfer_file_resend(xfer->offset);
        if (xfer->code == AppPB_FileXFer_Code_XFER_ERR_CHK)
            return app_nanopb_xfer_file_resend(xfer->offset);
        /* 接收方:结束传输字 -> 整体校验 */
        if (xfer->type != AppPB_FileXFer_Type_FILE_END || !app_nanopb_xfer_file.recv.active)
            return false;
        app_nanopb_xfer_file.recv.active = false;
        bool ok = (app_nanopb_xfer_file.recv.offset == app_nanopb_xfer_file.recv.exp_size) &&
                  (app_sys_crc8(app_nanopb_xfer_file.recv.buffer, app_nanopb_xfer_file.recv.offset)
                   == app_nanopb_xfer_file.recv.exp_crc8) &&
                  (app_sys_checksum8(app_nanopb_xfer_file.recv.buffer, app_nanopb_xfer_file.recv.offset)
                   == app_nanopb_xfer_file.recv.exp_cks8);
        if (ok)
            APP_SYS_LOG_INFO("file recv end ok size:%u", app_nanopb_xfer_file.recv.offset);
        else
            APP_SYS_LOG_ERROR("file recv end fail size:%u/%u",
                             app_nanopb_xfer_file.recv.offset, app_nanopb_xfer_file.recv.exp_size);
        app_nanopb_xfer_file_send_ack(ok ? APP_MODULE_XFER_FILE_RESULT_OK
                                         : APP_MODULE_XFER_FILE_RESULT_CHK_FAIL,
                                       APP_MODULE_XFER_FILE_SUB_XFER, !ok);
        return ok;
    }
    default:
        return false;
    }
}

#endif