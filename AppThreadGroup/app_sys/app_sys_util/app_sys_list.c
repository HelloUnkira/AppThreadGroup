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
            list->tail = node;
        list->head = node;
    }
    if (target != NULL) {
        /* 更新插入节点 */
        node->prev = target->prev;
        node->next = target;
        /* 前项节点存在时 */
        if (target->prev != NULL)
            target->prev->next = node;
        if (target->prev == NULL)
            list->head = node;
        /* 更新后项节点 */
        target->prev = node;
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
            list->head = node;
        list->tail = node;
    }
    if (target != NULL) {
        /* 更新插入节点 */
        node->prev = target;
        node->next = target->next;
        /* 后项节点存在时 */
        if (target->next != NULL)
            target->next->prev = node;
        if (target->next == NULL)
            list->tail = node;
        /* 更新前项节点 */
        target->next = node;
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
        list->head = node->next;
    if (node->next == NULL)
        list->tail = node->prev;
    /* 接触关联索引 */
    if (node->prev != NULL)
        node->prev->next = node->next;
    if (node->next != NULL)
        node->next->prev = node->prev;
}

/*
 *单向链表
 */

/*@brief     节点头前插入
 *@param[in] list 链表实例
 *@param[in] node 链表节点实例(带插入节点)
 */
void app_sys_list_sl_pinsert(app_sys_list_sl_t *list, app_sys_list_sn_t *node)
{
    node->buddy = list->head;
    if (list->head == NULL)
        list->tail = node;
    list->head = node;
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
        list->head = node;
    list->tail = node;
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
        list->tail = node;
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
        list->head = node->buddy;
    if (list->tail == node)
        list->tail = target;
}
