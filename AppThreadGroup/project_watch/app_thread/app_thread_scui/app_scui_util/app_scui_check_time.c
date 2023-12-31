/*实现目标:
 *    scui超时回主界面以及息屏
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_scui_lib.h"

static app_mutex_t app_scui_check_time_mutex = {0};
static app_sys_timer_t app_scui_check_time_timer = {0};
static app_scui_check_time_t app_scui_check_time = {0};

/*@brief 重置界面状态
 */
void app_scui_check_time_reset(uint8_t over_time, uint8_t idle_time)
{
    APP_SYS_LOG_INFO("");
    app_mutex_process(&app_scui_check_time_mutex, app_mutex_take);
    app_scui_check_time.over_time_bak = over_time != 0 ? over_time : APP_SCUI_CHECK_TIME_OVER;
    app_scui_check_time.idle_time_bak = idle_time != 0 ? idle_time : APP_SCUI_CHECK_TIME_IDLE;
    /* 是否需要一起禁用???
    if (over_time == APP_SCUI_CHECK_TIME_OVER_MAX ||
        idle_time == APP_SCUI_CHECK_TIME_IDLE_MAX) {
        app_scui_check_time.over_time_bak = APP_SCUI_CHECK_TIME_OVER_MAX;
        app_scui_check_time.idle_time_bak = APP_SCUI_CHECK_TIME_IDLE_MAX;
    }
    */
    app_scui_check_time.dlps = true;
    app_scui_check_time.back = true;
    app_scui_check_time.over_time = app_scui_check_time.over_time_bak;
    app_scui_check_time.idle_time = app_scui_check_time.idle_time_bak;
    app_mutex_process(&app_scui_check_time_mutex, app_mutex_give);
}

/*@brief 界面状态控制更新
 *       内部使用: 被scui线程使用
 */
void app_scui_check_time_update(void)
{
    app_mutex_process(&app_scui_check_time_mutex, app_mutex_take);
    /* 约减超时等待 */
    if (app_scui_check_time.over_time != 0 &&
        app_scui_check_time.over_time != APP_SCUI_CHECK_TIME_OVER_MAX)
        app_scui_check_time.over_time--;
    /* 主界面超时等待结束发送休眠请求 */
    if (app_scui_check_time.over_time == 0) {
        if (app_scui_check_time.dlps) {
            app_scui_check_time.dlps = false;
            app_module_system_dlps_set(true);
        } else {
            /* 约减空闲等待 */
            if (app_scui_check_time.idle_time != 0 &&
                app_scui_check_time.idle_time != APP_SCUI_CHECK_TIME_IDLE_MAX)
                app_scui_check_time.idle_time--;
            /* 空闲等待结束回到主界面 */
            if (app_scui_check_time.idle_time == 0) {
                if (app_scui_check_time.back) {
                    app_scui_check_time.back = false;
                    /* 关闭鼠标 */
                    app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
                    app_scui_check_time_exec(false);
                }
            }
        }
    }
    app_mutex_process(&app_scui_check_time_mutex, app_mutex_give);
}

/*@brief 界面状态控制更新
 *       内部使用: 被scui线程使用
 */
static void app_scui_check_time_timer_handler(void *timer)
{
    /* 发送场景计时检查事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_sched,
        .event  = app_thread_scui_sched_check_time,
    };
    app_thread_package_notify(&package);
}

/*@brief scui 时间检查更新初始化
 */
void app_scui_check_time_ready(void)
{
    app_mutex_process(&app_scui_check_time_mutex, app_mutex_static);
    app_scui_check_time_timer.expired = app_scui_check_time_timer_handler;
    app_scui_check_time_timer.peroid  = 1000;
    app_scui_check_time_timer.reload  = true;
    
    app_scui_check_time.dlps = true;
    app_scui_check_time.back = true;
    app_scui_check_time.over_time = APP_SCUI_CHECK_TIME_OVER;
    app_scui_check_time.idle_time = APP_SCUI_CHECK_TIME_IDLE;
    app_scui_check_time.over_time_bak = APP_SCUI_CHECK_TIME_OVER;
    app_scui_check_time.idle_time_bak = APP_SCUI_CHECK_TIME_IDLE;
}

/*@brief 界面状态检查功能启停
 *@param status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_scui_check_time_exec(bool status)
{
    if (status)
        app_sys_timer_start(&app_scui_check_time_timer);
    else
        app_sys_timer_stop(&app_scui_check_time_timer);
}
