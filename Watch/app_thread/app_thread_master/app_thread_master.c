/*
 *实现目标:
 *    获得所有受管控线程传递的管道信息
 *    将管道中的全部信息摘出
 *    选择性的转交给对应的线程
 *灾难的产生:
 *    明细信息的发源地与去处是一件过于繁杂的事宜
 *    控制线程最为直接的目的是要实现中心化管理
 *    这意味着它需要得到所有明细才能精准无误的
 *    对数据进行合理的分发,构建一个体系化的容器去
 *    保存这些数据容器是一项有挑战性的工作
 *    在实际情况下我们反而发现
 *    最基本的容器居然更能高效的完成指定的工作
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_os_build_time.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_adaptor.h"
#include "app_thread_master.h"
#include "app_module_system.h"

/* 管道集合,为每一个线程分发一套传输管道 */
/* 信号量,控制线程组的进动 */
static app_sem_t  app_thread_sem_src = {0};
static app_pipe_t app_thread_pipe_src = {0};
static app_sem_t  app_thread_sem_dst[app_thread_id_num]  = {0};
static app_pipe_t app_thread_pipe_dst[app_thread_id_num] = {0};
/* 互斥锁,开关主从线程组 */
static bool        app_thread_run = true;
static app_mutex_t app_thread_mutex = {0};

/*@brief        通过从线程ID获得与主线程的交互管道
 *@param[in]    thread_id 线程ID
 *@param[out]   send 发送者管道
 *@param[out]   recv 接受者管道
 */
void app_thread_get_pipe_by_id(uint32_t thread_id, app_pipe_t **pipe)
{
    if (thread_id < app_thread_id_num)
       *pipe = &app_thread_pipe_dst[thread_id];
}

/*@brief        通过从线程ID获得管道同步资源
 *@param[in]    thread_id 线程ID
 *@param[out]   sem   线程信号量
 */
void app_thread_get_sync_by_id(uint32_t thread_id, app_sem_t **sem)
{
    if (thread_id < app_thread_id_num)
        *sem = &app_thread_sem_dst[thread_id];
}

/*@brief 主线程模组初始化
 */
void app_thread_master_ready(void)
{
    /* 编译时间输出 */
    app_os_build_time();
    /* 模组初始化 */
    app_module_system();
}

/*@brief 主线程服务例程
 */
void app_thread_master_routine(void)
{
    app_sem_t  *recv_sem  = NULL;
    app_pipe_t *recv_pipe = NULL;
    app_sem_t  *send_sem  = &app_thread_sem_src;
    app_pipe_t *send_pipe = &app_thread_pipe_src;
    app_package_t package = {0};
    /* 主流程 */
    while (true) {
        app_sem_take(send_sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(send_pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread masther recv too much package:%u\n",
                              app_sys_pipe_package_num(send_pipe));
        #endif
        /* 线程组不再派发事件包(如果系统中止) */
        app_mutex_take(&app_thread_mutex);
        bool run = app_thread_run;
        app_mutex_give(&app_thread_mutex);
        if (!run)
            continue;
        /* 主线程派发包到指定子线程 */
        while (app_sys_pipe_package_num(send_pipe)) {
            app_sys_pipe_take(send_pipe, &package);
            app_thread_get_sync_by_id(package.recv_tid, &recv_sem);
            app_thread_get_pipe_by_id(package.recv_tid, &recv_pipe);
            app_sys_pipe_give(recv_pipe, &package);
            app_sem_give(recv_sem);
        }
    }
}

/*@brief        线程组接收一个事件包
 *@param[in]    thread_id 线程ID
 *@param[in]    package   事件包
 #@retval       失败表明线程组中止,不接收新的事件包
 */
bool app_thread_package_notify(app_package_t *package)
{
    /* 线程组不接收新包(如果系统中止) */
    app_mutex_take(&app_thread_mutex);
    bool run = app_thread_run;
    app_mutex_give(&app_thread_mutex);
    if (!run)
        return false;
    /* 线程组接收新包 */
    app_sem_t  *send_sem  = &app_thread_sem_src;
    app_pipe_t *send_pipe = &app_thread_pipe_src;
    app_sys_pipe_give(send_pipe, package);
    app_sem_give(send_sem);
    return true;
}

/*@brief 准备并启动所有线程及其附属资源
 */
void app_thread_set_work_now(void)
{
    app_mutex_process(&app_thread_mutex);
    /* 就绪管道和同步资源 */
    for (uint32_t idx = 0; idx < app_thread_id_num; idx++)
        app_sys_pipe_ready(&app_thread_pipe_dst[idx]);
        app_sys_pipe_ready(&app_thread_pipe_src);
    for (uint32_t idx = 0; idx < app_thread_id_num; idx++)
        app_sem_process(&app_thread_sem_dst[idx]);
        app_sem_process(&app_thread_sem_src);
    /* 就绪线程子模组 */
    app_thread_master_ready();
    app_thread_mix_irq_ready();
    app_thread_mix_custom_ready();
    app_thread_data_manage_ready();
    app_thread_lvgl_ready();
    /* 就绪和启用线程组 */
    app_thread_process(&app_thread_master);
    app_thread_process(&app_thread_mix_irq);
    app_thread_process(&app_thread_mix_custom);
    app_thread_process(&app_thread_data_manage);
    app_thread_process(&app_thread_lvgl);
}

/*@brief 中止线程组工作
 */
void app_thread_set_abort(void)
{
    app_mutex_take(&app_thread_mutex);
    app_thread_run = false;
    app_mutex_give(&app_thread_mutex);
}

/*@brief 恢复线程组工作
 */
void app_thread_set_resume(void)
{
    app_mutex_take(&app_thread_mutex);
    app_thread_run = true;
    app_mutex_give(&app_thread_mutex);
    /* 因为之前丢弃的信号可能会导致有些事件的堆积 */
    app_sem_t *send_sem = &app_thread_sem_src;
    app_sem_give(send_sem);
}
