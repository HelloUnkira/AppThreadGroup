/*实现目标:
 *    泛型链表(侵入式单双链表, 带头尾节点)
 *    从当前链表节点地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#include "app_ext_lib.h"
#include "app_sys_list.h"

/*
 *双向链表
 */

/*@brief     重置链表
 *@param[in] list 链表实例
 */
void app_sys_list_dl_reset(app_sys_list_dl_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
void app_sys_list_dn_reset(app_sys_list_dn_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
app_sys_list_dn_t * app_sys_list_dl_head(app_sys_list_dl_t *list)
{
    return list->head;
}

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
app_sys_list_dn_t * app_sys_list_dl_tail(app_sys_list_dl_t *list)
{
    return list->tail;
}

/*@brief     链表前向结点
 *@param[in] list 链表实例
 *@retval    前向结点
 */
app_sys_list_dn_t * app_sys_list_dn_prev(app_sys_list_dn_t *node)
{
    return node->prev;
}

/*@brief     链表后向结点
 *@param[in] list 链表实例
 *@retval    后向结点
 */
app_sys_list_dn_t * app_sys_list_dn_next(app_sys_list_dn_t *node)
{
    return node->next;
}

/*@brief     节点前插入(prepend)
 *           节点target为NULL时为头部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dl_pinsert(app_sys_list_dl_t *list, app_sys_list_dn_t *target, app_sys_list_dn_t *node)
{
    if (target == NULL) {
        /* 更新插入节点 */
        node->prev = NULL;
        node->next = list->head;
    /* 检查头结点 */
    if (list->head != NULL)
        list->head->prev = node;
    if (list->head == NULL)
        list->tail  = node;
        list->head  = node;
    }
    if (target != NULL) {
        /* 更新插入节点 */
        node->prev = target->prev;
        node->next = target;
        /* 前项节点存在时 */
        if (target->prev != NULL)
            target->prev->next = node;
        if (target->prev == NULL)
            list->head    = node;
            /* 更新后项节点 */
            target->prev  = node;
    }
}

/*@brief     节点后插入(append)
 *           节点target为NULL时为尾部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dl_ainsert(app_sys_list_dl_t *list, app_sys_list_dn_t *target, app_sys_list_dn_t *node)
{
    if (target == NULL) {
        app_sys_list_dn_t *tail = app_sys_list_dl_tail(list);
        /* 更新插入节点 */
        node->prev = list->tail;
        node->next = NULL;
    /* 检查尾结点 */
    if (list->tail != NULL)
        list->tail->next = node;
    if (list->tail == NULL)
        list->head  = node;
        list->tail  = node;
    }
    if (target != NULL) {
        /* 更新插入节点 */
        node->prev = target;
        node->next = target->next;
        /* 后项节点存在时 */
        if (target->next != NULL)
            target->next->prev = node;
        if (target->next == NULL)
            list->tail    = node;
            /* 更新前项节点 */
            target->next  = node;
    }
}

/*@brief     节点移除
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带移除节点)
 */
void app_sys_list_dl_remove(app_sys_list_dl_t *list, app_sys_list_dn_t *node)
{
    app_sys_list_dn_t *prev = app_sys_list_dn_prev(node);
    app_sys_list_dn_t *next = app_sys_list_dn_next(node);
    
    /* 链表头尾节点一并检查 */
    if (node->prev == NULL)
        list->head  = node->next;
    if (node->next == NULL)
        list->tail  = node->prev;
    /* 接触关联索引 */
    if (node->prev != NULL)
        node->prev->next = node->next;
    if (node->next != NULL)
        node->next->prev = node->prev;
}

/*
 *单向链表
 */

/*@brief     重置链表
 *@param[in] list 链表实例
 */
void app_sys_list_sl_reset(app_sys_list_sl_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
void app_sys_list_sn_reset(app_sys_list_sn_t *node)
{
    node->buddy = NULL;
}

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
app_sys_list_sn_t * app_sys_list_sl_head(app_sys_list_sl_t *list)
{
    return list->head;
}

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
app_sys_list_sn_t * app_sys_list_sl_tail(app_sys_list_sl_t *list)
{
    return list->head;
}

/*@brief     链表临近结点
 *@param[in] list 链表实例
 *@retval    临近结点
 */
app_sys_list_sn_t * app_sys_list_sn_buddy(app_sys_list_sn_t *node)
{
    return node->buddy;
}

/*@brief     节点头前插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sl_pinsert(app_sys_list_sl_t *list, app_sys_list_sn_t *node)
{
        node->buddy = list->head;
    if (list->head == NULL)
        list->tail  = node;
        list->head  = node;
}

/*@brief     节点尾后插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sl_ainsert(app_sys_list_sl_t *list, app_sys_list_sn_t *node)
{
        node->buddy = NULL;
    if (list->tail != NULL)
        list->tail->buddy = node;
    if (list->tail == NULL)
        list->head  = node;
        list->tail  = node;
}

/*@brief     节点插入
 *           单链表插入需要指定目标节点,target不为NULL
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sl_insert(app_sys_list_sl_t *list, app_sys_list_sn_t *target, app_sys_list_sn_t *node)
{
    node->buddy = target->buddy;
    target->buddy = node;
    if (list->tail == target)
        list->tail  = node;
}

/*@brief     节点插入
 *           单链表删除需要指定目标节点
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sl_remove(app_sys_list_sl_t *list, app_sys_list_sn_t *target, app_sys_list_sn_t *node)
{
    if (target != NULL)
        target->buddy = node->buddy;
    if (list->head == node)
        list->head  = node->buddy;
    if (list->tail == node)
        list->tail  = target;
}

/*
 *链表容器复用:通用队列(Generics Queue)
 */

/*@brief     重置队列
 *@param[in] queue 队列实例
 */
void app_sys_queue_gq_reset(app_sys_queue_gq_t *queue)
{
    app_sys_list_sl_reset((app_sys_list_sl_t *)queue);
}

/*@brief     重置队列节点
 *@param[in] node 队列节点实例
 */
void app_sys_queue_gn_reset(app_sys_queue_gn_t *node)
{
    app_sys_list_sn_reset((app_sys_list_sn_t *)node);
}

/*@brief     队列访问队列头节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_gn_t * app_sys_queue_gq_head(app_sys_queue_gq_t *queue)
{
    return (app_sys_queue_gn_t *)app_sys_list_sl_head((app_sys_list_sl_t *)queue);
}

/*@brief     队列访问队列尾节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_gn_t * app_sys_queue_gq_tail(app_sys_queue_gq_t *queue)
{
    return (app_sys_queue_gn_t *)app_sys_list_sl_tail((app_sys_list_sl_t *)queue);
}

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_gn_t * app_sys_queue_gq_dequeue(app_sys_queue_gq_t *queue)
{
    app_sys_list_sn_t *node = app_sys_list_sl_head((app_sys_list_sl_t *)queue);
    app_sys_list_sl_remove((app_sys_list_sl_t *)queue, NULL, node);
    return (app_sys_queue_gn_t *)node;
}

/*@brief     队列节点入队列
 *@param[in] queue 队列实例
 *@param[in] node  队列节点实例
 */
void app_sys_queue_gq_enqueue(app_sys_queue_gq_t *queue, app_sys_queue_gn_t *node)
{
    app_sys_list_sl_ainsert((app_sys_list_sl_t *)queue, (app_sys_list_sn_t *)node);
}

/*
 *链表容器复用:优先队列(Priority Queue)
 */

/*@brief     重置队列
 *@param[in] queue 队列实例
 */
void app_sys_queue_pq_reset(app_sys_queue_pq_t *queue)
{
    app_sys_list_sl_reset((app_sys_list_sl_t *)queue);
}

/*@brief     重置队列节点
 *@param[in] node 队列节点实例
 */
void app_sys_queue_pn_reset(app_sys_queue_pn_t *node)
{
    app_sys_list_sn_reset((app_sys_list_sn_t *)node);
}

/*@brief     队列访问队列头节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_pn_t * app_sys_queue_pq_first(app_sys_queue_pq_t *queue)
{
    return (app_sys_queue_pn_t *)app_sys_list_sl_head((app_sys_list_sl_t *)queue);
}

/*@brief     队列访问队列尾节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_pn_t * app_sys_queue_pq_last(app_sys_queue_pq_t *queue)
{
    return (app_sys_queue_pn_t *)app_sys_list_sl_tail((app_sys_list_sl_t *)queue);
}

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_pn_t * app_sys_queue_pq_dequeue(app_sys_queue_pq_t *queue)
{
    app_sys_list_sn_t *node = app_sys_list_sl_head((app_sys_list_sl_t *)queue);
    app_sys_list_sl_remove((app_sys_list_sl_t *)queue, NULL, node);
    return (app_sys_queue_pn_t *)node;
}

/*@brief     队列节点入队列
 *@param[in] queue   队列实例
 *@param[in] node    队列节点实例
 *@param[in] compare 队列节点入队规则(希望node1排在node2之前返回true,否则false)
 */
void app_sys_queue_pq_enqueue(app_sys_queue_pq_t *queue, app_sys_queue_pn_t *node,
                              bool (*compare)(app_sys_queue_pn_t *node1, app_sys_queue_pn_t *node2))
{
    if (compare != NULL) {
        /* 迭代所有元素 */
        app_sys_list_sn_t *curr = NULL;
        app_sys_list_sl_tra((app_sys_list_sl_t *)queue, prev) {
            /* 第一次:比较队列头 */
            if (curr == NULL && compare(node, prev)) {
                app_sys_list_sl_pinsert((app_sys_list_sl_t *)queue, node);
                return;
            }
            /* 比较当前节点,如果命中则加入到当前节点前一节点之后*/
            if (curr != NULL && compare(node, curr)) {
                app_sys_list_sl_insert((app_sys_list_sl_t *)queue, prev, node);
                return;
            }
            curr = prev;
        }
    }
    app_sys_list_sl_ainsert((app_sys_list_sl_t *)queue, (app_sys_list_sn_t *)node);
}

/*
 *链表容器复用:通用栈(Generics Stack)
 */

/*@brief     重置栈
 *@param[in] stack 栈实例
 */
void app_sys_stack_gs_reset(app_sys_stack_gs_t *stack)
{
    app_sys_list_sl_reset((app_sys_list_sl_t *)stack);
}

/*@brief     重置栈节点
 *@param[in] node 栈节点实例
 */ 
void app_sys_stack_gn_reset(app_sys_stack_gn_t *node)
{
    app_sys_list_sn_reset((app_sys_list_sn_t *)node);
}

/*@brief     栈访问栈顶节点
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
app_sys_stack_gn_t * app_sys_stack_gs_top(app_sys_stack_gs_t *stack)
{
    return (app_sys_stack_gn_t *)app_sys_list_sl_head((app_sys_list_sl_t *)stack);
}

/*@brief     栈节点出栈
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
app_sys_stack_gn_t * app_sys_stack_gs_pop(app_sys_stack_gs_t *stack)
{
    app_sys_list_sn_t *node = app_sys_list_sl_head((app_sys_list_sl_t *)stack);
    app_sys_list_sl_remove((app_sys_list_sl_t *)stack, NULL, node);
    return (app_sys_stack_gn_t *)node;
}

/*@brief     栈节点入栈
 *@param[in] stack 栈实例
 *@param[in] node  栈节点实例
 */
void app_sys_stack_gs_push(app_sys_stack_gs_t *stack, app_sys_stack_gn_t *node)
{
    app_sys_list_sl_pinsert((app_sys_list_sl_t *)stack, (app_sys_list_sn_t *)node);
}

/*
 *keep adding
 */
