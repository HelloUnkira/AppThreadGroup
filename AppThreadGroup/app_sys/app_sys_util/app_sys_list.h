#ifndef APP_SYS_LIST_H
#define APP_SYS_LIST_H

/*
 *双向链表
 */

struct app_sys_list_d {
    union {
        struct app_sys_list_d *prev;//双向链表上一节点
        struct app_sys_list_d *head;//双向链表头
    };
    union {
        struct app_sys_list_d *next;//双向链表下一节点
        struct app_sys_list_d *tail;//双向链表尾
    };
};

typedef struct app_sys_list_d app_sys_list_dl_t;
typedef struct app_sys_list_d app_sys_list_dn_t;

/*@brief     重置链表
 *@param[in] list 链表实例
 */
static inline void app_sys_list_dl_reset(app_sys_list_dl_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
static inline void app_sys_list_dn_reset(app_sys_list_dn_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
static inline app_sys_list_dn_t * app_sys_list_dl_head(app_sys_list_dl_t *list)
{
    return list->head;
}

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
static inline app_sys_list_dn_t * app_sys_list_dl_tail(app_sys_list_dl_t *list)
{
    return list->tail;
}

/*@brief     链表前向结点
 *@param[in] list 链表实例
 *@retval    前向结点
 */
static inline app_sys_list_dn_t * app_sys_list_dn_prev(app_sys_list_dn_t *node)
{
    return node->prev;
}

/*@brief     链表后向结点
 *@param[in] list 链表实例
 *@retval    后向结点
 */
static inline app_sys_list_dn_t * app_sys_list_dn_next(app_sys_list_dn_t *node)
{
    return node->next;
}

/*@brief     节点前插入(prepend)
 *           节点target为NULL时为头部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dl_pinsert(app_sys_list_dl_t *list, app_sys_list_dn_t *target, app_sys_list_dn_t *node);

/*@brief     节点后插入(append)
 *           节点target为NULL时为尾部插入
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_dl_ainsert(app_sys_list_dl_t *list,  app_sys_list_dn_t *target, app_sys_list_dn_t *node);

/*@brief     节点移除
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带移除节点)
 */
void app_sys_list_dl_remove(app_sys_list_dl_t *list, app_sys_list_dn_t *node);

/* 向后遍历链表宏(backward traverse) */
#define app_sys_list_dl_btra(list, node)  \
    for (app_sys_list_dn_t *node = app_sys_list_dl_head(list); node != NULL; node = app_sys_list_dn_next(node))
/* 向前遍历链表宏(forward traverse) */
#define app_sys_list_dl_ftra(list, node)  \
    for (app_sys_list_dn_t *node = app_sys_list_dl_tail(list); node != NULL; node = app_sys_list_dn_prev(node))

/*
 *单向链表
 */

struct app_sys_list_s_n {
    struct app_sys_list_s_n *buddy; //单向链表就近项
};

struct app_sys_list_s_l {
    struct app_sys_list_s_n *head; //单向链表头
    struct app_sys_list_s_n *tail; //单向链表尾
};

typedef struct app_sys_list_s_l app_sys_list_sl_t;
typedef struct app_sys_list_s_n app_sys_list_sn_t;

/*@brief     重置链表
 *@param[in] list 链表实例
 */
static inline void app_sys_list_sl_reset(app_sys_list_sl_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/*@brief     重置链表节点
 *@param[in] list 链表节点实例
 */
static inline void app_sys_list_sn_reset(app_sys_list_sn_t *node)
{
    node->buddy = NULL;
}

/*@brief     链表头结点
 *@param[in] list 链表实例
 *@retval    头结点
 */
static inline app_sys_list_sn_t * app_sys_list_sl_head(app_sys_list_sl_t *list)
{
    return list->head;
}

/*@brief     链表尾结点
 *@param[in] list 链表实例
 *@retval    尾结点
 */
static inline app_sys_list_sn_t * app_sys_list_sl_tail(app_sys_list_sl_t *list)
{
    return list->head;
}

/*@brief     链表临近结点
 *@param[in] list 链表实例
 *@retval    临近结点
 */
static inline app_sys_list_sn_t * app_sys_list_sn_buddy(app_sys_list_sn_t *node)
{
    return node->buddy;
}

/*@brief     节点头前插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sl_pinsert(app_sys_list_sl_t *list, app_sys_list_sn_t *node);

/*@brief     节点尾后插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sl_ainsert(app_sys_list_sl_t *list, app_sys_list_sn_t *node);

/*@brief     节点插入
 *           单链表插入需要指定目标节点,target不为NULL
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sl_insert(app_sys_list_sl_t *list, app_sys_list_sn_t *target, app_sys_list_sn_t *node);

/*@brief     节点插入
 *           单链表删除需要指定目标节点
 *@param[in] list   链表实例
 *@param[in] target 链表节点实例(锚点)
 *@param[in] node   链表节点实例(带插入节点)
 */
void app_sys_list_sl_remove(app_sys_list_sl_t *list, app_sys_list_sn_t *target, app_sys_list_sn_t *node);

/* 遍历链表宏 */
#define app_sys_list_sl_tra(list, node)   \
    for (app_sys_list_sn_t *node = app_sys_list_sl_head(list); node != NULL; node = app_sys_list_sn_buddy(node))

/*
 *链表容器复用:通用队列
 */

typedef app_sys_list_sl_t app_sys_queue_gq_t;
typedef app_sys_list_sn_t app_sys_queue_gn_t;

/*@brief     重置队列
 *@param[in] queue 队列实例
 */
static inline void app_sys_queue_gq_reset(app_sys_queue_gq_t *queue)
{
    app_sys_list_sl_reset((app_sys_list_sl_t *)queue);
}

/*@brief     重置队列节点
 *@param[in] node 队列节点实例
 */
static inline void app_sys_queue_gn_reset(app_sys_queue_gn_t *node)
{
    app_sys_list_sn_reset((app_sys_list_sn_t *)node);
}

/*@brief     队列节点入队列
 *@param[in] queue 队列实例
 *@param[in] node  队列节点实例
 */
static inline void app_sys_queue_gq_enqueue(app_sys_queue_gq_t *queue, app_sys_queue_gn_t *node)
{
    app_sys_list_sl_ainsert((app_sys_list_sl_t *)queue, (app_sys_list_sn_t *)node);
}

/*@brief     队列节点出队列
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
static inline app_sys_queue_gn_t * app_sys_queue_gq_dequeue(app_sys_queue_gq_t *queue)
{
    app_sys_list_sn_t *node = app_sys_list_sl_head((app_sys_list_sl_t *)queue);
    app_sys_list_sl_remove((app_sys_list_sl_t *)queue, NULL, node);
    return (app_sys_queue_gn_t *)node;
}

/*@brief     队列访问队列头节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
static inline app_sys_queue_gn_t * app_sys_queue_gq_first(app_sys_queue_gq_t *queue)
{
    return (app_sys_queue_gn_t *)app_sys_list_sl_head((app_sys_list_sl_t *)queue);
}

/*@brief     队列访问队列尾节点
 *@param[in] queue 队列实例
 *@retval    队列节点实例
 */
static inline app_sys_queue_gn_t * app_sys_queue_gq_last(app_sys_queue_gq_t *queue)
{
    return (app_sys_queue_gn_t *)app_sys_list_sl_tail((app_sys_list_sl_t *)queue);
}

/*
 *链表容器复用:通用栈
 */

typedef app_sys_list_sl_t app_sys_stack_gs_t;
typedef app_sys_list_sn_t app_sys_stack_gn_t;

/*@brief     重置栈
 *@param[in] stack 栈实例
 */
static inline void app_sys_stack_gs_reset(app_sys_stack_gs_t *stack)
{
    app_sys_list_sl_reset((app_sys_list_sl_t *)stack);
}

/*@brief     重置栈节点
 *@param[in] node 栈节点实例
 */ 
static inline void app_sys_stack_gn_reset(app_sys_stack_gn_t *node)
{
    app_sys_list_sn_reset((app_sys_list_sn_t *)node);
}

/*@brief     栈节点入栈
 *@param[in] stack 栈实例
 *@param[in] node  栈节点实例
 */
static inline void app_sys_stack_gs_push(app_sys_stack_gs_t *stack, app_sys_stack_gn_t *node)
{
    app_sys_list_sl_pinsert((app_sys_list_sl_t *)stack, (app_sys_list_sn_t *)node);
}

/*@brief     栈节点出栈
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
static inline app_sys_stack_gn_t * app_sys_stack_gs_pop(app_sys_stack_gs_t *stack)
{
    app_sys_list_sn_t *node = app_sys_list_sl_head((app_sys_list_sl_t *)stack);
    app_sys_list_sl_remove((app_sys_list_sl_t *)stack, NULL, node);
    return (app_sys_stack_gn_t *)node;
}

/*@brief     栈访问栈顶节点
 *@param[in] stack 栈实例
 *@retval    栈节点实例
 */
static inline app_sys_stack_gn_t * app_sys_stack_gs_top(app_sys_stack_gs_t *stack)
{
    return (app_sys_stack_gn_t *)app_sys_list_sl_head((app_sys_list_sl_t *)stack);
}

/*
 *keep adding
 */

#endif
