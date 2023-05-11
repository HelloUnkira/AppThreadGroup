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
#include "app_module_transfer.h"
#include "app_module_transfer_mix.h"

#include "app_module_clock.h"

#include "cJSON.h"

/*@brief 打包传输系统时钟
 */
void app_module_transfer_notify_system_clock(void)
{
    char clock_string[20] = {0};
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 创建传输对象 */
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddStringToObject(json_item, "type", "sys clk");
    cJSON_AddObjectToObject(json_item, "src");
    cJSON *src_item = cJSON_GetObjectItem(json_item, "src");
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
    app_package_t package = {
        .thread  = app_thread_id_manage,
        .module  = app_thread_manage_transfer,
        .event   = app_thread_manage_transfer_notify,
        .data    = json_item,
        .size    = 0,
    };
    app_package_notify(&package);
}

/*@brief 传输接收系统时钟
 */
bool app_module_transfer_respond_system_clock(void *json_object)
{
    cJSON *json_item = json_object;
    /* 匹配是否为目标包 */
    if (strcmp(cJSON_GetStringValue(cJSON_GetObjectItem(json_item, "type")), "sys clk") != 0)
        return false;
    /* 获得目标包数据 */
    cJSON *src_item = cJSON_GetObjectItem(json_item, "src");
    /* 解析目标包数据 */
    app_module_clock_t clock = {
        .year       = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "year"))  + 0.5,
        .month      = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "mon"))   + 0.5,
        .day        = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "day"))   + 0.5,
        .minute     = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "min"))   + 0.5,
        .second     = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "sec"))   + 0.5,
        .zone_sec   = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "zone"))  + 0.5,
        .is_24      = cJSON_GetNumberValue(cJSON_GetObjectItem(src_item, "is_24")) + 0.5,
    };
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
    return true;
}
