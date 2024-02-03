/*实现目标:
 *     如果子线程如果不想使用事件调度
 *     任务调度分层可能是一个替补考量
 *     这是一个较为简要的实现选择
 *注意:
 *     不允许混合使用事件调度和任务调度
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 优先级排序入队列比较函数
 */
static bool app_thread_task_sort(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    app_thread_task_t *task1 = app_sys_own_ofs(app_thread_task_t, dl_node_toggle, node1);
    app_thread_task_t *task2 = app_sys_own_ofs(app_thread_task_t, dl_node_toggle, node2);
    return task1->priority >= task2->priority;
}

/*@brief  创建一个任务队列
 *@retval 任务队列实例
 */
app_thread_task_queue_t * app_thread_task_queue_create(void)
{
    app_thread_task_queue_t *task_queue = app_mem_alloc(sizeof(app_thread_task_queue_t));
    *task_queue = (app_thread_task_queue_t){0};
    app_sem_process(&task_queue->sem, app_sem_create);
    app_mutex_process(&task_queue->mutex, app_mutex_create);
    app_sys_list_dll_reset(&task_queue->dl_list_commit);
    app_sys_list_dll_reset(&task_queue->dl_list_toggle);
    return task_queue;
}

/*@brief 销毁一个任务队列(同步销毁所有任务)
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_destroy(app_thread_task_queue_t *task_queue)
{
    app_mutex_process(&task_queue->mutex, app_mutex_take);
    while (true) {
        app_sys_list_dln_t *node = NULL;
        if ((node = app_sys_list_dll_head(&task_queue->dl_list_commit)) == NULL)
            break;
        app_thread_task_t *task = app_sys_own_ofs(app_thread_task_t, dl_node_commit, node);
        app_sys_list_dll_remove(&task_queue->dl_list_commit, &task->dl_node_commit);
        app_sys_list_dll_remove(&task_queue->dl_list_toggle, &task->dl_node_toggle);
        app_mem_free(task);
    }
    app_mutex_process(&task_queue->mutex, app_mutex_give);
    app_mutex_process(&task_queue->mutex, app_mutex_destroy);
    app_sem_process(&task_queue->sem, app_sem_destroy);
    app_mem_free(task_queue);
}

/*@brief 提交一个任务
 *@param task_queue 任务队列实例
 *@param handler    任务体
 *@param priority   任务优先级(数字越大越优先响应)
 *@retval 任务实例
 */
app_thread_task_t * app_thread_task_commit(app_thread_task_queue_t *task_queue, void (*handler)(void), uint32_t priority)
{
    /* 重复提交检查 */
    app_sys_list_dll_btra(&task_queue->dl_list_commit, node) {
        app_thread_task_t *task = app_sys_own_ofs(app_thread_task_t, dl_node_commit, node);
        if (task->handler == handler) {
            task->priority = priority;
            APP_SYS_LOG_WARN("repetitive task:%p", handler);
            return NULL;
        }
    }
    
    app_thread_task_t *task = app_mem_alloc(sizeof(app_thread_task_t));
    app_sys_list_dln_reset(&task->dl_node_commit);
    app_sys_list_dln_reset(&task->dl_node_toggle);
    task->handler  = handler;
    task->priority = priority;
    app_mutex_process(&task_queue->mutex, app_mutex_take);
    app_sys_list_dll_ainsert(&task_queue->dl_list_commit, NULL, &task->dl_node_commit);
    app_mutex_process(&task_queue->mutex, app_mutex_give);
    return task;
}

/*@brief 触发一个任务
 *@param task_queue 任务队列实例
 *@param task 任务实例
 */
void app_thread_task_toggle(app_thread_task_queue_t *task_queue, app_thread_task_t *task)
{
    bool task_repetitive = false;
    app_mutex_process(&task_queue->mutex, app_mutex_take);
    /* 重复触发检查 */
    app_sys_list_dll_btra(&task_queue->dl_list_toggle, node) {
        app_thread_task_t *task_old = app_sys_own_ofs(app_thread_task_t, dl_node_toggle, node);
        if (task_old == task) {
            task_repetitive = true;
            break;
        }
    }
    if (task_repetitive)
        app_sys_list_dll_remove(&task_queue->dl_list_toggle, &task->dl_node_toggle);
    app_sys_queue_dlpq_enqueue(&task_queue->dl_list_toggle, &task->dl_node_toggle, app_thread_task_sort);
    app_mutex_process(&task_queue->mutex, app_mutex_give);
    app_sem_process(&task_queue->sem, app_sem_give);
}

/*@brief 任务队列等待任务
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_wait(app_thread_task_queue_t *task_queue)
{
    app_sem_process(&task_queue->sem, app_sem_take);
}

/*@brief 任务队列通知
 *@param task_queue 任务队列实例
 */
void app_thread_task_queue_notify(app_thread_task_queue_t *task_queue)
{
    app_sem_process(&task_queue->sem, app_sem_give);
}

/*@brief 任务队列调度任务
 *@param task_queue 任务队列实例
 *@param loop 单次调度(不等信号),或者常等待(等信号)
 */
void app_thread_task_queue_schedule(app_thread_task_queue_t *task_queue, bool loop)
{
   do {
        if (loop)
            app_sem_process(&task_queue->sem, app_sem_take);
        /* 循环取出第一个任务然后处理 */
        while (true) {
            app_mutex_process(&task_queue->mutex, app_mutex_take);
            app_sys_list_dln_t *node = app_sys_list_dll_head(&task_queue->dl_list_toggle);
            if (node == NULL) {
                app_mutex_process(&task_queue->mutex, app_mutex_give);
                break;
            }
            app_thread_task_t *task = app_sys_own_ofs(app_thread_task_t, dl_node_toggle, node);
            app_sys_list_dll_remove(&task_queue->dl_list_toggle, &task->dl_node_toggle);
            app_mutex_process(&task_queue->mutex, app_mutex_give);
            task->handler();
        }
    } while (loop);
}
