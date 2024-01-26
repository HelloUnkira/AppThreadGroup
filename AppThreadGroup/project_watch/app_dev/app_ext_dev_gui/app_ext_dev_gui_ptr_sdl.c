/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_EXT_DEV_GUI_USE_SDL

/*  */
#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
#define APP_DEV_GUI_DRV_HOR_RES     LV_DRV_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     LV_DRV_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        LV_DRV_ZOOM
#define APP_DEV_GUI_DRV_DBUF        LV_DRV_DBUFFER
#elif APP_EXT_DEV_GUI_IS_SCUI
#define APP_DEV_GUI_DRV_HOR_RES     SCUI_DRV_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     SCUI_DRV_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        1
#define APP_DEV_GUI_DRV_DBUF        SCUI_DRV_DBUFFER
#endif

/* 设备gui_ptr抽象操作参数 */
typedef struct {
    /*  */
    bool    status;
    bool    left_status;
    bool    right_status;
    int16_t pos_x;
    int16_t pos_y;
    /*  */
} app_dev_gui_ptr_cfg_t;

/*@brief 设备适配简易转接层
 */
static inline app_dev_t * app_dev_gui_ptr_inst(void)
{
    return &app_dev_gui_ptr;
}

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
/*@brief lvgl输入设备回调接口
 */
void app_dev_gui_ptr_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * indev_data)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    if (!cfg->status)
        return;

    (void)indev_drv;
    /* 传递给lvgl的touch事件 */
    indev_data->point.x = cfg->pos_x;
    indev_data->point.y = cfg->pos_y;
    /* 我们只用到了鼠标左键的适配功能 */
    indev_data->state   = cfg->left_status ?
                          LV_INDEV_STATE_PRESSED :
                          LV_INDEV_STATE_RELEASED;
}

#else
#endif

/*@brief SDL 输入设备回调接口
 */
void app_dev_gui_ptr_msg_cb(SDL_Event *event)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    if (!cfg->status)
        return;
    
    switch (event->type) {
    /* 鼠标抬起事件 */
    case SDL_MOUSEBUTTONUP:
        if(event->button.button == SDL_BUTTON_LEFT)
            cfg->left_status = false;
        break;
        if(event->button.button == SDL_BUTTON_RIGHT)
            cfg->right_status = false;
        break;
    /* 鼠标按下事件 */
    case SDL_MOUSEBUTTONDOWN:
        if(event->button.button == SDL_BUTTON_LEFT) {
            cfg->left_status = true;
            cfg->pos_x = event->motion.x / APP_DEV_GUI_DRV_ZOOM;
            cfg->pos_y = event->motion.y / APP_DEV_GUI_DRV_ZOOM;
        }
        if(event->button.button == SDL_BUTTON_RIGHT) {
            cfg->right_status = true;
            cfg->pos_x = event->motion.x / APP_DEV_GUI_DRV_ZOOM;
            cfg->pos_y = event->motion.y / APP_DEV_GUI_DRV_ZOOM;
        }
        break;
    /* 鼠标位置移动事件 */
    case SDL_MOUSEMOTION:
        cfg->pos_x = event->motion.x / APP_DEV_GUI_DRV_ZOOM;
        cfg->pos_y = event->motion.y / APP_DEV_GUI_DRV_ZOOM;
        break;
    /* 手指抬起事件 */
    case SDL_FINGERUP:
        cfg->left_status  = false;
        cfg->right_status = false;
        cfg->pos_x = APP_DEV_GUI_DRV_HOR_RES * event->tfinger.x / APP_DEV_GUI_DRV_ZOOM;
        cfg->pos_y = APP_DEV_GUI_DRV_VER_RES * event->tfinger.y / APP_DEV_GUI_DRV_ZOOM;
        break;
    /* 手指按下事件 */
    case SDL_FINGERDOWN:
        cfg->left_status  = true;
        cfg->right_status = true;
        cfg->pos_x = APP_DEV_GUI_DRV_HOR_RES * event->tfinger.x / APP_DEV_GUI_DRV_ZOOM;
        cfg->pos_y = APP_DEV_GUI_DRV_VER_RES * event->tfinger.y / APP_DEV_GUI_DRV_ZOOM;
        break;
    /* 手指位置移动事件 */
    case SDL_FINGERMOTION:
        cfg->pos_x = APP_DEV_GUI_DRV_HOR_RES * event->tfinger.x / APP_DEV_GUI_DRV_ZOOM;
        cfg->pos_y = APP_DEV_GUI_DRV_VER_RES * event->tfinger.y / APP_DEV_GUI_DRV_ZOOM;
        break;
    }
}

/*@brief gui_ptr设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_ptr_hal_ready(app_dev_t *driver)
{
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    /*  */
    cfg->status = true;
}

/*@brief gui_ptr设备进入dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_ptr_hal_dlps_enter(app_dev_t *driver)
{
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    /*  */
    cfg->status = false;
}

/*@brief gui_ptr设备退出dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_ptr_hal_dlps_exit(app_dev_t *driver)
{
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    /*  */
    cfg->status = true;
}

/* 静态配置的设备操作参数 */
static app_dev_gui_ptr_cfg_t app_dev_gui_ptr_cfg = {
    /*  */
    .status = false,
    .left_status = false,
    .right_status = false,
    . pos_x = 0,
    . pos_y = 0,
    /*  */
};

/* 静态配置的设备操作集合 */
static const app_dev_gui_ptr_api_t app_dev_gui_ptr_api = {
    .ready          = app_dev_gui_ptr_hal_ready,
    .dlps_enter     = app_dev_gui_ptr_hal_dlps_enter,
    .dlps_exit      = app_dev_gui_ptr_hal_dlps_exit,
};

/* 动态的设备操作数据 */
static app_dev_gui_ptr_data_t app_dev_gui_ptr_data = {
    .args = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gui_ptr = {
    .name = "app_dev_gui_ptr",
    .cfg  = &app_dev_gui_ptr_cfg,
    .api  = &app_dev_gui_ptr_api,
    .data = &app_dev_gui_ptr_data,
};

#endif

