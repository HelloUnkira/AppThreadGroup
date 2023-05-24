#ifndef APP_LV_DRV_SDL_MOUSEWHEEL_H
#define APP_LV_DRV_SDL_MOUSEWHEEL_H

#if APP_LV_DRV_USE_SDL

/*@brief lvgl 鼠标滑轮初始化
 */
void app_lv_mousewheel_ready(void);

/*@brief lvgl 鼠标滑轮进入低功耗
 */
void app_lv_mousewheel_dlps_enter(void);

/*@brief lvgl 鼠标滑轮退出低功耗
 */
void app_lv_mousewheel_dlps_exit(void);

#endif

#endif
