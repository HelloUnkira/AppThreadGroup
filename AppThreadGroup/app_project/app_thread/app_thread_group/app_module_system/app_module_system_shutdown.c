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
        /* 重置系统一般最简单的就是系统复位,程序重开始 */
        /* 否则需要手动对所有的状态和流程进行适配 */
        // app_arch_reset();
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    
    /* shutdown模式结束,进行系统模式切换 */
    return false;
}