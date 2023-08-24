/*实现目标:
 *    lv适配SDL模拟器
 */

#include "app_lv_lib.h"

#if APP_LV_DRV_USE_SDL

static uint32_t app_lv_keyboard_key;
static lv_indev_state_t app_lv_keyboard_state;

/*@brief  SDL键盘事件转lvgl按键事件组
 *@retval 返回lvgl按键事件
 */
static uint32_t app_lv_keyboard_lvgl_key(void)
{
    /* 重新映射一些key到LV_KEY_…管理组 */
    switch (app_lv_keyboard_key) {
    case SDLK_RIGHT:
    case SDLK_KP_PLUS:
        return LV_KEY_RIGHT;
    case SDLK_LEFT:
    case SDLK_KP_MINUS:
        return LV_KEY_LEFT;
    case SDLK_UP:
        return LV_KEY_UP;
    case SDLK_DOWN:
        return LV_KEY_DOWN;
    case SDLK_ESCAPE:
        return LV_KEY_ESC;
    case SDLK_BACKSPACE:
        return LV_KEY_BACKSPACE;
    case SDLK_DELETE:
        return LV_KEY_DEL;
    case SDLK_KP_ENTER:
    case '\r':
        return LV_KEY_ENTER;
    case SDLK_PAGEDOWN:
        return LV_KEY_NEXT;
    case SDLK_PAGEUP:
        return LV_KEY_PREV;
    default:
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
    data->state = app_lv_keyboard_state;
    data->key   = app_lv_keyboard_lvgl_key();
}

/*@brief SDL 输入设备回调接口
 */
void app_lv_keyboard_handler(SDL_Event *event)
{
    /* 我们只关心SDL_KEYDOWN和SDL_KEYUP事件 */
    switch (event->type) {
    case SDL_KEYDOWN:
        app_lv_keyboard_key = event->key.keysym.sym;
        app_lv_keyboard_state = LV_INDEV_STATE_PRESSED;
        break;
    case SDL_KEYUP:
        app_lv_keyboard_state = LV_INDEV_STATE_RELEASED;
        break;
    default:
        break;
    }
}

#endif
