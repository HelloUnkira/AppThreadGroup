/*
 *实现目的:
 *    协议传输线程
 *    管理约定的协议组件的解析和组合
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_protocol.h"
#include "app_module_protocol.h"
#include "app_adaptor_transfer.h"

/*@brief 协议线程模组初始化
 */
void app_thread_protocol_ready(void)
{
}

/*@brief 协议线程服务例程
 */
void app_thread_protocol_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_pipe_t *pipe = NULL;
    app_package_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_protocol, &sem);
    app_thread_get_pipe_by_id(app_thread_id_protocol, &pipe);
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
            case app_thread_protocol_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
                break;
            }
            case app_thread_protocol_transfer: {
                if (package.event == app_thread_protocol_transfer_tx) {
                    app_module_protocol_package_t ptl_pkg = {0};
                    memcpy(&ptl_pkg, package.data, package.size);
                    app_module_protocol_tx(&ptl_pkg);
                    app_adaptor_transfer_tx(&ptl_pkg);
                    app_mem_free(package.data);
                }
                if (package.event == app_thread_protocol_transfer_rx) {
                    app_module_protocol_package_t ptl_pkg = {0};
                    memcpy(&ptl_pkg, package.data, package.size);
                    app_module_protocol_rx(&ptl_pkg);
                    app_adaptor_transfer_rx(&ptl_pkg);
                    app_mem_free(package.data);
                }
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_SYS_LOG_ERROR("\n");
                APP_SYS_LOG_ERROR("thread protocol pipe recv a unknown package\n");
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
