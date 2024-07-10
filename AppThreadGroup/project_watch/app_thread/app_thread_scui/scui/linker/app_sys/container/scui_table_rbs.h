#ifndef SCUI_TABLE_RBS_H
#define SCUI_TABLE_RBS_H

/* 这个容器好像有问题, 暂时可以不去使用
 * table有充足的一级散列,可以降低节点的长度
 */

/* 转为适配app_sys_table_rbs接口
 */

#define scui_table_rbst_fd_t        app_sys_table_rbst_fd_t
#define scui_table_rbst_fc_t        app_sys_table_rbst_fc_t
#define scui_table_rbst_fv_t        app_sys_table_rbst_fv_t
#define scui_table_rbsn_t           app_sys_table_rbsn_t
#define scui_table_rbsl_t           app_sys_table_rbsl_t
#define scui_table_rbst_t           app_sys_table_rbst_t
#define scui_table_rbsn_reset       app_sys_table_rbsn_reset
#define scui_table_rbsl_reset       app_sys_table_rbsl_reset
#define scui_table_rbst_reset       app_sys_table_rbst_reset
#define scui_table_rbst_insert      app_sys_table_rbst_insert
#define scui_table_rbst_remove      app_sys_table_rbst_remove
#define scui_table_rbst_search      app_sys_table_rbst_search
#define scui_table_rbst_visit       app_sys_table_rbst_visit
#define scui_table_rbst_check       app_sys_table_rbst_check

#endif
