#ifndef APP_LV_DRIVER_H
#define APP_LV_DRIVER_H

/*@brief lvgl驱动设备开启
 */
void app_lv_driver_ready(void);

/*@brief lvgl的驱动设备SDL回调接口
 */
void app_lv_driver_handler(void);

/*@brief lvgl的驱动设备需要关机
 */
bool app_lv_driver_shutdown(void);

/*@brief lvgl驱动设备关闭
 */
void app_lv_driver_over(void);

#endif
