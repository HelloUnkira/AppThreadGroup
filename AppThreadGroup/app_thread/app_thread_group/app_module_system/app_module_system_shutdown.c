/*实现目标:
 *    系统工作模式:关机
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_system.h"
#include "app_module_data_center.h"
#include "app_module_dump.h"
#include "app_module_load.h"

#include "app_lv_event.h"

/*@brief 关机工作模式节拍调度
 */
bool app_module_system_mode_shutdown_ctrl(app_module_clock_t clock[1], app_module_system_t *system)
{
    /* 执行场景关机 */
    if (system->ctrl.shutdown.not_shutdown_yet) {
        system->ctrl.shutdown.not_shutdown_yet = false;
        app_lv_scene_shutdown();
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    
    /* shutdown模式结束,进行系统模式切换 */
    return false;
}
