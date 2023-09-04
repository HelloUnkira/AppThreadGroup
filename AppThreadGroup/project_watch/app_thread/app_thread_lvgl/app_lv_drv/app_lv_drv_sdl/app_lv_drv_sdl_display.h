#ifndef APP_LV_DRV_SDL_DISPLAY_H
#define APP_LV_DRV_SDL_DISPLAY_H

#if APP_LV_DRV_USE_SDL

/*@brief lvgl 屏幕初始化
 */
void app_lv_display_ready(void);

/*@brief lvgl 屏幕进入低功耗
 */
void app_lv_display_dlps_enter(void);

/*@brief lvgl 屏幕退出低功耗
 */
void app_lv_display_dlps_exit(void);

/*@brief lvgl 屏幕需要关机
 */
bool app_lv_display_shutdown(void);

#endif

#endif
