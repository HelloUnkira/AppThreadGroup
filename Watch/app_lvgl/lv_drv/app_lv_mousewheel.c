/*实现目标:
 *    lv适配SDL模拟器
 */

static app_mutex_t app_lv_mousewheel_mutex;
static int16_t app_lv_mousewheel_enc_diff = 0;
static lv_indev_state_t app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;

/*@brief lvgl SDL鼠标初始化
 */
void app_lv_mousewheel_ready(void)
{
    app_mutex_process(&app_lv_mousewheel_mutex);
}

/*@brief lvgl输入设备回调接口
 */
void app_lv_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    app_mutex_take(&app_lv_mousewheel_mutex);
    (void)indev_drv;
    /* 传递给lvgl的编码器事件 */
    data->state    = app_lv_mousewheel_state;
    data->enc_diff = app_lv_mousewheel_enc_diff;
    app_lv_mousewheel_enc_diff = 0;
    app_mutex_give(&app_lv_mousewheel_mutex);
}

/*@brief SDL输入设备回调接口
 */
void app_lv_mousewheel_handler(SDL_Event *event)
{
    app_mutex_take(&app_lv_mousewheel_mutex);
    switch(event->type) {
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
    app_mutex_give(&app_lv_mousewheel_mutex);
}
