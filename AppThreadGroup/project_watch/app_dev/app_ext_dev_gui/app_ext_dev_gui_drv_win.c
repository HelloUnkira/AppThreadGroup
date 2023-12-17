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

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL

/*@brief lvgl 输入设备回调接口
 *@brief lvgl 屏幕刷新回调接口
 */
void app_dev_gui_ptr_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_dev_gui_key_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_dev_gui_enc_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_dev_gui_disp_lv_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void app_dev_gui_disp_lv_rounder(lv_disp_drv_t* disp_drv, lv_area_t* area);

extern lv_img_dsc_t app_dev_gui_ptr_icon;

#else
#endif

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_dev_gui_ptr_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_dev_gui_key_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_dev_gui_enc_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_dev_gui_disp_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*@brief Win 屏幕需要关机
 */
bool app_dev_gui_disp_shutdown(void);

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_ext_dev_drv_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
    
    #if LV_DRV_DBUFFER
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buffer1[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    static lv_color_t buffer2[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2,
                          LV_DRV_HOR_RES * LV_DRV_VER_RES);
    #else
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buffer1[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    static lv_color_t buffer2[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    lv_disp_draw_buf_init(&disp_buf, buffer1, NULL,
                          LV_DRV_HOR_RES * LV_DRV_VER_RES);
    #endif
    /* 创建显示设备 */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = app_dev_gui_disp_lv_flush;
    disp_drv.rounder_cb = app_dev_gui_disp_lv_rounder;
    disp_drv.hor_res    = LV_DRV_HOR_RES;
    disp_drv.ver_res    = LV_DRV_VER_RES;
    disp_drv.antialiasing = 1;
    /* 创建显示 */
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    /* 创建显示的默认主题 */
    lv_disp_set_theme(disp, lv_theme_default_init(disp,
                            lv_palette_main(LV_PALETTE_BLUE),
                            lv_palette_main(LV_PALETTE_RED),
                            LV_THEME_DEFAULT_DARK,
                            LV_FONT_DEFAULT));
    /* 创建输入设备组 */
    data->group = lv_group_create();
    /* 注册输入设备:鼠标,键盘,鼠标滑轮 */
    static lv_indev_drv_t mo_dev;
    static lv_indev_drv_t kb_dev;
    static lv_indev_drv_t mw_dev;
    lv_indev_drv_init(&mo_dev);
    lv_indev_drv_init(&kb_dev);
    lv_indev_drv_init(&mw_dev);
    mo_dev.type = LV_INDEV_TYPE_POINTER;
    kb_dev.type = LV_INDEV_TYPE_KEYPAD;
    mw_dev.type = LV_INDEV_TYPE_ENCODER;
    mo_dev.read_cb = app_dev_gui_ptr_lv_read;
    kb_dev.read_cb = app_dev_gui_key_lv_read;
    mw_dev.read_cb = app_dev_gui_enc_lv_read;
    lv_indev_t *mo_indev = lv_indev_drv_register(&mo_dev);
    lv_indev_t *kb_indev = lv_indev_drv_register(&kb_dev);
    lv_indev_t *mw_indev = lv_indev_drv_register(&mw_dev);
    lv_indev_set_group(kb_indev, data->group);
    lv_indev_set_group(mw_indev, data->group);
    lv_group_set_default(data->group);
    /* 鼠标贴图:cursor */
    lv_obj_t *mo_cur = lv_img_create(lv_scr_act());
    lv_img_set_src(mo_cur, &app_dev_gui_ptr_icon);
    lv_indev_set_cursor(mo_indev, mo_cur);
    /* 设置顶部边界 */
    lv_obj_t *top = lv_layer_top();
    lv_obj_set_style_radius(top, 45, 0);
    lv_obj_set_style_border_width(top, 1, 0);
    lv_obj_set_style_border_color(top, lv_palette_main(LV_PALETTE_GREY), 0);
    
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
    
    #else
    #endif
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

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
/*@brief lvgl 设备组实例
 *@param driver 设备实例
 *@retval lvgl组
 */
static inline lv_group_t * app_dev_gui_drv_hal_group_inst(app_dev_t *driver)
{
    const app_dev_gui_drv_cfg_t *cfg = driver->cfg;
    app_dev_gui_drv_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return data->group;
}
#else
#endif

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
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    .group_inst     = app_dev_gui_drv_hal_group_inst,
    #else
    #endif
    /*  */
};

/* 动态的设备操作数据 */
static app_dev_gui_drv_data_t app_dev_gui_drv_data = {
    /*  */
    .shutdown = false,
    /*  */
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    .group = NULL,
    #else
    #endif
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
