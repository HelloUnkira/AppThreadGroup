/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_EXT_DEV_GUI_USE_SDL

/* 设备gui_key抽象操作参数 */
typedef struct {
    /*  */
    uint32_t key;
    bool state;
    /*  */
} app_dev_gui_key_cfg_t;

/*@brief 设备适配简易转接层
 */
static inline app_dev_t * app_dev_gui_key_inst(void)
{
    return &app_dev_gui_key;
}

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
/*@brief SDL键盘事件转lvgl按键事件组
 *@retval 返回lvgl按键事件
 */
static uint32_t app_dev_gui_key_lv_key(void)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    /* 重新映射一些key到LV_KEY_…管理组 */
    switch (cfg->key) {
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
        return cfg->key;
    }
}

/*@brief lvgl 输入设备回调接口
 */
void app_dev_gui_key_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * indev_data)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    (void)indev_drv;
    /* 传递给lvgl的key事件 */
    indev_data->state = cfg->state ? 
                        LV_INDEV_STATE_PRESSED :
                        LV_INDEV_STATE_RELEASED;
    indev_data->key   = app_dev_gui_key_lv_key();
}
#else
#endif

/*@brief SDL 输入设备回调接口
 */
void app_dev_gui_key_msg_cb(SDL_Event *event)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    /* 我们只关心SDL_KEYDOWN和SDL_KEYUP事件 */
    switch (event->type) {
    case SDL_KEYDOWN:
        cfg->key = event->key.keysym.sym;
        cfg->state = true;
        break;
    case SDL_KEYUP:
        cfg->state = false;
        break;
    default:
        break;
    }
}

/*@brief gui_key设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_key_hal_ready(app_dev_t *driver)
{
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    /* 无事可做 */
}

/*@brief gui_key设备进入dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_key_hal_dlps_enter(app_dev_t *driver)
{
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    /* 无事可做 */
}

/*@brief gui_key设备退出dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_key_hal_dlps_exit(app_dev_t *driver)
{
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    /* 无事可做 */
}

/* 静态配置的设备操作参数 */
static app_dev_gui_key_cfg_t app_dev_gui_key_cfg = {
    .key = 0,
    .state = false,
};

/* 静态配置的设备操作集合 */
static const app_dev_gui_key_api_t app_dev_gui_key_api = {
    .ready          = app_dev_gui_key_hal_ready,
    .dlps_enter     = app_dev_gui_key_hal_dlps_enter,
    .dlps_exit      = app_dev_gui_key_hal_dlps_exit,
};

/* 动态的设备操作数据 */
static app_dev_gui_key_data_t app_dev_gui_key_data = {
    .args = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gui_key = {
    .name = "app_dev_gui_key",
    .cfg  = &app_dev_gui_key_cfg,
    .api  = &app_dev_gui_key_api,
    .data = &app_dev_gui_key_data,
};

#endif
