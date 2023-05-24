/*实现目标:
 *    lv适配SDL模拟器
 */
    
#include "lvgl.h"
#include "lv_drv_conf.h"
#include "app_lv_drv.h"

#if APP_LV_DRV_USE_SDL

static bool app_lv_mousewheel_status = false;
static int16_t app_lv_mousewheel_enc_diff = 0;
static lv_indev_state_t app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;

/*@brief lvgl 鼠标滑轮初始化
 */
void app_lv_mousewheel_ready(void)
{
    app_lv_mousewheel_status = true;
}

/*@brief lvgl 鼠标滑轮进入低功耗
 */
void app_lv_mousewheel_dlps_enter(void)
{
    app_lv_mousewheel_status = false;
}

/*@brief lvgl 鼠标滑轮退出低功耗
 */
void app_lv_mousewheel_dlps_exit(void)
{
    app_lv_mousewheel_status = true;
}

/*@brief lvgl 输入设备回调接口
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

/*@brief SDL 输入设备回调接口
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

#endif
