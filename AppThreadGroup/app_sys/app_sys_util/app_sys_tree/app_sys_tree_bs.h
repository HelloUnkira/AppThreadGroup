#ifndef APP_SYS_TREE_BS_H
#define APP_SYS_TREE_BS_H

/* 实现目标:B*树 */

/*
 *容器(B*树)
 *该树实现参考B*树原理
 *1)所有非叶子节点是维护条目
 *2)所有叶子节点是实际数据的目标
 *3)有一个动态分裂值(如:2/3)
 */

/* 节点状态 */
typedef enum {
    app_sys_tree_bsn_state_common = 0,  /* 非叶子节点 */
    app_sys_tree_bsn_state_leaves = 1,  /* 叶子节点 */
    app_sys_tree_bsn_error        = 2,
} app_sys_tree_bsn_status_t;

/* B*树核心(B*树容器) */
typedef struct app_sys_tree_bsn {
    struct app_sys_tree_bsn *parent;        /* 父亲 */
    struct app_sys_tree_bsn *brother_l;     /* 左兄弟 */
    struct app_sys_tree_bsn *brother_r;     /* 右兄弟 */
    void    **child_data_state;             /* 数据元集或节点集(携带节点状态) */
    uint32_t *key;                          /* 关键字集 */
    uint32_t  number;                       /* 当前数据量 */
} app_sys_tree_bsn_t;

/* 获得叶子节点的关键字 */
/* 获得节点的关键字集合与数据元集合 */
typedef uint32_t (*app_sys_tree_bst_key_t)(void *data);
typedef void     (*app_sys_tree_bst_visit_t)(uint8_t state, uint32_t number, uint32_t *key, void **child, bool destroy);

/* B*树核心(B*树集合) */
typedef struct {
    app_sys_tree_bsn_t *root;
    app_sys_tree_bst_key_t key;
    app_sys_tree_bst_visit_t visit;
    uint32_t length;  /* 节点和叶子数组限制 */
    uint32_t criter;  /* 节点分裂值(criterion) */
    uint32_t number;  /* 节点当前值 */
} app_sys_tree_bst_t;

/*@brief 插入函数
 *@param tree B*树实例
 *@param data 数据项
 */
void app_sys_tree_bst_insert(app_sys_tree_bst_t *tree, void *data);

/*@brief 删除函数
 *@param tree    B*树实例
 *@param data    数据项
 *@param key_val 关键字
 */
void app_sys_tree_bst_remove(app_sys_tree_bst_t *tree, void **data, uint32_t key_val);

/*@brief 查找函数
 *@param tree    B*树实例
 *@param data    数据项
 *@param key_val 关键字
 */
void app_sys_tree_bst_search(app_sys_tree_bst_t *tree, void **data, uint32_t key_val);

/*@brief 配置函数
 *@param tree    B*树实例
 *@param length 数据项
 *@param key    关键字获取语义
 *@param visit  节点访问语义
 */
void app_sys_tree_bst_config(app_sys_tree_bst_t *tree, uint32_t length, app_sys_tree_bst_key_t key, app_sys_tree_bst_visit_t visit);

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree    B*树实例
 *@param destroy 是否销毁
 */
uint32_t app_sys_tree_bst_seq_tra(app_sys_tree_bst_t *tree, bool destroy);

#endif
