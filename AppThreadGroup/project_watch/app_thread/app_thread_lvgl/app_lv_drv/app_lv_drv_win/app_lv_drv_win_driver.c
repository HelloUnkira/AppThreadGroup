/*实现目标:
 *    lvgl驱动绑定
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

#if APP_LV_DRV_USE_WIN

static bool app_lv_driver_shutdown = false;
static lv_group_t * app_lv_driver_group = NULL;

/*@brief lvgl 输入设备回调接口
 *@brief lvgl 屏幕刷新回调接口
 */
void app_lv_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_lv_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_lv_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void app_lv_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void app_lv_display_rounder(lv_disp_drv_t* disp_drv, lv_area_t* area);

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_lv_mouse_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_lv_keyboard_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_lv_mousewheel_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT CALLBACK app_lv_display_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*@brief Win 输入设备回调接口
 */
HRESULT CALLBACK app_lv_driver_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (app_lv_mouse_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_lv_keyboard_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_lv_mousewheel_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    if (app_lv_display_msg_cb(hWnd, uMsg, wParam, lParam) == TRUE)
        return 0;
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

/*@brief lvgl输入设备组
 *retval 输入设备组
 */
lv_group_t * app_lv_driver_get_group(void)
{
    return app_lv_driver_group;
}

/*@brief lvgl驱动设备开启
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_ready(void)
{
    /* 初始化模拟器 */
    app_lv_mouse_ready();
    app_lv_mousewheel_ready();
    app_lv_keyboard_ready();
    app_lv_display_ready();
    /* 创建显示缓冲区 */
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
    disp_drv.flush_cb   = app_lv_display_flush;
    disp_drv.rounder_cb = app_lv_display_rounder;
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
    app_lv_driver_group = lv_group_create();
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
    mo_dev.read_cb = app_lv_mouse_read;
    kb_dev.read_cb = app_lv_keyboard_read;
    mw_dev.read_cb = app_lv_mousewheel_read;
    lv_indev_t *mo_indev = lv_indev_drv_register(&mo_dev);
    lv_indev_t *kb_indev = lv_indev_drv_register(&kb_dev);
    lv_indev_t *mw_indev = lv_indev_drv_register(&mw_dev);
    lv_indev_set_group(kb_indev, app_lv_driver_group);
    lv_indev_set_group(mw_indev, app_lv_driver_group);
    lv_group_set_default(app_lv_driver_group);
    /* 鼠标贴图:cursor */
    lv_obj_t *mo_cur = lv_img_create(lv_scr_act());
    lv_img_set_src(mo_cur, &app_lv_mouse_icon);
    lv_indev_set_cursor(mo_indev, mo_cur);
    /* 设置顶部边界 */
    lv_obj_t *top = lv_layer_top();
    lv_obj_set_style_radius(top, 45, 0);
    lv_obj_set_style_border_width(top, 1, 0);
    lv_obj_set_style_border_color(top, lv_palette_main(LV_PALETTE_GREY), 0);
}

/*@brief lvgl的驱动设备Win回调接口
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_handler(void)
{
    if (app_lv_driver_shutdown)
        return;
    if (app_lv_display_shutdown()) {
        app_lv_driver_shutdown = true;
        /* 重启系统 */
        APP_SYS_LOG_WARN("");
        if (app_module_system_dlps_get())
            app_module_system_dlps_set(false);
            app_module_system_valid_set(false);
        if (app_module_system_mode_get() != app_module_data_center_system_mode_shutdown)
            app_module_system_mode_set(app_module_data_center_system_mode_shutdown);
        else
            app_arch_reset();
    }
    
    HWND app_lv_display_get_window(void);
    /* 更新处理 */
    MSG message;
    while(true) {
        BOOL retval = PeekMessageW(&message, app_lv_display_get_window(), 0, 0, TRUE);
        if (!retval)
            break;
        /* 消息传递 */
        TranslateMessage(&message);
        DispatchMessageW(&message);
        
        if (message.message == WM_QUIT)
            app_lv_driver_shutdown = true;
    }
}

#endif
