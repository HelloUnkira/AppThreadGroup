/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"

#if       APP_MODULE_PROTOCOL_USE_JSON
#include "cJSON.h"
#include "app_json_xfer.h"
#include "app_json_xfer_mix.h"

/*@brief     协议适配层,推送协议数据
 *@param[in] json_object JSON对象
 *@retval    推送是否成功
 */
bool app_json_xfer_notify(cJSON *json_object)
{
    /* 获得JSON对象 */
    cJSON *json_item = json_object;
    /* 将JSON对象转为JSON数据流 */
    char *json_stream = cJSON_Print(json_item);
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO_RAW("json:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    APP_SYS_LOG_INFO_RAW(json_stream);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 压缩JSON数据流 */
    cJSON_Minify(json_stream);
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("json minify:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    APP_SYS_LOG_INFO_RAW(json_stream);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 传输JSON数据流 */
    app_module_transfer_notify(0, json_stream, strlen(json_stream));
    /* 回收JSON对象 */
    cJSON_free(json_stream);
    return true;
}

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_json_xfer_respond(uint8_t *json_stream)
{
    /* 解析JSON */
    cJSON *json_item = cJSON_Parse(json_stream);
    if (json_item == NULL) {
        char *error = cJSON_GetErrorPtr();
        APP_SYS_LOG_ERROR("");
        APP_SYS_LOG_ERROR_RAW("error:%s", error);
        APP_SYS_LOG_ERROR_RAW(APP_SYS_LOG_LINE);
        APP_SYS_LOG_ERROR_RAW(json_stream);
        APP_SYS_LOG_ERROR_RAW(APP_SYS_LOG_LINE);
        return false;
    }
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    char *json_format = cJSON_Print(json_item);
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO_RAW("json:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    APP_SYS_LOG_INFO_RAW(json_format);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    cJSON_free(json_format);
    #endif
    /* 解析JSON */
    uint32_t type = cJSON_GetNumberValue(cJSON_GetObjectItem(json_item, "type"));
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("json type:%d", type);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 匹配数据包 */
    bool retval = false;
    switch (type) {
    case app_json_xfer_msg_is_trace_text:
        retval = app_json_xfer_respond_trace_text(json_item);
        break;
    case app_json_xfer_msg_is_system_clock:
        retval = app_json_xfer_respond_system_clock(json_item);
        break;
    default:
        APP_SYS_LOG_INFO_RAW("unknown json type:%d", type);
        APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
        break;
    }
    /* 回收JSON对象 */
    cJSON_Delete(json_item);
    return retval;
}

#endif

