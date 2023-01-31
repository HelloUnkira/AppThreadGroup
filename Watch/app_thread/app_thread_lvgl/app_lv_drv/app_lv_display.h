#ifndef APP_LV_DISPLAY_H
#define APP_LV_DISPLAY_H

/*@brief lvgl SDL屏幕初始化
 */
void app_lv_display_ready(void);

/*@brief lvgl反初始化SDL屏幕
 */
void app_lv_display_over(void);

/*@brief lvgl屏幕刷新回调接口
 */
void app_lv_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/*@brief SDL输出设备回调接口
 */
void app_lv_display_handler(SDL_Event *event);

/*@brief SDL输出设备需要关机
 */
bool app_lv_display_shutdown(void);

#endif
