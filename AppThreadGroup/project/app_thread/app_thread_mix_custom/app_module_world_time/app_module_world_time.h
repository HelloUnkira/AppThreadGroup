#ifndef APP_MODULE_WORLD_TIME_H
#define APP_MODULE_WORLD_TIME_H

#define APP_MODULE_WORLD_TIME_MAX           (10)
#define APP_MODULE_WORLD_TIME_CITY_MAX      (75)

typedef struct {
    uint16_t index;       /* 城市索引 */
    int32_t  zone;        /* 时区偏移(+-秒) */
    int16_t  latitude;    /* 经度(保留2位小数扩大100倍传输(将分转换成度))(东经:+1;西经:-1;) */
    int16_t  longitude;   /* 纬度(保留2位小数扩大100倍传输(将分转换成度))(东经:+1;西经:-1;) */
    char     city[APP_MODULE_WORLD_TIME_CITY_MAX];
} app_module_world_time_t;

/*@brief     世界时间设置
 *@param[in] world_time 世界时间参数
 *@param[in] idx        世界时间索引
 */
void app_module_world_time_set(app_module_world_time_t *world_time, uint8_t idx);

/*@brief      世界时间获取
 *@param[out] world_time 世界时间参数
 *@param[in]  idx        世界时间索引
 */
void app_module_world_time_get(app_module_world_time_t *world_time, uint8_t idx);

/*@brief     获得时区通过城市索引号
 *@param[in] index 城市索引号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_index(uint16_t index);

/*@brief     获得时区通过城市代号
 *@param[in] city 城市代号
 *@retval    时区静态实例
 */
const app_module_world_time_t * app_module_world_time_get_by_name(char *city);

/*@brief      转化世界协调时UTC到对应城市时间
 *@param[in]  world_time 时区静态实例
 *@param[in]  clock      时钟实例{.utc,}
 *@param[out] clock      时钟实例{...}
 */
void app_module_world_time_change(app_module_clock_t *clock, const
                                  app_module_world_time_t *world_time);

#endif
