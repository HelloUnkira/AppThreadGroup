/*实现目标:
 *    协议自检(ACK推进式回环发包,验证各协议消息收发通路)
 *    protocol层统一逻辑语义,不区分json/nanopb(分离在protocol下层)
 *    nanopb:通用确认引擎驱动——发一条等一条ack,确认后推进下一条测试指令,替代定时器无脑发包;
 *            超时未获ack由引擎重发,3次超时则当次notify自动结束
 *    file:  内部多步调度(描述符/分包/结束),结束后经同一完成回调推进;
 *    json:  暂用定时器驱动(未启用)
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

/*@brief 待测试的协议消息类型序列 */
static const app_module_protocol_notify_type_t app_module_protocol_test_msg[] = {
    app_module_protocol_device_info,
    app_module_protocol_device_param,
    app_module_protocol_elec_card,
    app_module_protocol_system_clock,
    app_module_protocol_world_clock,
    app_module_protocol_alarm,
    app_module_protocol_weather,
    app_module_protocol_heart_rate,
    app_module_protocol_music,
    app_module_protocol_msg_info,
    app_module_protocol_contact,
    app_module_protocol_sport_tgt,
    app_module_protocol_user_phys,
    app_module_protocol_motion_sum,
    app_module_protocol_sport_state,
    app_module_protocol_not_disturb,
    app_module_protocol_position,
    app_module_protocol_fem_cycle,
    app_module_protocol_account,
    app_module_protocol_sport_mng,
    app_module_protocol_sport_rcd,
    app_module_protocol_ota,

    app_module_protocol_file,        /* 内部多步调度,完成后经完成回调推进 */
    app_module_protocol_trace_text,  /* 单发无确认,发出即完成 */
};

static uint32_t app_module_protocol_test_index = 0;
static bool     app_module_protocol_test_over  = false;

/*@brief 发送下一条测试指令(ACK推进式)
 *       常规协议与OTA:经确认引擎发送,等待对端ack后由完成回调推进;
 *       文件传输:内部多步调度,结束后由完成回调推进;
 *       trace_text:单发无确认,发出后立即推进
 */
static void app_module_protocol_test_next(void)
{
    if (app_module_protocol_test_over)
        return;
    /* 发送完所有消息则停止自检 */
    if (app_module_protocol_test_index >= app_sys_arr_len(app_module_protocol_test_msg)) {
        app_module_protocol_test_over = true;
        APP_SYS_LOG_WARN("protocol test over");
        return;
    }
    app_module_protocol_notify_type_t type = app_module_protocol_test_msg[app_module_protocol_test_index++];
    app_module_protocol_t protocol = {.notify.status = 0, .notify.type = type,};
    app_module_protocol_notify(&protocol);
    /* trace_text单发无确认,发出即完成,立即推进下一条 */
    if (type == app_module_protocol_trace_text)
        app_module_protocol_test_next();
}

#if APP_MODULE_PROTOCOL_USE_NANOPB
/*@brief 确认引擎完成回调
 *       上一条notify确认/超时结束后,推进到下一测试指令
 */
static void app_module_protocol_test_advance(uint16_t msg_tag, bool ok, uint32_t user)
{
    if (!ok)
        APP_SYS_LOG_ERROR("protocol test step fail tag:%u", msg_tag);
    app_module_protocol_test_next();
}
#else
/*@brief JSON协议自检定时器回调,定时发送一条协议消息(无ack确认,限速驱动)
 */
static void app_module_protocol_test_timer_handler(void *timer)
{
    if (app_module_protocol_test_index >= app_sys_arr_len(app_module_protocol_test_msg)) {
        app_sys_timer_stop(timer);
        APP_SYS_LOG_WARN("protocol test over");
        return;
    }
    app_module_protocol_t protocol = {.notify.status = 0,};
    protocol.notify.type = app_module_protocol_test_msg[app_module_protocol_test_index++];
    app_module_protocol_notify(&protocol);
}
#endif

/*@brief 协议自检
 *       回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void)
{
    app_module_protocol_test_index = 0;
    app_module_protocol_test_over  = false;
#if APP_MODULE_PROTOCOL_USE_NANOPB
    /* 注册确认引擎完成回调:ACK到达后推进下一条测试指令 */
    app_nanopb_xfer_ctrl_set_done(app_module_protocol_test_advance, 0);
    app_module_protocol_test_next();
#else
    /* JSON暂用定时器驱动(未启用) */
    static app_sys_timer_t timer = {0};
    timer.expired = app_module_protocol_test_timer_handler;
    timer.peroid  = APP_MODULE_PROTOCOL_TEST_PERIOD;
    timer.reload  = 1;
    app_sys_timer_start(&timer);
#endif
}
