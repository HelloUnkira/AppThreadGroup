#ifndef APP_LV_DRIVER_H
#define APP_LV_DRIVER_H

/*@brief lvgl驱动设备状态
 *       内部使用: 被lvgl线程使用
 */
bool app_lv_driver_status_get(void);

/*@brief lvgl驱动设备开启
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_ready(void);

/*@brief lvgl的驱动设备SDL回调接口
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_handler(void);

/*@brief lvgl的驱动设备需要关机
 *       内部使用: 被lvgl线程使用
 */
bool app_lv_driver_shutdown(void);

/*@brief lvgl驱动设备进入DLPS
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_dlps_enter(void);

/*@brief lvgl驱动设备退出DLPS
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_dlps_exit(void);

#endif
