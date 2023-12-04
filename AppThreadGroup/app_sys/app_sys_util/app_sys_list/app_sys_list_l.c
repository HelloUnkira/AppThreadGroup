/*实现目标:
 *    泛型链表(侵入式单双链表, 带头尾节点)
 *    从当前链表节点地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*
 *双向链表(Double Linked List)
 */

/*@brief 重置链表
 *@param list 链表实例
 */
void app_sys_list_dll_reset(app_sys_list_dll_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief 重置链表节点
 *@param list 链表节点实例
 */
void app_sys_list_dln_reset(app_sys_list_dln_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

/*@brief 链表头结点
 *@param list 链表实例
 *@retval 头结点
 */
app_sys_list_dln_t * app_sys_list_dll_head(app_sys_list_dll_t *list)
{
    return list->head;
}

/*@brief 链表尾结点
 *@param list 链表实例
 *@retval 尾结点
 */
app_sys_list_dln_t * app_sys_list_dll_tail(app_sys_list_dll_t *list)
{
    return list->tail;
}

/*@brief 链表前向结点
 *@param list 链表实例
 *@retval 前向结点
 */
app_sys_list_dln_t * app_sys_list_dln_prev(app_sys_list_dln_t *node)
{
    return node->prev;
}

/*@brief 链表后向结点
 *@param list 链表实例
 *@retval 后向结点
 */
app_sys_list_dln_t * app_sys_list_dln_next(app_sys_list_dln_t *node)
{
    return node->next;
}

/*@brief 节点前插入(prepend)
 *           节点target为NULL时为头部插入
 *@param list   链表实例
 *@param target 链表节点实例(锚点)
 *@param node   链表节点实例(带插入节点)
 */
void app_sys_list_dll_pinsert(app_sys_list_dll_t *list, app_sys_list_dln_t *target, app_sys_list_dln_t *node)
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

/*@brief 节点后插入(append)
 *           节点target为NULL时为尾部插入
 *@param list   链表实例
 *@param target 链表节点实例(锚点)
 *@param node   链表节点实例(带插入节点)
 */
void app_sys_list_dll_ainsert(app_sys_list_dll_t *list, app_sys_list_dln_t *target, app_sys_list_dln_t *node)
{
    if (target == NULL) {
        app_sys_list_dln_t *tail = app_sys_list_dll_tail(list);
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

/*@brief 节点移除
 *@param list 链表实例
 *@param node 链表节点实例(带移除节点)
 */
void app_sys_list_dll_remove(app_sys_list_dll_t *list, app_sys_list_dln_t *node)
{
    app_sys_list_dln_t *prev = app_sys_list_dln_prev(node);
    app_sys_list_dln_t *next = app_sys_list_dln_next(node);
    
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
 *单向链表(Single Linked List)
 */

/*@brief 重置链表
 *@param list 链表实例
 */
void app_sys_list_sll_reset(app_sys_list_sll_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief 重置链表节点
 *@param list 链表节点实例
 */
void app_sys_list_sln_reset(app_sys_list_sln_t *node)
{
    node->buddy = NULL;
}

/*@brief 链表头结点
 *@param list 链表实例
 *@retval 头结点
 */
app_sys_list_sln_t * app_sys_list_sll_head(app_sys_list_sll_t *list)
{
    return list->head;
}

/*@brief 链表尾结点
 *@param list 链表实例
 *@retval 尾结点
 */
app_sys_list_sln_t * app_sys_list_sll_tail(app_sys_list_sll_t *list)
{
    return list->head;
}

/*@brief 链表临近结点
 *@param list 链表实例
 *@retval 临近结点
 */
app_sys_list_sln_t * app_sys_list_sln_buddy(app_sys_list_sln_t *node)
{
    return node->buddy;
}

/*@brief 节点头前插入
 *@param list 链表实例
 *@param node 链表节点实例(带插入节点)
 */
void app_sys_list_sll_pinsert(app_sys_list_sll_t *list, app_sys_list_sln_t *node)
{
        node->buddy = list->head;
    if (list->head == NULL)
        list->tail  = node;
        list->head  = node;
}

/*@brief 节点尾后插入
 *@param list 链表实例
 *@param node 链表节点实例(带插入节点)
 */
void app_sys_list_sll_ainsert(app_sys_list_sll_t *list, app_sys_list_sln_t *node)
{
        node->buddy = NULL;
    if (list->tail != NULL)
        list->tail->buddy = node;
    if (list->tail == NULL)
        list->head  = node;
        list->tail  = node;
}

/*@brief 节点插入
 *           单链表插入需要指定目标节点,target不为NULL
 *@param list   链表实例
 *@param target 链表节点实例(锚点)
 *@param node   链表节点实例(带插入节点)
 */
void app_sys_list_sll_insert(app_sys_list_sll_t *list, app_sys_list_sln_t *target, app_sys_list_sln_t *node)
{
    node->buddy = target->buddy;
    target->buddy = node;
    if (list->tail == target)
        list->tail  = node;
}

/*@brief 节点插入
 *           单链表删除需要指定目标节点
 *@param list   链表实例
 *@param target 链表节点实例(锚点)
 *@param node   链表节点实例(带插入节点)
 */
void app_sys_list_sll_remove(app_sys_list_sll_t *list, app_sys_list_sln_t *target, app_sys_list_sln_t *node)
{
    if (target != NULL)
        target->buddy = node->buddy;
    if (list->head == node)
        list->head  = node->buddy;
    if (list->tail == node)
        list->tail  = target;
}

/*
 *链表容器复用:通用队列(Queue)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,队列头即链表头,队列尾即链表尾
 */

/*@brief 队列节点出队列
 *@param queue 队列实例
 *@retval 队列节点实例
 */
app_sys_queue_sln_t * app_sys_queue_slq_dequeue(app_sys_queue_slq_t *queue)
{
    app_sys_list_sln_t *node = app_sys_list_sll_head(queue);
    app_sys_list_sll_remove(queue, NULL, node);
    return node;
}

/*@brief 队列节点入队列
 *@param queue 队列实例
 *@param node  队列节点实例
 */
void app_sys_queue_slq_enqueue(app_sys_queue_slq_t *queue, app_sys_queue_sln_t *node)
{
    app_sys_list_sll_ainsert(queue, node);
}

/*@brief 队列节点出队列
 *@param queue 队列实例
 *@retval 队列节点实例
 */
app_sys_queue_dln_t * app_sys_queue_dlq_dequeue(app_sys_queue_dlq_t *queue)
{
    app_sys_list_dln_t *node = app_sys_list_dll_head(queue);
    app_sys_list_dll_remove(queue, node);
    return node;
}

/*@brief 队列节点入队列
 *@param queue 队列实例
 *@param node  队列节点实例
 */
void app_sys_queue_dlq_enqueue(app_sys_queue_dlq_t *queue, app_sys_queue_dln_t *node)
{
    app_sys_list_dll_ainsert(queue, NULL, node);
}

/*
 *链表容器复用:优先队列(Priority Queue)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,队列头即链表头,队列尾即链表尾
 */

/*@brief 队列节点出队列
 *@param queue 队列实例
 *@retval 队列节点实例
 */
app_sys_queue_slpn_t * app_sys_queue_slpq_dequeue(app_sys_queue_slpq_t *queue)
{
    app_sys_list_sln_t *node = app_sys_list_sll_head(queue);
    app_sys_list_sll_remove(queue, NULL, node);
    return node;
}

/*@brief 队列节点入队列
 *@param queue   队列实例
 *@param node    队列节点实例
 *@param compare 队列节点入队规则(希望node1排在node2之前返回true,否则false)
 */
void app_sys_queue_slpq_enqueue(app_sys_queue_slpq_t *queue, app_sys_queue_slpn_t *node,
                               bool (*compare)(app_sys_queue_slpn_t *node1, app_sys_queue_slpn_t *node2))
{
    if (compare != NULL) {
        /* 迭代所有元素 */
        app_sys_list_sln_t *prev = NULL;
        app_sys_list_sll_tra(queue, curr) {
            /* 第一次:比较队列头 */
            if (prev == NULL && compare(node, curr)) {
                app_sys_list_sll_pinsert(queue, node);
                return;
            }
            /* 比较当前节点,如果命中则加入到当前节点前一节点之后*/
            if (prev != NULL && compare(node, curr)) {
                app_sys_list_sll_insert(queue, prev, node);
                return;
            }
            curr = prev;
        }
    }
    app_sys_list_sll_ainsert(queue, node);
}

/*@brief 队列节点出队列
 *@param queue 队列实例
 *@retval 队列节点实例
 */
app_sys_queue_dlpn_t * app_sys_queue_dlpq_dequeue(app_sys_queue_dlpq_t *queue)
{
    app_sys_list_dln_t *node = app_sys_list_dll_head(queue);
    app_sys_list_dll_remove(queue, node);
    return node;
}

/*@brief 队列节点入队列
 *@param queue   队列实例
 *@param node    队列节点实例
 *@param compare 队列节点入队规则(希望node1排在node2之前返回true,否则false)
 */
void app_sys_queue_dlpq_enqueue(app_sys_queue_dlpq_t *queue, app_sys_queue_dlpn_t *node,
                               bool (*compare)(app_sys_queue_dlpn_t *node1, app_sys_queue_dlpn_t *node2))
{
    if (compare != NULL) {
        /* 迭代所有元素 */
        app_sys_list_dln_t *prev = NULL;
        app_sys_list_dll_btra(queue, curr) {
            /* 第一次:比较队列头 */
            if (prev == NULL && compare(node, curr)) {
                app_sys_list_dll_pinsert(queue, NULL, node);
                return;
            }
            /* 比较当前节点,如果命中则加入到当前节点前一节点之后*/
            if (prev != NULL && compare(node, curr)) {
                app_sys_list_dll_ainsert(queue, prev, node);
                return;
            }
            prev = curr;
        }
    }
    app_sys_list_dll_ainsert(queue, NULL, node);
}

/*
 *链表容器复用:通用栈(Stack)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,栈顶即链表头
 */

/*@brief 栈节点出栈
 *@param stack 栈实例
 *@retval 栈节点实例
 */
app_sys_stack_sln_t * app_sys_stack_sls_pop(app_sys_stack_sls_t *stack)
{
    app_sys_list_sln_t *node = app_sys_list_sll_head(stack);
    app_sys_list_sll_remove(stack, NULL, node);
    return node;
}

/*@brief 栈节点入栈
 *@param stack 栈实例
 *@param node  栈节点实例
 */
void app_sys_stack_sls_push(app_sys_stack_sls_t *stack, app_sys_stack_sln_t *node)
{
    app_sys_list_sll_pinsert(stack, node);
}

/*@brief 栈节点出栈
 *@param stack 栈实例
 *@retval 栈节点实例
 */
app_sys_stack_dln_t * app_sys_stack_dls_pop(app_sys_stack_dls_t *stack)
{
    app_sys_list_dln_t *node = app_sys_list_dll_head(stack);
    app_sys_list_dll_remove(stack, node);
    return node;
}

/*@brief 栈节点入栈
 *@param stack 栈实例
 *@param node  栈节点实例
 */
void app_sys_stack_dls_push(app_sys_stack_dls_t *stack, app_sys_stack_dln_t *node)
{
    app_sys_list_dll_pinsert(stack, NULL, node);
}

/*
 *keep adding
 */
