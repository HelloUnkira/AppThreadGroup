/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_WIN

/* 设备gui_ptr抽象操作参数 */
typedef struct {
    /*  */
    bool    status;
    bool    left_status;
    bool    right_status;
    bool    middle_status;
    LPARAM  value;
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
/*@brief lvgl 输入设备回调接口
 */
void app_dev_gui_ptr_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * indev_data)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    if (!cfg->status)
        return;
    
    (void)indev_drv;
    
    UINT app_dev_gui_disp_get_dpi(bool is_def);
    /* 传递给lvgl的touch事件 */
    indev_data->point.x = MulDiv(GET_X_LPARAM(cfg->value),
        app_dev_gui_disp_get_dpi(true), LV_DRV_ZOOM * app_dev_gui_disp_get_dpi(false));
    indev_data->point.y = MulDiv(GET_Y_LPARAM(cfg->value),
        app_dev_gui_disp_get_dpi(true), LV_DRV_ZOOM * app_dev_gui_disp_get_dpi(false));
    /* 我们只用到了鼠标左键的适配功能 */
    indev_data->state   = cfg->left_status ?
                          LV_INDEV_STATE_PRESSED :
                          LV_INDEV_STATE_RELEASED;
    /* 整理坐标范围 */
    if (indev_data->point.x < 0)
        indev_data->point.x = 0;
    if (indev_data->point.x > LV_DRV_HOR_RES - 1)
        indev_data->point.x = LV_DRV_HOR_RES - 1;
    if (indev_data->point.y < 0)
        indev_data->point.y = 0;
    if (indev_data->point.y > LV_DRV_VER_RES - 1)
        indev_data->point.y = LV_DRV_VER_RES - 1;
}
#elif APP_EXT_DEV_GUI_IS_SCUI
/*@brief scui 输入设备回调接口
 */
void app_dev_gui_ptr_scui_read(scui_indev_data_t *indev_data)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    if (!cfg->status)
        return;
    
    UINT app_dev_gui_disp_get_dpi(bool is_def);
    /* 传递给lvgl的touch事件 */
    indev_data->ptr.ptr_pos.x = MulDiv(GET_X_LPARAM(cfg->value),
        app_dev_gui_disp_get_dpi(true), app_dev_gui_disp_get_dpi(false));
    indev_data->ptr.ptr_pos.y = MulDiv(GET_Y_LPARAM(cfg->value),
        app_dev_gui_disp_get_dpi(true), app_dev_gui_disp_get_dpi(false));
    
    indev_data->type    = scui_indev_type_ptr;
    /* 我们只用到了鼠标左键的适配功能 */
    indev_data->state   = cfg->left_status ?
                          scui_indev_state_press :
                          scui_indev_state_release;
    /* 整理坐标范围 */
    if (indev_data->ptr.ptr_pos.x < 0)
        indev_data->ptr.ptr_pos.x = 0;
    if (indev_data->ptr.ptr_pos.x > SCUI_DRV_HOR_RES - 1)
        indev_data->ptr.ptr_pos.x = SCUI_DRV_HOR_RES - 1;
    if (indev_data->ptr.ptr_pos.y < 0)
        indev_data->ptr.ptr_pos.y = 0;
    if (indev_data->ptr.ptr_pos.y > SCUI_DRV_VER_RES - 1)
        indev_data->ptr.ptr_pos.y = SCUI_DRV_VER_RES - 1;
}
#else
#endif

/*@brief Win 输入设备接口
 */
static BOOL app_dev_gui_ptr_get_info(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    HMODULE moudle_handle = GetModuleHandleW(L"user32.dll");
    if (!moudle_handle)
        return FALSE;
    
    typedef BOOL(WINAPI* function_t)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
    
    function_t function = (function_t)(GetProcAddress(moudle_handle, "GetTouchInputInfo"));
    if (!function)
        return FALSE;
    
    return function(hTouchInput, cInputs, pInputs, cbSize);
}

/*@brief Win 输入设备接口
 */
static BOOL app_dev_gui_ptr_close(HTOUCHINPUT hTouchInput)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    HMODULE moudle_handle = GetModuleHandleW(L"user32.dll");
    if (!moudle_handle)
        return FALSE;

    typedef BOOL(WINAPI* function_t)(HTOUCHINPUT);

    function_t function = (function_t)(GetProcAddress(moudle_handle, "CloseTouchInputHandle"));
    if (!function)
        return FALSE;

    return function(hTouchInput);
}

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_dev_gui_ptr_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    app_dev_t *driver = app_dev_gui_ptr_inst();
    app_dev_gui_ptr_cfg_t *cfg = driver->cfg;
    app_dev_gui_ptr_data_t *data = driver->data;
    
    if (!cfg->status)
        return 0;
    
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        cfg->value = lParam;
        /* 鼠标左键按下抬起 */
        if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
            cfg->left_status = (uMsg == WM_LBUTTONDOWN);
        /* 鼠标右键按下抬起 */
        if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
            cfg->right_status = (uMsg == WM_RBUTTONDOWN);
        /* 鼠标中键按下抬起 */
        if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
            cfg->middle_status = (uMsg == WM_MBUTTONDOWN);
        return TRUE;
    case WM_TOUCH: {
        UINT touch_size = LOWORD(wParam);
        HTOUCHINPUT touch_handle = (HTOUCHINPUT)(lParam);
        PTOUCHINPUT touch_handle_t = app_mem_alloc(touch_size * sizeof(TOUCHINPUT));
        if (!touch_handle_t)
            break;
        POINT point = {0};
        if (app_dev_gui_ptr_get_info(touch_handle, touch_size, touch_handle_t, sizeof(TOUCHINPUT)))
            for (UINT idx = 0; idx < touch_size; idx++) {
                point.x = TOUCH_COORD_TO_PIXEL(touch_handle_t[idx].x);
                point.y = TOUCH_COORD_TO_PIXEL(touch_handle_t[idx].y);
                if (!ScreenToClient(hWnd, &point))
                    continue;
                uint16_t x = (uint16_t)(point.x & 0xffff);
                uint16_t y = (uint16_t)(point.y & 0xffff);

                DWORD MousePressedMask = TOUCHEVENTF_MOVE | TOUCHEVENTF_DOWN;

                cfg->value = (y << 16) | x;
                cfg->left_status  = (touch_handle_t[idx].dwFlags & MousePressedMask);
                cfg->right_status = (touch_handle_t[idx].dwFlags & MousePressedMask);
            }
        app_mem_free(touch_handle_t);
        app_dev_gui_ptr_close(touch_handle);
        return TRUE;
    }
    }
    return 0;
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
    .middle_status = false,
    .value = 0,
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
