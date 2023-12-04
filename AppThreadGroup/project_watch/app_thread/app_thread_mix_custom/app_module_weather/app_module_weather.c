/*实现目标:
 *    喝水提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 天气设置
 *@param weather 天气参数
 *@param idx     天气索引
 */
void app_module_weather_set(app_module_weather_t *weather, uint8_t idx)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_weather);
    memcpy(&data_center_src->module_weather.weather[idx], weather, sizeof(app_module_weather_t));
    app_module_data_center_give();
}

/*@brief 天气获取
 *@param weather 天气参数
 *@param idx     天气索引
 */
void app_module_weather_get(app_module_weather_t *weather, uint8_t idx)
{
    /* 提取数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_weather);
    memcpy(weather, &data_center_src->module_weather.weather[idx], sizeof(app_module_weather_t));
    app_module_data_center_give();
}
