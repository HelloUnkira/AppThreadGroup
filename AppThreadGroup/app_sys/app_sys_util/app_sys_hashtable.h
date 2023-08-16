#ifndef APP_SYS_HASHTABLE_H
#define APP_SYS_HASHTABLE_H

/*
 *哈希表(Generics HashTable)
 *仅链表容器扩展的链地址法
 */

typedef app_sys_list_dn_t app_sys_hashtable_dn_t;
typedef app_sys_list_dl_t app_sys_hashtable_dl_t;

typedef uint32_t (*app_sys_hashtable_dt_fd_t)(app_sys_hashtable_dn_t *node);
typedef bool     (*app_sys_hashtable_dt_fc_t)(app_sys_hashtable_dn_t *node1, app_sys_hashtable_dn_t *node2);
typedef void     (*app_sys_hashtable_dt_fv_t)(app_sys_hashtable_dn_t *node,  uint32_t idx);

typedef struct {
    app_sys_hashtable_dt_fd_t digest;   /* 摘要函数,散列函数 */
    app_sys_hashtable_dt_fc_t confirm;  /* 比较函数 */
    app_sys_hashtable_dl_t   *list;     /* 散列链表 */
    uint32_t length;
} app_sys_hashtable_dt_t;

/*@brief     重置哈希表二级节点
 *@param[in] node 哈希表二级节点实例
 */
void app_sys_hashtable_dn_reset(app_sys_hashtable_dn_t *node);

/*@brief     重置哈希表一级链表
 *@param[in] list   哈希表一级链表实例
 *@param[in] length 哈希表一级链表实例长度
 */
void app_sys_hashtable_dl_reset(app_sys_hashtable_dl_t *list, uint32_t length);

/*@brief     重置哈希表
 *@param[in] table   哈希表实例
 *@param[in] digest  哈希散列函数,哈希摘要函数
 *@param[in] confirm 哈希比较函数
 *@param[in] list    哈希表一级链表实例
 *@param[in] length  哈希表一级链表实例长度
 */
void app_sys_hashtable_dt_reset(app_sys_hashtable_dt_t   *table,
                                app_sys_hashtable_dt_fd_t digest,
                                app_sys_hashtable_dt_fc_t confirm,
                                app_sys_hashtable_dl_t   *list,     uint32_t length);

/*@brief     哈希表插入节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_hashtable_dt_insert(app_sys_hashtable_dt_t *table, app_sys_hashtable_dn_t *node);

/*@brief     哈希表移除节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_hashtable_dt_remove(app_sys_hashtable_dt_t *table, app_sys_hashtable_dn_t *node);

/*@brief     哈希表匹配节点(查找)
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例(假,伪造)
 *@retval    哈希表二级节点实例(真)
 */
app_sys_hashtable_dn_t * app_sys_hashtable_dt_search(app_sys_hashtable_dt_t *table, app_sys_hashtable_dn_t *node);

/*@brief     哈希表访问所有节点
 *@param[in] table 哈希表实例
 *@param[in] visit 哈希访问函数
 */
void app_sys_hashtable_dt_visit(app_sys_hashtable_dt_t *table, app_sys_hashtable_dt_fv_t visit);

/*
 *keep adding
 */

#endif
