/*实现目标
 *    侵入式迷你哈希表
 *    依赖侵入式链表,链地址法
 */

#include "app_ext_lib.h"
#include "app_sys_list.h"
#include "app_sys_hashtable.h"

/*
 *哈希表(Generics HashTable)
 *仅链表容器扩展的链地址法
 */

/*@brief     重置哈希表二级节点
 *@param[in] node 哈希表二级节点实例
 */
void app_sys_hashtable_gn_reset(app_sys_hashtable_gn_t *node)
{
    app_sys_list_dn_reset((app_sys_list_dn_t *)node);
}

/*@brief     重置哈希表一级链表
 *@param[in] list   哈希表一级链表实例
 *@param[in] length 哈希表一级链表实例长度
 */
void app_sys_hashtable_gl_reset(app_sys_hashtable_gl_t *list, uint32_t length)
{
    for (uint32_t idx = 0; idx < length; idx++)
        app_sys_list_dl_reset((app_sys_list_dl_t *)&list[idx]);
}

/*@brief     重置哈希表
 *@param[in] table   哈希表实例
 *@param[in] digest  哈希散列函数,哈希摘要函数
 *@param[in] confirm 哈希比较函数
 *@param[in] list    哈希表一级链表实例
 *@param[in] length  哈希表一级链表实例长度
 */
void app_sys_hashtable_gt_reset(app_sys_hashtable_gt_t   *table,
                                app_sys_hashtable_gt_fd_t digest,
                                app_sys_hashtable_gt_fc_t confirm,
                                app_sys_hashtable_gl_t   *list,     uint32_t length)
{
    table->digest  = digest;
    table->confirm = confirm;
    table->list    = list;
    table->length  = length;
}

/*@brief     哈希表插入节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_hashtable_gt_insert(app_sys_hashtable_gt_t *table, app_sys_hashtable_gn_t *node)
{
    /* 散列,追加(对称语义,二选其一) */
    uint32_t idx = table->digest(node) % table->length;
    // app_sys_list_dl_pinsert((app_sys_list_dl_t *)&table->list[idx], NULL, (app_sys_list_dn_t *)node);
       app_sys_list_dl_ainsert((app_sys_list_dl_t *)&table->list[idx], NULL, (app_sys_list_dn_t *)node);
}

/*@brief     哈希表移除节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_hashtable_gt_remove(app_sys_hashtable_gt_t *table, app_sys_hashtable_gn_t *node)
{
    /* 散列,移除 */
    uint32_t idx = table->digest(node) % table->length;
    app_sys_list_dl_remove((app_sys_list_dl_t *)&table->list[idx], (app_sys_list_dn_t *)node);
}

/*@brief     哈希表匹配节点(查找)
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例(假,伪造)
 *@retval    哈希表二级节点实例(真)
 */
app_sys_hashtable_gn_t * app_sys_hashtable_gt_search(app_sys_hashtable_gt_t *table, app_sys_hashtable_gn_t *node)
{
    /* 散列,遍历 */
    uint32_t idx = table->digest(node) % table->length;
    /* 对称语义,二选其一 */
    // app_sys_list_dl_ftra((app_sys_list_dl_t *)&table->list[idx], curr)
       app_sys_list_dl_btra((app_sys_list_dl_t *)&table->list[idx], curr) {
        if (table->confirm(curr, node) == true)
            return curr;
    }
    return NULL;
}

/*@brief     哈希表访问所有节点
 *@param[in] table 哈希表实例
 *@param[in] visit 哈希访问函数
 */
void app_sys_hashtable_gt_visit(app_sys_hashtable_gt_t *table, app_sys_hashtable_gt_fv_t visit)
{
    for (uint32_t idx = 0; idx < table->length; idx++) {
        /* 对称语义,二选其一 */
        // app_sys_list_dl_ftra((app_sys_list_dl_t *)&table->list[idx], curr)
           app_sys_list_dl_btra((app_sys_list_dl_t *)&table->list[idx], curr) {
           visit(curr, idx);
        }
    }
}

/*
 *keep adding
 */