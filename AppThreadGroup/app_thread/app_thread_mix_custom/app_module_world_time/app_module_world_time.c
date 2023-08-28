/*实现目标:
 *    世界时钟相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#define   APP_MODULE_WORLD_TIME_TABLE_H
#include "app_module_world_time_table.h"

/*@brief     世界时间设置
 *@param[in] world_time 世界时间参数
 *@param[in] idx        世界时间索引
 */
void app_module_world_time_set(app_module_world_time_t *world_time, uint8_t idx)
{
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_world_time);
    memcpy(&data_center->module_world_time.world_time[idx], world_time, sizeof(app_module_world_time_t));
    app_module_data_center_give();
}

/*@brief      世界时间获取
 *@param[out] world_time 世界时间参数
 *@param[in]  idx        世界时间索引
 */
void app_module_world_time_get(app_module_world_time_t *world_time, uint8_t idx)
{
    /* 提取数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_world_time);
    memcpy(world_time, &data_center->module_world_time.world_time[idx], sizeof(app_module_world_time_t));
    app_module_data_center_give();
}

/*@brief     获得时区通过城市索引号
 *@param[in] index 城市索引号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_index(uint16_t index)
{
    for (uint16_t idx = 0; idx < app_sys_arr_len(app_module_world_time_table); idx++)
        if (app_module_world_time_table[idx].index == index)
            return &app_module_world_time_table[idx];
    return NULL;
}

/*@brief     获得时区通过城市代号
 *@param[in] city 城市代号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_name(char *city)
{
    for (uint16_t idx = 0; idx < app_sys_arr_len(app_module_world_time_table); idx++)
        if (!strcmp(app_module_world_time_table[idx].city, city))
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
