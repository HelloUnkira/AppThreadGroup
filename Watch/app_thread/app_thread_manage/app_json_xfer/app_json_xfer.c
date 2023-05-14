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

#if      APP_MODULE_PROTOCOL_USE_JSON
#include "cJSON.h"
#include "app_json_xfer.h"
#include "app_json_xfer_mix.h"

/*@brief     协议适配层,推送协议数据
 *@param[in] json_object JSON对象
 *@retval    推送是否成功
 */
bool app_json_xfer_notify(void *json_object)
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
    /* 销毁JSON对象 */
    cJSON_Delete(json_item);
    /* 压缩JSON数据流 */
    cJSON_Minify(json_stream);
    /* 传输JSON数据流 */
    return app_json_xfer_throw(json_stream);
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
    /* 派发JSON */
    return app_json_xfer_dispatch(json_item);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_json_xfer_throw(uint8_t *json_stream)
{
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("json minify:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    APP_SYS_LOG_INFO_RAW(json_stream);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 本地回环 */
    #if APP_MODULE_PROTOCOL_LOCAL_LOOPBACK
    uint32_t size = strlen(json_stream) + 1;
    uint8_t *data = app_mem_alloc(size);
    memset(data, 0 , size);
    memcpy(data, json_stream, size);
    app_module_protocol_t protocol = {
        .respond.data    = data,
        .respond.size    = size,
        .respond.dynamic = true,
    };
    app_module_protocol_respond(&protocol);
    #endif
    /* 传给底层,发出去 */
    cJSON_free(json_stream);
}

/*@brief     协议适配层,派发协议数据
 *@param[in] json_object JSON对象
 *@retval    派发是否成功
 */
bool app_json_xfer_dispatch(void *json_object)
{
    cJSON *json_item = json_object;
    /* 解析数据包 */
    char *type_string = cJSON_GetStringValue(cJSON_GetObjectItem(json_item, "type"));
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("json type:%s", type_string);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 匹配数据包 */
    if (strcmp(type_string, "sys clk") == 0) {
        app_json_xfer_respond_system_clock(json_object);
        return true;
    }
    /* 继续添加其他类型... */
    cJSON_Delete(json_item);
    return false;
}

#endif

