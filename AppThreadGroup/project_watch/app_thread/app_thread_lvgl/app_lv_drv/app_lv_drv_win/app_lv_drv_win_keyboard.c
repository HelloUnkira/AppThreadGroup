/*实现目标:
 *    lv适配Win模拟器
 */

#include "app_lv_lib.h"

#if APP_LV_DRV_USE_WIN

static bool app_lv_keyboard_state = false;
static WPARAM app_lv_keyboard_key;

/*@brief  SDL键盘事件转lvgl按键事件组
 *@retval 返回lvgl按键事件
 */
static uint32_t app_lv_keyboard_lvgl_key(void)
{
    /* 重新映射一些key到LV_KEY_…管理组 */
    switch (app_lv_keyboard_key) {
    case VK_UP:
        return LV_KEY_UP;
    case VK_DOWN:
        return LV_KEY_DOWN;
    case VK_LEFT:
        return LV_KEY_LEFT;
    case VK_RIGHT:
        return LV_KEY_RIGHT;
    case VK_ESCAPE:
        return LV_KEY_ESC;
    case VK_DELETE:
        return LV_KEY_DEL;
    case VK_BACK:
        return LV_KEY_BACKSPACE;
    case VK_RETURN:
        return LV_KEY_ENTER;
    case VK_NEXT:
        return LV_KEY_NEXT;
    case VK_PRIOR:
        return LV_KEY_PREV;
    case VK_HOME:
        return LV_KEY_HOME;
    case VK_END:
        return LV_KEY_END;
    default:
        if (app_lv_keyboard_key >= 'A' && app_lv_keyboard_key <= 'Z')
            return app_lv_keyboard_key + 20;
        else
            return app_lv_keyboard_key;
    }
}

/*@brief lvgl 键盘初始化
 */
void app_lv_keyboard_ready(void)
{
}

/*@brief lvgl 键盘进入低功耗
 */
void app_lv_keyboard_dlps_enter(void)
{
    /* 无事可做 */
}

/*@brief lvgl 键盘退出低功耗
 */
void app_lv_keyboard_dlps_exit(void)
{
    /* 无事可做 */
}

/*@brief lvgl 输入设备回调接口
 */
void app_lv_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void)indev_drv;
    /* 传递给lvgl的key事件 */
    data->state = app_lv_keyboard_state ?
                  LV_INDEV_STATE_PRESSED :
                  LV_INDEV_STATE_RELEASED;
    data->key   = app_lv_keyboard_lvgl_key();
}

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_lv_keyboard_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
        app_lv_keyboard_state = (uMsg == WM_KEYDOWN);
        app_lv_keyboard_key   = wParam;
        return TRUE;
    }
    return 0;
}

#endif
