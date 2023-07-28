/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_remind_sedentary.h"
#include "app_module_remind_drink.h"
#include "app_module_do_not_disturb.h"
#include "app_module_shutdown.h"

/* 这里不存在并发读写导致的时序不同步,无需保护 */
static bool app_module_dump_status_not_over = true;

/*@brief 转储流程是否结束
 */
bool app_module_dump_not_over(void)
{
    return app_module_dump_status_not_over;
}

/*@brief 资源数据从内存转储到外存
 */
void app_module_dump_process(void)
{
    APP_SYS_LOG_WARN("start");
    APP_SYS_LOG_WARN("...");
    app_module_clock_dump();
    app_module_remind_alarm_dump();
    app_module_remind_sedentary_dump();
    app_module_remind_drink_dump();
    app_module_do_not_disturb_dump();
    /* ... */
    app_module_shutdown_dump();
    /* ... */
    app_module_dump_status_not_over = false;
    APP_SYS_LOG_WARN("end");
}

/*@brief 系统转储模组启动
 */
void app_module_dump_event(void)
{
    app_module_dump_status_not_over = true;
    /* 向线程发送转储事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_manage,
        .module = app_thread_manage_dump,
        .event  = 0,
    };
    app_thread_package_notify(&package);
    APP_SYS_LOG_WARN("");
}
