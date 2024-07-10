#ifndef APP_SYS_TABLE_RBS_H
#define APP_SYS_TABLE_RBS_H

typedef app_sys_tree_rbsn_t   app_sys_table_rbsn_t;
typedef app_sys_tree_rbsn_t * app_sys_table_rbsl_t;

typedef uint32_t (*app_sys_table_rbst_fd_t)(app_sys_table_rbsn_t *node);
typedef uint8_t  (*app_sys_table_rbst_fc_t)(app_sys_table_rbsn_t *node1, app_sys_table_rbsn_t *node2);
typedef void     (*app_sys_table_rbst_fv_t)(app_sys_table_rbsn_t *node,  uint32_t idx);

typedef struct {
    app_sys_tree_rbst_t     rbst_tree;
    app_sys_table_rbst_fd_t digest;   /* 摘要函数,散列函数 */
    app_sys_table_rbst_fc_t compare;  /* 比较函数 */
    app_sys_table_rbst_fc_t confirm;  /* 比较函数 */
    app_sys_table_rbst_fv_t visit;    /* 访问函数 */
    app_sys_table_rbsl_t   *list;     /* 散列链表 */
    uint32_t length;
} app_sys_table_rbst_t;

/*@brief 重置哈希表二级节点
 *@param node 哈希表二级节点实例
 */
void app_sys_table_rbsn_reset(app_sys_table_rbsn_t *node);

/*@brief 重置哈希表一级链表
 *@param list   哈希表一级链表实例
 *@param length 哈希表一级链表实例长度
 */
void app_sys_table_rbsl_reset(app_sys_table_rbsl_t *list, uint32_t length);

/*@brief 重置哈希表
 *@param table   哈希表实例
 *@param digest  哈希散列函数,哈希摘要函数
 *@param compare 哈希比较函数
 *@param confirm 哈希比较函数
 *@param visit   哈希访问函数
 *@param list    哈希表一级链表实例
 *@param length  哈希表一级链表实例长度
 */
void app_sys_table_rbst_reset(app_sys_table_rbst_t *table, app_sys_table_rbst_fd_t digest, app_sys_table_rbst_fc_t compare, app_sys_table_rbst_fc_t confirm, app_sys_table_rbst_fv_t visit, app_sys_table_rbsl_t *list, uint32_t length);

/*@brief 哈希表插入节点
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例
 */
void app_sys_table_rbst_insert(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node);

/*@brief 哈希表移除节点
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例
 */
void app_sys_table_rbst_remove(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node);

/*@brief 哈希表匹配节点(查找)
 *@param table 哈希表实例
 *@param node  哈希表二级节点实例(假,伪造)
 *@retval 哈希表二级节点实例(真)
 */
app_sys_table_rbsn_t * app_sys_table_rbst_search(app_sys_table_rbst_t *table, app_sys_table_rbsn_t *node);

/*@brief 哈希表访问所有节点
 *@param table 哈希表实例
 */
void app_sys_table_rbst_visit(app_sys_table_rbst_t *table);

/*@brief 哈希表检查所有节点
 *@param table 哈希表实例
 */
bool app_sys_table_rbst_check(app_sys_table_rbst_t *table);

/*
 *keep adding
 */

#endif
