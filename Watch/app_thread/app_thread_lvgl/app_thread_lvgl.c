/*
 *实现目的:
 *    lvgl线程
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_thread_interface.h"

/*@brief lvgl线程模组初始化
 */
void app_thread_lvgl_ready(void)
{
}

/*@brief lvgl线程服务例程
 */
void app_thread_lvgl_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_pipe_t *pipe = NULL;
    app_package_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_lvgl, &sem);
    app_thread_get_pipe_by_id(app_thread_id_lvgl, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_OS_LOG_WARN("thread lvgl recv too much package:%u\n",
                            app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_lvgl_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_OS_LOG_ERROR("\n");
                APP_OS_LOG_ERROR("thread lvgl pipe recv a unknown package\n");
                APP_OS_LOG_ERROR("package send_tid:%u\n", package.send_tid);
                APP_OS_LOG_ERROR("package recv_tid:%u\n", package.recv_tid);
                APP_OS_LOG_ERROR("package module:%u\n",   package.module);
                APP_OS_LOG_ERROR("package event:%u\n",    package.event);
                APP_OS_LOG_ERROR("package data:%p\n",     package.data);
                APP_OS_LOG_ERROR("package size:%u\n",     package.size);
                APP_OS_LOG_ERROR("\n");
                #endif
                break;
            }
            }
        }
    }
}
