#ifndef APP_LV_DRV_SDL_MOUSE_H
#define APP_LV_DRV_SDL_MOUSE_H

#if APP_LV_DRV_USE_SDL

/*@brief lvgl 鼠标初始化
 */
void app_lv_mouse_ready(void);

/*@brief lvgl 鼠标进入低功耗
 */
void app_lv_mouse_dlps_enter(void);

/*@brief lvgl 鼠标退出低功耗
 */
void app_lv_mouse_dlps_exit(void);

/* 鼠标图标:在lv_layer_sys层 */
extern lv_img_dsc_t app_lv_mouse_icon;

#endif

#endif
