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
#include "app_thread_data_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_dump.h"
#include "app_module_load.h"
#include "app_lv_event.h"

static    bool app_module_system_dlps_exec = false;
static    bool app_module_system_dlps_status = false;
static uint8_t app_module_system_delay = 0;
static uint8_t app_module_system_status = 0;
static app_mutex_t app_module_system_mutex = {0};

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

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_system_ctrl_check(app_module_clock_t clock[1])
{
    app_mutex_take(&app_module_system_mutex);
    uint8_t status = app_module_system_status;
    bool  is_valid = app_module_system_status == app_module_system_valid;
    app_mutex_give(&app_module_system_mutex);
    /* 系统开机加载流程 */
    app_module_load_event();
    if (app_module_load_not_over())
        return;
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
    app_lv_scene_start();
    if (is_valid)
        return;
    /* 执行场景转储 */
    app_lv_scene_stop();
    /* 系统关机转储流程 */
    app_module_dump_event();
    if (app_module_dump_not_over())
        return;
    /* 系统倒计时 */
    app_mutex_take(&app_module_system_mutex);
    if (app_module_system_delay == 0)
        /* 数据转储结束,重启系统 */
        app_os_reset();
        app_module_system_delay--;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief 初始化系统模组
 */
void app_module_system_ready(void)
{
    app_mutex_process(&app_module_system_mutex);
}

/*@brief     系统1毫秒更新事件
 *           硬件时钟中断或软件定时器中执行
 *@param[in] count 毫秒计数器,每毫秒+1
 */
void app_module_system_1msec_update(uint32_t count)
{
    /* timer msec update */
    app_module_timer_1ms_update();
    /* clock source */
    if (count % 1000 == 0) {
        uint64_t utc_new = 0; /* 在此处获取RTC的UTC */
        app_module_clock_1s_update(utc_new);
    }
    /* lvgl tick source */
    if (count % LV_SCHED_TICK_INC == 0)
        app_lv_tick_inc_update();
    if (count % LV_SCHED_TICK_EXEC == 0)
        app_lv_tick_exec_update();
    if (count % LV_SCHED_DRV_EVENT == 0)
        app_lv_drv_update();
}
