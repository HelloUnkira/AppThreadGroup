/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 *@param priority 事件优先级(app_thread_package_priority_*,0为默认)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol, uint32_t priority)
{
    app_module_protocol_t *protocol_new = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(protocol_new, protocol, sizeof(app_module_protocol_t));
    /* 传输对象发送通知 */
    app_thread_package_t package = {
        .thread   = app_thread_id_manage,
        .module   = app_thread_manage_protocol,
        .event    = app_thread_manage_protocol_notify,
        .priority = priority,
        .dynamic  = true,
        .data     = protocol_new,
        .size     = sizeof(app_module_protocol_t),
    };
    app_thread_package_notify(&package);
}

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol)
{
    app_module_protocol_t *protocol_new = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(protocol_new, protocol, sizeof(app_module_protocol_t));
    /* 传输对象发送通知(应答高于轮询:保证ack在超时计数前被消费) */
    app_thread_package_t package = {
        .thread   = app_thread_id_manage,
        .module   = app_thread_manage_protocol,
        .event    = app_thread_manage_protocol_respond,
        .priority = app_thread_package_priority_normal_above,
        .dynamic  = true,
        .data     = protocol_new,
        .size     = sizeof(app_module_protocol_t),
    };
    app_thread_package_notify(&package);
}

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    
    #if APP_MODULE_PROTOCOL_USE_NANOPB
    switch (protocol->type) {
    case app_module_protocol_ack: {
         app_nanopb_xfer_ack_arg_t *ack_cfg = (void *)protocol->data;
         app_nanopb_xfer_notify_ack(ack_cfg->code, ack_cfg->info, ack_cfg->msg);
         if (protocol->dynamic) app_mem_free(protocol->data);
         break;
    }
    case app_module_protocol_sync: {
         app_nanopb_xfer_sync_arg_t *sync_cfg = (void *)protocol->data;
         app_nanopb_xfer_notify_sync(sync_cfg->type, sync_cfg->msg);
         if (protocol->dynamic) app_mem_free(protocol->data);
         break;
    }
    case app_module_protocol_device_info: {
         app_nanopb_xfer_notify_device_info();
         break;
    }
    case app_module_protocol_device_param: {
         app_nanopb_xfer_notify_device_param();
         break;
    }
    case app_module_protocol_elec_card: {
         app_nanopb_xfer_notify_elec_card();
         break;
    }
    case app_module_protocol_system_clock: {
         app_nanopb_xfer_notify_system_clock();
         break;
    }
    case app_module_protocol_world_clock: {
         app_nanopb_xfer_notify_world_clock();
         break;
    }
    case app_module_protocol_alarm: {
         app_nanopb_xfer_notify_alarm();
         break;
    }
    case app_module_protocol_weather: {
         app_nanopb_xfer_notify_weather();
         break;
    }
    case app_module_protocol_heart_rate: {
         app_nanopb_xfer_notify_heart_rate();
         break;
    }
    case app_module_protocol_music: {
         app_nanopb_xfer_notify_music();
         break;
    }
    case app_module_protocol_msg_info: {
         app_nanopb_xfer_notify_msg_info();
         break;
    }
    case app_module_protocol_contact: {
         app_nanopb_xfer_notify_contact();
         break;
    }
    case app_module_protocol_sport_tgt: {
         app_nanopb_xfer_notify_sport_tgt();
         break;
    }
    case app_module_protocol_user_phys: {
         app_nanopb_xfer_notify_user_phys();
         break;
    }
    case app_module_protocol_motion_sum: {
         app_nanopb_xfer_notify_motion_sum();
         break;
    }
    case app_module_protocol_sport_state: {
         app_nanopb_xfer_notify_sport_state();
         break;
    }
    case app_module_protocol_not_disturb: {
         app_nanopb_xfer_notify_not_disturb();
         break;
    }
    case app_module_protocol_position: {
         app_nanopb_xfer_notify_position();
         break;
    }
    case app_module_protocol_fem_cycle: {
         app_nanopb_xfer_notify_fem_cycle();
         break;
    }
    case app_module_protocol_account: {
         app_nanopb_xfer_notify_account();
         break;
    }
    case app_module_protocol_sport_mng: {
         app_nanopb_xfer_notify_sport_mng();
         break;
    }
    case app_module_protocol_sport_rcd: {
         app_nanopb_xfer_notify_sport_rcd();
         break;
    }
    case app_module_protocol_file: {
         app_nanopb_xfer_notify_file();
         break;
    }
    case app_module_protocol_file_step: {
         app_nanopb_xfer_file_step();
         break;
    }
    case app_module_protocol_ctrl_step: {
         app_nanopb_xfer_ctrl_step();
         break;
    }
    default: {
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", protocol->type);
        break;
    }
    }
    #endif
}

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    /* 检查数据流 */
    APP_SYS_LOG_INFO_RAW("size:%d%s", protocol->size, app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("data:");
    for (uint32_t idx = 0; idx < protocol->size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", protocol->data[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    app_nanopb_xfer_respond(protocol->data, protocol->size);
    
    if (protocol->dynamic)
        app_mem_free(protocol->data);
}

#if APP_MODULE_PROTOCOL_TEST_ENABLE

/*@brief 发送下一条测试指令(ACK推进式)
 */
static void app_module_protocol_test_next(void)
{
    /* 待测试的协议消息类型序列 */
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
    
        app_module_protocol_file,        /* 内部多步调度,完成后经完成回调推进 */
    };
    
    static bool app_module_protocol_test_over  = false;
    if (app_module_protocol_test_over) return;
    
    static uint32_t app_module_protocol_test_index = 0;
    if (app_module_protocol_test_index >= app_sys_arr_len(app_module_protocol_test_msg)) {
        app_module_protocol_test_over = true;
        APP_SYS_LOG_WARN("protocol test over");
        return;
    }
    app_module_protocol_notify_type_t type = app_module_protocol_test_msg[app_module_protocol_test_index++];
    app_module_protocol_t protocol = {.status = 0, .type = type,};
    app_module_protocol_notify(&protocol, 0);
}

/*@brief 确认引擎完成回调:
 *       上一条确认/超时后推进下一条
 */
static void app_module_protocol_test_advance(uint16_t msg_tag, bool ok, uint32_t user)
{
    if (!ok) APP_SYS_LOG_ERROR("protocol test step fail tag:%u", msg_tag);
    app_module_protocol_test_next();
}

/*@brief 协议自检(回环发包)
 */
static void app_module_protocol_test_run(void)
{
#if APP_MODULE_PROTOCOL_USE_NANOPB
    /* 注册确认引擎完成回调:ACK到达后推进下一条测试指令 */
    app_nanopb_xfer_ctrl_set_done(app_module_protocol_test_advance, 0);
    app_module_protocol_test_next();
#endif
}
#endif

/*@brief 系统时钟模组初始化
 *       内部使用: 被namage线程使用
 */
void app_module_protocol_ready(void)
{
    #if APP_MODULE_PROTOCOL_USE_NANOPB
    /* 注入通用文件传输引擎的nanopb原语 */
    app_nanopb_xfer_file_init();
    #else
    #error "app module protocol only support nanopb"
    #endif
    
    #if APP_MODULE_PROTOCOL_TEST_ENABLE
    /* 协议自检:开机回环发包 */
    app_module_protocol_test_run();
    #endif
}
