#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 在飞事务上下文:一次仅一个待确认notify */
typedef struct {
    app_sys_timer_t timer;                          /* ack轮询定时器 */
    bool            active;                         /* 有在飞事务 */
    bool            ack_ok;                         /* 本事务已收ack */
    uint16_t        msg_tag;                        /* 事务消息tag */
    uint16_t        retry;                          /* 重发计数 */
    app_module_transfer_chan_t channel;             /* 传输信道 */
    uint8_t        *frame;                          /* 编码后待重发帧 */
    size_t          frame_size;                     /* 帧大小 */
    app_nanopb_xfer_ctrl_done_t on_done;            /* 完成回调 */
    uint32_t        user;                           /* 回调参数 */
} app_nanopb_xfer_ctrl_t;
static app_nanopb_xfer_ctrl_t app_nanopb_ctrl = {0};

/* 结束在飞事务并回调 */
static void app_nanopb_xfer_ctrl_close(bool ok)
{
    if (!app_nanopb_ctrl.active)
        return;
    app_sys_timer_stop(&app_nanopb_ctrl.timer);
    if (app_nanopb_ctrl.frame)
        app_mem_free(app_nanopb_ctrl.frame);
    uint16_t msg_tag = app_nanopb_ctrl.msg_tag;
    app_nanopb_xfer_ctrl_done_t on_done = app_nanopb_ctrl.on_done;
    uint32_t user = app_nanopb_ctrl.user;
    app_nanopb_ctrl.active = false;
    app_nanopb_ctrl.frame  = NULL;
    if (on_done)
        on_done(msg_tag, ok, user);
}

/* ack轮询定时器回调:投递轮询步进到manage线程 */
static void app_nanopb_xfer_ctrl_timer_handler(void *timer)
{
    if (!app_nanopb_ctrl.active)
        return;
    app_module_protocol_t protocol = {.notify.status = 0,};
    protocol.notify.type = app_module_protocol_ctrl_step;
    app_module_protocol_notify(&protocol);
}

/* 设置确认完成回调 */
void app_nanopb_xfer_ctrl_set_done(app_nanopb_xfer_ctrl_done_t on_done, uint32_t user)
{
    app_nanopb_ctrl.on_done = on_done;
    app_nanopb_ctrl.user    = user;
}

/* 推送待确认协议数据(发包等ack),有在飞事务则拒绝 */
bool app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_t channel, AppPB_MsgSet *message)
{
    if (app_nanopb_ctrl.active) {
        APP_SYS_LOG_WARN("ctrl busy, drop tag:%u", message->which_payload);
        return false;
    }
    size_t size = 0;
    if (!pb_get_encoded_size(&size, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("ctrl encode size fail:%u", (uint32_t)message->which_payload);
    uint8_t *buffer = app_mem_alloc(size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&stream, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("ctrl encode fail:%s", stream.errmsg);
    app_nanopb_ctrl.frame      = buffer;
    app_nanopb_ctrl.frame_size = size;
    app_nanopb_ctrl.channel    = channel;
    app_nanopb_ctrl.msg_tag    = (uint16_t)message->which_payload;
    app_nanopb_ctrl.retry      = 0;
    app_nanopb_ctrl.active     = true;
    app_module_transfer_notify(channel, buffer, size);
    app_nanopb_ctrl.timer.expired = app_nanopb_xfer_ctrl_timer_handler;
    app_nanopb_ctrl.timer.peroid  = APP_NANOPB_ACK_POLL_PERIOD;
    app_nanopb_ctrl.timer.reload  = 1;
    app_sys_timer_start(&app_nanopb_ctrl.timer);
    APP_SYS_LOG_INFO("ctrl send wait ack tag:%u", app_nanopb_ctrl.msg_tag);
    return true;
}

/* ack轮询步进:超时重发,超限结束 */
void app_nanopb_xfer_ctrl_step(void)
{
    if (!app_nanopb_ctrl.active) {
        app_sys_timer_stop(&app_nanopb_ctrl.timer);
        return;
    }
    if (++app_nanopb_ctrl.retry > APP_NANOPB_ACK_MAX_RETRY) {
        APP_SYS_LOG_ERROR("ctrl ack timeout, abort tag:%u retry:%u", app_nanopb_ctrl.msg_tag, app_nanopb_ctrl.retry - 1);
        app_nanopb_xfer_ctrl_close(false);
        return;
    }
    app_module_transfer_notify(app_nanopb_ctrl.channel, app_nanopb_ctrl.frame, app_nanopb_ctrl.frame_size);
    APP_SYS_LOG_WARN("ctrl resend tag:%u retry:%u", app_nanopb_ctrl.msg_tag, app_nanopb_ctrl.retry);
}

/* 消费对端ack:匹配当前事务则结束 */
bool app_nanopb_xfer_ctrl_respond_ack(uint16_t type, uint8_t code, uint16_t index)
{
    if (!app_nanopb_ctrl.active || type != app_nanopb_ctrl.msg_tag)
        return false;
    bool ok = (code == AppPB_ACK_ErrorCode_SUCCEED);
    APP_SYS_LOG_INFO("ctrl ack tag:%u ok:%d", type, ok);
    app_nanopb_xfer_ctrl_close(ok);
    return true;
}

/* 触发完成回调:供文件传输等独立状态机复用 */
void app_nanopb_xfer_ctrl_notify_done(uint16_t msg_tag, bool ok)
{
    if (app_nanopb_ctrl.on_done)
        app_nanopb_ctrl.on_done(msg_tag, ok, app_nanopb_ctrl.user);
}

/* 是否有在飞确认事务 */
bool app_nanopb_xfer_ctrl_active(void)
{
    return app_nanopb_ctrl.active;
}

#endif
