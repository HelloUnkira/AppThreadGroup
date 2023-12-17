/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_SDL

/* 设备gui_enc抽象操作参数 */
typedef struct {
    /*  */
    bool    status;
    int16_t enc_diff;
    bool    state;
    /*  */
} app_dev_gui_enc_cfg_t;

/*@brief 设备适配简易转接层
 */
static inline app_dev_t * app_dev_gui_enc_inst(void)
{
    return &app_dev_gui_enc;
}

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
/*@brief lvgl 输入设备回调接口
 */
void app_dev_gui_enc_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * indev_data)
{
    app_dev_t *driver = app_dev_gui_enc_inst();
    app_dev_gui_enc_cfg_t *cfg = driver->cfg;
    app_dev_gui_enc_data_t *data = driver->data;
    
    if (!cfg->status)
        return;
    
    (void)indev_drv;
    
    lv_key_t key = LV_KEY_ENTER;
    if (cfg->enc_diff > 0)
        key = LV_KEY_LEFT;
    if (cfg->enc_diff < 0)
        key = LV_KEY_RIGHT;
    /* 传递给lvgl的编码器事件 */
    indev_data->state    = cfg->state;
    indev_data->enc_diff = cfg->enc_diff;
    indev_data->key      = key;
    cfg->enc_diff = 0;
}
#else
#endif

/*@brief SDL 输入设备回调接口
 */
void app_dev_gui_enc_msg_cb(SDL_Event *event)
{
    app_dev_t *driver = app_dev_gui_enc_inst();
    app_dev_gui_enc_cfg_t *cfg = driver->cfg;
    app_dev_gui_enc_data_t *data = driver->data;
    
    if (!cfg->status)
        return;
    
    switch (event->type) {
    /* 滚轮滑动事件 */
    case SDL_MOUSEWHEEL:
    #ifdef __EMSCRIPTEN__
        if(event->wheel.y < 0) cfg->enc_diff++;
        if(event->wheel.y > 0) cfg->enc_diff--;
    #else
        cfg->enc_diff -= event->wheel.y;
    #endif
        break;
    /* 滚轮按下事件 */
    case SDL_MOUSEBUTTONDOWN:
        if(event->button.button == SDL_BUTTON_MIDDLE)
            cfg->state = LV_INDEV_STATE_PRESSED;
        break;
    /* 滚轮抬起事件 */
    case SDL_MOUSEBUTTONUP:
        if(event->button.button == SDL_BUTTON_MIDDLE)
            cfg->state = LV_INDEV_STATE_RELEASED;
        break;
    default:
        break;
    }
}

/*@brief gui_enc设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_hal_ready(app_dev_t *driver)
{
    app_dev_gui_enc_cfg_t *cfg = driver->cfg;
    app_dev_gui_enc_data_t *data = driver->data;
    /*  */
    cfg->status = true;
}

/*@brief gui_enc设备进入dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_hal_dlps_enter(app_dev_t *driver)
{
    app_dev_gui_enc_cfg_t *cfg = driver->cfg;
    app_dev_gui_enc_data_t *data = driver->data;
    /*  */
    cfg->status = false;
}

/*@brief gui_enc设备退出dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_hal_dlps_exit(app_dev_t *driver)
{
    app_dev_gui_enc_cfg_t *cfg = driver->cfg;
    app_dev_gui_enc_data_t *data = driver->data;
    /*  */
    cfg->status = true;
}

/* 静态配置的设备操作参数 */
static app_dev_gui_enc_cfg_t app_dev_gui_enc_cfg = {
    /*  */
    .status     = false,
    .enc_diff   = 0,
    .state      = false,
    /*  */
};

/* 静态配置的设备操作集合 */
static const app_dev_gui_enc_api_t app_dev_gui_enc_api = {
    .ready          = app_dev_gui_enc_hal_ready,
    .dlps_enter     = app_dev_gui_enc_hal_dlps_enter,
    .dlps_exit      = app_dev_gui_enc_hal_dlps_exit,
};

/* 动态的设备操作数据 */
static app_dev_gui_enc_data_t app_dev_gui_enc_data = {
    .args = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gui_enc = {
    .name = "app_dev_gui_enc",
    .cfg  = &app_dev_gui_enc_cfg,
    .api  = &app_dev_gui_enc_api,
    .data = &app_dev_gui_enc_data,
};

#endif
