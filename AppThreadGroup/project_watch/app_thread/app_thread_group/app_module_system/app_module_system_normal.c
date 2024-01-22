/*实现目标:
 *    系统工作模式:正常
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
#include "app_lv_lib.h"
#elif APP_EXT_DEV_GUI_IS_SCUI
#include "app_scui_lib.h"
#else
#endif

/*@brief 正常工作模式节拍调度
 */
bool app_module_system_mode_normal_ctrl(app_module_clock_t clock[1], app_module_system_t *system)
{
    /* 辅助初始化:随机数种子 */
    if (system->ctrl.normal.not_srand_yet) {
        system->ctrl.normal.not_srand_yet = false;
        srand(clock[0].utc);
    }
    /* 系统开机加载流程 */
    if (system->ctrl.normal.not_load_yet) {
        system->ctrl.normal.not_load_yet = false;
        app_module_data_load_event();
    }
    if (app_module_data_load_not_over())
        return true;
    /* DLPS管控 */
    if (system->dlps_exec) {
        system->dlps_exec = false;
        /* 进入dlps */
        if (system->dlps_status) {
            APP_SYS_LOG_WARN("dlps enter");
            app_module_gesture_stop();
            app_dev_gesture_dlps_exec(&app_dev_gesture, true, 0);
            app_dev_gesture_irq_switch(&app_dev_gesture, true);
        }
        /* 退出dlps */
        if (!system->dlps_status) {
            APP_SYS_LOG_WARN("dlps exit");
            app_module_gesture_start();
            app_dev_gesture_dlps_exec(&app_dev_gesture, false, 0);
            app_dev_gesture_irq_switch(&app_dev_gesture, false);
        }
    }
    /* 执行场景加载,设备模组启动 */
    if (system->ctrl.normal.not_start_yet) {
        system->ctrl.normal.not_start_yet = false;
        
        #if 0
        #elif APP_EXT_DEV_GUI_IS_LVGL
        app_lv_scene_start();
        #elif APP_EXT_DEV_GUI_IS_SCUI
        app_scui_scene_start();
        #else
        #endif
        
        app_module_gesture_start();
        app_dev_gesture_dlps_exec(&app_dev_gesture, false, 0);
        app_dev_gesture_irq_switch(&app_dev_gesture, false);
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    /* 执行场景转储,设备模组关闭 */
    if (system->ctrl.normal.not_stop_yet) {
        system->ctrl.normal.not_stop_yet = false;
        
        #if 0
        #elif APP_EXT_DEV_GUI_IS_LVGL
        app_lv_scene_stop();
        #elif APP_EXT_DEV_GUI_IS_SCUI
        app_scui_scene_stop();
        #else
        #endif
        
        app_module_gesture_stop();
        app_dev_gesture_dlps_exec(&app_dev_gesture, true, 0);
        app_dev_gesture_irq_switch(&app_dev_gesture, true);
    }
    /* 系统关机转储流程 */
    if (system->ctrl.normal.not_dump_yet) {
        system->ctrl.normal.not_dump_yet = false;
        app_module_data_dump_event();
    }
    if (app_module_data_dump_not_over())
        return true;
    
    /* normal模式结束,进行系统模式切换 */
    /* 重置系统一般最简单的就是系统复位,程序重开始 */
    /* 否则需要手动对所有的状态和流程进行适配 */
    // app_arch_reset();
    return false;
}
