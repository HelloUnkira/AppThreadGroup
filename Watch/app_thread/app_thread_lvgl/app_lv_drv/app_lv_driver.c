/*
 *实现目的:
 *    lvgl驱动绑定
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "app_lv_mouse.h"
#include "app_lv_mousewheel.h"
#include "app_lv_keyboard.h"
#include "app_lv_display.h"
#include "app_lv_mouse_icon.h"

static bool app_lv_driver_status = false;

/*@brief lvgl驱动设备状态
 *       内部使用: 被lvgl线程使用
 */
bool app_lv_driver_status_get(void)
{
    return app_lv_driver_status;
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
    app_lv_driver_status = true;
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
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = app_lv_display_flush;
    disp_drv.hor_res  = LV_DRV_HOR_RES;
    disp_drv.ver_res  = LV_DRV_VER_RES;
    disp_drv.antialiasing = 1;
    /* 创建显示 */
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    /* 创建显示的默认主题 */
    lv_theme_t *theme = lv_theme_default_init(disp,
                          lv_palette_main(LV_PALETTE_BLUE),
                          lv_palette_main(LV_PALETTE_RED),
                          LV_THEME_DEFAULT_DARK,
                          LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, theme);
    /* 创建资源组 */
    lv_group_t *group = lv_group_create();
    lv_group_set_default(group);
    /* 注册输入设备:鼠标,键盘,鼠标滑轮 */
    static lv_indev_drv_t mo_dev;
    static lv_indev_drv_t mw_dev;
    static lv_indev_drv_t kb_dev;
    lv_indev_drv_init(&mo_dev);
    lv_indev_drv_init(&mw_dev);
    lv_indev_drv_init(&kb_dev);
    mo_dev.type = LV_INDEV_TYPE_POINTER;
    mw_dev.type = LV_INDEV_TYPE_ENCODER;
    kb_dev.type = LV_INDEV_TYPE_KEYPAD;
    mo_dev.read_cb = app_lv_mouse_read;
    mw_dev.read_cb = app_lv_mousewheel_read;
    kb_dev.read_cb = app_lv_keyboard_read;
    lv_indev_t *mo_indev = lv_indev_drv_register(&mo_dev);
    lv_indev_t *mw_indev = lv_indev_drv_register(&mw_dev);
    lv_indev_t *kb_indev = lv_indev_drv_register(&kb_dev);
    // lv_indev_set_group(mo_indev, group);
    lv_indev_set_group(mw_indev, group);
    lv_indev_set_group(kb_indev, group);
    /* 鼠标贴图:cursor */
    lv_obj_t *mo_cur = lv_img_create(lv_scr_act());
    lv_img_set_src(mo_cur, &app_lv_mouse_icon);
    lv_indev_set_cursor(mo_indev, mo_cur);
}

/*@brief lvgl的驱动设备SDL回调接口
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_handler(void)
{
    /* 更新处理 */
    SDL_Event event = {0};
    while(SDL_PollEvent(&event)) {
        /* 更新鼠标事件回调 */
        app_lv_mouse_handler(&event);
        /* 更新鼠标滚轮事件回调 */
        app_lv_mousewheel_handler(&event);
        /* 更新键盘事件回调 */
        app_lv_keyboard_handler(&event);
        /* 更新显示事件回调 */
        app_lv_display_handler(&event);
    }
}

/*@brief lvgl的驱动设备需要关机
 *       内部使用: 被lvgl线程使用
 */
bool app_lv_driver_shutdown(void)
{
    return app_lv_display_shutdown();
}

/*@brief lvgl驱动设备进入DLPS
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_dlps_enter(void)
{
    APP_SYS_LOG_WARN("app_lv_driver_dlps_enter");
    app_lv_driver_status = false;
    app_lv_display_over();
}

/*@brief lvgl驱动设备退出DLPS
 *       内部使用: 被lvgl线程使用
 */
void app_lv_driver_dlps_exit(void)
{
    app_lv_display_ready();
    app_lv_driver_status = true;
    APP_SYS_LOG_WARN("app_lv_driver_dlps_exit");
}
