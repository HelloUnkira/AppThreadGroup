/*实现目标:
 *    系统相关的功能组件
 *    注意:这是一个事件调度的缝合层
 *         它与常规的子模组不一样
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_data_center.h"
#include "app_module_dump.h"
#include "app_module_load.h"

#include "app_lv_event.h"

static    bool app_module_system_dlps_exec = false;
static    bool app_module_system_dlps_status = false;
static uint8_t app_module_system_delay = 0;
static uint8_t app_module_system_status = 0;
static uint8_t app_module_system_mode = 0;
static uint8_t app_module_system_mode_bak = 0;
static app_mutex_t app_module_system_mutex = {0};

/*@brief     系统进出DLPS
 *@param[in] status true:进入dlps;false:退出dlps
 */
void app_module_system_dlps_set(bool status)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_dlps_exec = true;
    app_module_system_dlps_status = status;
    app_mutex_give(&app_module_system_mutex);
    app_lv_scene_dlps(status);
}

/*@brief  系统进出DLPS
 *@retval status true:进入dlps;false:退出dlps
 */
bool app_module_system_dlps_get(void)
{
    app_mutex_take(&app_module_system_mutex);
    bool status = app_module_system_dlps_status;
    app_mutex_give(&app_module_system_mutex);
    return status;
}

/*@brief     设置系统状态
 *@param[in] status 系统状态枚举量
 */
void app_module_system_status_set(uint8_t status)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_status = status;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief  获取系统状态
 *@retval 系统状态枚举量
 */
uint8_t app_module_system_status_get(void)
{
    app_mutex_take(&app_module_system_mutex);
    uint8_t status = app_module_system_status;
    app_mutex_give(&app_module_system_mutex);
    return status;
}

/*@brief     设置系统延时
 *@param[in] delay 系统延时秒数
 */
void app_module_system_delay_set(uint8_t delay)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_delay = delay;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief  获取系统延时
 *@retval 系统延时秒数
 */
uint8_t app_module_system_delay_get(void)
{
    app_mutex_take(&app_module_system_mutex);
    uint8_t delay = app_module_system_delay;
    app_mutex_give(&app_module_system_mutex);
    return delay;
}

/*@brief     设置系统工作模式
 *@param[in] status 系统工作模式枚举量
 */
void app_module_system_mode_set(uint8_t mode)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_mode_bak = mode;
    app_mutex_give(&app_module_system_mutex);
    
    /* 更新系统工作模式 */
    app_module_data_center_t *data_center = NULL;
    app_module_data_center_load(app_module_data_center_system_profile);
    app_module_data_center_source(&data_center);
    data_center->system_profile.system_mode = mode;
    app_module_data_center_dump();
}

/*@brief  获取系统工作模式
 *@retval 系统工作模式枚举量
 */
uint8_t app_module_system_mode_get(void)
{
    app_mutex_take(&app_module_system_mutex);
    uint8_t mode = app_module_system_mode_bak;
    app_mutex_give(&app_module_system_mutex);
    return mode;
}

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_system_ctrl_check(app_module_clock_t clock[1])
{
    static struct {
        uint8_t not_load_yet:1;
        uint8_t not_dump_yet:1;
        uint8_t not_start_yet:1;
        uint8_t not_stop_yet:1;
        uint8_t not_shutdown_yet:1;
    } ctrl_status = {
        .not_load_yet       = true,
        .not_dump_yet       = true,
        .not_start_yet      = true,
        .not_stop_yet       = true,
        .not_shutdown_yet   = true,
    };
    
    app_mutex_take(&app_module_system_mutex);
    uint8_t   mode = app_module_system_mode;
    uint8_t status = app_module_system_status;
    bool  is_valid = app_module_system_status == app_module_system_valid;
    app_mutex_give(&app_module_system_mutex);
    /* 系统开机加载流程 */
    if (ctrl_status.not_load_yet) {
        ctrl_status.not_load_yet = false;
        app_module_load_event();
    }
    if (app_module_load_not_over())
        return;
    /* 工作模式1:低功耗 */
    if (mode == app_module_system_low_power) {
        /* 在这里强行禁用一些功能或模组 */
        /* 低功耗模式也就只是功能受限的正常模式 */
    }
    /* 工作模式1:低功耗 */
    /* 工作模式2:正常 */
    if (mode == app_module_system_low_power ||
        mode == app_module_system_normal) {
        /* 仅正常模式DLPS才会有效 */
        app_mutex_take(&app_module_system_mutex);
        bool   dlps_exec = app_module_system_dlps_exec;
        bool dlps_status = app_module_system_dlps_status;
        app_module_system_dlps_exec = false;
        app_mutex_give(&app_module_system_mutex);
        /* 执行DLPS检测 */
        if (dlps_exec) {
            /* 进入dlps */
            if (dlps_status) {
                APP_SYS_LOG_WARN("dlps enter");
            }
            /* 退出dlps */
            if (!dlps_status) {
                APP_SYS_LOG_WARN("dlps exit");
            }
        }
        /* 执行场景加载 */
        if (ctrl_status.not_start_yet) {
            ctrl_status.not_start_yet = false;
            app_lv_scene_start();
        }
        /* 系统工作中... */
        if (is_valid)
            return;
        /* 执行场景转储 */
        if (ctrl_status.not_stop_yet) {
            ctrl_status.not_stop_yet = false;
            app_lv_scene_stop();
        }
    }
    /* 工作模式1:低功耗 */
    if (mode == app_module_system_low_power) {
        /* 在这里恢复禁用一些功能或模组 */
        /* 低功耗模式也就只是功能受限的正常模式 */
    }
    /* 工作模式3:关机(假关机,系统处于监控状态) */
    if (mode == app_module_system_shutdown) {
        /* 执行场景关机 */
        if (ctrl_status.not_shutdown_yet) {
            ctrl_status.not_shutdown_yet = false;
            app_lv_scene_shutdown();
        }
        /* 系统工作中... */
        if (is_valid)
            return;
    }
    /* 系统关机转储流程 */
    if (ctrl_status.not_dump_yet) {
        ctrl_status.not_dump_yet = false;
        app_module_dump_event();
    }
    if (app_module_dump_not_over())
        return;
    /* 系统倒计时 */
    app_mutex_take(&app_module_system_mutex);
    /* 数据转储结束,重置系统 */
    if (app_module_system_delay == 0) {
        app_module_system_status = app_module_system_valid;
        app_module_system_mode   = app_module_system_mode_bak;
        memset(&ctrl_status, 0xff, sizeof(ctrl_status));
    }
    if (app_module_system_delay != 0)
        app_module_system_delay--;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief 初始化系统模组
 */
void app_module_system_ready(void)
{
    app_mutex_process(&app_module_system_mutex);
    
    /* 更新系统工作模式 */
    app_module_data_center_t *data_center = NULL;
    app_module_data_center_load(app_module_data_center_system_profile);
    app_module_data_center_source(&data_center);
    app_module_system_mode = data_center->system_profile.system_mode;
    APP_SYS_LOG_WARN("system mode:%u", app_module_system_mode);
    app_module_data_center_dump();
}

/*@brief     系统1毫秒更新事件
 *           硬件时钟中断中执行
 *@param[in] count 毫秒计数器,每毫秒+1
 */
void app_module_system_1msec_update(uint32_t count)
{
    /* 线程组不在工作中,Tick是没有意义的 */
    if (app_thread_group_status_get()) {
        /* timer msec update */
        app_module_timer_1ms_update();
        /* clock source */
        if (count % 1000 == 0) {
            uint64_t utc_new = 0; /* 在此处获取RTC的UTC */
            app_module_clock_1s_update(utc_new);
        }
    }
}
