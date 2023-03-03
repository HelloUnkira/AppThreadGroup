#ifndef APP_LV_KEYBOARD_H
#define APP_LV_KEYBOARD_H

/*@brief lvgl 键盘初始化
 */
void app_lv_keyboard_ready(void);

/*@brief lvgl 键盘进入低功耗
 */
void app_lv_keyboard_dlps_enter(void);

/*@brief lvgl 键盘退出低功耗
 */
void app_lv_keyboard_dlps_exit(void);

#endif
