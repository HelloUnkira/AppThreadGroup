#ifndef APP_LV_MOUSEWHEEL_H
#define APP_LV_MOUSEWHEEL_H

/*@brief lvgl 鼠标滑轮初始化
 */
void app_lv_mousewheel_ready(void);

/*@brief lvgl 鼠标滑轮反初始化
 */
void app_lv_mousewheel_over(void);

/*@brief lvgl输入设备回调接口
 */
void app_lv_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/*@brief SDL输入设备回调接口
 */
void app_lv_mousewheel_handler(SDL_Event *event);

#endif
