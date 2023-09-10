#ifndef APP_LV_WHEEL_SCENE_H
#define APP_LV_WHEEL_SCENE_H

/*@brief     轮盘显示(激活)
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_show(app_lv_wheel_t *wheel);

/*@brief     轮盘隐藏(失活)
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_hide(app_lv_wheel_t *wheel);

/*@brief     轮盘子场景状态复位
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_reset(app_lv_wheel_t *wheel);

#endif
