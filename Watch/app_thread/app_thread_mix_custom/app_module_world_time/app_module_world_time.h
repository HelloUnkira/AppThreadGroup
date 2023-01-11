#ifndef APP_MODULE_WORLD_TIME_H
#define APP_MODULE_WORLD_TIME_H

typedef struct {
    const uint16_t index;        /* 城市索引 */
    const char    *name;         /* 城市代号 */
    const int32_t  zone;         /* 时区偏移(+-秒) */
    const float    latitude;     /* 经度 */
    const float    longitude;    /* 纬度 */
} app_module_world_time_t;

/*@brief     获得时区通过城市索引号
 *@param[in] index 城市索引号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_index(uint16_t index);

/*@brief     获得时区通过城市代号
 *@param[in] name 城市代号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_name(char *name);

/*@brief      转化世界协调时UTC到对应城市时间
 *@param[in]  world_time 时区静态实例
 *@param[in]  clock      时钟实例{.utc,}
 *@param[out] clock      时钟实例{...}
 */
void app_module_world_time_change(app_module_clock_t *clock, const
                                  app_module_world_time_t *world_time);

#endif
