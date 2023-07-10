#ifndef APP_THREAD_MASTER_H
#define APP_THREAD_MASTER_H

/*@brief        设置子线程执行时间
 *              注意:这里的时间设置为累加设置
 *@param[in]    uint32_t thread 线程ID
 *@param[out]   子线程执行时间(us)
 */
#if APP_SYS_LOG_EXECUTE
void app_thread_execute_us_set(uint32_t thread, double *execute_us);
#endif

/*@brief        获得子线程执行时间
 *@param[in]    uint32_t thread 线程ID
 *@param[out]   子线程执行时间(us)
 */
#if APP_SYS_LOG_EXECUTE
void app_thread_execute_us_get(uint32_t thread, double *execute_us);
#endif

/*@brief        通过从线程ID获得与主线程的同步资源
 *@param[in]    thread 线程ID
 *@param[out]   sem 信号量
 */
#ifdef APP_EXT_OS_H
void app_thread_get_sync(uint32_t thread, app_sem_t **sem);
#endif

/*@brief        通过从线程ID获得与主线程的交互管道
 *@param[in]    thread 线程ID
 *@param[out]   pipe 管道
 */
#ifdef APP_SYS_PIPE_H
void app_thread_get_pipe(uint32_t thread, app_sys_pipe_t **pipe);
#endif

/* 主线程调度模式
 * 线程组以事件组优先级进行实时调度或分时调度
 * 实时调度会保证优先级高的事件被立即激活
 * 分时调度以指定时间片内收集的事件进行调度
 * 分时调度会降低部分并发性但能节约部分调度性能
 */
#define APP_THREAD_MASTER_REALTIME      0
#define APP_THREAD_MASTER_TIME_SLICE    10

/* 线程组线程包裹接收最大警告线(警告) */
#if     APP_THREAD_MASTER_REALTIME
#define APP_THREAD_PACKAGE_MAX  20
#else
#define APP_THREAD_PACKAGE_MAX  100
#endif

/* 这是app_sys_pipe_pkg_t的副本重名 */
typedef struct {
    void    *buddy;         /* 管道是优先队列 */
    uint64_t thread:10;     /* 接收者线程 */
    uint64_t module:10;     /* 接收者线程模组 */
    uint64_t event:10;      /* 接收者线程模组事件 */
    uint64_t priority:8;    /* 接收者线程模组事件优先级(数字越大优先级越高) */
    uint64_t dynamic:1;     /* 本次传输包裹状态 */
    uint32_t size;          /* 协议数据流大小 */
    void    *data;          /* 协议数据流(浅拷贝) */
} app_thread_package_t;

/* 事件包裹推荐优先级 */
/* 归0初始化事件包默认为最低优先级 */
/* 实时优先级专用于密集化时间敏感事件 */
typedef enum {
    app_thread_package_priority_lowest = 0,
    app_thread_package_priority_normal_below,
    app_thread_package_priority_normal,
    app_thread_package_priority_normal_above,
    app_thread_package_priority_highest,
    app_thread_package_priority_real_time,
} app_thread_package_priority_t;

/*@brief        线程组接收一个事件包
 *@param[in]    thread_id 线程ID
 *@param[in]    package   事件包
 #@retval       失败表明线程组中止,不接收新的事件包
 */
bool app_thread_package_notify(app_thread_package_t *package);

#endif
