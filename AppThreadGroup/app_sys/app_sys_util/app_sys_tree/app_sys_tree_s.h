#ifndef	APP_SYS_TREE_S_H
#define	APP_SYS_TREE_S_H

/* 实现目标:侵入式伸展树 */

/* 节点关系,节点颜色 */
typedef enum {
    app_sys_tree_sn_side_l = 0,     /* 左孩子方向 */
    app_sys_tree_sn_side_r = 1,     /* 右孩子方向 */
    app_sys_tree_sn_error  = 2,
} app_sys_tree_sn_status_t;

/* 伸展树容器(节点) */
typedef struct app_sys_tree_sn {
    /* 使用堆栈维护伸展树可以去除parent指针 */
    /* 将空间占用进一步其紧缩到双向链表大小,这里不使用 */
    struct app_sys_tree_sn *parent;     /* 节点父亲 */
    struct app_sys_tree_sn *child_l;    /* 节点左孩子 */
    struct app_sys_tree_sn *child_r;    /* 节点右孩子 */
} app_sys_tree_sn_t;

/* 比较函数:node1严格小于node1返回非零值 */
typedef uint8_t (*app_sys_tree_st_compare_t)(app_sys_tree_sn_t *node1, app_sys_tree_sn_t *node2);
/* 匹配函数:node1与node2的关键字一致返回0 */
typedef uint8_t (*app_sys_tree_st_confirm_t)(app_sys_tree_sn_t *node1, app_sys_tree_sn_t *node2);
/* 访问函数:节点 */
typedef void (*app_sys_tree_st_visit_t)(app_sys_tree_sn_t *node);

/* 伸展树集合(树根) */
typedef struct {
    app_sys_tree_sn_t *root;
    app_sys_tree_st_compare_t compare;
    app_sys_tree_st_confirm_t confirm;
    app_sys_tree_st_visit_t   visit;
} app_sys_tree_st_t;

/*@brief 插入函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_insert(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 删除函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_remove(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 搜索函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 搜索函数(前驱节点)
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_prev(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 搜索函数(后继节点)
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_next(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 搜索函数(最小节点)
 *@param tree 伸展树实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_min(app_sys_tree_st_t *tree);

/*@brief 搜索函数(最大节点)
 *@param tree 伸展树实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_max(app_sys_tree_st_t *tree);

/*@brief 伸展树合并
 *@param tree  伸展树实例
 *@param tree1 伸展树实例
 *@param tree2 伸展树实例
 */
void app_sys_tree_st_join(app_sys_tree_st_t *tree, app_sys_tree_st_t *tree1, app_sys_tree_st_t *tree2);

/*@brief 伸展树分裂
 *@param tree  伸展树实例
 *@param node  伸展节点实例
 *@param tree1 伸展树实例
 *@param tree2 伸展树实例
 */
void app_sys_tree_st_split(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node, app_sys_tree_st_t *tree1, app_sys_tree_st_t *tree2);

/* 向后遍历伸展树 */
#define app_sys_tree_st_btra(tree, node)    \
    for (app_sys_tree_sn_t *node = app_sys_tree_st_search_min(tree); node != NULL; node = app_sys_tree_st_search_next(node))
/* 向前遍历伸展树 */
#define app_sys_tree_st_ftra(tree, node)    \
    for (app_sys_tree_sn_t *node = app_sys_tree_st_search_max(tree); node != NULL; node = app_sys_tree_st_search_prev(node))

/*@brief 复位函数
 *@param tree 伸展树实例
 */
void app_sys_tree_st_reset(app_sys_tree_st_t *tree);

/*@brief 复位函数
 *@param node 伸展节点实例
 */
void app_sys_tree_sn_reset(app_sys_tree_sn_t *node);

/*@brief 配置函数
 *@param tree 伸展树实例
 *@param compare 伸展树比较语义
 *@param confirm 伸展树确认语义
 *@param visit   伸展树访问语义
 */
void app_sys_tree_st_config(app_sys_tree_st_t *tree, app_sys_tree_st_compare_t compare, app_sys_tree_st_confirm_t confirm, app_sys_tree_st_visit_t visit);

/*@brief 根切换函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_root_set(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node);

/*@brief 根切换函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_root_get(app_sys_tree_st_t *tree, app_sys_tree_sn_t **node);

/*扩展组件:
 *层序遍历,需要提供一个足够大的队列或栈(不超过所有节点大小)
 *迭代过程不可以做修改动作,因为队列或栈状态不一致
 */

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree  伸展树实例
 *@param queue 伸展树节点实例队列数组
 *@param len   伸展树节点实例队列数组长度
 */
void app_sys_tree_st_seq_tra(app_sys_tree_st_t *tree, app_sys_tree_sn_t **queue, int32_t len);

#endif
