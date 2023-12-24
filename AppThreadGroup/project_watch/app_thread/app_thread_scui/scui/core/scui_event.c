/*实现目标:
 *    事件队列
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/* 同步原语:上锁,解锁 */
void (*scui_event_sync_lock_cb)(void)   = NULL;
void (*scui_event_sync_unlock_cb)(void) = NULL;
/* 同步原语:等待,通报 */
void (*scui_event_sync_wait_cb)(void)   = NULL;
void (*scui_event_sync_notify_cb)(void) = NULL;
/* 事件队列 */
static scui_event_queue_t scui_event_queue = {0};

/*@brief 事件队列同步原语
 *@param lock   同步原语:上锁
 *@param unlock 同步原语:解锁
 *@param wait   同步原语:等待
 *@param notify 同步原语:通报
 */
void scui_event_sync_ready(void (*lock)(void), void (*unlock)(void),
                           void (*wait)(void), void (*notify)(void))
{
    scui_event_sync_lock_cb   = lock;
    scui_event_sync_unlock_cb = unlock;
    scui_event_sync_wait_cb   = wait;
    scui_event_sync_notify_cb = notify;
}

/*@brief 事件同步等待
 */
void scui_event_sync_wait(void)
{
    if (scui_event_sync_wait_cb != NULL)
        scui_event_sync_wait_cb();
}

/*@brief 事件队列事件数量
 *@retval 事件数量
 */
uint32_t scui_event_num(void)
{
    return scui_event_queue.list_num;
}

/*@brief 优先级排序入队列比较函数
 */
static bool scui_event_sort(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    scui_event_t *event1 = app_sys_own_ofs(scui_event_t, dl_node, node1);
    scui_event_t *event2 = app_sys_own_ofs(scui_event_t, dl_node, node2);
    return event1->priority >= event2->priority;
}

/*@brief 事件包匹配函数
 */
static bool scui_event_confirm(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    scui_event_t *event1 = app_sys_own_ofs(scui_event_t, dl_node, node1);
    scui_event_t *event2 = app_sys_own_ofs(scui_event_t, dl_node, node2);
    return event1->object == event2->object && event1->type == event2->type;
}

/*@brief 事件包匹配函数
 *@param event 事件包
 */
void scui_event_enqueue(scui_event_t *event)
{
    bool absorb_flag = false;
    scui_event_t *event_old = NULL;
    scui_event_t *event_new = NULL;
    
    if (event == NULL) {
        APP_SYS_LOG_ERROR("error args");
        return;
    }
    
    /* 同步原语:上锁 */
    if (scui_event_sync_lock_cb == NULL)
        scui_event_sync_lock_cb();
    
    /* 事件包合并检查,如果合并回调不为空且查找到旧事件,合并它 */
    if (event->absorb != NULL) {
        app_sys_list_dll_ftra(&scui_event_queue.dl_list, node)
        if (scui_event_confirm(&event->dl_node, node)) {
            event_old = app_sys_own_ofs(scui_event_t, dl_node, node);
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
    if (scui_event_sync_unlock_cb == NULL)
        scui_event_sync_unlock_cb();
    /* 同步原语:通报 */
    if (scui_event_sync_notify_cb == NULL)
        scui_event_sync_notify_cb();
}

/*@brief 事件包匹配函数
 *@param event 事件包
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit)
{
    bool retval = false;
    scui_event_t *event_new = NULL;
    
    if (event == NULL) {
        APP_SYS_LOG_ERROR("error args");
        return retval;
    }
    
    /* 同步原语:上锁 */
    if (scui_event_sync_lock_cb == NULL)
        scui_event_sync_lock_cb();
    
    /* 资源包提取出管道 */
    if (scui_event_queue.list_num != 0) {
        /* 需要命中指定资源包 */
        if (hit) {
            app_sys_list_dll_btra(&scui_event_queue.dl_list, node)
            if (scui_event_confirm(&event->dl_node, node)) {
                event_new = app_sys_own_ofs(scui_event_t, dl_node, node);
                break;
            }
        } else {
            app_sys_list_dln_t *node = app_sys_list_dll_head(&scui_event_queue.dl_list);
            event_new = app_sys_own_ofs(scui_event_t, dl_node, node);
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
    if (scui_event_sync_unlock_cb == NULL)
        scui_event_sync_unlock_cb();
    
    return retval;
}
