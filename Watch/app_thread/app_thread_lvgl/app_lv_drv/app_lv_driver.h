#ifndef APP_LV_DRIVER_H
#define APP_LV_DRIVER_H

/*@brief lvgl输入设备组
 *retval 输入设备组
 */
lv_group_t *app_lv_driver_get_kb_group(void);

/*@brief lvgl输入设备组
 *retval 输入设备组
 */
lv_group_t *app_lv_driver_get_mw_group(void);

/*@brief lvgl驱动设备开启
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_ready(void);

/*@brief lvgl的驱动设备SDL回调接口
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_handler(void);

#endif
