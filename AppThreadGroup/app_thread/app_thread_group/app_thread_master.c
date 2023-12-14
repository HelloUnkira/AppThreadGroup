/*实现目标:
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

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#define APP_THREAD_PIPE_MEM_MASTER_SIZE     ((APP_THREAD_PACKAGE_MASTER_MAX + 3) * sizeof(app_sys_pipe_pkg_t))
#define APP_THREAD_PIPE_MEM_SLAVE_SIZE      ((APP_THREAD_PACKAGE_SLAVE_MAX  + 3) * sizeof(app_sys_pipe_pkg_t))

/* 信号量,控制线程组的进动 */
/* 管道集合,控制线程组的事件包传递 */
static app_mutex_t app_thread_mutex = {0};
static app_sem_t app_thread_sem_src = {0};
static app_sem_t app_thread_sem_dst_s[app_thread_id_s_e - app_thread_id_s_s - 1] = {0};
static app_sem_t app_thread_sem_dst_d[app_thread_id_d_e - app_thread_id_d_s - 1] = {0};
static app_sys_pipe_t app_thread_pipe_src = {0};
static app_sys_pipe_t app_thread_pipe_dst_s[app_thread_id_s_e - app_thread_id_s_s - 1] = {0};
static app_sys_pipe_t app_thread_pipe_dst_d[app_thread_id_d_e - app_thread_id_d_s - 1] = {0};
static uint8_t app_thread_pipe_src_mem[APP_THREAD_PIPE_MEM_MASTER_SIZE] = {0};
static uint8_t app_thread_pipe_dst_mem_s[app_thread_id_s_e - app_thread_id_s_s - 1][APP_THREAD_PIPE_MEM_SLAVE_SIZE] = {0};
static uint8_t app_thread_pipe_dst_mem_d[app_thread_id_d_e - app_thread_id_d_s - 1][APP_THREAD_PIPE_MEM_SLAVE_SIZE] = {0};
static uint8_t app_thread_used_s[(app_thread_id_s_e - app_thread_id_s_s - 1) / 8 + 1] = {0};
static uint8_t app_thread_used_d[(app_thread_id_d_e - app_thread_id_d_s - 1) / 8 + 1] = {0};
/* 计算子线程工作时间(us) */
#if APP_THREAD_SLAVE_EXECUTE_TIME
static double app_thread_execute_us_s[app_thread_id_s_e - app_thread_id_s_s - 1] = {0};
static double app_thread_execute_us_d[app_thread_id_d_e - app_thread_id_d_s - 1] = {0};
#endif
/* 记录线程包的执行序列 */
#if APP_THREAD_PACKAGE_RECORD_CNT >= 10
#define APP_THREAD_PACKAGE_RECORD_SIZE (APP_THREAD_PACKAGE_RECORD_CNT * sizeof(app_thread_package_t))
static uint8_t app_thread_record_buffer[APP_THREAD_PACKAGE_RECORD_SIZE] = {0};
static app_sys_rbuf_t app_thread_record_rbuf = {0};
#endif

/*@brief 访问线程事件包
 *@param package 事件包
 */
static void app_thread_package_visit(app_thread_package_t *package)
{
    APP_SYS_LOG_ERROR_RAW("package:");
    APP_SYS_LOG_ERROR_RAW("< thread:%u, ",      package->thread);
    APP_SYS_LOG_ERROR_RAW("- module:%u, ",      package->module);
    APP_SYS_LOG_ERROR_RAW("- event:%u, ",       package->event);
    APP_SYS_LOG_ERROR_RAW("- data:%p, ",        package->data);
    APP_SYS_LOG_ERROR_RAW("- size:%u, ",        package->size);
    APP_SYS_LOG_ERROR_RAW("- byte_align:%u, ",  package->byte_align);
    APP_SYS_LOG_ERROR_RAW("- byte_fixed:%u >",  package->byte_fixed);
    APP_SYS_LOG_ERROR_RAW(app_sys_log_line());
}

/*@brief 设置子线程执行时间
 *       注意:这里的时间设置为累加设置
 *@param uint32_t thread 线程ID
 *@param 子线程执行时间(us)
 */
#if APP_THREAD_SLAVE_EXECUTE_TIME
void app_thread_execute_us_set(uint32_t thread, double *execute_us)
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    /* 注意:这里的时间设置为累加设置 */
    if (app_thread_id_s_s < thread && thread < app_thread_id_s_e)
        app_thread_execute_us_s[thread - app_thread_id_s_s - 1] += *execute_us;
    if (app_thread_id_d_s < thread && thread < app_thread_id_d_e)
        app_thread_execute_us_d[thread - app_thread_id_d_s - 1] += *execute_us;
}
#endif

/*@brief 获得子线程执行时间
 *@param uint32_t thread 线程ID
 *@param 子线程执行时间(us)
 */
#if APP_THREAD_SLAVE_EXECUTE_TIME
void app_thread_execute_us_get(uint32_t thread, double *execute_us)
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    if (app_thread_id_s_s < thread && thread < app_thread_id_s_e)
        *execute_us = app_thread_execute_us_s[thread - app_thread_id_s_s - 1];
    if (app_thread_id_d_s < thread && thread < app_thread_id_d_e)
        *execute_us = app_thread_execute_us_d[thread - app_thread_id_d_s - 1];
}
#endif

/*@brief 判断一个动态子线程id号是否合法
 *@param thread 动态子线程id号
 *@retval 动态子线程id号是否合法
 */
bool app_thread_id_is_valid(uint32_t thread)
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    app_mutex_process(&app_thread_mutex, app_mutex_take);
    bool valid = app_sys_bit_ext_get(app_thread_used_d, thread - app_thread_id_d_s - 1, 8) != 0;
    app_mutex_process(&app_thread_mutex, app_mutex_give);
    return valid;
}

/*@brief 获得一个动态子线程id号
 *       子线程获得id号之后就获得了主从线程组的调度规则
 *       否则子线程就只能向普通子线程一样工作
 *@retval 动态子线程id号
 */
uint32_t app_thread_id_alloc(void)
{
    uint32_t thread = 0;
    app_mutex_process(&app_thread_mutex, app_mutex_take);
    for (uint32_t idx = app_thread_id_d_s + 1; idx <= app_thread_id_d_e - 1; idx++)
        if (app_sys_bit_ext_get(app_thread_used_d, idx, 8) == 0) {
            app_sys_bit_ext_set(app_thread_used_d, idx - app_thread_id_d_s - 1, 8);
            thread = idx;
            break;
        }
    app_mutex_process(&app_thread_mutex, app_mutex_give);
    
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    return thread;
}

/*@brief 回收一个动态子线程id号
 *@param thread 动态子线程id号
 *@param burn   动态子线程指定的回收流程,快速批量处理不再响应的事件包
 */
void app_thread_id_free(uint32_t thread, void (*burn)(app_thread_package_t *package))
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    app_mutex_process(&app_thread_mutex, app_mutex_take);
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    /* 从主管道提取指定线程的事件包,并使用对应的处理流程回收 */
    pipe = &app_thread_pipe_src;
    uint32_t package_num = app_sys_pipe_num(pipe);
    for (uint32_t idx = 0; idx < package_num; idx++) {
        app_sys_pipe_take(pipe, &package, false);
        if (package.thread == thread) {
            APP_SYS_ASSERT(burn != NULL);
            burn(&package);
            continue;
        }
        if (app_sys_pipe_give(pipe, &package, false) == -1) {
            app_sys_pipe_walk(pipe, app_thread_package_visit);
            APP_SYS_LOG_ERROR("thread pipe recv too much package");
            APP_SYS_ASSERT(false);
        }
    }
    /* 从指定动态子线程提取线程的事件包,并使用对应的处理流程回收 */
    app_thread_src_pipe(thread, &pipe);
    while (app_sys_pipe_num(pipe) != 0) {
        app_sys_pipe_take(pipe, &package, false);
        APP_SYS_ASSERT(burn != NULL);
        burn(&package);
    }
    app_sys_bit_ext_rst(app_thread_used_d, thread - app_thread_id_d_s - 1, 8);
    app_mutex_process(&app_thread_mutex, app_mutex_give);
}

/*@brief 通过从线程ID获得管道同步资源
 *@param thread 线程ID
 *@param sem 线程信号量
 */
void app_thread_src_sem(uint32_t thread, app_sem_t **sem)
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    if (app_thread_id_s_s < thread && thread < app_thread_id_s_e)
        *sem = &app_thread_sem_dst_s[thread - app_thread_id_s_s - 1];
    if (app_thread_id_d_s < thread && thread < app_thread_id_d_e)
        *sem = &app_thread_sem_dst_d[thread - app_thread_id_d_s - 1];
}

/*@brief 通过从线程ID获得与主线程的交互管道
 *@param thread 线程ID
 *@param pipe 管道
 */
void app_thread_src_pipe(uint32_t thread, app_sys_pipe_t **pipe)
{
    APP_SYS_ASSERT((app_thread_id_s_s < thread && thread < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread && thread < app_thread_id_d_e));
    
    if (app_thread_id_s_s < thread && thread < app_thread_id_s_e)
        *pipe = &app_thread_pipe_dst_s[thread - app_thread_id_s_s - 1];
    if (app_thread_id_d_s < thread && thread < app_thread_id_d_e)
        *pipe = &app_thread_pipe_dst_d[thread - app_thread_id_d_s - 1];
}

/*@brief 主线程准备
 */
void app_thread_master_prepare(void)
{
    /* 准备互斥锁资源 */
    app_mutex_process(&app_thread_mutex, app_mutex_static);
    /* 准备信号量资源 */
    app_sem_process(&app_thread_sem_src, app_sem_static);
    for (uint32_t idx = app_thread_id_s_s + 1; idx <= app_thread_id_s_e - 1; idx++)
        app_sem_process(&app_thread_sem_dst_s[idx - app_thread_id_s_s - 1], app_sem_static);
    for (uint32_t idx = app_thread_id_d_s + 1; idx <= app_thread_id_d_e - 1; idx++)
        app_sem_process(&app_thread_sem_dst_d[idx - app_thread_id_d_s - 1], app_sem_static);
    /* 准备管道资源 */
    app_sys_pipe_ready(&app_thread_pipe_src, (uintptr_t)app_thread_pipe_src_mem, APP_THREAD_PIPE_MEM_MASTER_SIZE);
    for (uint32_t idx = app_thread_id_s_s + 1; idx <= app_thread_id_s_e - 1; idx++)
        app_sys_pipe_ready(&app_thread_pipe_dst_s[idx - app_thread_id_s_s - 1],
                 (uintptr_t)app_thread_pipe_dst_mem_s[idx - app_thread_id_s_s - 1], APP_THREAD_PIPE_MEM_SLAVE_SIZE);
    for (uint32_t idx = app_thread_id_d_s + 1; idx <= app_thread_id_d_e - 1; idx++)
        app_sys_pipe_ready(&app_thread_pipe_dst_d[idx - app_thread_id_d_s - 1],
                 (uintptr_t)app_thread_pipe_dst_mem_d[idx - app_thread_id_d_s - 1], APP_THREAD_PIPE_MEM_SLAVE_SIZE);
    /* 记录线程包的执行序列 */
    #if APP_THREAD_PACKAGE_RECORD_CNT >= 10
    app_sys_rbuf_ready(&app_thread_record_rbuf, 1, app_thread_record_buffer, APP_THREAD_PACKAGE_RECORD_SIZE);
    #endif
}

/*@brief 主线程调度
 */
void app_thread_master_schedule(void)
{
    app_thread_process(&app_thread_master, app_thread_static);
}

/*@brief 主线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_master_routine)
{
    app_sem_t *recv_sem = NULL;
    app_sem_t *send_sem = &app_thread_sem_src;
    app_sys_pipe_t *recv_pipe = NULL;
    app_sys_pipe_t *send_pipe = &app_thread_pipe_src;
    app_sys_pipe_pkg_t package = {0};
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
    }
    /* 主流程 */
    while (true) {
        #if APP_THREAD_MASTER_REALTIME
        app_sem_process(send_sem, app_sem_take);
        #else
        app_delay_ms(APP_THREAD_MASTER_TIME_SLICE);
        #endif
        app_mutex_process(&app_thread_mutex, app_mutex_take);
        /* 线程包数量警告检查 */
        uint32_t pkg_num = app_sys_pipe_num(send_pipe);
        if (APP_THREAD_PACKAGE_MASTER_MAX <= pkg_num)
            APP_SYS_LOG_ERROR("thread master recv too much package:%u", pkg_num);
        /* 主线程派发包到指定子线程 */
        while (app_sys_pipe_num(send_pipe) != 0) {
            app_sys_pipe_take(send_pipe, &package, false);
            app_thread_src_sem(package.thread, &recv_sem);
            app_thread_src_pipe(package.thread, &recv_pipe);
            /* 分拣派发无需再做额外的事件合并了 */
            if (app_sys_pipe_give(recv_pipe, &package, false) == -1) {
                app_sys_pipe_walk(recv_pipe, app_thread_package_visit);
                APP_SYS_LOG_ERROR("thread pipe recv too much package");
                APP_SYS_ASSERT(false);
            }
            app_sem_process(recv_sem, app_sem_give);
        }
        app_mutex_process(&app_thread_mutex, app_mutex_give);
    }
}

/*@brief 线程组接收一个事件包
 *@param package 事件包
 *@retval 失败表明线程组中止,不接收新的事件包
 */
bool app_thread_package_notify(app_thread_package_t *package)
{
    /* 线程组接收新包 */
    app_sem_t *send_sem = &app_thread_sem_src;
    app_sys_pipe_t *send_pipe = &app_thread_pipe_src;
    app_mutex_process(&app_thread_mutex, app_mutex_take);
    if (app_sys_pipe_give(send_pipe, package, true) == -1) {
        app_sys_pipe_walk(send_pipe, app_thread_package_visit);
        APP_SYS_LOG_ERROR("thread pipe recv too much package");
        APP_SYS_ASSERT(false);
    }
    app_mutex_process(&app_thread_mutex, app_mutex_give);
    #if APP_THREAD_MASTER_REALTIME
    app_sem_process(send_sem, app_sem_give);
    #endif
    return true;
}

/*@brief 线程组记录事件包
 *       事件包以实际执行时间顺序
 *@param package 事件包
 *@param record  true记录;false读取
 */
#if APP_THREAD_PACKAGE_RECORD_CNT >= 10
void app_thread_package_record(app_thread_package_t *package, bool record)
{
    if (app_sys_rbuf_space(&app_thread_record_rbuf) == 0 || !record) {
        APP_SYS_LOG_INFO("package record");
        for (uint32_t idx = 0; !app_sys_rbuf_empty(&app_thread_record_rbuf); idx++) {
            app_thread_package_t package_old = {0};
            APP_SYS_RBUF_GETS_FIXED(&app_thread_record_rbuf, &package_old, app_thread_package_t);
            APP_SYS_LOG_INFO_RAW("package:");
            APP_SYS_LOG_INFO_RAW("< idx:%d, ",    idx);
            APP_SYS_LOG_INFO_RAW("- thread:%u, ", package_old.thread);
            APP_SYS_LOG_INFO_RAW("- module:%u, ", package_old.module);
            APP_SYS_LOG_INFO_RAW("- event:%u, ",  package_old.event);
            APP_SYS_LOG_INFO_RAW("- data:%p, ",   package_old.data);
            APP_SYS_LOG_INFO_RAW("- size:%u >",   package_old.size);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
        }
    }
    if (record) {
        APP_SYS_RBUF_PUTS_FIXED(&app_thread_record_rbuf, package, app_thread_package_t);
    }
}
#endif

/*@brief 线程组断言通知
 */
void app_thread_assert_notify(void)
{
    /* 提取最近事件包 */
    #if APP_THREAD_PACKAGE_RECORD_CNT >= 10
    app_thread_package_record(NULL, false);
    #endif
}