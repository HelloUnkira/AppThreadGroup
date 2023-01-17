#ifndef APP_THREAD_MASTER_H
#define APP_THREAD_MASTER_H

/*@brief        通过从线程ID获得与主线程的同步资源或交互管道
 *@param[in]    thread_id 线程ID
 *@param[out]   pipe 管道
 *@param[out]   sem  信号量
 */
void app_thread_get_sync_by_id(uint32_t thread_id, app_sem_t  **sem);
void app_thread_get_pipe_by_id(uint32_t thread_id, app_pipe_t **pipe);

/* 线程组线程包裹接收最大警告线(警告) */
#define APP_THREAD_PACKAGE_MAX  20

/*@brief        线程组接收一个事件包
 *@param[in]    thread_id 线程ID
 *@param[in]    package   事件包
 #@retval       失败表明线程组中止,不接收新的事件包
 */
bool app_thread_package_notify(app_package_t *package);

/* 线程组编号 */
typedef enum {
    app_thread_id_mix_irq,          /* 混合中断线程(priority:2) */
    app_thread_id_mix_custom,       /* 混合常规线程(priority:3) */
    app_thread_id_source_manage,    /* 数据管理线程(priority:3) */
    app_thread_id_protocol,         /* 协议解析线程(priority:3) */
    app_thread_id_lvgl,             /* lvgl线程(priority:3) */
    /* 继续添加其他线程 */
    app_thread_id_unknown,      /* 占位符:未知来源者或未知去处 */
    app_thread_id_num,          /* 占位符:子线程组数量 */
} app_thread_id_set;

/*@brief 初始化所有线程及其附属资源
 *       启动所有线程
 */
void app_thread_set_work_now(void);

/*@brief 中止线程组工作
 */
void app_thread_set_abort(void);

/*@brief 恢复线程组工作
 */
void app_thread_set_resume(void);

#endif
