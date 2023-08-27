#ifndef APP_SYS_TABLE_DL_H
#define APP_SYS_TABLE_DL_H

typedef app_sys_list_dn_t app_sys_table_dln_t;
typedef app_sys_list_dl_t app_sys_table_dll_t;

typedef uint32_t (*app_sys_table_dlt_fd_t)(app_sys_table_dln_t *node);
typedef bool     (*app_sys_table_dlt_fc_t)(app_sys_table_dln_t *node1, app_sys_table_dln_t *node2);
typedef void     (*app_sys_table_dlt_fv_t)(app_sys_table_dln_t *node,  uint32_t idx);

typedef struct {
    app_sys_table_dlt_fd_t digest;   /* 摘要函数,散列函数 */
    app_sys_table_dlt_fc_t confirm;  /* 比较函数 */
    app_sys_table_dlt_fv_t visit;    /* 访问函数 */
    app_sys_table_dll_t   *list;     /* 散列链表 */
    uint32_t length;
} app_sys_table_dlt_t;

/*@brief     重置哈希表二级节点
 *@param[in] node 哈希表二级节点实例
 */
void app_sys_table_dln_reset(app_sys_table_dln_t *node);

/*@brief     重置哈希表一级链表
 *@param[in] list   哈希表一级链表实例
 *@param[in] length 哈希表一级链表实例长度
 */
void app_sys_table_dll_reset(app_sys_table_dll_t *list, uint32_t length);

/*@brief     重置哈希表
 *@param[in] table   哈希表实例
 *@param[in] digest  哈希散列函数,哈希摘要函数
 *@param[in] confirm 哈希比较函数
 *@param[in] visit   哈希访问函数
 *@param[in] list    哈希表一级链表实例
 *@param[in] length  哈希表一级链表实例长度
 */
void app_sys_table_dlt_reset(app_sys_table_dlt_t *table, app_sys_table_dlt_fd_t digest, app_sys_table_dlt_fc_t confirm, app_sys_table_dlt_fv_t visit, app_sys_table_dll_t *list, uint32_t length);

/*@brief     哈希表插入节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_table_dlt_insert(app_sys_table_dlt_t *table, app_sys_table_dln_t *node);

/*@brief     哈希表移除节点
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例
 */
void app_sys_table_dlt_remove(app_sys_table_dlt_t *table, app_sys_table_dln_t *node);

/*@brief     哈希表匹配节点(查找)
 *@param[in] table 哈希表实例
 *@param[in] node  哈希表二级节点实例(假,伪造)
 *@retval    哈希表二级节点实例(真)
 */
app_sys_table_dln_t * app_sys_table_dlt_search(app_sys_table_dlt_t *table, app_sys_table_dln_t *node);

/*@brief     哈希表访问所有节点
 *@param[in] table 哈希表实例
 */
void app_sys_table_dlt_visit(app_sys_table_dlt_t *table);

/*
 *keep adding
 */

#endif
