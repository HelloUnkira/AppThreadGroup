#ifndef APP_LV_MOUSE_H
#define APP_LV_MOUSE_H

/*@brief lvgl SDL鼠标初始化
 */
void app_lv_mouse_ready(void);

/*@brief lvgl输入设备回调接口
 */
void app_lv_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/*@brief SDL输入设备回调接口
 */
void app_lv_mouse_handler(SDL_Event *event);

#endif
