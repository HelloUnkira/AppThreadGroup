/*实现目标:
 *    lv适配SDL模拟器
 */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "lvgl.h"

static app_mutex_t app_lv_mouse_mutex;
static bool app_lv_mouse_left_status  = false;
static bool app_lv_mouse_right_status = false;
static int16_t app_lv_mouse_pos_x = 0;
static int16_t app_lv_mouse_pos_y = 0;

/*@brief lvgl SDL鼠标初始化
 */
void app_lv_mouse_ready(void)
{
    app_mutex_process(&app_lv_mouse_mutex);
}

/*@brief lvgl输入设备回调接口
 */
void app_lv_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    app_mutex_take(&app_lv_mouse_mutex);
    (void)indev_drv;
    /* 传递给lvgl的touch事件 */
    data->point.x = app_lv_mouse_pos_x;
    data->point.y = app_lv_mouse_pos_y;
    /* 我们只用到了鼠标左键的适配功能 */
    data->state   = app_lv_mouse_left_status ?
                    LV_INDEV_STATE_PRESSED :
                    LV_INDEV_STATE_RELEASED;
    app_mutex_give(&app_lv_mouse_mutex);
}

/*@brief SDL输入设备回调接口
 */
void app_lv_mouse_handler(SDL_Event *event)
{
    app_mutex_take(&app_lv_mouse_mutex);
    switch(event->type) {
        /* 鼠标抬起事件 */
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                app_lv_mouse_left_status = false;
            break;
            if(event->button.button == SDL_BUTTON_RIGHT)
                app_lv_mouse_right_status = false;
            break;
        /* 鼠标按下事件 */
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                app_lv_mouse_left_status = true;
                app_lv_mouse_pos_x = event->motion.x / LV_DRV_ZOOM;
                app_lv_mouse_pos_y = event->motion.y / LV_DRV_ZOOM;
            }
            if(event->button.button == SDL_BUTTON_RIGHT) {
                app_lv_mouse_right_status = true;
                app_lv_mouse_pos_x = event->motion.x / LV_DRV_ZOOM;
                app_lv_mouse_pos_y = event->motion.y / LV_DRV_ZOOM;
            }
            break;
        /* 鼠标位置移动事件 */
        case SDL_MOUSEMOTION:
            app_lv_mouse_pos_x = event->motion.x / LV_DRV_ZOOM;
            app_lv_mouse_pos_y = event->motion.y / LV_DRV_ZOOM;
            break;
        /* 手指抬起事件 */
        case SDL_FINGERUP:
            app_lv_mouse_left_status  = false;
            app_lv_mouse_right_status = false;
            app_lv_mouse_pos_x = LV_HOR_RES * event->tfinger.x / LV_DRV_ZOOM;
            app_lv_mouse_pos_y = LV_VER_RES * event->tfinger.y / LV_DRV_ZOOM;
            break;
        /* 手指按下事件 */
        case SDL_FINGERDOWN:
            app_lv_mouse_left_status  = true;
            app_lv_mouse_right_status = true;
            app_lv_mouse_pos_x = LV_HOR_RES * event->tfinger.x / LV_DRV_ZOOM;
            app_lv_mouse_pos_y = LV_VER_RES * event->tfinger.y / LV_DRV_ZOOM;
            break;
        /* 手指位置移动事件 */
        case SDL_FINGERMOTION:
            app_lv_mouse_pos_x = LV_HOR_RES * event->tfinger.x / LV_DRV_ZOOM;
            app_lv_mouse_pos_y = LV_VER_RES * event->tfinger.y / LV_DRV_ZOOM;
            break;
    }
    app_mutex_give(&app_lv_mouse_mutex);
}
