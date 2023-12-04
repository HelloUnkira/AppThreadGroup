/*实现目标:
 *    与master映射的slave通用结构线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 从线程服务例程结构模板
 *           这是通用化的结构模板,每个子线程均使用它
 *@param app_thread_id 子线程ID号
 *@param ready_cb      子线程服务例程就绪部
 *@param package_cb    子线程服务例程处理部
 */
void app_thread_slave_process(uint32_t app_thread_id,
                              app_thread_slave_process_ready_cb   ready_cb,
                              app_thread_slave_process_package_cb package_cb)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_src_sem(app_thread_id, &sem);
    app_thread_src_pipe(app_thread_id, &pipe);
    /* 这是通用化的结构模板 */
    APP_SYS_ASSERT(ready_cb   != NULL);
    APP_SYS_ASSERT(package_cb != NULL);
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 额外扩充一个初始化流程,执行推延到此处 */
    ready_cb();
    /* 主流程 */
    while (true) {
        app_sem_process(sem, app_sem_take);
        /* 计算事件处理时间(开始) */
        #if APP_THREAD_SLAVE_EXECUTE_TIME
        app_execute_us_t execute_us = {0};
        app_execute_us(&execute_us, true);
        #endif
        /* 线程包数量警告检查 */
        uint32_t pkg_num = app_sys_pipe_num(pipe);
        if (APP_THREAD_PACKAGE_MAX <= pkg_num)
            APP_SYS_LOG_WARN("thread %u recv too much package:%u", app_thread_id, pkg_num);
        /* 指定子线程处理主线程派发包 */
        while (app_sys_pipe_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package, false);
            /* 计算事件处理时间(开始) */
            #if APP_THREAD_SLAVE_EXECUTE_TIME_CHECK
            bool execute_us_remind = true;
            app_execute_us_t execute_us = {0};
            app_execute_us(&execute_us, true);
            #endif
            /* 处理子线程自定义包裹 */
            uint32_t discard_count = 0; bool record = true;
            if (package_cb(&package, &discard_count, &record)) {
                /* 线程组记录事件包 */
                if (record) {
                    #if APP_THREAD_PACKAGE_RECORD_CNT >= 10
                    app_thread_package_record(&package, true);
                    #endif
                }
                /* 系统对于此事件包的处理负载过大,主动丢弃一部分相同的事件包 */
                /* 这是无奈之举,应该避免事件包的丢弃,这会对事件系统产生危害 */
                for (uint32_t idx = 0; idx < discard_count; idx++)
                    if (app_sys_pipe_num(pipe) != 0)
                        app_sys_pipe_take(pipe, &package, true);
                    else
                        break;
                /* 我们向外界提供警告,希望期盼进一步的优化 */
                if (discard_count != 0) {
                    #if APP_THREAD_SLAVE_EXECUTE_TIME_CHECK
                    APP_SYS_LOG_WARN("thread %u discard package:%d", app_thread_id, discard_count);
                    execute_us_remind = false;
                    #endif
                }
            } else {
                /* 处理系统包裹,或者未知包裹 */
                switch (package.module) {
                case 0: {
                    if (package.event == app_thread_event_work)
                        app_module_work_execute((void *)package.data);
                    
                    break;
                }
                default: {
                    APP_SYS_LOG_ERROR("thread %u pipe recv a unknown package", app_thread_id);
                    APP_SYS_LOG_ERROR("package thread:%u", package.thread);
                    APP_SYS_LOG_ERROR("package module:%u", package.module);
                    APP_SYS_LOG_ERROR("package event:%u",  package.event);
                    APP_SYS_LOG_ERROR("package data:%p",   package.data);
                    APP_SYS_LOG_ERROR("package size:%u",   package.size);
                    break;
                }
                }
            }
            /* 计算事件处理时间(结束) */
            #if APP_THREAD_SLAVE_EXECUTE_TIME_CHECK
            uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
            if (ms > APP_THREAD_SLAVE_EXECUTE_TIME_CHECK_MS && execute_us_remind) {
                APP_SYS_LOG_WARN("thread %u package execute %d ms", app_thread_id, ms);
                APP_SYS_LOG_WARN("package thread:%u", package.thread);
                APP_SYS_LOG_WARN("package module:%u", package.module);
                APP_SYS_LOG_WARN("package event:%u",  package.event);
                APP_SYS_LOG_WARN("package data:%p",   package.data);
                APP_SYS_LOG_WARN("package size:%u",   package.size);
            }
            #endif
        }
        /* 计算事件处理时间(结束) */
        #if APP_THREAD_SLAVE_EXECUTE_TIME
        double ms = app_execute_us(&execute_us, false) / 1000.0;
        app_thread_execute_us_set(app_thread_id, &ms);
        #endif
    }
}
