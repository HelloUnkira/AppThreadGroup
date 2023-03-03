#ifndef APP_LV_MOUSE_H
#define APP_LV_MOUSE_H

/*@brief lvgl 鼠标初始化
 */
void app_lv_mouse_ready(void);

/*@brief lvgl 鼠标进入低功耗
 */
void app_lv_mouse_dlps_enter(void);

/*@brief lvgl 鼠标退出低功耗
 */
void app_lv_mouse_dlps_exit(void);

#endif
