/*
 *实现目的:
 *    lvgl线程
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_os_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "app_lv_mouse.h"
#include "app_lv_mousewheel.h"
#include "app_lv_keyboard.h"
#include "app_lv_display.h"
#include "app_lv_mouse_icon.h"

/*@brief lvgl线程模组初始化
 */
void app_thread_lvgl_ready(void)
{
    /* 初始化框架 */
    lv_init();
    /* 初始化模拟器 */
    app_lv_mouse_ready();
    app_lv_mousewheel_ready();
    app_lv_keyboard_ready();
    app_lv_display_ready();
    /* 创建显示缓冲区 */
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buffer1[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    static lv_color_t buffer2[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2,
                          LV_DRV_HOR_RES * LV_DRV_VER_RES);
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
    static lv_indev_drv_t mouse;
    static lv_indev_drv_t mousewheel;
    static lv_indev_drv_t keyboard;
    lv_indev_drv_init(&mouse);
    lv_indev_drv_init(&mousewheel);
    lv_indev_drv_init(&keyboard);
    mouse.type      = LV_INDEV_TYPE_POINTER;
    mousewheel.type = LV_INDEV_TYPE_ENCODER;
    keyboard.type   = LV_INDEV_TYPE_KEYPAD;
    mouse.read_cb       = app_lv_mouse_read;
    mousewheel.read_cb  = app_lv_mousewheel_read;
    keyboard.read_cb    = app_lv_keyboard_read;
    lv_indev_t *mo_indev = lv_indev_drv_register(&mouse);
    lv_indev_t *kb_indev = lv_indev_drv_register(&keyboard);
    lv_indev_t *mw_indev = lv_indev_drv_register(&mousewheel);
    lv_indev_set_group(mo_indev, group);
    lv_indev_set_group(kb_indev, group);
    lv_indev_set_group(mw_indev, group);
    /* 鼠标贴图:cursor */
    lv_obj_t *mc_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(mc_obj, &app_lv_mouse_icon);
    lv_indev_set_cursor(mo_indev, mc_obj);
    
}

/*@brief lvgl线程服务例程
 */
void app_thread_lvgl_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_pipe_t *pipe = NULL;
    app_package_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_lvgl, &sem);
    app_thread_get_pipe_by_id(app_thread_id_lvgl, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_OS_LOG_WARN("thread lvgl recv too much package:%u\n",
                            app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_lvgl_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
                break;
            }
            case app_thread_lvgl_sched: {
                /* lvgl内部时钟约减事件 */
                if (package.event == app_thread_lvgl_sched_reduce) {
                    lv_tick_inc(LV_SCHED_TICK_REDUCE);
                    /* lvgl时钟事件调度 */
                    lv_timer_handler();
                }
                /* sdl检查事件 */
                if (package.event == app_thread_lvgl_sched_sdl) {
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
                    if (app_lv_display_shutdown()) {
                        static bool app_lv_shutdown = false;
                        if (app_lv_shutdown)
                            return;
                        APP_OS_LOG_WARN("app_lv_shutdown\n");
                        app_lv_shutdown = true;
                        app_lv_display_over();
                        /* 重启系统 */
                        app_module_system_delay_set(2);
                        app_module_system_status_set(app_module_system_reset);
                    }
                }
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_OS_LOG_ERROR("\n");
                APP_OS_LOG_ERROR("thread lvgl pipe recv a unknown package\n");
                APP_OS_LOG_ERROR("package send_tid:%u\n", package.send_tid);
                APP_OS_LOG_ERROR("package recv_tid:%u\n", package.recv_tid);
                APP_OS_LOG_ERROR("package module:%u\n",   package.module);
                APP_OS_LOG_ERROR("package event:%u\n",    package.event);
                APP_OS_LOG_ERROR("package data:%p\n",     package.data);
                APP_OS_LOG_ERROR("package size:%u\n",     package.size);
                APP_OS_LOG_ERROR("\n");
                #endif
                break;
            }
            }
        }
    }
}
