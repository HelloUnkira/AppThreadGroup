/*实现目标:
 *    协议自检(定时回环发包,验证各协议消息收发通路)
 *    protocol层统一逻辑语义,不区分json/nanopb(分离在protocol下层)
 *    用软件定时器每隔一段时间发一个包,避免一口气全发导致底层响应堆积
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#define APP_MODULE_PROTOCOL_TEST_TIMER_PERIOD     150 /* 周期ms,100~200发一个包 */

static app_sys_timer_t app_module_protocol_test_timer = {0};
static uint32_t app_module_protocol_test_index = 0;

/*@brief 协议自检定时器回调,定时发送一个协议消息
 */
static void app_module_protocol_test_timer_handler(void *timer)
{
    /*@brief 待测试的协议消息类型序列 */
    static const app_module_protocol_notify_type_t app_module_protocol_test_msg[] = {
        app_module_protocol_ack,
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
    };
    
    /* 发送完所有消息则停止自检 */
    if (app_module_protocol_test_index >= app_sys_arr_len((app_module_protocol_test_msg))) {
        app_sys_timer_stop(&app_module_protocol_test_timer);
        APP_SYS_LOG_WARN("test over");
        return;
    }
    /* 定时发送一个协议消息 */
    app_module_protocol_t protocol = {.notify.status = 0,};
    protocol.notify.type = app_module_protocol_test_msg[app_module_protocol_test_index++];
    app_module_protocol_notify(&protocol);
}

/*@brief 协议自检
 *       定时回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void)
{
    /* 初始化自检定时器,周期发送 */
    app_module_protocol_test_index = 0;
    app_module_protocol_test_timer.expired   = app_module_protocol_test_timer_handler;
    app_module_protocol_test_timer.peroid    = APP_MODULE_PROTOCOL_TEST_TIMER_PERIOD;
    app_module_protocol_test_timer.reload    = 1;
    app_sys_timer_start(&app_module_protocol_test_timer);
}
