/*实现目标:
 *    事件队列
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 事件队列 */
static scui_event_queue_t scui_event_queue = {0};

/*@brief 事件队列初始化
 */
void scui_event_ready(void)
{
    scui_sem_process(&scui_event_queue.sem, scui_sem_static);
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_static);
    app_sys_list_dll_reset(&scui_event_queue.dl_list);
}

/*@brief 事件同步等待
 */
void scui_event_wait(void)
{
    scui_sem_process(&scui_event_queue.sem, scui_sem_take);
}

/*@brief 优先级排序入队列比较函数
 */
static bool scui_event_sort(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    scui_event_t *event1 = scui_own_ofs(scui_event_t, dl_node, node1);
    scui_event_t *event2 = scui_own_ofs(scui_event_t, dl_node, node2);
    return event1->priority >= event2->priority;
}

/*@brief 事件包匹配函数
 */
static bool scui_event_confirm(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    scui_event_t *event1 = scui_own_ofs(scui_event_t, dl_node, node1);
    scui_event_t *event2 = scui_own_ofs(scui_event_t, dl_node, node2);
    return event1->object == event2->object && event1->type == event2->type;
}

/*@brief 事件包入列函数
 *@param event 事件包
 */
void scui_event_enqueue(scui_event_t *event)
{
    bool absorb_flag = false;
    scui_event_t *event_old = NULL;
    scui_event_t *event_new = NULL;
    
    if (event == NULL) {
        SCUI_LOG_ERROR("error args");
        return;
    }
    
    /* 同步原语:上锁 */
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_take);
    
    /* 事件包合并检查,如果合并回调不为空且查找到旧事件,合并它 */
    if (event->absorb != NULL) {
        app_sys_list_dll_ftra(&scui_event_queue.dl_list, node)
        if (scui_event_confirm(&event->dl_node, node)) {
            event_old = scui_own_ofs(scui_event_t, dl_node, node);
            event->absorb(event_old, event);
            absorb_flag = true;
            break;
        }
    }
    /* 生成资源包, 转储消息资源资源 */
    if (!absorb_flag) {
        event_new = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_event_t));
        if (event_new != NULL) {
            memcpy(event_new, event, sizeof(scui_event_t));
            app_sys_list_dln_reset(&event_new->dl_node);
            /* 资源包加入到管道(优先队列) */
            if (event_new->priority == 0)
                app_sys_list_dll_ainsert(&scui_event_queue.dl_list, NULL, &event_new->dl_node);
            else
                app_sys_queue_dlpq_enqueue(&scui_event_queue.dl_list, &event_new->dl_node, scui_event_sort);
            scui_event_queue.list_num++;
        }
    }
    
    /* 同步原语:解锁 */
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_give);
    /* 同步原语:通报 */
    if (!absorb_flag)
        scui_sem_process(&scui_event_queue.sem, scui_sem_give);
}

/*@brief 事件包出列函数
 *@param event 事件包
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit)
{
    bool retval = false;
    scui_event_t *event_new = NULL;
    
    if (event == NULL) {
        SCUI_LOG_ERROR("error args");
        return retval;
    }
    
    /* 同步原语:上锁 */
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_take);
    
    /* 资源包提取出管道 */
    if (scui_event_queue.list_num != 0) {
        /* 需要命中指定资源包 */
        if (hit) {
            app_sys_list_dll_btra(&scui_event_queue.dl_list, node)
            if (scui_event_confirm(&event->dl_node, node)) {
                event_new = scui_own_ofs(scui_event_t, dl_node, node);
                break;
            }
        } else {
            app_sys_list_dln_t *node = app_sys_list_dll_head(&scui_event_queue.dl_list);
            event_new = scui_own_ofs(scui_event_t, dl_node, node);
        }
        if (event_new != NULL) {
            app_sys_list_dll_remove(&scui_event_queue.dl_list, &event_new->dl_node);
            scui_event_queue.list_num--;
            retval = true;
        }
    }
    /* 转储消息资源资源, 销毁资源包 */
    if (event_new != NULL) {
        memcpy(event, event_new, sizeof(scui_event_t));
        SCUI_MEM_FREE(event_new);
    }
    
    /* 同步原语:解锁 */
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_give);
    
    return retval;
}

/*@brief 事件队列事件数量
 *@retval 事件数量
 */
uint32_t scui_event_num(void)
{
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_take);
    uint32_t list_num = scui_event_queue.list_num;
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_give);
    return list_num;
}
