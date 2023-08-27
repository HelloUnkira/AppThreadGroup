/*实现目标:
 *    侵入式哈希表
 *    侵入式红黑树容器扩展的链地址法
 *    从当前地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief     重置哈希表二级节点
 *@param[in] node 哈希表二级节点实例
 */
void app_sys_table_rbsn_reset(app_sys_table_rbsn_t *node)
{
    app_sys_tree_rbsn_reset(node);
}

/*@brief     重置哈希表一级链表
 *@param[in] list   哈希表一级链表实例
 *@param[in] length 哈希表一级链表实例长度
 */
void app_sys_table_rbsl_reset(app_sys_table_rbsl_t *list, uint32_t length)
{
    for (uint32_t idx = 0; idx < length; list[idx] = NULL, idx++);
}

/*@brief     重置哈希表
 *@param[in] table   哈希表实例
 *@param[in] digest  哈希散列函数,哈希摘要函数
 *@param[in] compare 哈希比较函数
 *@param[in] confirm 哈希比较函数
 *@param[in] visit   哈希访问函数
 *@param[in] list    哈希表一级链表实例
 *@param[in] length  哈希表一级链表实例长度
 */
void app_sys_table_rbst_reset(app_sys_table_rbst_t *table, app_sys_table_rbst_fd_t digest, app_sys_table_rbst_fc_t compare, app_sys_table_rbst_fc_t confirm, app_sys_table_rbst_fv_t visit, app_sys_table_rbsl_t *list, uint32_t length)
{
    table->digest  = digest;
    table->compare = compare;
    table->confirm = confirm;
    table->visit   = visit;
    table->list    = list;
    table->length  = length;
    app_sys_tree_rbst_reset(&table->rbst_tree);
    app_sys_tree_rbst_config(&table->rbst_tree, table->compare, table->confirm, NULL);
    /* 我们不使用visit语义,因为我们不使用层级遍历接口:app_sys_tree_rbst_seq_tra */
}

/*@brief     哈希表插入节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_table_rbst_insert(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node)
{
    /* 散列,追加(对称语义,二选其一) */
    uint32_t idx = table->digest(node) % table->length;
    app_sys_tree_rbsn_t *root = table->list[idx];
    app_sys_tree_rbst_root_set(&table->rbst_tree,  root);
    app_sys_tree_rbst_insert(&table->rbst_tree, node);
    app_sys_tree_rbst_root_get(&table->rbst_tree, &root);
    table->list[idx] = root;
}

/*@brief     哈希表移除节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_table_rbst_remove(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node)
{
    /* 散列,移除 */
    uint32_t idx = table->digest(node) % table->length;
    app_sys_tree_rbsn_t *root = table->list[idx];
    app_sys_tree_rbst_root_set(&table->rbst_tree,  root);
    app_sys_tree_rbst_remove(&table->rbst_tree, node);
    app_sys_tree_rbst_root_get(&table->rbst_tree, &root);
    table->list[idx] = root;
}

/*@brief     哈希表匹配节点(查找)
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例(假,伪造)
 *@retval    哈希表二级节点实例(真)
 */
app_sys_table_rbsn_t * app_sys_table_rbst_search(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node)
{
    /* 散列,遍历 */
    uint32_t idx = table->digest(node) % table->length;
    app_sys_tree_rbsn_t *target = NULL;
    app_sys_tree_rbsn_t *root = table->list[idx];
    app_sys_tree_rbst_root_set(&table->rbst_tree,  root);
    target = app_sys_tree_rbst_search(&table->rbst_tree, node);
    app_sys_tree_rbst_root_get(&table->rbst_tree, &root);
    table->list[idx] = root;
    return target;
}

/*@brief     哈希表访问所有节点
 *@param[in] table 哈希表实例
 */
void app_sys_table_rbst_visit(app_sys_table_rbst_t *table)
{
    for (uint32_t idx = 0; idx < table->length; idx++) {
        app_sys_tree_rbsn_t *root = table->list[idx];
        app_sys_tree_rbst_root_set(&table->rbst_tree,  root);
        /* 对称语义,二选其一 */
        // app_sys_tree_rbst_iter_ftra(&table->rbst_tree, curr)
           app_sys_tree_rbst_iter_btra(&table->rbst_tree, curr) {
               table->visit(curr, idx);
           }
        app_sys_tree_rbst_root_get(&table->rbst_tree, &root);
        table->list[idx] = root;
    }
}

/*
 *keep adding
 */
