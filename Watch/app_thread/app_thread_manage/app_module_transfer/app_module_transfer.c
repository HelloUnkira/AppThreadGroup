/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_transfer.h"

#include "cJSON.h"

/*@brief     协议适配层,推送协议数据
 *@param[in] json_object JSON对象
 *@retval    推送是否成功
 */
bool app_module_transfer_notify(void *json_object)
{
    /* 获得JSON对象 */
    cJSON *json_item = json_object;
    /* 将JSON对象转为JSON数据流 */
    char *json_stream = cJSON_Print(json_item);
    /* 检查JSON */
    #if APP_SYS_LOG_TRANSFER_CHECK
    APP_SYS_LOG_INFO("app_module_respond json:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(json_stream);
    #endif
    /* 销毁JSON对象 */
    cJSON_Delete(json_item);
    /* 压缩JSON数据流 */
    cJSON_Minify(json_stream);
    /* 检查JSON */
    /* 传输JSON数据流 */
    return app_module_transfer_throw(json_stream);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_module_transfer_respond(uint8_t *json_stream)
{
    /* 解析JSON */
    cJSON *json_item = cJSON_Parse(json_stream);
    if (json_item == NULL) {
        APP_SYS_LOG_ERROR("app_module_respond error:");
        APP_SYS_LOG_ERROR("%s", json_stream);
        return false;
    }
    /* 检查JSON */
    #if APP_SYS_LOG_TRANSFER_CHECK
    char *json_format = cJSON_Print(json_item);
    APP_SYS_LOG_INFO("app_module_respond json:%d", strlen(json_stream));
    APP_SYS_LOG_INFO_RAW(json_format);
    app_mem_free(json_format);
    #endif
    /* 派发JSON */
    return app_module_transfer_dispatch(json_item);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_module_transfer_throw(uint8_t *json_stream)
{
}

/*@brief     协议适配层,派发协议数据
 *@param[in] json_object JSON对象
 *@retval    派发是否成功
 */
bool app_module_transfer_dispatch(void *json_object)
{
    cJSON *json_item = json_object;
    
    cJSON_Delete(json_item);
}
