/*实现目标:
 *    系统相关的功能组件
 *    注意,这是一个事件调度的缝合层
 *    它与常规的子模组不一样,见文件布局
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_data_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_dump.h"
#include "app_module_load.h"

static    bool app_module_system_dump = false;
static    bool app_module_system_dlps_exec = false;
static    bool app_module_system_dlps_status = false;
static uint8_t app_module_system_delay = 0;
static uint8_t app_module_system_status = 0;
static app_mutex_t app_module_system_mutex = {0};

/*@brief 设置系统转储成功标记
 */
void app_module_system_dump_set(bool over)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_dump = over;
    app_mutex_give(&app_module_system_mutex);
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

/*@brief     系统进出DLPS
 *@param[in] status true:进入dlps;false:退出dlps
 */
void app_module_system_dlps_set(bool status)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_dlps_exec = true;
    app_module_system_dlps_status = status;
    app_mutex_give(&app_module_system_mutex);
    /* 向线程发送场景休眠唤醒事件 */
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
    };
    if (status)
        package.event = app_thread_lvgl_sched_dlps_enter;
    else
        package.event = app_thread_lvgl_sched_dlps_exit;
    app_package_notify(&package);
    APP_SYS_LOG_WARN("ui scene");
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

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_system_ctrl_check(app_module_clock_t clock[1])
{
    static bool not_dump_yet = true;
    static bool not_load_yet = true;
    app_mutex_take(&app_module_system_mutex);
    bool        dump = app_module_system_dump;
    bool   dlps_exec = app_module_system_dlps_exec;
    bool dlps_status = app_module_system_dlps_status;
    app_module_system_dlps_exec = false;
    uint8_t  delay = app_module_system_delay;
    uint8_t status = app_module_system_status;
    bool  is_valid = app_module_system_status == app_module_system_valid;
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
    /* 执行加载 */
    if (not_load_yet) {
        not_load_yet = false;
        /* 向线程发送场景启动事件 */
        {
            app_package_t package = {
                .send_tid = app_thread_id_unknown,
                .recv_tid = app_thread_id_lvgl,
                .module   = app_thread_lvgl_ui_scene,
                .event    = app_thread_lvgl_ui_scene_start,
            };
            app_package_notify(&package);
            APP_SYS_LOG_WARN("ui scene start");
        }
        /* 向线程发送加载事件 */
        {
            app_package_t package = {
                .send_tid = app_thread_id_unknown,
                .recv_tid = app_thread_id_data_manage,
                .module   = app_thread_data_manage_load,
                .event    = 0,
            };
            app_package_notify(&package);
            APP_SYS_LOG_WARN("data load");
        }
    }
    if (is_valid)
        return;
    /* 执行转储 */
    if (not_dump_yet) {
        not_dump_yet = false;
        /* 向线程发送场景停止事件 */
        {
            app_package_t package = {
                .send_tid = app_thread_id_unknown,
                .recv_tid = app_thread_id_lvgl,
                .module   = app_thread_lvgl_ui_scene,
                .event    = app_thread_lvgl_ui_scene_stop,
            };
            app_package_notify(&package);
            APP_SYS_LOG_WARN("ui scene stop");
        }
        /* 向线程发送转储事件 */
        {
            app_package_t package = {
                .send_tid = app_thread_id_unknown,
                .recv_tid = app_thread_id_data_manage,
                .module   = app_thread_data_manage_dump,
                .event    = 0,
            };
            app_package_notify(&package);
            APP_SYS_LOG_WARN("data dump");
        }
    }
    /* 延时到期,数据转储结束,重启系统 */
    if (!delay && dump) {
        app_os_reset();
        app_module_system_dump_set(false);
    }
    /* 保证数据转储成功后再进行重启 */
    if (!delay)
        return;
    /* 重启系统倒计时 */
    app_mutex_take(&app_module_system_mutex);
    app_module_system_delay--;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief 初始化系统模组
 */
void app_module_system_ready(void)
{
    app_mutex_process(&app_module_system_mutex);
}

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_module_assert(char *file, uint32_t line, bool cond)
{
    if (cond)
        return;
    /* 输出错误信息 */
    APP_SYS_LOG_ERROR("[%s][%d]", file, line);
    /* 异常导致的错误直接重启系统(不转储信息) */
    app_os_reset();
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
    if (count % LV_SCHED_SDL_EVNET == 0)
        app_lv_drv_update();
}
