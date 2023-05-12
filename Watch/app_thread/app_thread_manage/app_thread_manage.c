/*
 *实现目的:
 *    数据中心管理线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_dump.h"
#include "app_module_load.h"
#include "app_module_shutdown.h"
#include "app_module_transfer.h"

#include "cJSON.h"

/*@brief 数据管理线程初始化
 */
void app_thread_manage_ready(void)
{
    /* cJSON组件初始化 */
    cJSON_Hooks cjson_hooks = {
        .malloc_fn = app_mem_alloc,
        .free_fn   = app_mem_free,
    };
    cJSON_InitHooks(&cjson_hooks);
}

/*@brief 数据管理线程服务例程
 */
void app_thread_manage_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync(app_thread_id_manage, &sem);
    app_thread_get_pipe(app_thread_id_manage, &pipe);
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
    }
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread data manage recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package, false);
            /* 计算事件处理时间(开始) */
            #if APP_SYS_LOG_EXECUTE
            bool execute_ms_remind = true;
            app_execute_ms_t execute_ms = {0};
            app_execute_ms(&execute_ms, true);
            #endif
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_manage_system: {
                if (package.event == app_thread_group_work)
                    app_sys_work_execute((void *)package.data);
                break;
            }
            case app_thread_manage_dump: {
                /* 将系统敏感的资源转储到外存 */
                app_module_dump_process();
                break;
            }
            case app_thread_manage_load: {
                /* 将系统敏感的资源加载到内存 */
                app_module_load_process();
                break;
            }
            case app_thread_manage_transfer: {
                if (package.event == app_thread_manage_transfer_notify)
                    app_module_transfer_notify(package.data);
                if (package.event == app_thread_manage_transfer_respond)
                    app_module_transfer_respond(package.data);
                break;
            }
            default: {
                #if APP_SYS_LOG_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread data center pipe recv a unknown package");
                APP_SYS_LOG_ERROR("package thread:%u", package.thread);
                APP_SYS_LOG_ERROR("package module:%u", package.module);
                APP_SYS_LOG_ERROR("package event:%u",  package.event);
                APP_SYS_LOG_ERROR("package data:%p",   package.data);
                APP_SYS_LOG_ERROR("package size:%u",   package.size);
                #endif
                break;
            }
            }
            /* 计算事件处理时间(结束) */
            #if APP_SYS_LOG_EXECUTE
            uint32_t ms = app_execute_ms(&execute_ms, false);
            if (ms > APP_SYS_LOG_EXECUTE_MS && execute_ms_remind) {
                APP_SYS_LOG_WARN("thread data center package execute %d ms", ms);
                APP_SYS_LOG_WARN("package thread:%u", package.thread);
                APP_SYS_LOG_WARN("package module:%u", package.module);
                APP_SYS_LOG_WARN("package event:%u",  package.event);
                APP_SYS_LOG_WARN("package data:%p",   package.data);
                APP_SYS_LOG_WARN("package size:%u",   package.size);
            }
            #endif
        }
    }
}
