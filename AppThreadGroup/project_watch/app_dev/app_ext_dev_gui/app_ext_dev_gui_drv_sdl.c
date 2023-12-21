/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_SDL

/* 设备gui_drv抽象操作参数 */
typedef struct {
    void *args;
} app_dev_gui_drv_cfg_t;

/*@brief gui_drv设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_drv_hal_ready(app_dev_t *driver)
{
    app_dev_gui_drv_api_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    #else
    #endif
    
}

/*@brief gui_drv设备定时回调
 *@param driver 设备实例
 */
static inline void app_dev_gui_drv_hal_timer_handler(app_dev_t *driver)
{
    app_dev_gui_drv_api_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    #else
    #endif
    
    /*@brief SDL 屏幕需要关机
     */
    bool app_dev_gui_disp_shutdown(void);
    
    /* 终止驱动调度 */
    if (app_dev_gui_disp_shutdown())
        data->shutdown = true;
    if (data->shutdown)
        return;
    
    /*@brief SDL 输入设备回调接口
     */
    void app_dev_gui_ptr_msg_cb(SDL_Event *event);
    void app_dev_gui_key_msg_cb(SDL_Event *event);
    void app_dev_gui_enc_msg_cb(SDL_Event *event);
    void app_dev_gui_disp_msg_cb(SDL_Event *event);
    
    /* 更新处理 */
    SDL_Event event = {0};
    while(SDL_PollEvent(&event)) {
        /* 更新鼠标事件回调 */
        app_dev_gui_ptr_msg_cb(&event);
        /* 更新鼠标滚轮事件回调 */
        app_dev_gui_enc_msg_cb(&event);
        /* 更新键盘事件回调 */
        app_dev_gui_key_msg_cb(&event);
        /* 更新显示事件回调 */
        app_dev_gui_disp_msg_cb(&event);
    }
}

/*@brief 设备组终止
 *@param driver 设备实例
 *@retval true:终止;false:继续;
 */
static inline bool app_dev_gui_drv_hal_shutdown(app_dev_t *driver)
{
    app_dev_gui_drv_api_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return data->shutdown;
}

/* 静态配置的设备操作参数 */
static app_dev_gui_drv_api_t app_dev_gui_drv_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static app_dev_gui_drv_api_t app_dev_gui_drv_api = {
    .ready          = app_dev_gui_drv_hal_ready,
    .timer_handler  = app_dev_gui_drv_hal_timer_handler,
    .shutdown       = app_dev_gui_drv_hal_shutdown,
};

/* 动态的设备操作数据 */
static app_dev_gui_drv_data_t app_dev_gui_drv_data = {
    /*  */
    .shutdown = false,
    /*  */
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gui_drv = {
    .name = "app_dev_gui_drv",
    .cfg  = &app_dev_gui_drv_cfg,
    .api  = &app_dev_gui_drv_api,
    .data = &app_dev_gui_drv_data,
};

#endif
