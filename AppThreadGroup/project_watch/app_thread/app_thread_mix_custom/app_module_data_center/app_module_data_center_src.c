/*实现目标:
 *    数据中心记录
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 数据中心类型与数据源映射表 */
uint32_t app_module_data_center_src_crc32 = 0;
app_module_data_center_src_t *app_module_data_center_src = NULL;
app_module_data_center_src_type_t app_module_data_center_src_type = 0;
app_module_data_center_src_table_t app_module_data_center_src_table[] = {
    {app_module_data_center_src_module_source,      "module source",        sizeof(app_module_data_center_src->module_source)},
    {app_module_data_center_src_module_weather,     "module weather",       sizeof(app_module_data_center_src->module_weather)},
    {app_module_data_center_src_module_world_time,  "module world time",    sizeof(app_module_data_center_src->module_world_time)},
    {app_module_data_center_src_remind_alarm,       "remind alarm",         sizeof(app_module_data_center_src->remind_alarm)},
    {app_module_data_center_src_remind_calendar,    "remind calendar",      sizeof(app_module_data_center_src->remind_calendar)},
    {app_module_data_center_src_remind_matter,      "remind matter",        sizeof(app_module_data_center_src->remind_matter)},
    {app_module_data_center_src_system_profile,     "system profile",       sizeof(app_module_data_center_src->system_profile)},
    {app_module_data_center_src_system_data,        "system data"   ,       sizeof(app_module_data_center_src->system_data)},
    {app_module_data_center_src_user_profile,       "user profile"  ,       sizeof(app_module_data_center_src->user_profile)},
    {app_module_data_center_src_user_data,          "user data"     ,       sizeof(app_module_data_center_src->user_data)},
};
uint32_t app_module_data_center_src_table_size = app_sys_arr_len(app_module_data_center_src_table);

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
void app_module_data_center_src_reset(void)
{
    APP_SYS_ASSERT(app_module_data_center_src != NULL);
    APP_SYS_ASSERT(app_module_data_center_src_type > app_module_data_center_src_none);
    APP_SYS_ASSERT(app_module_data_center_src_type < app_module_data_center_src_num);
    /* 求字段大小,如sizeof(app_module_data_center_src->module_source) */
    app_module_data_center_src_t *data_center_src = app_module_data_center_src;
    uint32_t data_size = app_module_data_center_src_table[app_module_data_center_src_type - 1].data_size;
    memset(&data_center_src->offset, 0, data_size);
    
    switch (app_module_data_center_src_type) {
    case app_module_data_center_src_module_source:
        app_module_clock_reset(&data_center_src->module_source.clock);
        app_module_remind_drink_reset(&data_center_src->module_source.remind_drink);
        app_module_remind_sedentary_reset(&data_center_src->module_source.remind_sedentary);
        app_module_do_not_disturb_reset(&data_center_src->module_source.do_not_disturb);
        break;
    case app_module_data_center_src_module_weather:
        break;
    case app_module_data_center_src_module_world_time:
        break;
    case app_module_data_center_src_remind_alarm:
        break;
    case app_module_data_center_src_remind_calendar:
        break;
    case app_module_data_center_src_remind_matter:
        break;
    case app_module_data_center_src_system_profile:
        break;
    case app_module_data_center_src_system_data:
        break;
    case app_module_data_center_src_user_profile:
        break;
    case app_module_data_center_src_user_data:
        break;
    default:
        break;
    }
}
