/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_trace_text.h"
#include "app_thread_group.h"
#include "app_module_protocol.h"
#include "app_module_clock.h"

#if       APP_MODULE_PROTOCOL_USE_JSON
#include "cJSON.h"
#include "app_json_xfer.h"
#include "app_json_xfer_mix.h"

/*@brief 打包传输系统追踪日志文本
 */
void app_json_xfer_notify_trace_text(void)
{
    app_sys_trace_text_peek_reset();
    while (true) {
        /* 循环提取日志信息 */
        char trace_text[APP_SYS_TRACE_TEXT_MAX + 1] = {0};
        app_sys_trace_text_peek(trace_text);
        if (strlen(trace_text) == 0)
            break;
        /* 创建传输对象 */
        cJSON *json_item = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_item, "type", app_json_xfer_msg_is_trace_text);
        /* 数据打包 */
        cJSON_AddStringToObject(json_item, "text", trace_text);
        /* 传输对象发送通知 */
        app_json_xfer_notify(json_item);
        /* 销毁传输对象 */
        cJSON_Delete(json_item);
    }
}

/*@brief 传输接收系统追踪日志文本
 */
bool app_json_xfer_respond_trace_text(cJSON *json_object)
{
    cJSON *json_item = json_object;
    /* 获得目标包数据 */
    cJSON *text_item = cJSON_GetObjectItem(json_item, "text");
    /* 解析目标包数据 */
    char *trace_text = cJSON_GetStringValue(text_item);
#if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("trace text:%s", trace_text);
#endif
    return true;
}

/*@brief 打包传输系统时钟
 */
void app_json_xfer_notify_system_clock(void)
{
    char clock_string[20] = {0};
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 创建传输对象 */
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddStringToObject(json_item, "type", app_json_xfer_msg_is_system_clock);
    cJSON_AddObjectToObject(json_item, "text");
    cJSON *src_item = cJSON_GetObjectItem(json_item, "text");
    /* 数据打包 */
    cJSON_AddNumberToObject(src_item, "year",   clock.year);
    cJSON_AddNumberToObject(src_item, "mon",    clock.month);
    cJSON_AddNumberToObject(src_item, "day",    clock.day);
    cJSON_AddNumberToObject(src_item, "hour",   clock.hour);
    cJSON_AddNumberToObject(src_item, "min",    clock.minute);
    cJSON_AddNumberToObject(src_item, "sec",    clock.second);
    cJSON_AddNumberToObject(src_item, "zone",   clock.zone_sec);
    cJSON_AddNumberToObject(src_item, "is_24",  clock.is_24);
    /* 传输对象发送通知 */
    app_json_xfer_notify(json_item);
    /* 销毁传输对象 */
    cJSON_Delete(json_item);
}

/*@brief 传输接收系统时钟
 */
bool app_json_xfer_respond_system_clock(cJSON *json_object)
{
    cJSON *json_item = json_object;
    /* 获得目标包数据 */
    cJSON *text_item = cJSON_GetObjectItem(json_item, "text");
    /* 解析目标包数据 */
    app_module_clock_t clock = {
        .year       = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "year"))  + 0.5,
        .month      = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "mon"))   + 0.5,
        .day        = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "day"))   + 0.5,
        .minute     = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "min"))   + 0.5,
        .second     = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "sec"))   + 0.5,
        .zone_sec   = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "zone"))  + 0.5,
        .is_24      = cJSON_GetNumberValue(cJSON_GetObjectItem(text_item, "is_24")) + 0.5,
    };
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("system_clock.year:%u",    clock.year);
    APP_SYS_LOG_INFO("system_clock.month:%u",   clock.month);
    APP_SYS_LOG_INFO("system_clock.day:%u",     clock.day);
    APP_SYS_LOG_INFO("system_clock.hour:%u",    clock.hour);
    APP_SYS_LOG_INFO("system_clock.minute:%u",  clock.minute);
    APP_SYS_LOG_INFO("system_clock.second:%u",  clock.second);
    APP_SYS_LOG_INFO("system_clock.zone:%u",    clock.zone_sec);
    APP_SYS_LOG_INFO("system_clock.mode:%u",    clock.is_24);
    #endif
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
    return true;
}

#endif
