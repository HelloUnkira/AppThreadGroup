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
#include "app_module_protocol_mix.h"
#include "app_module_clock.h"

#include "cJSON.h"

/*@brief 协议,发送系统时钟
 */
void app_module_protocol_system_time(void)
{
    char clock_string[20] = {0};
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 创建传输对象 */
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddStringToObject(json_item, "module", "system clock");
    cJSON_AddObjectToObject(json_item, "source");
    cJSON *src_item = cJSON_GetObjectItem(json_item, "source");
    sprintf(clock_string, "%d", clock.year);
    cJSON_AddStringToObject(src_item, "year", clock_string);
    sprintf(clock_string, "%d", clock.month);
    cJSON_AddStringToObject(src_item, "month", clock_string);
    sprintf(clock_string, "%d", clock.day);
    cJSON_AddStringToObject(src_item, "day", clock_string);
    sprintf(clock_string, "%d", clock.hour);
    cJSON_AddStringToObject(src_item, "hour", clock_string);
    sprintf(clock_string, "%d", clock.minute);
    cJSON_AddStringToObject(src_item, "minute", clock_string);
    sprintf(clock_string, "%d", clock.second);
    cJSON_AddStringToObject(src_item, "second", clock_string);
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
