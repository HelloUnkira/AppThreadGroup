/*实现目标:
 *    lv适配SDL模拟器
 */
    
#include "lvgl.h"
#include "lv_drv_conf.h"
#include <SDL2/SDL.h>

static bool app_lv_mousewheel_status = false;
static int16_t app_lv_mousewheel_enc_diff = 0;
static lv_indev_state_t app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;

/*@brief lvgl 鼠标滑轮初始化
 */
void app_lv_mousewheel_ready(void)
{
    app_lv_mousewheel_status = true;
}

/*@brief lvgl 鼠标滑轮反初始化
 */
void app_lv_mousewheel_over(void)
{
    app_lv_mousewheel_status = false;
}

/*@brief lvgl输入设备回调接口
 */
void app_lv_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    if (!app_lv_mousewheel_status)
        return;
    
    (void)indev_drv;
    /* 传递给lvgl的编码器事件 */
    data->state    = app_lv_mousewheel_state;
    data->enc_diff = app_lv_mousewheel_enc_diff;
    app_lv_mousewheel_enc_diff = 0;
}

/*@brief SDL输入设备回调接口
 */
void app_lv_mousewheel_handler(SDL_Event *event)
{
    if (!app_lv_mousewheel_status)
        return;
    
    switch (event->type) {
    /* 滚轮滑动事件 */
    case SDL_MOUSEWHEEL:
    #ifdef __EMSCRIPTEN__
        if(event->wheel.y < 0) app_lv_mousewheel_enc_diff++;
        if(event->wheel.y > 0) app_lv_mousewheel_enc_diff--;
    #else
        app_lv_mousewheel_enc_diff -= event->wheel.y;
    #endif
        break;
    /* 滚轮按下事件 */
    case SDL_MOUSEBUTTONDOWN:
        if(event->button.button == SDL_BUTTON_MIDDLE)
            app_lv_mousewheel_state = LV_INDEV_STATE_PRESSED;
        break;
    /* 滚轮抬起事件 */
    case SDL_MOUSEBUTTONUP:
        if(event->button.button == SDL_BUTTON_MIDDLE)
            app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;
        break;
    default:
        break;
    }
}
