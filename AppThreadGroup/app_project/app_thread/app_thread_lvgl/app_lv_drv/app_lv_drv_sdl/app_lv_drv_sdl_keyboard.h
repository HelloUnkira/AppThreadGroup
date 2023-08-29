#ifndef APP_LV_DRV_SDL_KEYBOARD_H
#define APP_LV_DRV_SDL_KEYBOARD_H

#if APP_LV_DRV_USE_SDL

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

#endif
