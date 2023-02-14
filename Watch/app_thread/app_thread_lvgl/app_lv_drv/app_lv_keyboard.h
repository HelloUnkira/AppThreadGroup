#ifndef APP_LV_KEYBOARD_H
#define APP_LV_KEYBOARD_H

/*@brief lvgl SDL键盘初始化
 */
void app_lv_keyboard_ready(void);

/*@brief lvgl 键盘初始化
 */
void app_lv_keyboard_over(void);

/*@brief lvgl输入设备回调接口
 */
void app_lv_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/*@brief SDL输入设备回调接口
 */
void app_lv_keyboard_handler(SDL_Event *event);

#endif
