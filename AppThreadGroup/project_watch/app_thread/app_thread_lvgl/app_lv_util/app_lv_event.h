#ifndef APP_LV_EVENT_H
#define APP_LV_EVENT_H

/*@brief lvgl tick执行
 */
void app_lv_tick_exec_update(void);

/*@brief lvgl drv更新
 */
void app_lv_drv_update(void);

/*@brief lvgl wheel更新
 *@param wheel 轮盘实例
 *@param size  轮盘实例大小
 */
void app_lv_wheel_update(void *wheel, uint32_t size);

/*@brief lvgl 场景启动
 */
void app_lv_scene_start(void);

/*@brief lvgl 场景停止
 */
void app_lv_scene_stop(void);

/*@brief lvgl 场景低功耗
 */
void app_lv_scene_dlps(bool status);

/*@brief lvgl 场景停止
 */
void app_lv_scene_shutdown(void);

#endif
