#ifndef APP_SYS_TREE_RBS_H
#define APP_SYS_TREE_RBS_H

/* 实现目标:侵入式红黑树 */

/*
 *容器(红黑树)
 *该树实现由Zephyr/lib/os/rb源码改编
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

/* 深度为K的二叉树,最多有2^k - 1个节点,红黑树堆栈配置最大空间的理论参考值 */
#define APP_SYS_TREE_RBS_PBITS(type)    ((sizeof(type)) * 8)
#define APP_SYS_TREE_RBS_TBITS(type)    ((sizeof(type)) < 8 ? 2 : 3)
#define APP_SYS_TREE_RBS_DEPTH          ((APP_SYS_TREE_RBS_PBITS(void *) -  \
                                          APP_SYS_TREE_RBS_TBITS(void *) - 1) * 2 + 1)

/* 节点关系,节点颜色 */
typedef enum {
    app_sys_tree_rbsn_side_l  = 0,  /* 左孩子方向 */
    app_sys_tree_rbsn_side_r  = 1,  /* 右孩子方向 */
    app_sys_tree_rbsn_color_r = 0,  /* 红色节点 */
    app_sys_tree_rbsn_color_b = 1,  /* 黑色节点 */
    app_sys_tree_rbsn_error   = 2,
} app_sys_tree_rbsn_status_t;

/* 红黑树核心(红黑树容器) */
typedef struct app_sys_tree_rbsn {
    struct app_sys_tree_rbsn *child[2];     /* 节点左右孩子和颜色 */
} app_sys_tree_rbsn_t;

/* 比较函数:node1严格小于node2返回非零值 */
/* 匹配函数:node1与node2的关键字一致返回0 */
/* 访问函数:节点和颜色 */
typedef uint8_t (*app_sys_tree_rbst_compare_t)(app_sys_tree_rbsn_t *node1, app_sys_tree_rbsn_t *node2);
typedef uint8_t (*app_sys_tree_rbst_confirm_t)(app_sys_tree_rbsn_t *node1, app_sys_tree_rbsn_t *node2);
typedef void    (*app_sys_tree_rbst_visit_t)(app_sys_tree_rbsn_t *node, uint8_t color);

/* 红黑树核心(红黑树集合) */
typedef struct {
    app_sys_tree_rbsn_t        *root;
    app_sys_tree_rbsn_t       **stack;    /* 堆栈 */
    app_sys_tree_rbst_compare_t compare;
    app_sys_tree_rbst_confirm_t confirm;
    app_sys_tree_rbst_visit_t   visit;
    app_sys_tree_rbsn_status_t *iter_side;
    app_sys_tree_rbsn_t       **iter_stack;
    uint32_t                    iter_top;
} app_sys_tree_rbst_t;

/*@brief         插入函数
 *@param[in,out] tree 红黑树实例
 *@param[in,out] node 红黑节点实例
 */
void app_sys_tree_rbst_insert(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node);

/*@brief         删除函数
 *@param[in,out] tree 红黑树实例
 *@param[in,out] node 红黑节点实例
 */
void app_sys_tree_rbst_remove(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node);

/*@brief     搜索函数
 *@param[in] tree 红黑树实例
 *@param[in] node 红黑节点实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node);

/*@brief     搜索函数(最小节点)
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search_min(app_sys_tree_rbst_t *tree);

/*@brief     搜索函数(最大节点)
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search_max(app_sys_tree_rbst_t *tree);

/*@brief     迭代函数(最小节点)
 *           连续使用prev或next前需要使用min或max重置迭代栈
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_min(app_sys_tree_rbst_t *tree);

/*@brief     迭代函数(最大节点)
 *           连续使用prev或next前需要使用min或max重置迭代栈
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_max(app_sys_tree_rbst_t *tree);

/*@brief     迭代函数(前驱节点)
 *           连续使用它前需要迭代到min或max
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_prev(app_sys_tree_rbst_t *tree);

/*@brief     迭代函数(后继节点)
 *           连续使用它前需要迭代到min或max
 *@param[in] tree 红黑树实例
 *@retval    红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_next(app_sys_tree_rbst_t *tree);

/*@brief     迭代函数(终止迭代)
 *           不使用迭代函数时使用该函数回收内部资源
 *@param[in] tree 红黑树实例
 *@param[in] used 为true时检查迭代器是否还存在
                  为false时终止迭代
 *@retval    used 为true时返回迭代器状态
 *                为false时返回false以用于迭代退出
 */
bool app_sys_tree_rbst_iter_used(app_sys_tree_rbst_t *tree, bool used);

/* 向后遍历红黑树 */
#define app_sys_tree_rbst_iter_btra(tree, node)  \
    for (app_sys_tree_rbsn_t *node = app_sys_tree_rbst_iter_min(tree); node != NULL || app_sys_tree_rbst_iter_used(tree, false); node = app_sys_tree_rbst_iter_next(tree))
/* 向前遍历红黑树 */
#define app_sys_tree_rbst_iter_ftra(tree, node)   \
    for (app_sys_tree_rbsn_t *node = app_sys_tree_rbst_iter_max(tree); node != NULL || app_sys_tree_rbst_iter_used(tree, false); node = app_sys_tree_rbst_iter_prev(tree))

/*@brief     复位函数
 *@param[in] tree 红黑树实例
 */
void app_sys_tree_rbst_reset(app_sys_tree_rbst_t *tree);

/*@brief     复位函数
 *@param[in] node 红黑节点实例
 */
void app_sys_tree_rbsn_reset(app_sys_tree_rbsn_t *node);

/*@brief     配置函数
 *@param[in] tree 红黑树实例
 *@param[in] compare 红黑树比较语义
 *@param[in] confirm 红黑树确认语义
 *@param[in] visit   红黑树访问语义
 */
void app_sys_tree_rbst_config(app_sys_tree_rbst_t *tree, app_sys_tree_rbst_compare_t compare, app_sys_tree_rbst_confirm_t confirm, app_sys_tree_rbst_visit_t visit);

/*@brief     根切换函数
 *@param[in] tree 红黑树实例
 *@param[in] node 红黑节点实例
 */
void app_sys_tree_rbst_root_set(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node);

/*@brief      根切换函数
 *@param[in]  tree 红黑树实例
 *@param[out] node 红黑节点实例
 */
void app_sys_tree_rbst_root_get(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **node);

/*@brief     数据的层序遍历并访问(窥探)
 *@param[in] tree  红黑树实例
 *@param[in] queue 红黑树节点实例队列数组
 *@param[in] len   红黑树节点实例队列数组长度
 */
void app_sys_tree_rbst_seq_tra(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **queue, int32_t len);

/*@brief     验证一棵树是否为红黑树(内部检查)
 *@param[in] tree  红黑树实例
 *@param[in] stack 红黑树节点实例栈数组
 *@param[in] flags 红黑树节点实例栈数组
 *@param[in] len   红黑树节点实例栈数组长度
 *@retval    0:失败;1:成功
 */
uint8_t app_sys_tree_rbst_check_valid(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **stack, int32_t *flags, int32_t len);

#endif
