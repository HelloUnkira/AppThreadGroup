/*实现目标:
 *    系统工作模式:关机
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

/*@brief 关机工作模式节拍调度
 */
bool app_module_system_mode_shutdown_ctrl(app_module_clock_t clock[1], app_module_system_t *system)
{
    /* 执行场景关机 */
    if (system->ctrl.shutdown.not_shutdown_yet) {
        system->ctrl.shutdown.not_shutdown_yet = false;
        
        #if 0
        #elif APP_EXT_DEV_GUI_IS_LVGL
        app_lv_scene_shutdown();
        #elif APP_EXT_DEV_GUI_IS_SCUI
        app_scui_scene_shutdown();
        #else
        #endif
        
        // 关机前的最后所需要转储的日志信息
        void app_module_system_message_shutdown(void);
        app_module_system_message_shutdown();
        /* 设备模组关闭 */
        app_module_gesture_stop();
        app_dev_gesture_dlps_exec(&app_dev_gesture, true, 1);
        app_dev_gesture_irq_switch(&app_dev_gesture, false);
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    
    /* shutdown模式结束,进行系统模式切换 */
    return false;
}
