/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_WIN

/* 设备gui_key抽象操作参数 */
typedef struct {
    /*  */
    WPARAM key;
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
/*@brief WIN键盘事件转lvgl按键事件组
 *@retval 返回lvgl按键事件
 */
static uint32_t app_dev_gui_key_lv_key(void)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    /* 重新映射一些key到LV_KEY_…管理组 */
    switch (cfg->key) {
    case VK_UP:     return LV_KEY_UP;
    case VK_DOWN:   return LV_KEY_DOWN;
    case VK_LEFT:   return LV_KEY_LEFT;
    case VK_RIGHT:  return LV_KEY_RIGHT;
    case VK_ESCAPE: return LV_KEY_ESC;
    case VK_DELETE: return LV_KEY_DEL;
    case VK_BACK:   return LV_KEY_BACKSPACE;
    case VK_RETURN: return LV_KEY_ENTER;
    case VK_NEXT:   return LV_KEY_NEXT;
    case VK_PRIOR:  return LV_KEY_PREV;
    case VK_HOME:   return LV_KEY_HOME;
    case VK_END:    return LV_KEY_END;
    default:
        if (cfg->key >= 'A' && cfg->key <= 'Z')
            return cfg->key + 20;
        else
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
#elif APP_EXT_DEV_GUI_IS_SCUI
/*@brief WIN键盘事件转lvgl按键事件组
 *@retval 返回lvgl按键事件
 */
static uint32_t app_dev_gui_key_scui_key(void)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    /* 重新映射一些key到scui_event_key_val_t…管理组 */
    switch (cfg->key) {
    case VK_UP:     return scui_event_key_val_up;
    case VK_DOWN:   return scui_event_key_val_down;
    case VK_LEFT:   return scui_event_key_val_left;
    case VK_RIGHT:  return scui_event_key_val_right;
    case VK_ESCAPE: return scui_event_key_val_esc;
    case VK_DELETE: return scui_event_key_val_del;
    case VK_BACK:   return scui_event_key_val_backspace;
    case VK_RETURN: return scui_event_key_val_enter;
    case VK_NEXT:   return scui_event_key_val_next;
    case VK_PRIOR:  return scui_event_key_val_prev;
    case VK_HOME:   return scui_event_key_val_home;
    case VK_END:    return scui_event_key_val_end;
    default:
            return -1;
    }
}

/*@brief scui 输入设备回调接口
 */
void app_dev_gui_key_scui_read(scui_indev_data_t *indev_data)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    indev_data->type    = scui_indev_type_key;
    /* 传递给scui的key事件 */
    indev_data->state = cfg->state ?
                        scui_indev_state_press :
                        scui_indev_state_release;
    /*  */
    indev_data->key.key_id  = app_dev_gui_key_scui_key();
    indev_data->key.key_val = 0;
    /*  */
    if (cfg->state == scui_indev_state_release)
        cfg->key = -1;
}
#else
#endif

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_dev_gui_key_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    app_dev_t *driver = app_dev_gui_key_inst();
    app_dev_gui_key_cfg_t *cfg = driver->cfg;
    app_dev_gui_key_data_t *data = driver->data;
    
    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
        cfg->key   =  wParam;
        cfg->state = (uMsg == WM_KEYDOWN);
        return TRUE;
    }
    return 0;
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
