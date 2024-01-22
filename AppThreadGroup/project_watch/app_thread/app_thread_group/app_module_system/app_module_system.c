/*实现目标:
 *    系统相关的功能组件
 *    注意:这是一个事件调度的缝合层
 *         它与常规的子模组不一样
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

static app_module_system_t app_module_system = {0};
static app_mutex_t app_module_system_mutex = {0};

/*@brief 系统进出DLPS
 *@param dlps true:进入dlps;false:退出dlps
 */
void app_module_system_dlps_set(bool dlps)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    if (app_module_system.dlps_status != dlps) {
        app_module_system.dlps_exec = true;
        
        #if 0
        #elif APP_EXT_DEV_GUI_IS_LVGL
        app_lv_scene_dlps(dlps);
        #elif APP_EXT_DEV_GUI_IS_SCUI
        app_scui_scene_dlps(dlps);
        #else
        #endif
    }
    app_module_system.dlps_status = dlps;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
}

/*@brief 系统进出DLPS
 *@retval true:进入dlps;false:退出dlps
 */
bool app_module_system_dlps_get(void)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    bool dlps = app_module_system.dlps_status;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
    return dlps;
}

/*@brief 设置系统运行状态
 *@param valid 系统运行
 */
void app_module_system_valid_set(bool valid)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    app_module_system.valid = valid;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
}

/*@brief 获取系统运行状态
 *@retval 系统运行
 */
bool app_module_system_valid_get(void)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    uint8_t valid = app_module_system.valid;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
    return valid;
}

/*@brief 设置系统工作模式
 *@param mode 系统工作模式枚举量
 */
void app_module_system_mode_set(uint8_t mode)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    app_module_system.mode_bak = mode;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
    
    /* 更新系统工作模式 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_system_profile);
    #if APP_ARCH_IS_PC
    data_center_src->system_profile.system_mode = app_module_data_center_system_mode_normal;
    #else
    data_center_src->system_profile.system_mode = mode;
    #endif
    app_module_data_center_give();
}

/*@brief 获取系统工作模式
 *@retval 系统工作模式枚举量
 */
uint8_t app_module_system_mode_get(void)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    uint8_t mode = app_module_system.mode_bak;
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
    return mode;
}

/*@brief 系统状态控制更新
 *@param clock 时钟实例
 */
void app_module_system_ctrl_check(app_module_clock_t clock[1])
{
    bool app_module_system_mode_normal_ctrl(app_module_clock_t clock[1], app_module_system_t *system);
    bool app_module_system_mode_shutdown_ctrl(app_module_clock_t clock[1], app_module_system_t *system);
    
    /* 系统模式调度 */
    bool system_is_working = false;
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    switch (app_module_system.mode) {
    case app_module_data_center_system_mode_normal: {
        system_is_working = app_module_system_mode_normal_ctrl(clock, &app_module_system);
        break;
    }
    case app_module_data_center_system_mode_shutdown: {
        system_is_working = app_module_system_mode_shutdown_ctrl(clock, &app_module_system);
        break;
    }
    default:
        app_module_system.mode     = app_module_data_center_system_mode_normal;
        app_module_system.mode_bak = app_module_data_center_system_mode_normal;
        break;
    }
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
    
    /* 模式托管中 */
    if (system_is_working)
        return;
    
    /* 系统倒计时 */
    app_mutex_process(&app_module_system_mutex, app_mutex_take);
    /* 数据转储结束,重置系统 */
    app_module_system.valid = true;
    app_module_system.mode  = app_module_system.mode_bak;
    memset(&app_module_system.ctrl, 0xff, sizeof(app_module_system.ctrl));
    app_mutex_process(&app_module_system_mutex, app_mutex_give);
}

/*@brief 初始化系统模组
 */
void app_module_system_ready(void)
{
    app_mutex_process(&app_module_system_mutex, app_mutex_static);
    /* 更新系统工作模式 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_system_profile);
    app_module_system.mode = data_center_src->system_profile.system_mode;
    APP_SYS_LOG_WARN("system mode:%u", app_module_system.mode);
    if (app_module_system.mode >= app_module_data_center_system_mode_num) {
        app_module_system.mode  = app_module_data_center_system_mode_normal;
        data_center_src->system_profile.system_mode = app_module_system.mode;
    }
    app_module_data_center_give();
    /* 准备系统监管 */
    app_module_system.valid = true;
    memset(&app_module_system.ctrl, 0xff, sizeof(app_module_system.ctrl));
}
