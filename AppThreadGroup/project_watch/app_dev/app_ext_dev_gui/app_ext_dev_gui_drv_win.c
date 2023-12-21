/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_WIN

/* 设备gui_drv抽象操作参数 */
typedef struct {
    void *args;
} app_dev_gui_drv_cfg_t;

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_ext_dev_drv_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /*@brief Win 输入设备回调接口
     */
    HRESULT CALLBACK app_dev_gui_ptr_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT CALLBACK app_dev_gui_key_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT CALLBACK app_dev_gui_enc_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT CALLBACK app_dev_gui_disp_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    if (app_dev_gui_ptr_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_dev_gui_key_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_dev_gui_enc_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_dev_gui_disp_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

/*@brief gui_drv设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_drv_hal_ready(app_dev_t *driver)
{
    const app_dev_gui_drv_cfg_t *cfg = driver->cfg;
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
    const app_dev_gui_drv_cfg_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    #else
    #endif
    
    /*@brief Win 屏幕需要关机
     */
    bool app_dev_gui_disp_shutdown(void);
    
    /* 终止驱动调度 */
    if (app_dev_gui_disp_shutdown())
        data->shutdown = true;
    if (data->shutdown)
        return;
    
    HWND app_dev_gui_disp_get_window(void);
    /* 更新处理 */
    MSG message;
    while(true) {
        BOOL retval = PeekMessageW(&message, app_dev_gui_disp_get_window(), 0, 0, TRUE);
        if (!retval)
            break;
        /* 消息传递 */
        TranslateMessage(&message);
        DispatchMessageW(&message);
        
        if (message.message == WM_QUIT)
            data->shutdown = true;
    }
}

/*@brief 设备组终止
 *@param driver 设备实例
 *@retval true:终止;false:继续;
 */
static inline bool app_dev_gui_drv_hal_shutdown(app_dev_t *driver)
{
    const app_dev_gui_drv_cfg_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return data->shutdown;
}

/* 静态配置的设备操作参数 */
static const app_dev_gui_drv_cfg_t app_dev_gui_drv_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static app_dev_gui_drv_api_t app_dev_gui_drv_api = {
    .ready          = app_dev_gui_drv_hal_ready,
    .timer_handler  = app_dev_gui_drv_hal_timer_handler,
    .shutdown       = app_dev_gui_drv_hal_shutdown,
    /*  */
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
