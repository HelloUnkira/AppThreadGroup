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
 */
void app_module_protocol_notify(app_module_protocol_t *protocol)
{
    app_module_protocol_t *protocol_new = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(protocol_new, protocol, sizeof(app_module_protocol_t));
    /* 传输对象发送通知 */
    app_thread_package_t package = {
        .thread  = app_thread_id_manage,
        .module  = app_thread_manage_protocol,
        .event   = app_thread_manage_protocol_notify,
        .dynamic = true,
        .data    = protocol_new,
        .size    = sizeof(app_module_protocol_t),
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
    /* 传输对象发送通知 */
    app_thread_package_t package = {
        .thread  = app_thread_id_manage,
        .module  = app_thread_manage_protocol,
        .event   = app_thread_manage_protocol_respond,
        .dynamic = true,
        .data    = protocol_new,
        .size    = sizeof(app_module_protocol_t),
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
    
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    switch (protocol->notify.type) {
    case app_module_protocol_ack: {
         app_json_xfer_notify_ack();
         break;
    }
    case app_module_protocol_trace_text: {
         app_json_xfer_notify_trace_text();
         break;
    }
    case app_module_protocol_device_info: {
         app_json_xfer_notify_device_info();
         break;
    }
    case app_module_protocol_device_param: {
         app_json_xfer_notify_device_param();
         break;
    }
    case app_module_protocol_elec_card: {
         app_json_xfer_notify_elec_card();
         break;
    }
    case app_module_protocol_system_clock: {
         app_json_xfer_notify_system_clock();
         break;
    }
    case app_module_protocol_world_clock: {
         app_json_xfer_notify_world_clock();
         break;
    }
    case app_module_protocol_alarm: {
         app_json_xfer_notify_alarm();
         break;
    }
    case app_module_protocol_weather: {
         app_json_xfer_notify_weather();
         break;
    }
    case app_module_protocol_heart_rate: {
         app_json_xfer_notify_heart_rate();
         break;
    }
    case app_module_protocol_music: {
         app_json_xfer_notify_music();
         break;
    }
    case app_module_protocol_msg_info: {
         app_json_xfer_notify_msg_info();
         break;
    }
    case app_module_protocol_contact: {
         app_json_xfer_notify_contact();
         break;
    }
    case app_module_protocol_sport_tgt: {
         app_json_xfer_notify_sport_tgt();
         break;
    }
    case app_module_protocol_user_phys: {
         app_json_xfer_notify_user_phys();
         break;
    }
    case app_module_protocol_motion_sum: {
         app_json_xfer_notify_motion_sum();
         break;
    }
    case app_module_protocol_sport_state: {
         app_json_xfer_notify_sport_state();
         break;
    }
    case app_module_protocol_not_disturb: {
         app_json_xfer_notify_not_disturb();
         break;
    }
    case app_module_protocol_position: {
         app_json_xfer_notify_position();
         break;
    }
    case app_module_protocol_fem_cycle: {
         app_json_xfer_notify_fem_cycle();
         break;
    }
    case app_module_protocol_account: {
         app_json_xfer_notify_account();
         break;
    }
    case app_module_protocol_sport_mng: {
         app_json_xfer_notify_sport_mng();
         break;
    }
    case app_module_protocol_sport_rcd: {
         app_json_xfer_notify_sport_rcd();
         break;
    }
    case app_module_protocol_file: {
         app_json_xfer_notify_file();
         break;
    }
    case app_module_protocol_file_step: {
         app_json_xfer_file_step();
         break;
    }
    case app_module_protocol_ctrl_step: {
         app_json_xfer_ctrl_step();
         break;
    }
    case app_module_protocol_ota: {
         app_json_xfer_notify_ota();
         break;
    }
    default: {
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", protocol->notify.type);
        break;
    }
    }
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    switch (protocol->notify.type) {
    case app_module_protocol_ack: {
         app_nanopb_xfer_notify_ack();
         break;
    }
    case app_module_protocol_trace_text: {
         app_nanopb_xfer_notify_trace_text();
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
    case app_module_protocol_ota: {
         app_nanopb_xfer_notify_ota();
         break;
    }
    default: {
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", protocol->notify.type);
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
    APP_SYS_LOG_INFO_RAW("size:%d%s", protocol->respond.size, app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("data:");
    for (uint32_t idx = 0; idx < protocol->respond.size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", protocol->respond.data[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    app_json_xfer_respond(protocol->respond.data);
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    app_nanopb_xfer_respond(protocol->respond.data, protocol->respond.size);
    #endif
    
    if (protocol->respond.dynamic)
        app_mem_free(protocol->respond.data);
}

/*@brief 系统时钟模组初始化
 *       内部使用: 被namage线程使用
 */
void app_module_protocol_ready(void)
{
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    /* cJSON组件初始化 */
    cJSON_Hooks cjson_hooks = {
        .malloc_fn = app_mem_alloc,
        .free_fn   = app_mem_free,
    };
    cJSON_InitHooks(&cjson_hooks);
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    /* 注入通用文件传输引擎的nanopb原语 */
    app_nanopb_xfer_file_init();
    #else
    #error "app module protocol is unknown"
    #endif
}
