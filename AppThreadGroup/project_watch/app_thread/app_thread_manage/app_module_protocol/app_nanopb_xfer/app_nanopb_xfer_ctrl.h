#ifndef APP_NANOPB_XFER_CTRL_H
#define APP_NANOPB_XFER_CTRL_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 通用确认引擎:发包等ack,超时重发,超限自动结束 */
#define APP_NANOPB_ACK_POLL_PERIOD  100     /* ack轮询周期 */
#define APP_NANOPB_ACK_MAX_RETRY    3       /* ack重发上限 */

/*@brief 确认完成回调
 *@param msg_tag 消息tag
 *@param ok      是否成功
 *@param user    用户参数
 */
typedef void (*app_nanopb_xfer_ctrl_done_t)(uint16_t msg_tag, bool ok, uint32_t user);

/* 确认引擎上下文:一次"未决发送"(已发出待对端ack确认)的状态; 引擎同一时刻只保一个 */
typedef struct {
    app_sys_timer_t                    timer;      /* ack轮询定时器 */
    bool                               active;     /* 是否有未决发送 */
    bool                               ack_ok;     /* 本发送已收ack */
    uint16_t                           msg_tag;    /* 待确认的消息tag */
    uint16_t                           retry;      /* 重发计数 */
    uint8_t                           *frame;      /* 编码后待重发帧 */
    size_t                             frame_size; /* 帧大小 */
    app_module_transfer_chan_t         channel;    /* 传输信道 */
    app_nanopb_xfer_ctrl_done_t        on_done;    /* 完成回调 */
    uint32_t                           user;       /* 回调参数 */
} app_nanopb_xfer_ctrl_t;

/*@brief 设置确认完成回调
 *@param on_done 回调
 *@param user    参数
 */
void app_nanopb_xfer_ctrl_set_done(app_nanopb_xfer_ctrl_done_t on_done, uint32_t user);

/*@brief 推送待确认协议数据(发包等ack),有未决发送则拒绝
 *@param channel 传输信道
 *@param message 协议对象
 *@retval 是否成功
 */
bool app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_t channel, AppPB_MsgSet *message);

/*@brief ack轮询步进:超时重发,超限结束 */
void app_nanopb_xfer_ctrl_step(void);

/*@brief 消费对端ack:匹配当前事务则结束
 *@param msg  被应答消息标识
 *@param code 错误码
 *@retval 是否消费
 */
bool app_nanopb_xfer_ctrl_respond_ack(uint16_t msg, uint8_t code);

/*@brief 触发完成回调
 *@param msg_tag 消息tag
 *@param ok      是否成功
 */
void app_nanopb_xfer_ctrl_notify_done(uint16_t msg_tag, bool ok);

/*@brief 是否有未决发送
 *@retval true-有
 */
bool app_nanopb_xfer_ctrl_active(void);

#endif

#endif