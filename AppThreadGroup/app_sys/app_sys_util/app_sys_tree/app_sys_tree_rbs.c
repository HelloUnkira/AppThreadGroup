/*实现目标:
 *    嵌入式红黑树
 *    从当前树节点地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#define APP_SYS_LOG_RECORD_LIMIT    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 条件检查语义 */
#define APP_SYS_TREE_RBST_CHECK(cond, str)  if (cond) APP_SYS_LOG_ERROR(str)

/*@brief 红黑树(设置语义)
 *@param tree 红黑树实例
 *@param root 红黑树根
 */
static inline void app_sys_tree_rbst_set_root(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *root)
{
    tree->root = root;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树比较语义
 */
static inline void app_sys_tree_rbst_set_compare(app_sys_tree_rbst_t *tree, app_sys_tree_rbst_compare_t compare)
{
    tree->compare = compare;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树确认语义
 */
static inline void app_sys_tree_rbst_set_confirm(app_sys_tree_rbst_t *tree, app_sys_tree_rbst_confirm_t confirm)
{
    tree->confirm = confirm;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树访问语义
 */
static inline void app_sys_tree_rbst_set_visit(app_sys_tree_rbst_t *tree, app_sys_tree_rbst_visit_t visit)
{
    tree->visit = visit;
}

/*@brief 红黑树(节点栈)
 *@param tree  红黑树实例
 *@param stack 红黑树节点栈
 */
static inline void app_sys_tree_rbst_set_stack(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **stack)
{
    tree->stack = stack;
}

/*@brief 红黑树(节点栈)
 *@param tree  红黑树实例
 *@param iter_stack 红黑树节点栈
 */
static inline void app_sys_tree_rbst_set_iter_stack(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **iter_stack)
{
    tree->iter_stack = iter_stack;
}

/*@brief 红黑树(节点栈)
 *@param tree  红黑树实例
 *@param iter_side 红黑树节点栈
 */
static inline void app_sys_tree_rbst_set_iter_side(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_status_t *iter_side)
{
    tree->iter_side = iter_side;
}

/*@brief 红黑树(节点栈)
 *@param tree 红黑树实例
 *@param iter_top 红黑树节点栈
 */
static inline void app_sys_tree_rbst_set_iter_top(app_sys_tree_rbst_t *tree, uint32_t iter_top)
{
    tree->iter_top = iter_top;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树根
 */
static inline app_sys_tree_rbsn_t * app_sys_tree_rbst_get_root(app_sys_tree_rbst_t *tree)
{
    return tree->root;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树比较语义
 */
static inline app_sys_tree_rbst_compare_t app_sys_tree_rbst_get_compare(app_sys_tree_rbst_t *tree)
{
    return tree->compare;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树确认语义
 */
static inline app_sys_tree_rbst_confirm_t app_sys_tree_rbst_get_confirm(app_sys_tree_rbst_t *tree)
{
    return tree->confirm;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树访问语义
 */
static inline app_sys_tree_rbst_visit_t app_sys_tree_rbst_get_visit(app_sys_tree_rbst_t *tree)
{
    return tree->visit;
}

/*@brief 红黑树(节点栈)
 *@param tree 红黑树实例
 *@retval 红黑树节点栈
 */
static inline app_sys_tree_rbsn_t ** app_sys_tree_rbst_get_stack(app_sys_tree_rbst_t *tree)
{
    return tree->stack;
}

/*@brief 红黑树(节点栈)
 *@param tree 红黑树实例
 *@retval 红黑树节点栈
 */
static inline app_sys_tree_rbsn_t ** app_sys_tree_rbst_get_iter_stack(app_sys_tree_rbst_t *tree)
{
    return tree->iter_stack;
}

/*@brief 红黑树(节点栈)
 *@param tree 红黑树实例
 *@retval 红黑树节点栈
 */
static inline app_sys_tree_rbsn_status_t * app_sys_tree_rbst_get_iter_side(app_sys_tree_rbst_t *tree)
{
    return tree->iter_side;
}

/*@brief 红黑树(节点栈)
 *@param tree 红黑树实例
 *@retval 红黑树节点栈
 */
static inline uint32_t app_sys_tree_rbst_get_iter_top(app_sys_tree_rbst_t *tree)
{
    return tree->iter_top;
}

/*@brief 红黑节点(设置语义)
 *@param node  红黑节点实例
 *@param color 红黑节点颜色
 */
static inline void app_sys_tree_rbsn_set_color(app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_status_t color)
{
    uintptr_t retval = 0;
    retval |= (uintptr_t)(node->child[0]);
    retval &= (uintptr_t)(~1);
    retval |= (uintptr_t)(color & 1);
    node->child[0] = (app_sys_tree_rbsn_t *)(retval);
}

/*@brief 红黑节点(设置语义)
 *@param node  红黑节点实例
 *@param child 红黑节点实例
 *@param side  红黑节点方向
 */
static inline void app_sys_tree_rbsn_set_child(app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_t *child, app_sys_tree_rbsn_status_t side)
{
    if (side == app_sys_tree_rbsn_side_l) {
        uintptr_t retval = 0;
        retval |= (uintptr_t)(node->child[0]);
        retval &= (uintptr_t)(1);
        retval |= (uintptr_t)(child);
        node->child[0] = (app_sys_tree_rbsn_t *)retval;
    }
    if (side == app_sys_tree_rbsn_side_r)
        node->child[1] = child;
}

/*@brief 红黑节点(获取语义)
 *@param node 红黑节点实例
 *@retval 红黑节点颜色
 */
static inline app_sys_tree_rbsn_status_t app_sys_tree_rbsn_get_color(app_sys_tree_rbsn_t *node)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->child[0]);
    retval &= (uintptr_t)(1);
    
    return (app_sys_tree_rbsn_status_t)retval;
}

/*@brief 红黑节点(获取语义)
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static inline app_sys_tree_rbsn_t * app_sys_tree_rbsn_get_child(app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_status_t side)
{
    if (side == app_sys_tree_rbsn_side_l) {
        uintptr_t retval = 0;
        retval |= (uintptr_t)(node->child[0]);
        retval &= (uintptr_t)(~1);
        return (app_sys_tree_rbsn_t *)retval;
    }
    if (side == app_sys_tree_rbsn_side_r)
        return node->child[1];
    return NULL;
}

/*@brief 红黑节点(获取语义)
 *@param node   红黑节点实例
 *@param parent 红黑节点实例
 *@retval 红黑节点方向
 */
static inline app_sys_tree_rbsn_status_t app_sys_tree_rbsn_get_side(app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_t *parent)
{
    if (node == app_sys_tree_rbsn_get_child(parent, app_sys_tree_rbsn_side_l))
        return app_sys_tree_rbsn_side_l;
    if (node == app_sys_tree_rbsn_get_child(parent, app_sys_tree_rbsn_side_r))
        return app_sys_tree_rbsn_side_r;
    return app_sys_tree_rbsn_error;
}

/*@brief 红黑节点(获取语义)
 *@param side 红黑节点方向
 *@retval 红黑节点方向
 */
static inline app_sys_tree_rbsn_status_t app_sys_tree_rbsn_get_side_other(app_sys_tree_rbsn_status_t side)
{
    return side == app_sys_tree_rbsn_side_l ? app_sys_tree_rbsn_side_r :
           side == app_sys_tree_rbsn_side_r ? app_sys_tree_rbsn_side_l : app_sys_tree_rbsn_error;
}

/* 辅助定制宏,美化代码,剔除边缘逻辑 */
#define APP_SYS_TREE_RBST_CHECK_RETURN(expr)         if (expr) return;
#define APP_SYS_TREE_RBST_CHECK_RETURN_NULL(expr)    if (expr) return NULL;

/*@brief 红黑树核心动作:交换(左右旋转)
 *@param stack 红黑节点栈
 *@param size  红黑节点栈顶
 */
static void app_sys_tree_rbst_rotate(app_sys_tree_rbsn_t **stack, uint32_t size)
{
    APP_SYS_TREE_RBST_CHECK(stack == NULL, "stack");
    APP_SYS_TREE_RBST_CHECK(size < 2, "size");
    /* 到栈顶位置获得父子节点 */
    app_sys_tree_rbsn_t *parent = stack[size - 2];
    app_sys_tree_rbsn_t *child  = stack[size - 1];
    /* 获得父子节点的关系 */
    app_sys_tree_rbsn_status_t side       = app_sys_tree_rbsn_get_side(child, parent);
    app_sys_tree_rbsn_status_t side_other = app_sys_tree_rbsn_get_side_other(side);
    APP_SYS_TREE_RBST_CHECK(side       == app_sys_tree_rbsn_error, "side");
    APP_SYS_TREE_RBST_CHECK(side_other == app_sys_tree_rbsn_error, "side_other");
    /* 获得子节点的俩个孩子 */
    app_sys_tree_rbsn_t *son_outer = app_sys_tree_rbsn_get_child(child, side);
    app_sys_tree_rbsn_t *son_inner = app_sys_tree_rbsn_get_child(child, side_other);
    /* 存在祖父节点 */
    if (size >= 3) {
        /* 获取祖父节点索引 */
        app_sys_tree_rbsn_t *ancestor = stack[size - 3];
        /* 获得祖父与父亲的关系 */
        app_sys_tree_rbsn_status_t side_parent = app_sys_tree_rbsn_get_side(parent, ancestor);
        APP_SYS_TREE_RBST_CHECK(side_parent == app_sys_tree_rbsn_error, "parent:side");
        /* 现在孩子成为新的父节点 */
        app_sys_tree_rbsn_set_child(ancestor, child, side_parent);
    }
    /* 更新父子关系,父亲获得对位孙子为新孩子 */
    // app_sys_tree_rbsn_set_child(child,  son_outer, side);
       app_sys_tree_rbsn_set_child(child,  parent,    side_other);
       app_sys_tree_rbsn_set_child(parent, son_inner, side);
    /* 更新堆栈俩个节点关系 */
    stack[size - 2] = child;
    stack[size - 1] = parent;
}

/*@brief 搜索函数(大小元)
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static app_sys_tree_rbsn_t * app_sys_tree_rbst_search_min_or_max(app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_status_t side)
{
    APP_SYS_TREE_RBST_CHECK(node == NULL, "node");
    APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
    
    while (node != NULL && app_sys_tree_rbsn_get_child(node, side) != NULL)
        node = app_sys_tree_rbsn_get_child(node, side);
    
    return node;
}

/*@brief 从栈顶按指定方向节点入迭代栈
 *@param tree 红黑树实例
 *@param side 红黑节点方向
 */
static uint32_t app_sys_tree_rbst_iter_limb(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_status_t side)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
    
    uint32_t iter_top = app_sys_tree_rbst_get_iter_top(tree);
    app_sys_tree_rbsn_t **iter_stack = app_sys_tree_rbst_get_iter_stack(tree);
    app_sys_tree_rbsn_status_t *iter_side = app_sys_tree_rbst_get_iter_side(tree);
    APP_SYS_TREE_RBST_CHECK(iter_top == 0, "iter_top");
    app_sys_tree_rbsn_t *node = NULL;
    
    while ((node = app_sys_tree_rbsn_get_child(iter_stack[iter_top - 1], side)) != NULL) {
        APP_SYS_TREE_RBST_CHECK(iter_top >= APP_SYS_TREE_RBS_DEPTH, "stack");
        iter_top++;
        iter_side[iter_top - 1]  = side;
        iter_stack[iter_top - 1] = node;
    }
    
    /* 栈已经更新,栈顶索引一并更新 */
    app_sys_tree_rbst_set_iter_top(tree, iter_top);
    return iter_top;
}

/*@brief 迭代栈就绪并更新到大小元,或者回收
 *@param tree 红黑树实例
 *@param side 红黑节点方向
 *@param used 红黑节点栈不使用
 */
static app_sys_tree_rbsn_t * app_sys_tree_rbsn_iter_min_or_max_used(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_status_t side, bool used)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "node");
    
    app_sys_tree_rbsn_t **iter_stack =  NULL;
    app_sys_tree_rbsn_status_t *iter_side = NULL;
    /* 先回收资源 */
    iter_stack = app_sys_tree_rbst_get_iter_stack(tree);
    iter_side  = app_sys_tree_rbst_get_iter_side(tree);
    app_sys_tree_rbst_set_iter_stack(tree, NULL);
    app_sys_tree_rbst_set_iter_side(tree, NULL);
    app_sys_tree_rbst_set_iter_top(tree, 0);
    if (iter_stack != NULL)
        app_mem_free(iter_stack);
    if (iter_side != NULL)
        app_mem_free(iter_side);
    /* 不使用了 */
    if (!used)
        return NULL;
    APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
    /* 再生成资源 */
    iter_stack = app_mem_alloc(sizeof(app_sys_tree_rbsn_t *) * APP_SYS_TREE_RBS_DEPTH);
    iter_side  = app_mem_alloc(sizeof(app_sys_tree_rbsn_status_t) * APP_SYS_TREE_RBS_DEPTH);
    app_sys_tree_rbst_set_iter_stack(tree, iter_stack);
    app_sys_tree_rbst_set_iter_side(tree,  iter_side);
    /* 需要使用 */
    if (used) {
        uint32_t iter_top = 1;
        iter_side[iter_top - 1]  = app_sys_tree_rbsn_error;
        iter_stack[iter_top - 1] = app_sys_tree_rbst_get_root(tree);
        app_sys_tree_rbst_set_iter_top(tree, iter_top);
        if (iter_stack[iter_top - 1] == NULL)
            return NULL;
        iter_top = app_sys_tree_rbst_iter_limb(tree, side);
        return iter_stack[iter_top - 1];
    }
}

/*@brief 搜索函数(前驱和后继)
 *@param tree 红黑树实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static app_sys_tree_rbsn_t * app_sys_tree_rbst_iter_prev_or_next(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_status_t side)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "node");
    APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
    
    uint32_t iter_top = app_sys_tree_rbst_get_iter_top(tree);
    app_sys_tree_rbsn_t **iter_stack = app_sys_tree_rbst_get_iter_stack(tree);
    app_sys_tree_rbsn_status_t *iter_side = app_sys_tree_rbst_get_iter_side(tree);
    app_sys_tree_rbsn_status_t  side_other = app_sys_tree_rbsn_get_side_other(side);
    app_sys_tree_rbsn_t *child = NULL;
    
    if (iter_top != 0) {
        /* 向左方向:存在左子节点,则走到左子树最右节点 */
        /* 向右方向:存在右子节点,则走到右子树最左节点 */
        if ((child = app_sys_tree_rbsn_get_child(iter_stack[iter_top - 1], side)) != NULL) {
            APP_SYS_TREE_RBST_CHECK(iter_top >= APP_SYS_TREE_RBS_DEPTH, "stack");
            iter_top++;
            iter_side[iter_top - 1]  = side;
            iter_stack[iter_top - 1] = child;
            app_sys_tree_rbst_set_iter_top(tree, iter_top);
            iter_top = app_sys_tree_rbst_iter_limb(tree, side_other);
            return iter_stack[iter_top - 1];
        }
        /* 不存在子节点 */
        /* 向左方向:本节点时父节点的右节点,回退到父节点去 */
        /* 向右方向:本节点时父节点的左节点,回退到父节点去 */
        if (iter_side[iter_top - 1] != side) {
            iter_top--;
            app_sys_tree_rbst_set_iter_top(tree, iter_top);
            return iter_top == 0 ? NULL : iter_stack[iter_top - 1];
        }
        /* 沿着堆栈去找来时的路 */
        /* 向左方向,则来时的路是某一右节点,向右方向,则来时的路是某一左节点 */
        for (iter_top; iter_top > 0 && iter_side[iter_top - 1] == side; iter_top--);
             iter_top--;
        app_sys_tree_rbst_set_iter_top(tree, iter_top);
        return iter_top == 0 ? NULL : iter_stack[iter_top - 1];
    }
    return NULL;
}

/*@brief 查找函数
 *@param tree   红黑树实例
 *@param target 红黑节点实例
 *@retval 红黑节点实例
 */
static app_sys_tree_rbsn_t * app_sys_tree_rbst_search_only(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *target)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBST_CHECK(app_sys_tree_rbst_get_compare(tree) == NULL, "compare");
    APP_SYS_TREE_RBST_CHECK(app_sys_tree_rbst_get_confirm(tree) == NULL, "confirm");
    APP_SYS_TREE_RBST_CHECK(target == NULL, "target");
    
    app_sys_tree_rbsn_t *node = app_sys_tree_rbst_get_root(tree);
    
    while (node != NULL) {
        /* 关键字匹配成功,返回 */
        if (app_sys_tree_rbst_get_confirm(tree)(target, node) == 0)
            return node;
        /* 该结点小于此本结点,到左子树去 */
        if (app_sys_tree_rbst_get_compare(tree)(target, node) != 0) {
            node = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
            continue;
        }
        /* 该结点大于此本结点,到右子树去 */
        if (app_sys_tree_rbst_get_compare(tree)(target, node) == 0) {
            node = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
            continue;
        }
    }
    return NULL;
}

/*@brief 插入和删除的准备函数(节点入栈)
 *@param tree   红黑树实例
 *@param node   红黑节点实例
 *@param stack  红黑节点栈
 *@param size   红黑节点栈顶
 *@param remove 删除节点的准备
 */
static void app_sys_tree_rbst_get_stack_prepare(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node, app_sys_tree_rbsn_t **stack, uint32_t *size, bool remove)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL,  "tree");
    APP_SYS_TREE_RBST_CHECK(node == NULL,  "node");
    APP_SYS_TREE_RBST_CHECK(size == NULL,  "size");
    APP_SYS_TREE_RBST_CHECK(stack == NULL, "stack");
    /* 根节点入栈 */
    stack[(*size = 1) - 1] = app_sys_tree_rbst_get_root(tree);
    /* 自此以后从该节点向下依次渐入 */
    do {
        /* 删除动作时额外需要去停留在此处,插入动作时无需 */
        /* 因为我们关心的是按顺序排序而不是冲突修改 */
        if (remove && app_sys_tree_rbst_get_confirm(tree)(node, stack[*size - 1]) == 0)
            return;
        /* 红黑树具有二叉查找树的性质,可以以此规则获得迭代记录 */
        uint32_t side = app_sys_tree_rbsn_error;
        /* 如果目标小于该节点 */
        if (app_sys_tree_rbst_get_compare(tree)(node, stack[*size - 1]) != 0)
            side = app_sys_tree_rbsn_side_l;
        /* 如果目标大于等于该节点 */
        if (app_sys_tree_rbst_get_compare(tree)(node, stack[*size - 1]) == 0)
            side = app_sys_tree_rbsn_side_r;
        /* 再检查一次(实际使用时被裁空) */
        APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
        /* 获取其孩子 */
        app_sys_tree_rbsn_t *child = app_sys_tree_rbsn_get_child(stack[*size - 1], side);
        /* 检查孩子存在性决定是否继续向下搜索 */
        if (child == NULL)
            break;
        if (child != NULL)
            stack[(*size)++] = child;
        /* 当没有找到参数相同的节点break退出时 */
        /* 此时栈顶保留了最后一次查找记录 */
        /* 它停留的位置其孩子刚好为空(适合插入) */
    } while (1);
}

/*@brief 插入调整函数
 *@param stack 红黑节点栈
 *@param size  红黑节点栈顶
 */
static void app_sys_tree_rbst_insert_adjust(app_sys_tree_rbsn_t **stack, uint32_t size)
{
    APP_SYS_TREE_RBST_CHECK(stack == NULL, "stack");
    
    while (size > 1) {
        app_sys_tree_rbsn_t *node    = stack[size - 1];
        app_sys_tree_rbsn_t *parent  = stack[size - 2];
        app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
        app_sys_tree_rbsn_t *child_r = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
        /* 节点的左右孩子不是红色的 */
        if (child_l != NULL) {
            app_sys_tree_rbsn_status_t color_l = app_sys_tree_rbsn_get_color(child_l);
            APP_SYS_TREE_RBST_CHECK(color_l == app_sys_tree_rbsn_color_r, "child red");
        }
        if (child_r != NULL) {
            app_sys_tree_rbsn_status_t color_r = app_sys_tree_rbsn_get_color(child_r);
            APP_SYS_TREE_RBST_CHECK(color_r == app_sys_tree_rbsn_color_r, "child red");
        }
        /* 父节点是黑节点无需调整 */
        app_sys_tree_rbsn_status_t color_p = app_sys_tree_rbsn_get_color(parent);
        APP_SYS_TREE_RBST_CHECK_RETURN(color_p == app_sys_tree_rbsn_color_b);
        /* 红节点一定有父亲 */
        APP_SYS_TREE_RBST_CHECK(size <= 2, "lost parent");
        /* 获取祖父节点 */
        app_sys_tree_rbsn_t *ancestor = stack[size - 3];
        /* 确认父亲与祖父的对位关系 */
        app_sys_tree_rbsn_status_t side       = app_sys_tree_rbsn_get_side(parent, ancestor);
        app_sys_tree_rbsn_status_t side_other = app_sys_tree_rbsn_get_side_other(side);
        APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
        /* 获得父亲的兄弟节点 */
        app_sys_tree_rbsn_t *uncle = app_sys_tree_rbsn_get_child(ancestor, side_other);
        /* 情况1:叔叔节点是红色(父亲也是红色节点,黑色节点下沉) */
        if (uncle != NULL && app_sys_tree_rbsn_get_color(uncle) == app_sys_tree_rbsn_color_r) {
            /* 1.染红祖父节点 */
            app_sys_tree_rbsn_set_color(ancestor, app_sys_tree_rbsn_color_r);
            /* 2.染黑叔叔节点与父亲节点 */
            app_sys_tree_rbsn_set_color(parent, app_sys_tree_rbsn_color_b);
            app_sys_tree_rbsn_set_color(uncle, app_sys_tree_rbsn_color_b);
            /* 3.红色节点冲突回退到祖父节点 */
            size -= 2;
            continue; 
        }
        /* 情况2:叔叔节点不存在或为黑色(本地旋转修正该树) */
        app_sys_tree_rbsn_status_t side_parent = app_sys_tree_rbsn_get_side(node, parent);
        APP_SYS_TREE_RBST_CHECK(side_parent == app_sys_tree_rbsn_error, "side_parent");
        /* 情况2.1:本节点与祖父节点是对位关系,需要先旋转至顺位关系 */
        if (side_parent != side) {
            app_sys_tree_rbst_rotate(stack, size);
            node = stack[size - 1];
        }
        /* 情况2.2:本节点与祖父节点是顺位关系 */
        /* 1.旋转父节点与祖父节点 */
        app_sys_tree_rbst_rotate(stack, size - 1);
        /* 2.原祖父节点染成红色 */
        app_sys_tree_rbsn_set_color(stack[size - 3], app_sys_tree_rbsn_color_b);
        /* 3.原父节点染成黑色 */
        app_sys_tree_rbsn_set_color(stack[size - 2], app_sys_tree_rbsn_color_r);
        /* 如果是到这种情况,需要注意一下流程: */
        /* 本节点(插入时的颜色为红色),父节点为红色,兄弟节点为黑色 */
        /* 旋转过后,本节点晋升为祖父节点(染成黑色),原父节点和祖父节点成为新的 */
        /* 父节点和兄弟节点(染成红色),原兄弟节点下沉,原黑色不变 */
        return;
    }
    /* 退出循环则到根节点,但根节点一定是黑色的 */
    app_sys_tree_rbsn_set_color(stack[0], app_sys_tree_rbsn_color_b);
}

/*@brief 删除调整函数
 *@param stack 红黑节点栈
 *@param size  红黑节点栈顶
 *@param none  红黑节点(假节点)
 */
static void app_sys_tree_rbst_remove_adjust(app_sys_tree_rbsn_t **stack, uint32_t size, app_sys_tree_rbsn_t *none)
{
    /* 一直调整到根节点或调整完毕 */
    while (size > 1) {
        app_sys_tree_rbsn_t *node   = stack[size - 1];
        app_sys_tree_rbsn_t *parent = stack[size - 2];
        /* 获得孩子与父亲的关系 */
        app_sys_tree_rbsn_status_t side       = app_sys_tree_rbsn_get_side(node, parent);
        app_sys_tree_rbsn_status_t side_other = app_sys_tree_rbsn_get_side_other(side);
        APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
        /* 获得兄弟节点 */
        app_sys_tree_rbsn_t *sibling = app_sys_tree_rbsn_get_child(parent, side_other);
        /* 删除一个黑色节点导致的不平衡,兄弟节点一定存在 */
        APP_SYS_TREE_RBST_CHECK(sibling == NULL, "sibling");
        /* 情况1:如果兄弟节点是红色的 */
        if (app_sys_tree_rbsn_get_color(sibling) == app_sys_tree_rbsn_color_r) {
            /* 要注意的一点:如果兄弟节点为红色 */
            /* 那么父节点和它的所有子节点必为黑色 */
            /* 1.兄弟节点覆盖当前节点 */
            stack[size - 1] = sibling;
            /* 2.兄弟节点与父节点旋转 */
            app_sys_tree_rbst_rotate(stack, size);
            /* 3.原父节点下沉为兄弟节点,染成红色 */
            app_sys_tree_rbsn_set_color(parent, app_sys_tree_rbsn_color_r);
            /* 4.原兄弟节点晋升为父节点,染成黑色 */
            app_sys_tree_rbsn_set_color(sibling, app_sys_tree_rbsn_color_b);
            /* 对兄弟节点旋转意味着自己下沉一级,将自己重新加入到集合中 */
            /* 现在它是原父节点(现在兄弟节点)的子节点 */
            /* 5.当前节点重新加入枝干 */
            stack[size++] = node;
            /* 6.重新定位父节点 */
            parent = stack[size - 2];
            /* 7.重新定位兄弟节点 */
            sibling = app_sys_tree_rbsn_get_child(parent, side_other);
        }
        /* 注意,旋转到下一级的时候 */
        /* 它的新兄弟一定是存在的,因为如果不存在 */
        /* 那么树在旋转之前本身就不是平衡的 */
        APP_SYS_TREE_RBST_CHECK(sibling == NULL, "sibling");
        /* 获得兄弟节点的孩子们 */
        app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(sibling, app_sys_tree_rbsn_side_l);
        app_sys_tree_rbsn_t *child_r = app_sys_tree_rbsn_get_child(sibling, app_sys_tree_rbsn_side_r);
        /* 情况2:兄弟孩子都是黑色的 */
        if ((child_l == NULL || app_sys_tree_rbsn_get_color(child_l) == app_sys_tree_rbsn_color_b) &&
            (child_r == NULL || app_sys_tree_rbsn_get_color(child_r) == app_sys_tree_rbsn_color_b)) {
            /* 需要删除节点,断开关联 */
            if (node == none)
                app_sys_tree_rbsn_set_child(parent, NULL, side);
            /* 兄弟只有黑孩子的情况有简单的解决办法 */
            /* 1.将新兄弟节点染成红色 */
            app_sys_tree_rbsn_set_color(sibling, app_sys_tree_rbsn_color_r);
            /* 2.父结点缺少一个黑色,向上迭代 */
            if (app_sys_tree_rbsn_get_color(parent) == app_sys_tree_rbsn_color_b) {
                size--;
                continue;
            }
            /* 3.父节点是红色的,重新上色使整个树OK */
            app_sys_tree_rbsn_set_color(parent, app_sys_tree_rbsn_color_b);
            return;
        }
        APP_SYS_TREE_RBST_CHECK(((child_l == NULL || app_sys_tree_rbsn_get_color(child_l) == app_sys_tree_rbsn_color_b) &&
                                 (child_r == NULL || app_sys_tree_rbsn_get_color(child_r) == app_sys_tree_rbsn_color_b)), "child");
        /* 我们知道兄弟姐妹至少有一个红色的孩子 */
        /* 固定它,使远/外的位置(即在N的对面)肯定是红色的 */
        /* 如果远处位置是红色的(即在N的对面)不管里面是红是黑都不重要 */
        app_sys_tree_rbsn_t *son_outer = app_sys_tree_rbsn_get_child(sibling, side_other);
        app_sys_tree_rbsn_t *son_inner = app_sys_tree_rbsn_get_child(sibling, side);
        /* 情况3:兄弟远处位置不存在红色节点 */
        if (son_outer == NULL || app_sys_tree_rbsn_get_color(son_outer) == app_sys_tree_rbsn_color_b) {
            /* 1.获得内部节点 */
            son_inner = app_sys_tree_rbsn_get_child(sibling, side);
            /* 2.兄弟节点写入到栈顶 */
            stack[size - 1] = sibling;
            /* 3.内部的红节点入栈 */
            stack[size++] = son_inner;
            /* 4.旋转内部节点与兄弟节点 */
            app_sys_tree_rbst_rotate(stack, size);
            /* 5.原兄弟节点下沉为远端节点,染成红色 */
            /* 6.原内部节点晋升为兄弟节点,染成黑色 */
            app_sys_tree_rbsn_set_color(sibling,   app_sys_tree_rbsn_color_r);
            app_sys_tree_rbsn_set_color(son_inner, app_sys_tree_rbsn_color_b);
            /* 7.更新兄弟节点 */
            sibling = stack[size - 2];
            /* 8.更新远端节点 */
            son_outer = app_sys_tree_rbsn_get_child(sibling, side_other);
            /* 9.栈顶恢复成当前节点 */
            stack[size - 2] = node;
            /* 10.回退栈顶 */
            size--;
        }
        /* 最后,兄弟结点必须在远/外插槽中有一个红色的子结点 */
        /* 我们可以旋转兄弟和我们的父元素并重新着色以生成一个有效的树 */
        APP_SYS_TREE_RBST_CHECK(son_outer == NULL || app_sys_tree_rbsn_get_color(son_outer) == app_sys_tree_rbsn_color_b, "son_outer");
        /* 情况4:兄弟远处位置存在红色节点 */
        /* 1.交换父亲与兄弟的颜色 */
        app_sys_tree_rbsn_set_color(sibling, app_sys_tree_rbsn_get_color(parent));
        app_sys_tree_rbsn_set_color(parent,  app_sys_tree_rbsn_color_b);
        /* 2.远端红孩子染成黑色补色 */
        app_sys_tree_rbsn_set_color(son_outer, app_sys_tree_rbsn_color_b);
        /* 3.栈顶更新为兄弟节点 */
        stack[size - 1] = sibling;
        /* 4.旋转兄弟节点与父节点 */
        app_sys_tree_rbst_rotate(stack, size);
        /* 需要删除节点,断开关联 */
        if (node == none)
            app_sys_tree_rbsn_set_child(parent, NULL, side);
        return;
    }
}

/*@brief 插入函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
static void app_sys_tree_rbst_insert_node(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBST_CHECK(node == NULL, "node");
    APP_SYS_TREE_RBST_CHECK(app_sys_tree_rbst_get_compare(tree) == NULL, "compare");
    /* 辅助初始化节点(可能需要) */
    app_sys_tree_rbsn_set_child(node, NULL, app_sys_tree_rbsn_side_l);
    app_sys_tree_rbsn_set_child(node, NULL, app_sys_tree_rbsn_side_r);
    /* 第一次插入 */
    if (app_sys_tree_rbst_get_root(tree) == NULL) {
        app_sys_tree_rbst_set_root(tree, node);
        app_sys_tree_rbsn_set_color(node, app_sys_tree_rbsn_color_b);
        return;
    }
    /* 1.获得其对应的栈 */
    app_sys_tree_rbsn_t **stack = app_sys_tree_rbst_get_stack(tree);
    /* 2.获取插入的枝干 */
    uint32_t size = -1;
    app_sys_tree_rbst_get_stack_prepare(tree, node, stack, &size, 0);
    /* 节点栈溢出检测 */
    APP_SYS_TREE_RBST_CHECK(size + 1 >= APP_SYS_TREE_RBS_DEPTH, "depth");
    /* 3.获得父节点 */
    app_sys_tree_rbsn_t *parent = stack[size - 1];
    /* 4.获得父子节点的关联 */
    app_sys_tree_rbsn_status_t side = app_sys_tree_rbsn_error;
    if (app_sys_tree_rbst_get_compare(tree)(node, parent) != 0)
        side = app_sys_tree_rbsn_side_l;
    if (app_sys_tree_rbst_get_compare(tree)(node, parent) == 0)
        side = app_sys_tree_rbsn_side_r;
    /* 5.建立亲子关系 */
    app_sys_tree_rbsn_set_child(parent, node, side);
    /* 6.染红插入的节点 */
    app_sys_tree_rbsn_set_color(node, app_sys_tree_rbsn_color_r);
    /* 7.当前节点入栈 */
    stack[size++] = node;
    /* 8.修复该树 */
    app_sys_tree_rbst_insert_adjust(stack, size);
    /* 可能调整到根节点 */
    app_sys_tree_rbst_set_root(tree, stack[0]);
    /* 树的根是黑色的 */
    app_sys_tree_rbsn_status_t color = app_sys_tree_rbsn_get_color(stack[0]);
    APP_SYS_TREE_RBST_CHECK(color != app_sys_tree_rbsn_color_b, "root");
}

/*@brief 删除函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
static void app_sys_tree_rbst_remove_node(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK(tree == NULL, "tree");
    /* 1.获得其对应的栈 */
    app_sys_tree_rbsn_t **stack = app_sys_tree_rbst_get_stack(tree);
    /* 2.获取删除的枝干 */
    uint32_t size = -1;
    app_sys_tree_rbst_get_stack_prepare(tree, node, stack, &size, 1);
    /* 3.没找到要删除节点 */
    if (app_sys_tree_rbst_get_confirm(tree)(node, stack[size - 1]) != 0)
        return;
    /* 侵入式数据结构需要额外更新节点栈 */
    /* 我们只能删除一个有0或1个子节点的节点 */
    /* 如果我们有两个,那么选择边0的"最大"子节点(最小的1也可以) */
    /* 并与它交换我们在树中的位置 */
    /* 4.获得目标节点 */
    if (app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l) != NULL &&
        app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r) != NULL) {
        uint32_t size0 = size;
        /* 1.获得左孩子 */
        app_sys_tree_rbsn_t *child = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
        /* 2.保存当前节点父亲 */
        app_sys_tree_rbsn_t *parent_h = size > 1 ? stack[size - 2] : NULL;
        /* 3.左孩子入栈 */
        stack[size++] = child;
        /* 4.迭代寻找其右孩子 */
        while (app_sys_tree_rbsn_get_child(child, app_sys_tree_rbsn_side_r) != NULL) {
            child = app_sys_tree_rbsn_get_child(child, app_sys_tree_rbsn_side_r);
            stack[size++] = child;
        }
        /* 5.获得要交换的孩子的父亲 */
        app_sys_tree_rbsn_t *parent_l = stack[size - 2];
        /* 这里有一些特殊情况需要检查 */
        /* 原则上,这是通过在节点之间交换子指针 */
        /* 并将指向它们的节点从它们的父节点重新定位来实现的,但是: */
        /* (1)上面的节点可能是树的根,没有父节点 */
        /* (2)下面的节点可能是上面节点的直接子节点 */
        /* 记得交换两个节点的颜色位 */
        /* 且没有父指针,所以跟踪这个结构的堆栈也需要交换! */
        if (parent_h == NULL)
            app_sys_tree_rbst_set_root(tree, child);
        if (parent_h != NULL) {
            app_sys_tree_rbsn_status_t side = app_sys_tree_rbsn_get_side(node, parent_h);
            APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
            /* 为高父亲设置低孩子 */
            app_sys_tree_rbsn_set_child(parent_h, child, side);
        }
        /* 直属亲子关系 */
        if (parent_l == node) {
            /* 交换亲子关系(交换它们的左孩子) */
            app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(child, app_sys_tree_rbsn_side_l);
            app_sys_tree_rbsn_set_child(node, child_l, app_sys_tree_rbsn_side_l);
            app_sys_tree_rbsn_set_child(child, node, app_sys_tree_rbsn_side_l);
        }
        /* 跨度亲子关系 */
        if (parent_l != node) {
            app_sys_tree_rbsn_status_t side_low = app_sys_tree_rbsn_get_side(child, parent_l);
            APP_SYS_TREE_RBST_CHECK(side_low == app_sys_tree_rbsn_error, "side_low");
            /* 更新低父亲指向高孩子 */
            app_sys_tree_rbsn_set_child(parent_l, node, side_low);
            /* 交换俩个节点的左孩子(可能存在) */
            app_sys_tree_rbsn_t *child_t = app_sys_tree_rbsn_get_child(node,  app_sys_tree_rbsn_side_l);
            app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(child, app_sys_tree_rbsn_side_l);
            app_sys_tree_rbsn_set_child(node,  child_l, app_sys_tree_rbsn_side_l);
            app_sys_tree_rbsn_set_child(child, child_t, app_sys_tree_rbsn_side_l);
        }
        /* 交换俩个节点的右孩子 */
        app_sys_tree_rbsn_t *child_r = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
        app_sys_tree_rbsn_set_child(child, child_r, app_sys_tree_rbsn_side_r);
        /* 原child的右孩子一定不存在 */
        app_sys_tree_rbsn_set_child(node, NULL, app_sys_tree_rbsn_side_r);
        /* 更新堆栈关系 */
        app_sys_tree_rbsn_t *child_t1 = stack[size  - 1];
        app_sys_tree_rbsn_t *child_t2 = stack[size0 - 1];
        stack[size  - 1] = child_t2;
        stack[size0 - 1] = child_t1;
        /* 交换俩个节点颜色 */
        app_sys_tree_rbsn_status_t color = app_sys_tree_rbsn_get_color(node);
        app_sys_tree_rbsn_set_color(node, app_sys_tree_rbsn_get_color(child));
        app_sys_tree_rbsn_set_color(child, color);
    }
    app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
    app_sys_tree_rbsn_t *child_r = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
    APP_SYS_TREE_RBST_CHECK(child_l != NULL && child_r != NULL, "child");
    /* 5.获得唯一的孩子,或者没有 */
    app_sys_tree_rbsn_t *child = child_l == NULL ? child_r : child_l;
    /* 6.如果是删除根节点 */
    if (size < 2) {
        app_sys_tree_rbst_set_root(tree, child);
        if (child != NULL)
            app_sys_tree_rbsn_set_color(child, app_sys_tree_rbsn_color_b);
        return;
    }
    /* 7.获得要删除节点的父亲 */
    app_sys_tree_rbsn_t *parent = stack[size - 2];
    /* 8.调整修复树 */
    /* 删除的节点没有孩子 */
    if (child == NULL) {
        /* 红色的无子节点可以直接删除 */
        if (app_sys_tree_rbsn_get_color(node) == app_sys_tree_rbsn_color_r) {
            app_sys_tree_rbsn_status_t side = app_sys_tree_rbsn_get_side(node, parent);
            APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
            app_sys_tree_rbsn_set_child(parent, NULL, side);
        }
        /* 特殊情况:如果要删除的节点是没有子节点的 */
        /* 那么我们在做缺少的黑色旋转时将它留在原地 */
        /* 当它们隔离它时将用适当的NULL替换它 */
        /* 调整去除无子的黑色节点(此时节点未删除) */
        if (app_sys_tree_rbsn_get_color(node) == app_sys_tree_rbsn_color_b)
            app_sys_tree_rbst_remove_adjust(stack, size, node);
    }
    /* 删除的节点有一个孩子 */
    if (child != NULL) {
        app_sys_tree_rbsn_status_t side = app_sys_tree_rbsn_get_side(node, parent);
        APP_SYS_TREE_RBST_CHECK(side == app_sys_tree_rbsn_error, "side");
        app_sys_tree_rbsn_set_child(parent, child, side);    /* 删除节点 */
        /* 如果该孩子可以直接染色修复 */
        if (app_sys_tree_rbsn_get_color(node)  == app_sys_tree_rbsn_color_r ||
            app_sys_tree_rbsn_get_color(child) == app_sys_tree_rbsn_color_r)
            app_sys_tree_rbsn_set_color(child, app_sys_tree_rbsn_color_b);
        else {
            /* 俩个节点都为黑色,但已经删除了node节点,让孩子节点顶替上去 */
            stack[size - 1] = child;
            /* 节点已经删除,直接修复 */
            app_sys_tree_rbst_remove_adjust(stack, size, NULL);
        }
    }
    /* 我们已经旋转到根了 */
    app_sys_tree_rbst_set_root(tree, stack[0]);
}

/*@brief 插入函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbst_insert(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_RBST_CHECK_RETURN(node == NULL);
    
    app_sys_tree_rbsn_t **stack = app_mem_alloc(sizeof(app_sys_tree_rbsn_t *) * APP_SYS_TREE_RBS_DEPTH);
    app_sys_tree_rbst_set_stack(tree, stack);
    app_sys_tree_rbst_insert_node(tree, node);
    app_sys_tree_rbst_set_stack(tree, NULL);
    app_mem_free(stack);
}

/*@brief 删除函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbst_remove(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_RBST_CHECK_RETURN(node == NULL);
    
    app_sys_tree_rbsn_t **stack = app_mem_alloc(sizeof(app_sys_tree_rbsn_t *) * APP_SYS_TREE_RBS_DEPTH);
    app_sys_tree_rbst_set_stack(tree, stack);
    app_sys_tree_rbst_remove_node(tree, node);
    app_sys_tree_rbst_set_stack(tree, NULL);
    app_mem_free(stack);
}

/*@brief 搜索函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(node == NULL);
    
    return app_sys_tree_rbst_search_only(tree, node);
}

/*@brief 搜索函数(最小节点)
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search_min(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    app_sys_tree_rbsn_t *root = app_sys_tree_rbst_get_root(tree);
    return app_sys_tree_rbst_search_min_or_max(root, app_sys_tree_rbsn_side_l);
}

/*@brief 搜索函数(最大节点)
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbsn_t * app_sys_tree_rbst_search_max(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    app_sys_tree_rbsn_t *root = app_sys_tree_rbst_get_root(tree);
    return app_sys_tree_rbst_search_min_or_max(root, app_sys_tree_rbsn_side_r);
}

/*@brief 迭代函数(最小节点)
 *       连续使用prev或next前需要使用min或max重置迭代栈
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_min(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    return app_sys_tree_rbsn_iter_min_or_max_used(tree, app_sys_tree_rbn_side_l, true);
}

/*@brief 迭代函数(最大节点)
 *       连续使用prev或next前需要使用min或max重置迭代栈
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_max(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    return app_sys_tree_rbsn_iter_min_or_max_used(tree, app_sys_tree_rbn_side_r, true);
}

/*@brief 迭代函数(前驱节点)
 *       连续使用它前需要迭代到min或max
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_prev(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    return app_sys_tree_rbst_iter_prev_or_next(tree, app_sys_tree_rbn_side_l);
}

/*@brief 迭代函数(后继节点)
 *       连续使用它前需要迭代到min或max
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbst_iter_next(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    return app_sys_tree_rbst_iter_prev_or_next(tree, app_sys_tree_rbn_side_r);
}

/*@brief 迭代函数(终止迭代)
 *       不使用迭代函数时使用该函数回收内部资源
 *@param tree 红黑树实例
 *@param used 为true时检查迭代器是否还存在
              为false时终止迭代
 *@retval 为true时返回迭代器状态
 *        为false时返回false以用于迭代退出
 */
bool app_sys_tree_rbst_iter_used(app_sys_tree_rbst_t *tree, bool used)
{
    APP_SYS_TREE_RBST_CHECK_RETURN_NULL(tree == NULL);
    
    if (used) {
        uint32_t iter_top = app_sys_tree_rbst_get_iter_top(tree);
        app_sys_tree_rbsn_t **iter_stack = app_sys_tree_rbst_get_iter_stack(tree);
        app_sys_tree_rbsn_status_t *iter_side = app_sys_tree_rbst_get_iter_side(tree);
        return iter_top != 0 || iter_stack != NULL || iter_side != NULL;
    }
    app_sys_tree_rbsn_iter_min_or_max_used(tree, app_sys_tree_rbn_error, false);
    return false;
}

/*@brief 复位函数
 *@param tree 红黑树实例
 */
void app_sys_tree_rbst_reset(app_sys_tree_rbst_t *tree)
{
    APP_SYS_TREE_RBST_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbst_set_root(tree, NULL);
    app_sys_tree_rbst_set_stack(tree, NULL);
    app_sys_tree_rbst_set_compare(tree, NULL);
    app_sys_tree_rbst_set_confirm(tree, NULL);
    app_sys_tree_rbst_set_visit(tree, NULL);
    app_sys_tree_rbst_set_iter_side(tree, NULL);
    app_sys_tree_rbst_set_iter_stack(tree, NULL);
    app_sys_tree_rbst_set_iter_top(tree, 0);
}

/*@brief 复位函数
 *@param node 红黑节点实例
 */
void app_sys_tree_rbsn_reset(app_sys_tree_rbsn_t *node)
{
    app_sys_tree_rbsn_set_child(node, NULL, app_sys_tree_rbsn_side_l);
    app_sys_tree_rbsn_set_child(node, NULL, app_sys_tree_rbsn_side_r);
}

/*@brief 配置函数
 *@param tree 红黑树实例
 *@param compare 红黑树比较语义
 *@param confirm 红黑树确认语义
 *@param visit   红黑树访问语义
 */
void app_sys_tree_rbst_config(app_sys_tree_rbst_t *tree, app_sys_tree_rbst_compare_t compare, app_sys_tree_rbst_confirm_t confirm, app_sys_tree_rbst_visit_t visit)
{
    APP_SYS_TREE_RBST_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbst_set_compare(tree, compare);
    app_sys_tree_rbst_set_confirm(tree, confirm);
    app_sys_tree_rbst_set_visit(tree, visit);
}

/*@brief 根切换函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbst_root_set(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t *node)
{
    APP_SYS_TREE_RBST_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbst_set_root(tree, node);
}

/*@brief 根切换函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbst_root_get(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **node)
{
    *node = app_sys_tree_rbst_get_root(tree);
}

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree  红黑树实例
 *@param queue 红黑树节点实例队列数组
 *@param len   红黑树节点实例队列数组长度
 */
void app_sys_tree_rbst_seq_tra(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **queue, int32_t len)
{
    /* 层序遍历(这里使用循环队列): */
    
    /* 获取插入的起始位置 */
    int32_t queue_h  = 0;
    int32_t queue_t  = 0;
    int32_t node_num = 1;
    queue[queue_t++] = app_sys_tree_rbst_get_root(tree);
    
    do {
        /* 节点出队列 */
        app_sys_tree_rbsn_t *node = queue[queue_h++];
        node_num--;
        APP_SYS_TREE_RBST_CHECK_RETURN(node == NULL);
        /* 节点访问 */
        app_sys_tree_rbst_get_visit(tree)(node, app_sys_tree_rbsn_get_color(node));
        /* 这是一个循环队列 */
        if (queue_h >= len)
            queue_h  = 0;
        if (queue_t >= len)
            queue_t  = 0; 
        
        /* 子节点入队列 */
        app_sys_tree_rbsn_t *child_l = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
        app_sys_tree_rbsn_t *child_r = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
        
        /* 左孩子入队列 */
        if (child_l != NULL) {
            queue[queue_t++] = child_l;
            node_num++;
        }
        
        /* 右孩子入队列 */
        if (child_r != NULL) {
            queue[queue_t++] = child_r;
            node_num++;
        }
        
        /* 队列太小,警告 */
        APP_SYS_TREE_RBST_CHECK(node_num >= len, "overflow");
    } while (node_num > 0);
}

/*@brief 验证一棵树是否为红黑树(内部检查)
 *@param tree  红黑树实例
 *@param stack 红黑树节点实例栈数组
 *@param flags 红黑树节点实例栈数组
 *@param len   红黑树节点实例栈数组长度
 *@retval 0:失败;1:成功
 */
uint8_t app_sys_tree_rbst_check_valid(app_sys_tree_rbst_t *tree, app_sys_tree_rbsn_t **stack, int32_t *flags, int32_t len)
{
    /* 验证:1根节点不是黑色 */
    if (app_sys_tree_rbst_get_root(tree) == NULL)
        return 1;
    if (app_sys_tree_rbsn_get_color(app_sys_tree_rbst_get_root(tree)) == app_sys_tree_rbsn_color_r)
        return 0;
    
    /* 初始化flags的值 */
    for (int32_t idx = 0; idx < len; flags[idx++] = app_sys_tree_rbsn_side_l);
    /* 深度优先搜索,使用flags同步维护栈进动情况 */
    app_sys_tree_rbsn_t *node    = NULL;
    app_sys_tree_rbsn_t *child_l = NULL;
    app_sys_tree_rbsn_t *child_r = NULL;
    /* 从根节点开始从左进动 */
    /* 入栈节点只有当左右孩子都使用完毕后才退栈 */
    int32_t stack_t = 0;    //栈顶位置
    int32_t len_max = 0;    //一条从根到叶子的最大度
    int32_t len_cur_b = 0;
    
    /* 1.根节点入栈 */
    stack[stack_t++] = app_sys_tree_rbst_get_root(tree);
    len_cur_b++;
    flags[stack_t - 1] = app_sys_tree_rbsn_side_l;
    
    do {
        /* 2.获得栈顶元素的左右孩子 */
        node    = stack[stack_t - 1];
        child_l = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_l);
        child_r = app_sys_tree_rbsn_get_child(node, app_sys_tree_rbsn_side_r);
        
        /* 验证:2红色节点不相连 */
        if (child_l != NULL)
        if (app_sys_tree_rbsn_get_color(node) == app_sys_tree_rbsn_color_r &&
            app_sys_tree_rbsn_get_color(child_l) == app_sys_tree_rbsn_color_r)
            return 0;
                    
        if (child_r != NULL)
        if (app_sys_tree_rbsn_get_color(node) == app_sys_tree_rbsn_color_r &&
            app_sys_tree_rbsn_get_color(child_r) == app_sys_tree_rbsn_color_r)
            return 0;
        
        /* 3.左孩子未曾进过栈 */
        if (flags[stack_t - 1] == app_sys_tree_rbsn_side_l) {
            /* 3.1.标记左孩子进过栈,下一次该右孩子进了 */
            flags[stack_t - 1] = app_sys_tree_rbsn_side_r;
            /* 左孩子存在,可以进栈 */
            if(child_l != NULL) {
                stack[stack_t++] = child_l;
                /* 如果左孩子是黑色的,计算其度 */
                if (app_sys_tree_rbsn_get_color(child_l) == app_sys_tree_rbsn_color_b)
                    len_cur_b++;
                flags[stack_t - 1] = app_sys_tree_rbsn_side_l;//左孩子的左孩子未曾进栈
                continue;
            } else {
                /* 当前节点不存在左孩子 */
                /* 说明一条从根到叶子的路径产生了 */
                if (len_max == 0)
                    len_max = len_cur_b;
                /* 验证:路径黑色节点不一致 */
                if (len_max != len_cur_b)
                    return 0;
            }
        }
        
        /* 4.右孩子未曾进过栈 */
        if (flags[stack_t - 1] == app_sys_tree_rbsn_side_r) {
            /* 3.2标记右孩子进过栈,下一次该退栈了 */
            flags[stack_t - 1] = app_sys_tree_rbsn_error;
            /* 右孩子存在,可以进栈 */
            if (child_r != NULL) {
                stack[stack_t++] = child_r;
                /* 如果右孩子是黑色的,计算其度 */
                if (app_sys_tree_rbsn_get_color(child_r) == app_sys_tree_rbsn_color_b)
                    len_cur_b++;
                flags[stack_t - 1] = app_sys_tree_rbsn_side_l;//右孩子的左孩子未曾进栈
                continue;
            } else {
                /* 当前节点不存在右孩子, 说明一条从根到叶子的路径产生了 */
                if (len_max == 0)
                    len_max = len_cur_b;
                /* 验证:路径黑色节点不一致 */
                if (len_max != len_cur_b)
                    return 0;
            }
        }
        
        /* 5.都进栈了,当前节点该退栈了 */
        if (flags[stack_t - 1] == app_sys_tree_rbsn_error) {
            if (app_sys_tree_rbsn_get_color(node) == app_sys_tree_rbsn_color_b)
                len_cur_b--; 
            stack_t--;
        }
        
        /* 堆栈太小,警告 */
        APP_SYS_TREE_RBST_CHECK(stack_t >= len, "overflow");
    } while (stack_t > 0);
    
    /* 6.退栈完毕了,验证完成 */
    return 1;
}
