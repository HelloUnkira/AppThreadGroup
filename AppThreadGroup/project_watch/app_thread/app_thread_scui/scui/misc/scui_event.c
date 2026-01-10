/*实现目标:
 *    事件队列
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 事件队列 */
static scui_event_queue_t scui_event_queue = {0};

/*@brief 事件吸收回调(空吸收)
 *@param evt_old 旧事件
 *@param evt_new 新事件
 */
void scui_event_absorb_none(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
}

/*@brief 事件队列初始化
 */
void scui_event_ready(void)
{
    scui_sem_process(&scui_event_queue.sem, scui_sem_static);
    scui_mutex_process(&scui_event_queue.mutex, scui_mutex_static);
    scui_list_dll_reset(&scui_event_queue.dl_list);
}

/*@brief 事件同步等待
 */
void scui_event_wait(void)
{
    scui_sem_process(&scui_event_queue.sem, scui_sem_take);
}

/*@brief 优先级排序入队列比较函数
 */
static bool scui_event_sort(scui_list_dln_t *node1, scui_list_dln_t *node2)
{
    scui_event_t *event1 = scui_own_ofs(scui_event_t, dl_node, node1);
    scui_event_t *event2 = scui_own_ofs(scui_event_t, dl_node, node2);
    return event1->style.prior > event2->style.prior;
}

/*@brief 事件包匹配函数
 */
static bool scui_event_confirm(scui_list_dln_t *node1, scui_list_dln_t *node2)
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
        scui_list_dll_ftra(&scui_event_queue.dl_list, node)
        if (scui_event_confirm(&event->dl_node, node)) {
            event_old = scui_own_ofs(scui_event_t, dl_node, node);
            event->absorb(event_old, event);
            absorb_flag = true;
            break;
        }
    }
    /* 生成资源包, 转储消息资源资源 */
    if (!absorb_flag) {
        event_new = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_event_t));
        if (event_new != NULL) {
            memcpy(event_new, event, sizeof(scui_event_t));
            scui_list_dln_reset(&event_new->dl_node);
            /* 优先级调整到中间,为常规优先级 */
            if (event_new->style.prior == 0)
                event_new->style.prior  = 0xFF / 2;
            /* 资源包加入到管道(优先队列) */
            if (event_new->style.prior == 0)
                scui_list_dll_ainsert(&scui_event_queue.dl_list, NULL, &event_new->dl_node);
            else
                scui_queue_dlpq_enqueue(&scui_event_queue.dl_list, &event_new->dl_node, scui_event_sort);
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
 *@param hit   命中指定对象(指定事件)
 *@param any   命中指定对象(非指定事件)
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit, bool any)
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
            // 额外定制的类型补充, 命中指定对象
            if (any) {
                scui_event_t *event_tar = NULL;
                scui_list_dll_btra(&scui_event_queue.dl_list, node) {
                    event_tar = scui_own_ofs(scui_event_t, dl_node, node);
                    if (event_tar->object == event->object) {
                        event_new = event_tar;
                        break;
                    }
                }
            } else {
                scui_list_dll_btra(&scui_event_queue.dl_list, node)
                if (scui_event_confirm(&event->dl_node, node)) {
                    event_new = scui_own_ofs(scui_event_t, dl_node, node);
                    break;
                }
            }
        } else {
            scui_list_dln_t *node = scui_list_dll_head(&scui_event_queue.dl_list);
            event_new = scui_own_ofs(scui_event_t, dl_node, node);
        }
        if (event_new != NULL) {
            scui_list_dll_remove(&scui_event_queue.dl_list, &event_new->dl_node);
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

/*@brief 事件回调列表事件准备
 *@param cb_list 事件回调列表
 */
void scui_event_cb_ready(scui_event_cb_list_t *cb_list)
{
    SCUI_ASSERT(cb_list != NULL);
    
    scui_list_dll_reset(&cb_list->dl_list);
}

/*@brief 事件回调列表事件清空
 *@param cb_list 事件回调列表
 */
void scui_event_cb_clear(scui_event_cb_list_t *cb_list)
{
    SCUI_ASSERT(cb_list != NULL);
    
    scui_list_dln_t *node = NULL;
    while ((node = scui_list_dll_head(&cb_list->dl_list)) != NULL) {
        scui_event_cb_node_t *cb_node = scui_own_ofs(scui_event_cb_node_t, dl_node, node);
        /* 抓取事件,移除它 */
        scui_list_dll_remove(&cb_list->dl_list, &cb_node->dl_node);
        SCUI_MEM_FREE(cb_node);
    }
}

/*@brief 事件回调列表事件查找
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_find(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node)
{
    SCUI_ASSERT(cb_list != NULL);
    SCUI_ASSERT(cb_node != NULL);
    
    scui_event_cb_node_t *cb_node_inner = NULL;
    
    /* 一个事件至多一个响应回调, 优先匹配对应事件 */
    scui_list_dll_ftra(&cb_list->dl_list, node) {
        cb_node_inner = scui_own_ofs(scui_event_cb_node_t, dl_node, node);
        if (cb_node->event == cb_node_inner->event) {
            cb_node->event_cb = cb_node_inner->event_cb;
            return;
        }
    }
    
    /* 一个事件至多一个响应回调, 其次匹配集成事件 */
    scui_list_dll_ftra(&cb_list->dl_list, node) {
        cb_node_inner = scui_own_ofs(scui_event_cb_node_t, dl_node, node);
        bool cb_node_unmatch = true;
        
        if (cb_node_unmatch)    // 序列通配回调:sched
        if (cb_node_inner->event == scui_event_sched_all &&
            scui_event_type_sched(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)    // 序列通配回调:widget
        if (cb_node_inner->event == scui_event_widget_all &&
            scui_event_type_widget(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)    // 序列通配回调:ptr
        if (cb_node_inner->event == scui_event_ptr_all &&
            scui_event_type_ptr(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)    // 序列通配回调:enc
        if (cb_node_inner->event == scui_event_enc_all &&
            scui_event_type_enc(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)    // 序列通配回调:key
        if (cb_node_inner->event == scui_event_key_all &&
            scui_event_type_key(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)    // 序列通配回调:custom
        if (cb_node_inner->event == scui_event_custom_all &&
            scui_event_type_custom(cb_node->event))
            cb_node_unmatch = false;
        
        if (cb_node_unmatch)
            continue;
        
        // 匹配到了
        cb_node->event_cb = cb_node_inner->event_cb;
        return;
    }
    
    cb_node->event_cb = NULL;
}

/*@brief 事件回调列表事件添加
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_add(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node)
{
    SCUI_ASSERT(cb_list != NULL);
    SCUI_ASSERT(cb_node != NULL);
    
    if (cb_node->event_cb == NULL)
        return;
    
    /* 一个事件至多一个响应回调,新的替换旧的 */
    scui_event_cb_node_t *cb_node_old = NULL;
    scui_list_dll_btra(&cb_list->dl_list, node) {
        cb_node_old = scui_own_ofs(scui_event_cb_node_t, dl_node, node);
        if (cb_node_old->event == cb_node->event) {
            cb_node_old->event_cb = cb_node->event_cb;
            return;
        }
    }
    
    /* 初始化节点然后入列 */
    scui_event_cb_node_t *cb_node_new = NULL;
     cb_node_new = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_event_cb_node_t));
    *cb_node_new = *cb_node;
    scui_list_dln_reset(&cb_node_new->dl_node);
    scui_list_dll_ainsert(&cb_list->dl_list, NULL, &cb_node_new->dl_node);
}

/*@brief 事件回调列表事件移除
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_del(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node)
{
    SCUI_ASSERT(cb_list != NULL);
    SCUI_ASSERT(cb_node != NULL);
    
    /* 一个事件至多一个响应回调 */
    scui_event_cb_node_t *cb_node_old = NULL;
    scui_list_dll_btra(&cb_list->dl_list, node) {
        cb_node_old = scui_own_ofs(scui_event_cb_node_t, dl_node, node);
        if (cb_node_old->event == cb_node->event)
            break;
        cb_node_old = NULL;
    }
    
    if (cb_node_old == NULL)
        return;
    /* 抓取到目标事件,移除它 */
    scui_list_dll_remove(&cb_list->dl_list, &cb_node_old->dl_node);
    SCUI_MEM_FREE(cb_node_old);
}
