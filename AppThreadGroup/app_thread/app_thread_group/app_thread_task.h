#ifndef APP_THREAD_TASK_H
#define APP_THREAD_TASK_H

/* 线程任务控制块(TCB) */
typedef struct {
    app_sys_list_dln_t dl_node_commit;
    app_sys_list_dln_t dl_node_toggle;
    uint32_t priority;
    void (*handler)(void);
} app_thread_task_t;

/* 线程任务队列 */
typedef struct {
    app_sem_t sem;
    app_mutex_t mutex;
    app_sys_list_dll_t dl_list_commit;
    app_sys_list_dll_t dl_list_toggle;
} app_thread_task_queue_t;

/*@brief  创建一个任务队列
 *@retval 任务队列实例
 */
app_thread_task_queue_t * app_thread_task_queue_create(void);

/*@brief 销毁一个任务队列(同步销毁所有任务)
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_destroy(app_thread_task_queue_t *task_queue);

/*@brief 提交一个任务
 *@param task_queue 任务队列实例
 *@param handler    任务体
 *@param priority   任务优先级(数字越大越优先响应)
 *@retval 任务实例
 */
app_thread_task_t * app_thread_task_commit(app_thread_task_queue_t *task_queue, void (*handler)(void), uint32_t priority);

/*@brief 触发一个任务
 *@param task_queue 任务队列实例
 *@param task 任务实例
 */
void app_thread_task_toggle(app_thread_task_queue_t *task_queue, app_thread_task_t *task);

/*@brief 任务队列等待任务
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_wait(app_thread_task_queue_t *task_queue);

/*@brief 任务队列通知
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_notify(app_thread_task_queue_t *task_queue);

/*@brief 任务队列调度任务
 *@param task_queue 任务队列实例
 *@param loop 单次调度(不等信号),或者常等待(等信号)
 */
void app_thread_task_queue_schedule(app_thread_task_queue_t *task_queue, bool loop);

#endif
