/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_protocol.h"

#if 0
#elif APP_MODULE_PROTOCOL_USE_JSON
#include "cJSON.h"
#include "app_json_xfer.h"
#include "app_json_xfer_mix.h"
#elif APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_pb_msg_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"
#else
#error "app module protocol is unknown"
#endif

/*@brief     传输协议
 *@param[in] protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol)
{
    app_module_protocol_t *protocol_new = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(protocol_new, protocol, sizeof(app_module_protocol_t));
    /* 传输对象发送通知 */
    app_package_t package = {
        .thread  = app_thread_id_manage,
        .module  = app_thread_manage_protocol,
        .event   = app_thread_manage_protocol_notify,
        .dynamic = true,
        .data    = protocol_new,
        .size    = sizeof(app_module_protocol_t),
    };
    app_package_notify(&package);
}

/*@brief     传输协议
 *@param[in] protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol)
{
    app_module_protocol_t *protocol_new = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(protocol_new, protocol, sizeof(app_module_protocol_t));
    /* 传输对象发送通知 */
    app_package_t package = {
        .thread  = app_thread_id_manage,
        .module  = app_thread_manage_protocol,
        .event   = app_thread_manage_protocol_respond,
        .dynamic = true,
        .data    = protocol_new,
        .size    = sizeof(app_module_protocol_t),
    };
    app_package_notify(&package);
}

/*@brief     传输协议
 *@param[in] data 传输数据
 *@param[in] size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    switch (protocol->notify.type) {
    case app_module_protocol_system_clock: {
         app_json_xfer_notify_system_clock();
         break;
    }
    default: {
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", protocol->notify.type);
        break;
    }
    }
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    switch (protocol->notify.type) {
    case app_module_protocol_system_clock: {
         app_nanopb_xfer_notify_system_clock();
         break;
    }
    default: {
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", protocol->notify.type);
        break;
    }
    }
    #else
    #error "app module protocol is unknown"
    #endif
}

/*@brief     传输协议
 *@param[in] data 传输数据
 *@param[in] size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    /* 检查数据流 */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("size:%d", protocol->respond.size);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    APP_SYS_LOG_INFO_RAW("data:%s", protocol->respond.data);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    
    #if 0
    #elif APP_MODULE_PROTOCOL_USE_JSON
    app_json_xfer_respond(protocol->respond.data);
    #elif APP_MODULE_PROTOCOL_USE_NANOPB
    app_nanopb_xfer_respond(protocol->respond.data, protocol->respond.size);
    #else
    #error "app module protocol is unknown"
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
    #else
    #error "app module protocol is unknown"
    #endif
}
