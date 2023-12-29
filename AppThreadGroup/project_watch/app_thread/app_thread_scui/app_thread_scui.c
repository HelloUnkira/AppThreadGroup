/*实现目标:
 *    scui线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "scui_lib.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_scui_routine_ready_cb(void)
{
    scui_engine_ready();
    scui_engine_execute_status_set(true);
    /* 模组初始化 */
    app_scui_timer_ready();
    app_scui_check_time_ready();
    /* 初始化启动lvgl调度定时器 */
    app_scui_timer_start();
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
        #if 0
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
