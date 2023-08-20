#ifndef APP_SYS_TREE_RB_H
#define APP_SYS_TREE_RB_H

/* 实现目标:侵入式红黑树 */

/*
 *容器(红黑树)
 *该树实现由Linux源码改编
 *注:红黑树规则
 *1)节点为红色或黑色
 *2)根结点是黑色的
 *3)所有叶(NULL)是黑色的
 *4)每个红结点的两个子结点都是黑色的
 *5)从根到叶的每条简单路径包含相同数量的黑节点
 *4和5保证了O(log n)
 *因为4意味着一条路径上不可能有两个连续的红结点
 *因此每个红结点后面都有一个黑结点
 *所以如果B是每条简单路径上的黑结点数
 *那么从4出发的最长可能路径就是2B
 */

/* 节点关系,节点颜色 */
typedef enum {
    app_sys_tree_rbn_side_l  = 0,   /* 左孩子方向 */
    app_sys_tree_rbn_side_r  = 1,   /* 右孩子方向 */
    app_sys_tree_rbn_color_r = 0,   /* 红色节点 */
    app_sys_tree_rbn_color_b = 1,   /* 黑色节点 */
    app_sys_tree_rbn_error   = 2,
} app_sys_tree_rbn_status_t;

/* 红黑树容器(节点) */
typedef struct app_sys_tree_rbn {
    /* 使用堆栈维护红黑树可以去除parent指针,然后将颜色值挂载到其他俩个孩子之一 */
    /* 将空间占用进一步其紧缩到双向链表大小,这里不使用 */
    struct app_sys_tree_rbn *parent_color;  /* 节点父亲(以及颜色) */
    struct app_sys_tree_rbn *child_l;       /* 节点左孩子 */
    struct app_sys_tree_rbn *child_r;       /* 节点右孩子 */
} app_sys_tree_rbn_t;

/* 比较函数:node1严格小于node2返回非零值 */
typedef uint8_t (*app_sys_tree_rbt_compare_t)(app_sys_tree_rbn_t *node1, app_sys_tree_rbn_t *node2);
/* 匹配函数:node1与node2的关键字一致返回0 */
typedef uint8_t (*app_sys_tree_rbt_confirm_t)(app_sys_tree_rbn_t *node1, app_sys_tree_rbn_t *node2);
/* 访问函数 */
typedef void (*app_sys_tree_rbt_visit_t)(app_sys_tree_rbn_t *node, uint8_t color);

/* 红黑树集合(树根) */
typedef struct {
    app_sys_tree_rbn_t *root;
    app_sys_tree_rbt_compare_t compare;
    app_sys_tree_rbt_confirm_t confirm;
} app_sys_tree_rbt_t;

/*@brief         插入函数
 *@param[in,out] tree 红黑树实例
 *@param[in,out] node 红黑节点实例
 */
void app_sys_tree_rbt_insert(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node);

/*@brief         删除函数
 *@param[in,out] tree 红黑树实例
 *@param[in,out] node 红黑节点实例
 */
void app_sys_tree_rbt_remove(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node);

/*@brief     搜索函数
 *@param[in] tree 红黑树实例
 *@param[in] node 红黑节点实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node);

/*@brief     搜索函数(前驱节点)
 *@param[in] node 红黑节点实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbn_search_prev(app_sys_tree_rbn_t *node);

/*@brief     搜索函数(后继节点)
 *@param[in] node 红黑节点实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbn_search_next(app_sys_tree_rbn_t *node);

/*@brief     搜索函数(最小节点)
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search_min(app_sys_tree_rbt_t *tree);

/*@brief     搜索函数(最大节点)
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search_max(app_sys_tree_rbt_t *tree);

/* 向后遍历红黑树 */
#define app_sys_tree_rbt_btra(tree, node)   \
    for (app_sys_tree_rbn_t *node = app_sys_tree_rbt_search_min(tree); node != NULL; node = app_sys_tree_rbn_search_next(node))
/* 向前遍历红黑树 */
#define app_sys_tree_rbt_ftra(tree, node)    \
    for (app_sys_tree_rbn_t *node = app_sys_tree_rbt_search_max(tree); node != NULL; node = app_sys_tree_rbn_search_prev(node))

/*@brief     复位函数
 *@param[in] tree 红黑树实例
 */
void app_sys_tree_rbt_reset(app_sys_tree_rbt_t *tree);

/*@brief     复位函数
 *@param[in] node 红黑节点实例
 */
void app_sys_tree_rbn_reset(app_sys_tree_rbn_t *node);

/*@brief     配置函数
 *@param[in] tree 红黑树实例
 *@param[in] compare 红黑树比较语义
 *@param[in] confirm 红黑树确认语义
 */
void app_sys_tree_rbt_config(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_compare_t compare, app_sys_tree_rbt_confirm_t confirm);

/*@brief     根切换函数
 *@param[in] tree 红黑树实例
 *@param[in] node 红黑节点实例
 */
void app_sys_tree_rbt_root_set(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node);

/*@brief     根切换函数
 *@param[in] tree 红黑树实例
 *@param[in] node 红黑节点实例
 */
void app_sys_tree_rbt_root_get(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t **node);

/*扩展组件:
 *层序遍历,需要提供一个足够大的队列或栈(不超过所有节点大小)
 *迭代过程不可以做修改动作,因为队列或栈状态不一致
 */

/*@brief     数据的层序遍历并访问(窥探)
 *@param[in] tree  红黑树实例
 *@param[in] visit 红黑树节点访问函数
 */
void app_sys_tree_rbt_seq_tra(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_visit_t visit, app_sys_tree_rbn_t **queue, int32_t len);

/*@brief     验证一棵树是否为红黑树(内部检查)
 *@param[in] tree  红黑树实例
 *@param[in] visit 红黑树节点访问函数
 *@retval    0:失败;1:成功
 */
uint8_t app_sys_tree_rbt_check_valid(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t **stack, int32_t *flags, int32_t len);

#endif
