#ifndef APP_SYS_LIST_L_H
#define APP_SYS_LIST_L_H

/*
 *双向链表(Double Linked List)
 */

struct app_sys_list_dl {
    union {
        struct app_sys_list_dl *prev;//双向链表上一节点
        struct app_sys_list_dl *head;//双向链表头
    };
    union {
        struct app_sys_list_dl *next;//双向链表下一节点
        struct app_sys_list_dl *tail;//双向链表尾
    };
};

typedef struct app_sys_list_dl app_sys_list_dll_t;
typedef struct app_sys_list_dl app_sys_list_dln_t;

/*@brief     重置链表
 *@param[in] list 链表实例
 */
void app_sys_list_dll_reset(app_sys_list_dll_t *list);

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
void app_sys_list_dln_reset(app_sys_list_dln_t *node);

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
app_sys_list_dln_t * app_sys_list_dll_head(app_sys_list_dll_t *list);

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
app_sys_list_dln_t * app_sys_list_dll_tail(app_sys_list_dll_t *list);

/*@brief     链表前向结点
 *@param[in] list 链表实例
 *@retval    前向结点
 */
app_sys_list_dln_t * app_sys_list_dln_prev(app_sys_list_dln_t *node);

/*@brief     链表后向结点
 *@param[in] list 链表实例
 *@retval    后向结点
 */
app_sys_list_dln_t * app_sys_list_dln_next(app_sys_list_dln_t *node);

/*@brief     节点前插入(prepend)
 *           节点target为NULL时为头部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dll_pinsert(app_sys_list_dll_t *list, app_sys_list_dln_t *target, app_sys_list_dln_t *node);

/*@brief     节点后插入(append)
 *           节点target为NULL时为尾部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dll_ainsert(app_sys_list_dll_t *list,  app_sys_list_dln_t *target, app_sys_list_dln_t *node);

/*@brief     节点移除
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带移除节点)
 */
void app_sys_list_dll_remove(app_sys_list_dll_t *list, app_sys_list_dln_t *node);

/* 向后遍历链表宏(backward traverse) */
#define app_sys_list_dll_btra(list, node)  \
    for (app_sys_list_dln_t *node = app_sys_list_dll_head(list); node != NULL; node = app_sys_list_dln_next(node))
/* 向前遍历链表宏(forward traverse) */
#define app_sys_list_dll_ftra(list, node)  \
    for (app_sys_list_dln_t *node = app_sys_list_dll_tail(list); node != NULL; node = app_sys_list_dln_prev(node))

/*
 *单向链表(Single Linked List)
 */

struct app_sys_list_sln {
    struct app_sys_list_sln *buddy; //单向链表就近项
};

struct app_sys_list_sll {
    struct app_sys_list_sln *head; //单向链表头
    struct app_sys_list_sln *tail; //单向链表尾
};

typedef struct app_sys_list_sll app_sys_list_sll_t;
typedef struct app_sys_list_sln app_sys_list_sln_t;

/*@brief     重置链表
 *@param[in] list 链表实例
 */
void app_sys_list_sll_reset(app_sys_list_sll_t *list);

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
void app_sys_list_sln_reset(app_sys_list_sln_t *node);

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
app_sys_list_sln_t * app_sys_list_sll_head(app_sys_list_sll_t *list);

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
app_sys_list_sln_t * app_sys_list_sll_tail(app_sys_list_sll_t *list);

/*@brief     链表临近结点
 *@param[in] list 链表实例
 *@retval    临近结点
 */
app_sys_list_sln_t * app_sys_list_sln_buddy(app_sys_list_sln_t *node);

/*@brief     节点头前插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sll_pinsert(app_sys_list_sll_t *list, app_sys_list_sln_t *node);

/*@brief     节点尾后插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sll_ainsert(app_sys_list_sll_t *list, app_sys_list_sln_t *node);

/*@brief     节点插入
 *           单链表插入需要指定目标节点,target不为NULL
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sll_insert(app_sys_list_sll_t *list, app_sys_list_sln_t *target, app_sys_list_sln_t *node);

/*@brief     节点插入
 *           单链表删除需要指定目标节点
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sll_remove(app_sys_list_sll_t *list, app_sys_list_sln_t *target, app_sys_list_sln_t *node);

/* 遍历链表宏 */
#define app_sys_list_sll_tra(list, node)   \
    for (app_sys_list_sln_t *node = app_sys_list_sll_head(list); node != NULL; node = app_sys_list_sln_buddy(node))

/*
 *链表容器复用:通用队列(Queue)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,队列头即链表头,队列尾即链表尾
 */

typedef app_sys_list_sll_t app_sys_queue_slq_t;
typedef app_sys_list_sln_t app_sys_queue_sln_t;
typedef app_sys_list_dll_t app_sys_queue_dlq_t;
typedef app_sys_list_dln_t app_sys_queue_dln_t;

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_sln_t * app_sys_queue_slq_dequeue(app_sys_queue_slq_t *queue);

/*@brief     队列节点入队列
 *@param[in] queue 队列实例
 *@param[in] node  队列节点实例
 */
void app_sys_queue_slq_enqueue(app_sys_queue_slq_t *queue, app_sys_queue_sln_t *node);

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_dln_t * app_sys_queue_dlq_dequeue(app_sys_queue_dlq_t *queue);

/*@brief     队列节点入队列
 *@param[in] queue 队列实例
 *@param[in] node  队列节点实例
 */
void app_sys_queue_dlq_enqueue(app_sys_queue_dlq_t *queue, app_sys_queue_dln_t *node);

/*
 *链表容器复用:优先队列(Priority Queue)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,队列头即链表头,队列尾即链表尾
 */

typedef app_sys_list_sll_t app_sys_queue_slpq_t;
typedef app_sys_list_sln_t app_sys_queue_slpn_t;
typedef app_sys_list_dll_t app_sys_queue_dlpq_t;
typedef app_sys_list_dln_t app_sys_queue_dlpn_t;

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_slpn_t * app_sys_queue_slpq_dequeue(app_sys_queue_slpq_t *queue);

/*@brief     队列节点入队列
 *@param[in] queue   队列实例
 *@param[in] node    队列节点实例
 *@param[in] compare 队列节点入队规则(希望node1排在node2之前返回true,否则false)
 */
void app_sys_queue_slpq_enqueue(app_sys_queue_slpq_t *queue, app_sys_queue_slpn_t *node,
                               bool (*compare)(app_sys_queue_slpn_t *node1, app_sys_queue_slpn_t *node2));

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
app_sys_queue_dlpn_t * app_sys_queue_dlpq_dequeue(app_sys_queue_dlpq_t *queue);

/*@brief     队列节点入队列
 *@param[in] queue   队列实例
 *@param[in] node    队列节点实例
 *@param[in] compare 队列节点入队规则(希望node1排在node2之前返回true,否则false)
 */
void app_sys_queue_dlpq_enqueue(app_sys_queue_dlpq_t *queue, app_sys_queue_dlpn_t *node,
                               bool (*compare)(app_sys_queue_dlpn_t *node1, app_sys_queue_dlpn_t *node2));

/*
 *链表容器复用:通用栈(Stack)
 *因为它是完全意义上的复用,但不应该不受限制的使用
 *所以保留了链表的基本操作,栈顶即链表头
 */

typedef app_sys_list_sll_t app_sys_stack_sls_t;
typedef app_sys_list_sln_t app_sys_stack_sln_t;
typedef app_sys_list_dll_t app_sys_stack_dls_t;
typedef app_sys_list_dln_t app_sys_stack_dln_t;

/*@brief     栈节点出栈
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
app_sys_stack_sln_t * app_sys_stack_sls_pop(app_sys_stack_sls_t *stack);

/*@brief     栈节点入栈
 *@param[in] stack 栈实例
 *@param[in] node  栈节点实例
 */
void app_sys_stack_sls_push(app_sys_stack_sls_t *stack, app_sys_stack_sln_t *node);

/*@brief     栈节点出栈
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
app_sys_stack_dln_t * app_sys_stack_dls_pop(app_sys_stack_dls_t *stack);

/*@brief     栈节点入栈
 *@param[in] stack 栈实例
 *@param[in] node  栈节点实例
 */
void app_sys_stack_dls_push(app_sys_stack_dls_t *stack, app_sys_stack_dln_t *node);

/*
 *keep adding
 */

#endif
