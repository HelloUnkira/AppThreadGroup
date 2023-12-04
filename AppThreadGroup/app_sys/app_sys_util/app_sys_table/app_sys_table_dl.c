/*实现目标:
 *    侵入式哈希表
 *    双链表容器扩展的链地址法
 *    从当前地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 重置哈希表二级节点
 *@param node 哈希表二级节点实例
 */
void app_sys_table_dln_reset(app_sys_table_dln_t *node)
{
    app_sys_list_dln_reset(node);
}

/*@brief 重置哈希表一级链表
 *@param list   哈希表一级链表实例
 *@param length 哈希表一级链表实例长度
 */
void app_sys_table_dll_reset(app_sys_table_dll_t *list, uint32_t length)
{
    for (uint32_t idx = 0; idx < length; idx++)
        app_sys_list_dll_reset(&list[idx]);
}

/*@brief 重置哈希表
 *@param table   哈希表实例
 *@param digest  哈希散列函数,哈希摘要函数
 *@param confirm 哈希比较函数
 *@param visit   哈希访问函数
 *@param list    哈希表一级链表实例
 *@param length  哈希表一级链表实例长度
 */
void app_sys_table_dlt_reset(app_sys_table_dlt_t *table, app_sys_table_dlt_fd_t digest, app_sys_table_dlt_fc_t confirm, app_sys_table_dlt_fv_t visit, app_sys_table_dll_t *list, uint32_t length)
{
    table->digest  = digest;
    table->confirm = confirm;
    table->visit   = visit;
    table->list    = list;
    table->length  = length;
}

/*@brief 哈希表插入节点
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例
 */
void app_sys_table_dlt_insert(app_sys_table_dlt_t *table, app_sys_table_dln_t *node)
{
    /* 散列,追加(对称语义,二选其一) */
    uint32_t idx = table->digest(node) % table->length;
    // app_sys_list_dll_pinsert(&table->list[idx], NULL, node);
       app_sys_list_dll_ainsert(&table->list[idx], NULL, node);
}

/*@brief 哈希表移除节点
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例
 */
void app_sys_table_dlt_remove(app_sys_table_dlt_t *table, app_sys_table_dln_t *node)
{
    /* 散列,移除 */
    uint32_t idx = table->digest(node) % table->length;
    app_sys_list_dll_remove(&table->list[idx], node);
}

/*@brief 哈希表匹配节点(查找)
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例(假,伪造)
 *@retval 哈希表二级节点实例(真)
 */
app_sys_table_dln_t * app_sys_table_dlt_search(app_sys_table_dlt_t *table, app_sys_table_dln_t *node)
{
    /* 散列,遍历 */
    uint32_t idx = table->digest(node) % table->length;
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&table->list[idx], curr)
       app_sys_list_dll_btra(&table->list[idx], curr) {
        if (table->confirm(curr, node) == true)
            return curr;
    }
    return NULL;
}

/*@brief 哈希表访问所有节点
 *@param table 哈希表实例
 */
void app_sys_table_dlt_visit(app_sys_table_dlt_t *table)
{
    for (uint32_t idx = 0; idx < table->length; idx++) {
        /* 对称语义,二选其一 */
        // app_sys_list_dll_ftra(&table->list[idx], curr)
           app_sys_list_dll_btra(&table->list[idx], curr) {
           table->visit(curr, idx);
        }
    }
}

/*
 *keep adding
 */
