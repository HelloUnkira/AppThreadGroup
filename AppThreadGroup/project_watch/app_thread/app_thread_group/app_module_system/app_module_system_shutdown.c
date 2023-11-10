/*实现目标:
 *    系统工作模式:关机
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#include "app_lv_event.h"

/*@brief 关机工作模式节拍调度
 */
bool app_module_system_mode_shutdown_ctrl(app_module_clock_t clock[1], app_module_system_t *system)
{
    /* 执行场景关机 */
    if (system->ctrl.shutdown.not_shutdown_yet) {
        system->ctrl.shutdown.not_shutdown_yet = false;
        app_lv_scene_shutdown();
        // 关机前的最后所需要转储的日志信息
        void app_module_system_message_shutdown(void);
        app_module_system_message_shutdown();
        /* 设备模组关闭 */
        app_module_fusion_analysis_stop();
        app_dev_gesture_dlps_exec(&app_dev_gesture, true, 1);
        app_dev_gesture_irq_switch(&app_dev_gesture, false);
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    
    /* shutdown模式结束,进行系统模式切换 */
    return false;
}
