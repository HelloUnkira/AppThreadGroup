/*实现目标:
 *    scui线程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"
#include "app_scui_lib.h"

#if 0   /* draw 测试 */

static void app_thread_scui_draw_test_routine(scui_surface_t *surface)
{
    #if 0
    
    static uint8_t color = 0;
    static uint8_t alpha = 0;
    
    scui_color_t pixel = {.full = 0xFFFFFFFF};
    scui_draw_area_fill(surface, &surface->clip, &pixel, surface->alpha);
    
    pixel = (SCUI_PIXEL_TYPE){0};
    // pixel.ch.r = color;
    pixel.ch.g = color;
    // pixel.ch.b = color;
    scui_draw_area_fill(surface, &surface->clip, &pixel, alpha);
    
    color += 1;
    alpha += 1;
    
    #else
    
    static uint8_t count = 0;
    
    scui_image_unit_t image_unit = {0};
    scui_handle_t handle = scui_image_prj_image_src_00_theme_02_ringbmp;
    image_unit.image = scui_handle_get(handle);
    scui_area_t src_clip = {
        .w = image_unit.image->pixel.width,
        .h = image_unit.image->pixel.height,
    };
    scui_area_t dst_clip = {
        .x = (surface->clip.w - src_clip.w) / 2,
        .y = (surface->clip.h - src_clip.h) / 2,
        .w = src_clip.w,
        .h = src_clip.h,
    };
    scui_color_t color = {0};
    scui_alpha_t alpha = 0xFF;
    
    /* 注意:半透明效果会反复叠加,先全屏刷新保持帧缓冲一致性 */
    scui_color_t pixel = {.full = 0xFFFFFFFF};
    scui_draw_area_fill(surface, &surface->clip, &pixel, surface->alpha);
    
    scui_image_cache_load(&image_unit);
    scui_draw_image(surface, &dst_clip, &image_unit, &src_clip, color, alpha);
    scui_image_cache_unload(&image_unit);
    
    #endif
}

static void app_thread_scui_draw_test_anima_prepare(void *anima)
{
    APP_SYS_LOG_WARN("");
}

static void app_thread_scui_draw_test_anima_expired(void *anima)
{
    scui_frame_buffer_refr_toggle(app_thread_scui_draw_test_routine);
    // APP_SYS_LOG_WARN("");
}

static void app_thread_scui_draw_test_anima_finish(void *anima)
{
    APP_SYS_LOG_WARN("");
}

static void app_thread_scui_draw_test(void)
{
    scui_anima_t anima = {
        .prepare = app_thread_scui_draw_test_anima_prepare,
        .expired = app_thread_scui_draw_test_anima_expired,
        .finish  = app_thread_scui_draw_test_anima_finish,
        .reload  = 0xFF,
        .peroid  = SCUI_ANIMA_TICK,
    };
    static scui_handle_t anima_handle = SCUI_HANDLE_INVALID;
    scui_anima_create(&anima, &anima_handle);
    scui_anima_start(anima_handle);
}

#else

static void app_thread_scui_draw_test(void)
{
}

#endif

/*@brief scui refr子线程
 */
static APP_THREAD_GROUP_HANDLER(app_thread_scui_refr_routine)
{
    app_thread_scui_draw_test();
    
    while (true) {
        /*@brief scui 屏幕刷新回调接口
         */
        #if APP_EXT_DEV_GUI_IS_SCUI
        void app_dev_gui_disp_scui_flush(scui_surface_t *suface);
        scui_frame_buffer_refr_routine(app_dev_gui_disp_scui_flush);
        #endif
    }
}

/*@brief scui draw子线程
 */
static APP_THREAD_GROUP_HANDLER(app_thread_scui_draw_routine)
{
    while (true) {
        scui_engine_wait();
        scui_engine_execute();
    }
}

/*@brief 子线程服务例程就绪部
 */
static void app_thread_scui_routine_ready_cb(void)
{
    /* 初始化与scui绑定的驱动设备 */
    app_dev_gui_disp_ready(&app_dev_gui_disp);
    app_dev_gui_ptr_ready(&app_dev_gui_ptr);
    app_dev_gui_enc_ready(&app_dev_gui_enc);
    app_dev_gui_key_ready(&app_dev_gui_key);
    app_dev_gui_drv_ready(&app_dev_gui_drv);
    /* 模组初始化 */
    app_scui_timer_ready();
    app_scui_check_time_ready();
    /* 初始化scui */
    scui_ready();
    /* 初始化启动scui调度定时器 */
    app_scui_timer_start();
    /* 创建refr子线程 */
    static app_thread_t app_thread_scui_refr = {0};
    app_thread_group_create(&app_thread_scui, &app_thread_scui_refr, app_thread_scui_refr_routine);
    app_thread_process(&app_thread_scui_refr, app_thread_static);
    /* 创建draw子线程 */
    static app_thread_t app_thread_scui_draw = {0};
    app_thread_group_create(&app_thread_scui, &app_thread_scui_draw, app_thread_scui_draw_routine);
    app_thread_process(&app_thread_scui_draw, app_thread_static);
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_scui_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_scui_sched: {
        /* scui时钟调度事件 */
        if (package->event == app_thread_scui_sched_exec) {
            /* scui默认是事件调度,而不是时钟调度 */
            // scui_engine_execute();
        }
        /* lvgl驱动检查事件 */
        if (package->event == app_thread_scui_sched_drv) {
            app_dev_gui_drv_timer_handler(&app_dev_gui_drv);
            #if APP_EXT_DEV_GUI_IS_SCUI
            /*@brief scui 输入设备回调接口
             */
            void app_dev_gui_ptr_scui_read(scui_indev_data_t *indev_data);
            void app_dev_gui_enc_scui_read(scui_indev_data_t *indev_data);
            void app_dev_gui_key_scui_read(scui_indev_data_t *indev_data);
            /*  */
            scui_indev_data_t indev_data;
            /* 事件派发 */
            app_dev_gui_ptr_scui_read(&indev_data);
            scui_indev_notify(&indev_data);
            app_dev_gui_enc_scui_read(&indev_data);
            scui_indev_notify(&indev_data);
            app_dev_gui_key_scui_read(&indev_data);
            scui_indev_notify(&indev_data);
            #endif
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
