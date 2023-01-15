/*
 *实现目的:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_clock.h"
#include "app_module_alarm.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"

/*@brief 混合事件线程模组初始化
 */
void app_thread_mix_custom_ready(void)
{
    /* 模组初始化 */
    app_module_clock_ready();
    app_module_alarm_group_ready();
    app_module_stopwatch_ready();
    app_module_countdown_ready();
}

/*@brief 混合事件线程服务例程
 */
void app_thread_mix_custom_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_pipe_t *pipe = NULL;
    app_package_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_mix_custom, &sem);
    app_thread_get_pipe_by_id(app_thread_id_mix_custom, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread mix custom recv too much package:%u\n",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_mix_custom_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
                break;
            }
            case app_thread_mix_custom_clock: {
                if (package.event == app_thread_mix_custom_clock_event_update)
                    app_module_clock_event_update();
                if (package.event == app_thread_mix_custom_clock_timestamp_update) {
                    uint64_t utc_new = package.data != NULL ? *(uint64_t *)package.data : 0;
                    app_module_clock_timestamp_update(utc_new);
                }
                break;
            }
            case app_thread_mix_custom_alarm: {
                if (package.size != sizeof(app_module_alarm_t))
                    break;
                if (package.data == NULL)
                    break;
                app_module_alarm_t *alarm = package.data;
                /* 根据实际的情况处理或者转发事件包到特定的线程特定的模组 */
                if (package.event == app_thread_mix_custom_alarm_expired);
                /* 常规式闹钟事件 */
                if (package.event == app_thread_mix_custom_alarm_month);
                if (package.event == app_thread_mix_custom_alarm_week);
                /* 提醒式闹钟事件 */
                if (package.event == app_thread_mix_custom_alarm_repeat);
                break;
            }
            case app_thread_mix_custom_stopwatch: {
                if (package.event == app_thread_mix_custom_stopwatch_msec_update)
                    app_module_stopwatch_msec_update();
                break;
            }
            case app_thread_mix_custom_countdown: {
                if (package.event == app_thread_mix_custom_countdown_msec_update)
                    app_module_countdown_msec_update();
                /* 根据实际的情况处理或者转发事件包到特定的线程特定的模组 */
                if (package.event == app_thread_mix_custom_countdown_expired);
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_SYS_LOG_ERROR("\n");
                APP_SYS_LOG_ERROR("thread mix custom pipe recv a unknown package\n");
                APP_SYS_LOG_ERROR("package send_tid:%u\n", package.send_tid);
                APP_SYS_LOG_ERROR("package recv_tid:%u\n", package.recv_tid);
                APP_SYS_LOG_ERROR("package module:%u\n",   package.module);
                APP_SYS_LOG_ERROR("package event:%u\n",    package.event);
                APP_SYS_LOG_ERROR("package data:%p\n",     package.data);
                APP_SYS_LOG_ERROR("package size:%u\n",     package.size);
                APP_SYS_LOG_ERROR("\n");
                #endif
                break;
            }
            }
        }
    }
}
