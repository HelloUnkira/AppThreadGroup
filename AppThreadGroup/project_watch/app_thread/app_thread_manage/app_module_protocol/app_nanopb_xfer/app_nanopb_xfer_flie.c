/*实现目标:
 *    nanopb协议-文件(flie)消息:注入通用文件传输引擎原语,薄壳委托
 *    状态机/校验/应答固化为通用流程(module层),本模组仅做序列化协议的收发包适配
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 发送文件子消息(高速信道)
 *@param msg 文件子消息(描述符/分包/结束)
 */
static void app_nanopb_xfer_file_send(AppPB_MsgSet *msg)
{
    app_nanopb_xfer_notify(app_module_transfer_chan_high, msg);
}

/*@brief 发送文件应答ack
 *@param type  子消息tag
 *@param code  错误码
 *@param index 拉取/重传游标
 */
static void app_nanopb_xfer_file_ack_send(uint16_t type, uint8_t code, uint16_t index)
{
    app_nanopb_xfer_notify_ack_ext(type, code, index);
}

/*@brief 投递文件传输轮询步进到manage线程 */
static void app_nanopb_xfer_file_post_step(void)
{
    app_module_protocol_t protocol = {.notify.status = 0,};
    protocol.notify.type = app_module_protocol_file_step;
    app_module_protocol_notify(&protocol);
}

/*@brief 文件发送完成:向通用确认引擎通报(ACK推进式测试据此进入下一指令)
 *@param ok 是否成功
 */
static void app_nanopb_xfer_file_done(bool ok)
{
    app_nanopb_xfer_ctrl_notify_done(AppPB_MsgSet_file_tag, ok);
}

/*@brief 初始化文件传输引擎(注入nanopb原语)
 */
void app_nanopb_xfer_file_init(void)
{
    static const app_module_xfer_file_ops_t ops = {
        .meta_only = false,
        .chunk     = APP_MODULE_XFER_FILE_CHUNK,
        .send_file = app_nanopb_xfer_file_send,
        .send_ack  = app_nanopb_xfer_file_ack_send,
        .post_step = app_nanopb_xfer_file_post_step,
    };
    app_module_xfer_file_config(&ops);
    app_module_xfer_file_set_done(app_nanopb_xfer_file_done);
}

/*@brief 打包传输文件(拉取式:发送描述符后等待对端ack拉取分包)
 */
void app_nanopb_xfer_notify_file(void)
{
    app_module_xfer_file_notify(app_module_protocol_test_file_name());
}

/*@brief 文件传输轮询步进
 */
void app_nanopb_xfer_file_step(void)
{
    app_module_xfer_file_step();
}

/*@brief 传输接收应答注入(文件发送状态机)
 *@param ack 应答消息
 */
void app_nanopb_xfer_file_ack(AppPB_ACK *ack)
{
    app_module_xfer_file_ack((uint16_t)ack->type, (uint8_t)ack->error_code, (uint16_t)ack->index);
}

/*@brief 传输接收文件(开始/包/结束)
 *       委托通用引擎完成校验并回ack
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message)
{
    return app_module_xfer_file_respond(&message->payload.file);
}

/*@brief 打包传输OTA升级
 */
void app_nanopb_xfer_notify_ota(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_ota(&message);
#else
    message.which_payload = AppPB_MsgSet_ota_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收OTA升级
 */
bool app_nanopb_xfer_respond_ota(AppPB_MsgSet *message)
{
    AppPB_Ota *msg = &message->payload.ota;
    APP_SYS_LOG_INFO("ota.cmd:%u", msg->cmd);
    APP_SYS_LOG_INFO("ota.state:%u", msg->state);
    APP_SYS_LOG_INFO("ota.ready_cond:%u", msg->ready_cond);
    return true;
}

#endif