#ifndef APP_LV_DISP_H
#define APP_LV_DISP_H

/*@brief lvgl SDL屏幕初始化
 */
void app_lv_disp_ready(void);

/*@brief lvgl屏幕刷新回调接口
 */
void app_lv_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

#endif
