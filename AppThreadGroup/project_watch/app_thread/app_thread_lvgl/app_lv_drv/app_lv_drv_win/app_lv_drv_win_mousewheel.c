/*实现目标:
 *    lv适配Win模拟器
 */

#include "app_lv_lib.h"

#if APP_LV_DRV_USE_WIN

static bool app_lv_mousewheel_status = false;
static int16_t app_lv_mousewheel_enc_diff = 0;
static lv_indev_state_t app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;
static lv_key_t app_lv_mousewheel_key = 0;

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
    data->key      = app_lv_mousewheel_key;
    app_lv_mousewheel_enc_diff = 0;
}

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_lv_mousewheel_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!app_lv_mousewheel_status)
        return 0;
    
    switch (uMsg)
    {
    /* 鼠标中键按下抬起 */
    case WM_MBUTTONDOWN:
        app_lv_mousewheel_state = LV_INDEV_STATE_PRESSED;
    case WM_MBUTTONUP:
        app_lv_mousewheel_state = LV_INDEV_STATE_RELEASED;
        return TRUE;
    case WM_MOUSEWHEEL:
        app_lv_mousewheel_enc_diff = -(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
        app_lv_mousewheel_key = LV_KEY_ENTER;
        if (app_lv_mousewheel_enc_diff > 0)
            app_lv_mousewheel_key = LV_KEY_UP;
        if (app_lv_mousewheel_enc_diff < 0)
            app_lv_mousewheel_key = LV_KEY_DOWN;
        return TRUE;
    }
    return 0;
}

#endif
