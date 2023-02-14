#ifndef APP_THREAD_MASTER_H
#define APP_THREAD_MASTER_H

/* 这是app_sys_pipe_pkg_t的副本重名 */
typedef struct {
    void    *near;          /* 管道是队列 */
    uint32_t send_tid;      /* 发送者线程ID */
    uint32_t recv_tid;      /* 接收者线程ID */
    uint32_t module;        /* 接收者线程模组ID */
    uint32_t event;         /* 接收者线程模组事件 */
    uint32_t dynamic;       /* 本次传输包裹状态 */
    uint32_t size;          /* 协议数据流大小 */
    void    *data;          /* 协议数据流(浅拷贝) */
} app_package_t;

/*@brief        通过从线程ID获得与主线程的同步资源
 *@param[in]    thread_id 线程ID
 *@param[out]   sem 信号量
 */
#ifdef APP_OS_ADAPTOR_H
void app_thread_get_sync_by_id(uint32_t thread_id, app_sem_t **sem);
#endif

/*@brief        通过从线程ID获得与主线程的交互管道
 *@param[in]    thread_id 线程ID
 *@param[out]   pipe 管道
 */
#ifdef APP_SYS_PIPE_H
void app_thread_get_pipe_by_id(uint32_t thread_id, app_sys_pipe_t **pipe);
#endif

/* 线程组线程包裹接收最大警告线(警告) */
#define APP_THREAD_PACKAGE_MAX  20

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
    app_thread_id_data_manage,      /* 数据管理线程(priority:3) */
    app_thread_id_lvgl,             /* lvgl线程(priority:3) */
    /* 继续添加其他线程 */
    app_thread_id_unknown,          /* 占位符:未知来源,不可是未知去处 */
    app_thread_id_num,              /* 占位符:子线程组数量 */
} app_thread_id_set;

/*@brief 初始化所有线程及其附属资源
 *       启动所有线程
 */
void app_thread_group_run(void);

/*@brief 中止线程组工作
 */
void app_thread_set_abort(void);

/*@brief 恢复线程组工作
 */
void app_thread_set_resume(void);

#endif
