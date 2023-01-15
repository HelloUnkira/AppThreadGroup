/*
 *实现目的:
 *    有一个统一管理数据加载与转储的入口是极为必要的
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_source_manage.h"
#include "app_module_ext_mem.h"
#include "app_module_dump.h"
#include "app_module_load.h"
#include "app_module_trace.h"
#include "app_module_shutdown.h"

/*@brief 数据管理线程模组初始化
 */
void app_thread_source_manage_ready(void)
{
    /* 模组初始化 */
    app_module_ext_mem_ready();
    app_module_trace_text_ready();
}

/*@brief 数据管理线程服务例程
 */
void app_thread_source_manage_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_pipe_t *pipe = NULL;
    app_package_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_source_manage, &sem);
    app_thread_get_pipe_by_id(app_thread_id_source_manage, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread mix irq recv too much package:%u\n",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_source_manage_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
                break;
            }
            case app_thread_source_manage_dump: {
                /* 将系统敏感的资源转储到外存 */
                app_module_dump();
                app_module_shutdown_dump();
                break;
            }
            case app_thread_source_manage_load: {
                /* 将系统敏感的资源加载到内存 */
                app_module_load();
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_SYS_LOG_ERROR("\n");
                APP_SYS_LOG_ERROR("thread data manage pipe recv a unknown package\n");
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
