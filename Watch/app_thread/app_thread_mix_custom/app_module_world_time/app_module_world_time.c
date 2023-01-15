/*实现目标:
 *    世界时钟相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_clock.h"
#include "app_module_world_time.h"
#define   APP_MODULE_WORLD_TIME_TABLE_H
#include "app_module_world_time_table.h"

static const uint32_t app_module_world_time_table_size = 
               sizeof(app_module_world_time_table) /
               sizeof(app_module_world_time_table[0]);

/*@brief     获得时区通过城市索引号
 *@param[in] index 城市索引号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_index(uint16_t index)
{
    for (uint16_t idx = 0; idx < app_module_world_time_table_size; idx++)
        if (app_module_world_time_table[idx].index == index)
            return &app_module_world_time_table[idx];
    return NULL;
}

/*@brief     获得时区通过城市代号
 *@param[in] name 城市代号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_name(char *name)
{
    for (uint16_t idx = 0; idx < app_module_world_time_table_size; idx++)
        if (!strcmp(app_module_world_time_table[idx].name, name))
            return &app_module_world_time_table[idx];
    return NULL;
}

/*@brief      转化世界协调时UTC到对应城市时间
 *@param[in]  world_time 时区静态实例
 *@param[in]  clock      时钟实例{.utc,}
 *@param[out] clock      时钟实例{...}
 */
void app_module_world_time_change(app_module_clock_t *clock, const
                                  app_module_world_time_t *world_time)
{
    clock->utc += world_time->zone;
    app_module_clock_to_dtime(clock);
    app_module_clock_to_week(clock);
}
