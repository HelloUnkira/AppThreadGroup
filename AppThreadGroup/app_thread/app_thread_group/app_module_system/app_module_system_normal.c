/*实现目标:
 *    系统工作模式:正常
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_system.h"
#include "app_module_data_center.h"
#include "app_module_data_dump.h"
#include "app_module_data_load.h"

#include "app_lv_event.h"

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
        }
        /* 退出dlps */
        if (!system->dlps_status) {
            APP_SYS_LOG_WARN("dlps exit");
        }
    }
    /* 执行场景加载 */
    if (system->ctrl.normal.not_start_yet) {
        system->ctrl.normal.not_start_yet = false;
        app_lv_scene_start();
    }
    /* 系统工作中... */
    if (system->valid)
        return true;
    /* 执行场景转储 */
    if (system->ctrl.normal.not_stop_yet) {
        system->ctrl.normal.not_stop_yet = false;
        app_lv_scene_stop();
    }
    /* 系统关机转储流程 */
    if (system->ctrl.normal.not_dump_yet) {
        system->ctrl.normal.not_dump_yet = false;
        app_module_data_dump_event();
    }
    if (app_module_data_dump_not_over())
        return true;
    
    /* normal模式结束,进行系统模式切换 */
    return false;
}
