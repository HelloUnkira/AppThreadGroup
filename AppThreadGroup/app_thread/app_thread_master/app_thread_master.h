#ifndef APP_THREAD_MASTER_H
#define APP_THREAD_MASTER_H

/* 线程组工作模式
 * 线程组以事件组优先级进行实时调度或分时调度
 * 实时调度会保证优先级高的事件被立即激活
 * 分时调度以指定时间片内收集的事件进行调度
 * 分时调度会降低部分并发性但能节约部分调度性能
 */
#define APP_THREAD_GROUP_REALTIME   0
#define APP_THREAD_GROUP_TIME_SLICE 10

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
} app_package_t;

/* 事件包裹推荐优先级 */
/* 归0初始化事件包默认为最低优先级 */
/* 实时优先级专用于密集化时间敏感事件 */
typedef enum {
    app_package_priority_lowest = 0,
    app_package_priority_normal_below,
    app_package_priority_normal,
    app_package_priority_normal_above,
    app_package_priority_highest,
    app_package_priority_real_time,
} app_package_priority_t;

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

/* 线程组线程包裹接收最大警告线(警告) */
#if APP_THREAD_GROUP_REALTIME
#define APP_THREAD_PACKAGE_MAX  20
#else
#define APP_THREAD_PACKAGE_MAX  100
#endif

/*@brief        线程组接收一个事件包
 *@param[in]    thread_id 线程ID
 *@param[in]    package   事件包
 #@retval       失败表明线程组中止,不接收新的事件包
 */
bool app_package_notify(app_package_t *package);

/* 线程组编号 */
typedef enum {
    app_thread_id_mix_irq,          /* 混合中断线程(priority:2) */
    app_thread_id_mix_custom,       /* 混合常规线程(priority:3) */
    app_thread_id_manage,           /* 数据管理线程(priority:5) */
    app_thread_id_lvgl,             /* lvgl线程(priority:4) */
    /* 继续添加其他线程 */
    app_thread_id_number,              /* 占位符:子线程组数量 */
    app_thread_id_unknown,          /* 占位符:未知来源,不可为未知去处 */
} app_thread_id_t;

/* 线程组事件 */
/* 备注:每一个子线程都必须可以处理的系统公共事件 */
/* 备注:占用每一个子线程的0号模组 */
typedef enum {  /* 线程组模组 */
    app_thread_group_work,
    app_thread_group_num,
} app_thread_group_t;

/*@brief 初始化所有线程及其附属资源
 *       启动所有线程
 */
void app_thread_group_sched(void);

/*@brief 获得线程组初始化状态
 */
bool app_thread_group_status_get(void);

#endif
