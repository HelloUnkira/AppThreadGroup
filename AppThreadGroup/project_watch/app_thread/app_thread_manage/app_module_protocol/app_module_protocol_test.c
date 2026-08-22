/*实现目标:
 *    协议自检(回环发包,验证各协议消息收发通路)
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_manage.h"

#if APP_MODULE_PROTOCOL_TEST_ENABLE

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 依次发送一组协议消息
 *       走完整事件流:协议通知->线程调度->各消息notify打包->传输拆包->回环->响应解码->打印
 */
static void app_module_protocol_test_nanopb(void)
{
    app_module_protocol_t protocol = {
        .notify.status = 0,
    };
    
    /* 系统类 */
    protocol.notify.type = app_module_protocol_ack;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_device_info;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_device_param;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_elec_card;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_system_clock;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_world_clock;
    app_module_protocol_notify(&protocol);
    
    /* 功能类 */
    protocol.notify.type = app_module_protocol_alarm;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_weather;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_heart_rate;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_music;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_msg_info;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_contact;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_sport_tgt;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_user_phys;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_motion_sum;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_sport_state;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_not_disturb;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_position;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_fem_cycle;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_account;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_sport_mng;
    app_module_protocol_notify(&protocol);
    protocol.notify.type = app_module_protocol_sport_rcd;
    app_module_protocol_notify(&protocol);
    
    /* 文件类 */
    protocol.notify.type = app_module_protocol_ota;
    app_module_protocol_notify(&protocol);
}

#endif

#endif

/*@brief 协议自检
 *       开机回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void)
{
    #if APP_MODULE_PROTOCOL_TEST_ENABLE
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    /* json回环测试(待json端实现后补充) */
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    app_module_protocol_test_nanopb();
    #endif
    #endif
}
