/*实现目标:
 *    scui线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_scui_lib.h"

static app_thread_t app_thread_scui_refr = {0};

#if 0   /* draw 测试 */

static app_sys_timer_t app_thread_scui_draw_test_timer = {0};

static void app_thread_scui_draw_test_routine(scui_surface_t *surface)
{
    static uint8_t color = 0;
    static uint8_t alpha = 0;
    
    SCUI_PIXEL_TYPE pixel = scui_pixel_by_color((scui_color8888_t){.full = 0xFFFFFFFF});
    scui_draw_area_fill(surface, &surface->clip, &pixel, surface->alpha);
    
    pixel = (SCUI_PIXEL_TYPE){0};
    // pixel.ch.r = color >> 3;
    pixel.ch.g = color >> 2;
    // pixel.ch.b = color >> 3;
    scui_draw_area_fill(surface, &surface->clip, &pixel, alpha);
    
    color += 1;
    alpha += 1;
}

static void app_thread_scui_draw_test_timer_handler(void *timer)
{
    scui_surface_draw_routine(app_thread_scui_draw_test_routine);

    static uint8_t cnt = 0;
    cnt++;

    if (cnt >= 0xFF)
        app_sys_timer_stop(&app_thread_scui_draw_test_timer);
}

static void app_thread_scui_draw_test(void)
{
    app_thread_scui_draw_test_timer.expired = app_thread_scui_draw_test_timer_handler;
    app_thread_scui_draw_test_timer.peroid  = 10;
    app_thread_scui_draw_test_timer.reload  = true;
    app_sys_timer_start(&app_thread_scui_draw_test_timer);
}

#endif

/*@brief scui refr子线程
 */
static APP_THREAD_GROUP_HANDLER(app_thread_scui_refr_routine)
{
    #if 0
    app_thread_scui_draw_test();
    #endif
    
    while (true) {
        
        /*@brief scui 屏幕刷新回调接口
         */
        void app_dev_gui_disp_scui_flush(scui_surface_t *suface);
        scui_surface_refr_routine(app_dev_gui_disp_scui_flush);
    }
}

/*@brief 子线程服务例程就绪部
 */
static void app_thread_scui_routine_ready_cb(void)
{
    scui_engine_ready();
    scui_engine_execute_status_set(true);
    /* 初始化与scui绑定的驱动设备 */
    app_dev_gui_disp_ready(&app_dev_gui_disp);
    app_dev_gui_ptr_ready(&app_dev_gui_ptr);
    app_dev_gui_enc_ready(&app_dev_gui_enc);
    app_dev_gui_key_ready(&app_dev_gui_key);
    app_dev_gui_drv_ready(&app_dev_gui_drv);
    /* 模组初始化 */
    app_scui_timer_ready();
    app_scui_check_time_ready();
    /* 初始化启动scui调度定时器 */
    app_scui_timer_start();
    
    /* 创建refr子线程 */
    app_thread_group_create(&app_thread_scui, &app_thread_scui_refr, app_thread_scui_refr_routine);
    app_thread_process(&app_thread_scui_refr, app_thread_static);
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_scui_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_scui_sched: {
        /* scui时钟调度事件 */
        if (package->event == app_thread_scui_sched_exec)
            scui_engine_execute();
        /* lvgl驱动检查事件 */
        if (package->event == app_thread_scui_sched_drv) {
            app_dev_gui_drv_timer_handler(&app_dev_gui_drv);
            /*@brief scui 输入设备回调接口
             */
            void app_dev_gui_ptr_scui_read(scui_indev_data_t *indev_data);
            void app_dev_gui_enc_scui_read(scui_indev_data_t *indev_data);
            void app_dev_gui_key_scui_read(scui_indev_data_t *indev_data);
            /*  */
            scui_indev_data_t indev_data;
            /* 事件派发 */
            app_dev_gui_ptr_scui_read(&indev_data);
            scui_indev_data_notify(&indev_data);
            app_dev_gui_enc_scui_read(&indev_data);
            scui_indev_data_notify(&indev_data);
            app_dev_gui_key_scui_read(&indev_data);
            scui_indev_data_notify(&indev_data);
            /*  */
            if (app_dev_gui_drv_shutdown(&app_dev_gui_drv)) {
                static bool execute = true;
                if (execute) {
                    execute = false;
                    /* 重启系统 */
                    APP_SYS_LOG_WARN("");
                    app_module_system_dlps_set(false);
                    app_module_system_valid_set(false);
                    if (app_module_system_mode_get() != app_module_data_center_system_mode_shutdown)
                        app_module_system_mode_set(app_module_data_center_system_mode_shutdown);
                    else
                        app_arch_reset();
                }
            }
            *record = false;
        }
        /* scui场景计时检查 */
        if (package->event == app_thread_scui_sched_check_time)
            app_scui_check_time_update();
        /* 与scui绑定的驱动设备进入DLPS */
        if (package->event == app_thread_scui_sched_dlps_enter) {
            /* 进入dlps界面 */
            /*...*/
            /* 关闭设备(业务需求,不就地关闭鼠标,鼠标需要有唤醒能力) */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            // app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 与scui绑定的驱动设备退出DLPS */
        if (package->event == app_thread_scui_sched_dlps_exit) {
            /* 计时重置 */
            app_scui_check_time_reset(0, 0);
            app_scui_check_time_exec(true);
            /* 开启设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
            /* 退出dlps界面 */
            /*...*/
        }
        return true;
    }
    case app_thread_scui_ui: {
        /* 测试模式拦截该模组全部事件 */
        #if 1
        /* 禁用超时回退 */
        app_scui_check_time_reset(0, 0);
        app_scui_check_time_exec(false);
        return true;
        #endif
        
        /* 启动UI场景 */
        if (package->event == app_thread_scui_ui_scene_start) {
            app_scui_check_time_reset(0, 0);
            app_scui_check_time_exec(true);
            APP_SYS_LOG_WARN("ui scene start");
            /* 更新scui设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
        }
        /* 终止UI场景 */
        if (package->event == app_thread_scui_ui_scene_stop) {
            app_scui_check_time_reset(0, 0);
            app_scui_check_time_exec(true);
            app_module_system_dlps_set(false);
            APP_SYS_LOG_WARN("ui scene stop");
            /* 更新scui设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 进入UI场景(关机) */
        if (package->event == app_thread_scui_ui_scene_shutdown) {
            /* 禁用UI的一切交互,仅保留按压事件响应 */
            app_scui_check_time_reset(0, 0);
            app_scui_check_time_exec(false);
            APP_SYS_LOG_WARN("ui scene shutdown");
            /* 更新scui设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief scui线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_scui_routine)
{
    app_thread_slave_process(app_thread_id_scui,
                             app_thread_scui_routine_ready_cb,
                             app_thread_scui_routine_package_cb);
}
