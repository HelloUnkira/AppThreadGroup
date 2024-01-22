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
#define APP_SYS_TREE_RBT_CHECK(cond, str)   if (cond) APP_SYS_LOG_ERROR(str)

/*@brief 红黑树(设置语义)
 *@param tree 红黑树实例
 *@param root 红黑树根
 */
static inline void app_sys_tree_rbt_set_root(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *root)
{
    tree->root = root;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树比较语义
 */
static inline void app_sys_tree_rbt_set_compare(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_compare_t compare)
{
    tree->compare = compare;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树确认语义
 */
static inline void app_sys_tree_rbt_set_confirm(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_confirm_t confirm)
{
    tree->confirm = confirm;
}

/*@brief 红黑树(设置语义)
 *@param tree    红黑树实例
 *@param compare 红黑树访问语义
 */
static inline void app_sys_tree_rbt_set_visit(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_visit_t visit)
{
    tree->visit = visit;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树根
 */
static inline app_sys_tree_rbn_t * app_sys_tree_rbt_get_root(app_sys_tree_rbt_t *tree)
{
    return tree->root;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树比较语义
 */
static inline app_sys_tree_rbt_compare_t app_sys_tree_rbt_get_compare(app_sys_tree_rbt_t *tree)
{
    return tree->compare;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树确认语义
 */
static inline app_sys_tree_rbt_confirm_t app_sys_tree_rbt_get_confirm(app_sys_tree_rbt_t *tree)
{
    return tree->confirm;
}

/*@brief 红黑树(获取语义)
 *@param tree 红黑树实例
 *@retval 红黑树访问语义
 */
static inline app_sys_tree_rbt_visit_t app_sys_tree_rbt_get_visit(app_sys_tree_rbt_t *tree)
{
    return tree->visit;
}

/*@brief 红黑节点(设置语义)
 *@param node  红黑节点实例
 *@param color 红黑节点颜色
 */
static inline void app_sys_tree_rbn_set_color(app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t color)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->parent_color);
    retval &= (uintptr_t)(~1);
    retval |= (uintptr_t)(color & 1);
    
    node->parent_color = (app_sys_tree_rbn_t *)(retval);
}

/*@brief 红黑节点(设置语义)
 *@param node   红黑节点实例
 *@param parent 红黑节点实例
 */
static inline void app_sys_tree_rbn_set_parent(app_sys_tree_rbn_t *node, app_sys_tree_rbn_t *parent)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->parent_color);
    retval &= (uintptr_t)(1);
    retval |= (uintptr_t)(parent);
    
    node->parent_color = (app_sys_tree_rbn_t *)(retval);
}

/*@brief 红黑节点(设置语义)
 *@param node  红黑节点实例
 *@param child 红黑节点实例
 *@param side  红黑节点方向
 */
static inline void app_sys_tree_rbn_set_child(app_sys_tree_rbn_t *node, app_sys_tree_rbn_t *child, app_sys_tree_rbn_status_t side)
{
    if (side == app_sys_tree_rbn_side_l)
        node->child_l = child;
    if (side == app_sys_tree_rbn_side_r)
        node->child_r = child;
}

/*@brief 红黑节点(获取语义)
 *@param node 红黑节点实例
 *@retval 红黑节点颜色
 */
static inline app_sys_tree_rbn_status_t app_sys_tree_rbn_get_color(app_sys_tree_rbn_t *node)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->parent_color);
    retval &= (uintptr_t)(1);
    
    return (app_sys_tree_rbn_status_t)retval;
}

/*@brief 红黑节点(获取语义)
 *@param node 红黑节点实例
 *@retval 红黑节点实例
 */
static inline app_sys_tree_rbn_t * app_sys_tree_rbn_get_parent(app_sys_tree_rbn_t *node)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->parent_color);
    retval &= (uintptr_t)(~1);
    
    return (app_sys_tree_rbn_t *)retval;
}

/*@brief 红黑节点(获取语义)
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static inline app_sys_tree_rbn_t * app_sys_tree_rbn_get_child(app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t side)
{
    return side == app_sys_tree_rbn_side_l ? node->child_l :
           side == app_sys_tree_rbn_side_r ? node->child_r : NULL;
}

/*@brief 红黑节点(获取语义)
 *@param node   红黑节点实例
 *@param parent 红黑节点实例
 *@retval 红黑节点方向
 */
static inline app_sys_tree_rbn_status_t app_sys_tree_rbn_get_side(app_sys_tree_rbn_t *node, app_sys_tree_rbn_t *parent)
{
    return node == parent->child_l ? app_sys_tree_rbn_side_l :
           node == parent->child_r ? app_sys_tree_rbn_side_r : app_sys_tree_rbn_error;
}

/*@brief 红黑节点(获取语义)
 *@param side 红黑节点方向
 *@retval 红黑节点方向
 */
static inline app_sys_tree_rbn_status_t app_sys_tree_rbn_get_side_other(app_sys_tree_rbn_status_t side)
{
    return side == app_sys_tree_rbn_side_l ? app_sys_tree_rbn_side_r :
           side == app_sys_tree_rbn_side_r ? app_sys_tree_rbn_side_l : app_sys_tree_rbn_error;
}

/* 辅助定制宏,美化代码,剔除边缘逻辑 */
#define APP_SYS_TREE_RBT_CHECK_RETURN(expr)         if (expr) return;
#define APP_SYS_TREE_RBT_CHECK_RETURN_NULL(expr)    if (expr) return NULL;

/*@brief 红黑树核心动作:交换(左右旋转)
 *@param tree   红黑树实例
 *@param child  红黑节点实例
 *@param parent 红黑节点实例
 */
static void app_sys_tree_rbt_rotate(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *child, app_sys_tree_rbn_t *parent)
{
    APP_SYS_TREE_RBT_CHECK(tree   == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(child  == NULL, "child");
    APP_SYS_TREE_RBT_CHECK(parent == NULL, "parent");
    /* 1.获得父子节点的对位关系,父亲与对顶位孙子的对位关系 */
    app_sys_tree_rbn_status_t side       = app_sys_tree_rbn_get_side(child, parent);
    app_sys_tree_rbn_status_t side_other = app_sys_tree_rbn_get_side_other(side);
    /* 2.获得祖父节点,获得父亲对顶位的孙子节点 */
    app_sys_tree_rbn_t *ancestor = app_sys_tree_rbn_get_parent(parent);
    app_sys_tree_rbn_t *son      = app_sys_tree_rbn_get_child(child, side_other);
    /* 3.原父亲获得新孩子,孙子获得新父亲 */
    app_sys_tree_rbn_set_child(parent, son, side);
    if (son != NULL)
        app_sys_tree_rbn_set_parent(son, parent);
    /* 4.交换父子节点位置 */
    app_sys_tree_rbn_set_child(child, parent, side_other);
    app_sys_tree_rbn_set_parent(child, ancestor);
    app_sys_tree_rbn_set_parent(parent, child);
    /* 5.更新祖父节点 */
    if (ancestor == NULL)
        app_sys_tree_rbt_set_root(tree, child);
    if (ancestor != NULL) {
        side = app_sys_tree_rbn_get_side(parent, ancestor);
        app_sys_tree_rbn_set_child(ancestor, child, side);
    }
}

/*@brief 搜索函数(大小元)
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static app_sys_tree_rbn_t * app_sys_tree_rbt_search_min_or_max(app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t side)
{
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    APP_SYS_TREE_RBT_CHECK(side == app_sys_tree_rbn_error, "side");
    
    while (node != NULL && app_sys_tree_rbn_get_child(node, side) != NULL)
        node = app_sys_tree_rbn_get_child(node, side);
    
    return node;
}

/*@brief 搜索函数(前驱和后继)
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 *@retval 红黑节点实例
 */
static app_sys_tree_rbn_t * app_sys_tree_rbn_search_prev_or_next(app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t side)
{
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    APP_SYS_TREE_RBT_CHECK(side == app_sys_tree_rbn_error, "side");
    
    app_sys_tree_rbn_t *child  = app_sys_tree_rbn_get_child(node, side);
    app_sys_tree_rbn_t *parent = app_sys_tree_rbn_get_parent(node);
    app_sys_tree_rbn_status_t side_other = app_sys_tree_rbn_get_side_other(side);
    /* 先找孩子最大项 */
    if (child != NULL)
        return app_sys_tree_rbt_search_min_or_max(child, side_other);
    /* 节点的孩子不存在,找父亲 */
    while (parent != NULL) {
        /* 节点的孩子不存在,父亲与对应的孩子在同一边 */
        if (app_sys_tree_rbn_get_side(node, parent) == side_other)
            return parent;
        /* 父亲与对应孩子不在同一边,找递归项到第一个相遇的同边 */
        node    = parent;
        parent  = app_sys_tree_rbn_get_parent(parent);
    }
    /* 退出可能: */
    /* 父亲一开始就不存在(根节点) */
    /* 祖父节点不存在(迭代到根节点,端点边界值没有对应的下一项) */
    return NULL;
}

/*@brief 查找函数
 *@param tree   红黑树实例
 *@param target 红黑节点实例
 *@retval 红黑节点实例
 */
static app_sys_tree_rbn_t * app_sys_tree_rbt_search_only(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *target)
{
    APP_SYS_TREE_RBT_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(app_sys_tree_rbt_get_compare(tree) == NULL, "compare");
    APP_SYS_TREE_RBT_CHECK(app_sys_tree_rbt_get_confirm(tree) == NULL, "confirm");
    APP_SYS_TREE_RBT_CHECK(target == NULL, "target");
    
    app_sys_tree_rbn_t *node = app_sys_tree_rbt_get_root(tree);
    while (node != NULL) {
        /* 关键字匹配成功,返回 */
        if (app_sys_tree_rbt_get_confirm(tree)(target, node) == 0)
            return node;
        /* 该结点小于此本结点,到左子树去 */
        if (app_sys_tree_rbt_get_compare(tree)(target, node) != 0) {
            node = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
            continue;
        }
        /* 该结点大于此本结点,到右子树去 */
        if (app_sys_tree_rbt_get_compare(tree)(target, node) == 0) {
            node = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
            continue;
        }
    }
    return NULL;
}

/*@brief 单次插入函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
static void app_sys_tree_rbt_insert_only(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    APP_SYS_TREE_RBT_CHECK(app_sys_tree_rbt_get_compare(tree) == NULL, "compare");
    
    /* 1.初始化要插入的节点(插入节点都是红色的) */
    app_sys_tree_rbn_set_color(node,  app_sys_tree_rbn_color_r);
    app_sys_tree_rbn_set_child(node,  NULL, app_sys_tree_rbn_side_l);
    app_sys_tree_rbn_set_child(node,  NULL, app_sys_tree_rbn_side_r);
    app_sys_tree_rbn_set_parent(node, NULL);
    
    /* 2.如果是插入到根节点 */
    if (app_sys_tree_rbt_get_root(tree) == NULL) {
        app_sys_tree_rbt_set_root(tree, node);
        return;
    }
    
    app_sys_tree_rbn_t *insert_node   = app_sys_tree_rbt_get_root(tree);
    app_sys_tree_rbn_t *insert_node_l = NULL;
    app_sys_tree_rbn_t *insert_node_r = NULL;
    /* 3.从根节点开始寻找插入的起始位置并插入 */
    while (insert_node != NULL) {
        /* 左子树为空且该结点小于此本结点,表示结点可以插到其左子树 */
        if (app_sys_tree_rbt_get_compare(tree)(node, insert_node) != 0) {
            /* 获得其左孩子 */
            insert_node_l = app_sys_tree_rbn_get_child(insert_node, app_sys_tree_rbn_side_l);
            /* 左孩子不为空继续迭代,左孩子为空插入 */
            if (insert_node_l != NULL) {
                insert_node    = insert_node_l;
                continue;
            }
            app_sys_tree_rbn_set_child(insert_node, node, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_set_parent(node, insert_node);
            return;
        }
        /* 右子树为空且该节点不小于此本节点,表示结点可以插到其右子树 */
        if (app_sys_tree_rbt_get_compare(tree)(node, insert_node) == 0) {
            /* 获得其右孩子 */
            insert_node_r = app_sys_tree_rbn_get_child(insert_node, app_sys_tree_rbn_side_r);
            /* 右孩子不为空继续迭代,右孩子为空插入 */
            if (insert_node_r != NULL) {
                insert_node    = insert_node_r;
                continue;
            }
            app_sys_tree_rbn_set_child(insert_node, node, app_sys_tree_rbn_side_r);
            app_sys_tree_rbn_set_parent(node, insert_node);
            return;
        }
    }
}

/*@brief 插入调整函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
static void app_sys_tree_rbt_insert_adjust(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    
    do {
        app_sys_tree_rbn_t *parent = app_sys_tree_rbn_get_parent(node);
        /* 情况1:插入节点是根节点,该结点染色为黑色 */
        if (parent == NULL) {
            app_sys_tree_rbn_set_color(node, app_sys_tree_rbn_color_b);
            return;
        }
        /* 情况2:插入节点父节点是黑色,什么都不做 */
        APP_SYS_TREE_RBT_CHECK_RETURN(app_sys_tree_rbn_get_color(parent) == app_sys_tree_rbn_color_b);
        /* 情况3:插入节点的父节点是红色(无需判断,一定是红节点) */
        /* 1.获得祖父节点(红色父节点一定有祖父节点) */
        app_sys_tree_rbn_t *ancestor = app_sys_tree_rbn_get_parent(parent);
        app_sys_tree_rbn_status_t side       = app_sys_tree_rbn_get_side(parent, ancestor);
        app_sys_tree_rbn_status_t side_other = app_sys_tree_rbn_get_side_other(side);
        /* 2.获得叔叔节点 */
        app_sys_tree_rbn_t *uncle = app_sys_tree_rbn_get_child(ancestor, side_other);
        /* 情况3.1:同时出现双红色节点,将冲突向上调一层 */
        if (uncle != NULL && app_sys_tree_rbn_get_color(uncle) == app_sys_tree_rbn_color_r) {
            /* 1.将父节点和叔叔节点设为黑色 */
            app_sys_tree_rbn_set_color(parent, app_sys_tree_rbn_color_b);
            app_sys_tree_rbn_set_color(uncle,  app_sys_tree_rbn_color_b);
            /* 2.将祖父节点设为红色 */
            app_sys_tree_rbn_set_color(ancestor, app_sys_tree_rbn_color_r);
            /* 3.将祖父节点设为当前节点调整 */
            node = ancestor;
            continue;
        }
        
        /* 情况3.2:不管叔叔节点是否存在或者为黑色,此时调整只能在内部进行 */
        app_sys_tree_rbn_status_t side_child = app_sys_tree_rbn_get_side(node, parent);
        /* 情况3.2.1:如果是之字型分支,先通过旋转调整到俩个对位 */
        if ((side == app_sys_tree_rbn_side_l && side_child == app_sys_tree_rbn_side_r) ||
            (side == app_sys_tree_rbn_side_r && side_child == app_sys_tree_rbn_side_l)) {
            /* 1.将父节点作为新的当前节点 */
            /* 2.对父节点进行旋转 */
            app_sys_tree_rbt_rotate(tree, node, parent);
            /* 更新索引为下述情况使用 */
            side_child = app_sys_tree_rbn_get_side_other(side_child);
            /*  */
            app_sys_tree_rbn_t *temp1 = node;
            app_sys_tree_rbn_t *temp2 = parent;
            node   = temp2;
            parent = temp1;
        }

        /* 情况3.2.2:如果是对位分支,直接内部旋转 */
        if ((side == app_sys_tree_rbn_side_l && side_child == app_sys_tree_rbn_side_l) ||
            (side == app_sys_tree_rbn_side_r && side_child == app_sys_tree_rbn_side_r)) {
            /* 1.将父节点设为黑色 */
            app_sys_tree_rbn_set_color(parent, app_sys_tree_rbn_color_b); 
            /* 2.将祖父节点设为红色 */
            app_sys_tree_rbn_set_color(ancestor, app_sys_tree_rbn_color_r); 
            /* 3.对祖父节点进行旋转 */
            app_sys_tree_rbt_rotate(tree, parent, ancestor);
            /* 经过旋转调整了父节点和祖父节点的关系 */
            if (app_sys_tree_rbn_get_parent(parent) == NULL)
                app_sys_tree_rbt_set_root(tree, parent);
        }
        return;
    } while (1);
}

/*@brief 单次删除函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 */
static void app_sys_tree_rbt_remove_only(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t *side)
{
    APP_SYS_TREE_RBT_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    /* 找不到要删除的结点，就return */
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    /* 1.获得该节点的左右孩子 */
    app_sys_tree_rbn_t *child_l = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
    app_sys_tree_rbn_t *child_r = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
    /* 情况1:该结点是叶子结点,删除该结点 */
    /* 情况2:该结点仅有一个孩子,删除该节点 */
    /* 情况3:该结点有俩个孩子,找一个(前驱或后继)节点替换它 */
    if (child_l != NULL && child_r != NULL) {
        /* 1.获取其前驱和后继(这里使用最大前驱,也可以使用最小后继,反一下即可) */
        app_sys_tree_rbn_t *prev = app_sys_tree_rbt_search_min_or_max(child_l, app_sys_tree_rbn_side_r);
        /* 2.更新需要交换的俩个父亲的孩子信息 */
        app_sys_tree_rbn_t *prev_parent = app_sys_tree_rbn_get_parent(prev);
        app_sys_tree_rbn_t *node_parent = app_sys_tree_rbn_get_parent(node);
        /* 这里有一些特殊情况需要检查,原则上,这是通过在节点之间交换子指针 */
        /* 并将指向它们的节点从它们的父节点重新定位来实现的,但是: */
        /* (1)上面的节点可能是树的根,没有父节点 */
        if (node_parent == NULL)
            app_sys_tree_rbt_set_root(tree, prev);
        if (node_parent != NULL)
            app_sys_tree_rbn_set_child(node_parent, prev, app_sys_tree_rbn_get_side(node, node_parent));
        /* (2)下面的节点可能是上面节点的直接子节点 */
        if (prev_parent == node) {
            /* 交换俩个节点的左孩子 */
            app_sys_tree_rbn_t *node_child_l = prev;
            app_sys_tree_rbn_t *prev_child_l = app_sys_tree_rbn_get_child(prev, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_set_child(node, prev_child_l, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_set_child(prev, node, app_sys_tree_rbn_side_l);
            /* 左孩子父亲更新 */
            if (prev_child_l != NULL)
                app_sys_tree_rbn_set_parent(prev_child_l, node);
            /* 交换俩个节点的父亲 */
            app_sys_tree_rbn_set_parent(prev, node_parent);
            app_sys_tree_rbn_set_parent(node, prev);
        }
        if (prev_parent != node) {
            /* 更新低父亲指向高孩子 */
            app_sys_tree_rbn_set_child(prev_parent, node, app_sys_tree_rbn_get_side(prev, prev_parent));
            /* 交换俩个节点的左孩子 */
            app_sys_tree_rbn_t *node_child_l = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_t *prev_child_l = app_sys_tree_rbn_get_child(prev, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_set_child(node, prev_child_l, app_sys_tree_rbn_side_l);
            app_sys_tree_rbn_set_child(prev, node_child_l, app_sys_tree_rbn_side_l);
            /* 左孩子父亲更新 */
            if (node_child_l != NULL)
                app_sys_tree_rbn_set_parent(node_child_l, prev);
            if (prev_child_l != NULL)
                app_sys_tree_rbn_set_parent(prev_child_l, node);
            /* 交换俩个节点的父亲 */
            app_sys_tree_rbn_set_parent(prev, node_parent);
            app_sys_tree_rbn_set_parent(node, prev_parent);
        }
        /* 交换俩个节点的右孩子 */
        app_sys_tree_rbn_t *node_child_r = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
        app_sys_tree_rbn_t *prev_child_r = NULL;
        app_sys_tree_rbn_set_child(prev, node_child_r, app_sys_tree_rbn_side_r);
        app_sys_tree_rbn_set_child(node, prev_child_r, app_sys_tree_rbn_side_r);
        /* 右孩子父亲更新 */
        if (node_child_r != NULL)
            app_sys_tree_rbn_set_parent(node_child_r, prev);
        /* 交换俩个节点颜色 */
        app_sys_tree_rbn_status_t node_color = app_sys_tree_rbn_get_color(node);
        app_sys_tree_rbn_status_t prev_color = app_sys_tree_rbn_get_color(prev);
        app_sys_tree_rbn_set_color(node, prev_color);
        app_sys_tree_rbn_set_color(prev, node_color);
        /* 3.此时被删除元素变成其后继结点 */
        child_l = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
        child_r = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
    }
    /* 2.经过转化,变成获得可能存在的一个孩子 */
    app_sys_tree_rbn_t *child = child_l == NULL ? child_r : child_l;
    /* 3.解除目标target在原集合的关联 */
    app_sys_tree_rbn_t *parent = app_sys_tree_rbn_get_parent(node);
    /* 4.如果是要删除根节点 */
    if (parent == NULL) {
        app_sys_tree_rbt_set_root(tree, child);
        return;
    }
    /* 5.父亲获得新孩子 */
    *side = app_sys_tree_rbn_get_side(node, parent);
    app_sys_tree_rbn_set_child(parent, child, *side);
    /* 6.为孩子设置新父亲 */
    if (child != NULL)
        app_sys_tree_rbn_set_parent(child, parent);
}

/*@brief 删除调整函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 *@param side 红黑节点方向
 */
static void app_sys_tree_rbt_remove_adjust(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node, app_sys_tree_rbn_status_t side)
{
    APP_SYS_TREE_RBT_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_RBT_CHECK(node == NULL, "node");
    
    /* 情况1:删除空节点,无需调整 */
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    /* 1.获得删除的节点的父亲和左右孩子 */
    app_sys_tree_rbn_status_t color = app_sys_tree_rbn_get_color(node);
    app_sys_tree_rbn_t *child_l  = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
    app_sys_tree_rbn_t *child_r  = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
    app_sys_tree_rbn_t *only_one = child_l == NULL ? child_r : child_l;
    /* 情况2:删除节点为红色,无需调整 */
    APP_SYS_TREE_RBT_CHECK_RETURN(color == app_sys_tree_rbn_color_r);
    /* 情况3:自己为黑色但是唯一孩子为红色 */
    if (only_one != NULL && app_sys_tree_rbn_get_color(only_one) == app_sys_tree_rbn_color_r) {
        app_sys_tree_rbn_set_color(only_one, app_sys_tree_rbn_color_b);
        return;
    }
    /* 情况4:自己是黑色,删除导致缺失了一个黑色 */
    do {
        app_sys_tree_rbn_t *parent = app_sys_tree_rbn_get_parent(node);
        app_sys_tree_rbn_status_t side_sibling = app_sys_tree_rbn_get_side_other(side);
        /* 情况4.1:删除的节点为根节点(或者调整到根节点),什么都不做 */
        APP_SYS_TREE_RBT_CHECK_RETURN(parent == NULL);
        /* 否则,删除黑色节点导致的不平衡兄弟一定存在 */
        app_sys_tree_rbn_t *sibling = app_sys_tree_rbn_get_child(parent, side_sibling);
        /* 情况4.2:兄弟节点为红色 */
        if (app_sys_tree_rbn_get_color(sibling) == app_sys_tree_rbn_color_r) {
            /* 1.父节点染成红色,兄弟节点染成黑色 */
            app_sys_tree_rbn_set_color(sibling, app_sys_tree_rbn_color_b);
            app_sys_tree_rbn_set_color(parent,  app_sys_tree_rbn_color_r);
            /* 2.对父节点进行旋转 */
            app_sys_tree_rbt_rotate(tree, sibling, parent);
            /* 更新父子兄弟关系 */
            sibling = app_sys_tree_rbn_get_child(parent, side_sibling);
        }
        
        app_sys_tree_rbn_t *child_sibling_l = app_sys_tree_rbn_get_child(sibling, app_sys_tree_rbn_side_l);
        app_sys_tree_rbn_t *child_sibling_r = app_sys_tree_rbn_get_child(sibling, app_sys_tree_rbn_side_r);
        
        /* 情况4.3:孩子都是黑色 */
        if ((child_sibling_l == NULL || app_sys_tree_rbn_get_color(child_sibling_l) == app_sys_tree_rbn_color_b) &&
            (child_sibling_r == NULL || app_sys_tree_rbn_get_color(child_sibling_r) == app_sys_tree_rbn_color_b)) {
            if (app_sys_tree_rbn_get_color(parent) == app_sys_tree_rbn_color_b) {
                /* 情况4.3.1:父亲是黑色 */
                /* 1.染色兄弟 */
                app_sys_tree_rbn_set_color(sibling, app_sys_tree_rbn_color_r);
                /* 2.调整到上一层 */
                node = parent;
                parent = app_sys_tree_rbn_get_parent(node);
                APP_SYS_TREE_RBT_CHECK_RETURN(parent == NULL);
                side = app_sys_tree_rbn_get_side(node, parent);
                continue;
            } else {
                /* 情况4.3.2:父亲是红色 */
                /* 1.交换父亲兄弟的颜色 */
                app_sys_tree_rbn_set_color(parent,  app_sys_tree_rbn_color_b);
                app_sys_tree_rbn_set_color(sibling, app_sys_tree_rbn_color_r);
                return;
            }
        }
        
        app_sys_tree_rbn_t *child_red = NULL;
        /* 情况4.4:如果是之字型分支,先通过旋转调整到俩个对位 */
        /* 情况4.4.1:孩子节点是父左节点,兄弟左儿子是红色,右儿子是黑色 */
        /* 情况4.4.2:孩子节点是父右节点,兄弟右儿子是红色,左儿子是黑色 */
        if (side == app_sys_tree_rbn_side_l &&
           (child_sibling_r == NULL || app_sys_tree_rbn_get_color(child_sibling_r) == app_sys_tree_rbn_color_b))
            child_red = child_sibling_l;
        
        if (side == app_sys_tree_rbn_side_r &&
           (child_sibling_l == NULL || app_sys_tree_rbn_get_color(child_sibling_l) == app_sys_tree_rbn_color_b))
            child_red = child_sibling_r;
        
        if (child_red != NULL) {
            /* 1.染红兄弟节点,染黑红色孩子 */
            app_sys_tree_rbn_set_color(child_red, app_sys_tree_rbn_color_b);
            app_sys_tree_rbn_set_color(sibling,   app_sys_tree_rbn_color_r);
            /* 2.对兄弟节点进行旋转 */
            app_sys_tree_rbt_rotate(tree, child_red, sibling);
            /* 兄弟节点以及对应的左右孩子都更新了! */
            sibling = child_red;
            child_sibling_l = app_sys_tree_rbn_get_child(sibling, app_sys_tree_rbn_side_l);
            child_sibling_r = app_sys_tree_rbn_get_child(sibling, app_sys_tree_rbn_side_r);
        }
        
        child_red = NULL;
        /* 情况4.5:如果是对位分支,直接内部旋转 */
        /* 情况4.5.1:孩子节点是父左节点,兄弟的右孩子为红色 */
        /* 情况4.5.2:孩子节点是父右节点,兄弟的左孩子为红色 */
        if (side == app_sys_tree_rbn_side_l &&
           (child_sibling_r != NULL && app_sys_tree_rbn_get_color(child_sibling_r) == app_sys_tree_rbn_color_r))
            child_red = child_sibling_r;
        
        if (side == app_sys_tree_rbn_side_r &&
           (child_sibling_l != NULL && app_sys_tree_rbn_get_color(child_sibling_l) == app_sys_tree_rbn_color_r))
            child_red = child_sibling_l;
        
        if (child_red != NULL) {
            /* 1.兄弟获得父节点的颜色 */
            app_sys_tree_rbn_set_color(sibling, app_sys_tree_rbn_get_color(parent));
            /* 2.父节点获得兄弟节点的颜色 */
            app_sys_tree_rbn_set_color(parent, app_sys_tree_rbn_color_b);
            /* 3.染黑红子节点 */
            app_sys_tree_rbn_set_color(child_red, app_sys_tree_rbn_color_b);
            /* 4.对父节点进行旋转 */
            app_sys_tree_rbt_rotate(tree, sibling, parent);
        }
        
        return;
    } while (1);
}

/*@brief 插入函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbt_insert(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    
    app_sys_tree_rbt_insert_only(tree,   node);
    app_sys_tree_rbt_insert_adjust(tree, node);
}

/*@brief 删除函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbt_remove(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    
    app_sys_tree_rbn_status_t side = app_sys_tree_rbn_error;
    
    app_sys_tree_rbt_remove_only(tree,   node, &side);
    app_sys_tree_rbt_remove_adjust(tree, node,  side);
}

/*@brief 搜索函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(tree == NULL);
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(node == NULL);
    
    return app_sys_tree_rbt_search_only(tree, node);
}

/*@brief 搜索函数(最小节点)
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search_min(app_sys_tree_rbt_t *tree)
{
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(tree == NULL);
    
    app_sys_tree_rbn_t *root = app_sys_tree_rbt_get_root(tree);
    return app_sys_tree_rbt_search_min_or_max(root, app_sys_tree_rbn_side_l);
}

/*@brief 搜索函数(最大节点)
 *@param tree 红黑树实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbt_search_max(app_sys_tree_rbt_t *tree)
{
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(tree == NULL);
    
    app_sys_tree_rbn_t *root = app_sys_tree_rbt_get_root(tree);
    return app_sys_tree_rbt_search_min_or_max(root, app_sys_tree_rbn_side_r);
}

/*@brief 搜索函数(前驱节点)
 *@param node 红黑节点实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbn_search_prev(app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(node == NULL);
    
    return app_sys_tree_rbn_search_prev_or_next(node, app_sys_tree_rbn_side_l);
}

/*@brief 搜索函数(后继节点)
 *@param node 红黑节点实例
 *@retval 红黑节点实例
 */
app_sys_tree_rbn_t * app_sys_tree_rbn_search_next(app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN_NULL(node == NULL);
    
    return app_sys_tree_rbn_search_prev_or_next(node, app_sys_tree_rbn_side_r);
}

/*@brief 复位函数
 *@param tree 红黑树实例
 */
void app_sys_tree_rbt_reset(app_sys_tree_rbt_t *tree)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbt_set_root(tree, NULL);
    app_sys_tree_rbt_set_compare(tree, NULL);
    app_sys_tree_rbt_set_confirm(tree, NULL);
    app_sys_tree_rbt_set_visit(tree, NULL);
}

/*@brief 复位函数
 *@param node 红黑节点实例
 */
void app_sys_tree_rbn_reset(app_sys_tree_rbn_t *node)
{
    app_sys_tree_rbn_set_parent(node, NULL);
    app_sys_tree_rbn_set_child(node, NULL, app_sys_tree_rbn_side_l);
    app_sys_tree_rbn_set_child(node, NULL, app_sys_tree_rbn_side_r);
}

/*@brief 配置函数
 *@param tree 红黑树实例
 *@param compare 红黑树比较语义
 *@param confirm 红黑树确认语义
 *@param visit   红黑树访问语义
 */
void app_sys_tree_rbt_config(app_sys_tree_rbt_t *tree, app_sys_tree_rbt_compare_t compare, app_sys_tree_rbt_confirm_t confirm, app_sys_tree_rbt_visit_t visit)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbt_set_compare(tree, compare);
    app_sys_tree_rbt_set_confirm(tree, confirm);
    app_sys_tree_rbt_set_visit(tree, visit);
}

/*@brief 根切换函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbt_root_set(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL);
    
    app_sys_tree_rbt_set_root(tree, node);
}

/*@brief 根切换函数
 *@param tree 红黑树实例
 *@param node 红黑节点实例
 */
void app_sys_tree_rbt_root_get(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t **node)
{
    *node = app_sys_tree_rbt_get_root(tree);
}

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree  红黑树实例
 *@param queue 红黑树节点实例队列数组
 *@param len   红黑树节点实例队列数组长度
 */
void app_sys_tree_rbt_seq_tra(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t **queue, int32_t len)
{
    /* 层序遍历(这里使用循环队列): */
    
    /* 获取插入的起始位置 */
    int32_t queue_h  = 0;
    int32_t queue_t  = 0;
    int32_t node_num = 1;
    queue[queue_t++] = app_sys_tree_rbt_get_root(tree);
    
    do {
        /* 节点出队列 */
        app_sys_tree_rbn_t *node = queue[queue_h++];
        node_num--;
        APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
        /* 节点访问 */
        app_sys_tree_rbt_get_visit(tree)(node, app_sys_tree_rbn_get_color(node));
        /* 这是一个循环队列 */
        if (queue_h >= len)
            queue_h  = 0;
        if (queue_t >= len)
            queue_t  = 0; 
        
        /* 子节点入队列 */
        app_sys_tree_rbn_t *child_l = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
        app_sys_tree_rbn_t *child_r = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
        
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
        APP_SYS_TREE_RBT_CHECK(node_num >= len, "overflow");
    } while (node_num > 0);
}

/*@brief 验证一棵树是否为红黑树(内部检查)
 *@param tree  红黑树实例
 *@param stack 红黑树节点实例栈数组
 *@param flags 红黑树节点实例栈数组
 *@param len   红黑树节点实例栈数组长度
 *@retval 0:失败;1:成功
 */
uint8_t app_sys_tree_rbt_check_valid(app_sys_tree_rbt_t *tree, app_sys_tree_rbn_t **stack, int32_t *flags, int32_t len)
{
    /* 验证:1根节点不是黑色 */
    if (app_sys_tree_rbt_get_root(tree) == NULL)
        return 1;
    if (app_sys_tree_rbn_get_color(app_sys_tree_rbt_get_root(tree)) == app_sys_tree_rbn_color_r)
        return 0;
    
    /* 初始化flags的值 */
    for (int32_t idx = 0; idx < len; flags[idx++] = app_sys_tree_rbn_side_l);
    /* 深度优先搜索,使用flags同步维护栈进动情况 */
    app_sys_tree_rbn_t *node    = NULL;
    app_sys_tree_rbn_t *child_l = NULL;
    app_sys_tree_rbn_t *child_r = NULL;
    /* 从根节点开始从左进动 */
    /* 入栈节点只有当左右孩子都使用完毕后才退栈 */
    int32_t stack_t = 0;    //栈顶位置
    int32_t len_max = 0;    //一条从根到叶子的最大度
    int32_t len_cur_b = 0;
    
    /* 1.根节点入栈 */
    stack[stack_t++] = app_sys_tree_rbt_get_root(tree);
    len_cur_b++;
    flags[stack_t - 1] = app_sys_tree_rbn_side_l;
    
    do {
        /* 2.获得栈顶元素的左右孩子 */
        node    = stack[stack_t - 1];
        child_l = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_l);
        child_r = app_sys_tree_rbn_get_child(node, app_sys_tree_rbn_side_r);
        
        /* 验证:2红色节点不相连 */
        if (child_l != NULL)
        if (app_sys_tree_rbn_get_color(node) == app_sys_tree_rbn_color_r &&
            app_sys_tree_rbn_get_color(child_l) == app_sys_tree_rbn_color_r)
            return 0;
                    
        if (child_r != NULL)
        if (app_sys_tree_rbn_get_color(node) == app_sys_tree_rbn_color_r &&
            app_sys_tree_rbn_get_color(child_r) == app_sys_tree_rbn_color_r)
            return 0;
        
        /* 3.左孩子未曾进过栈 */
        if (flags[stack_t - 1] == app_sys_tree_rbn_side_l) {
            /* 3.1.标记左孩子进过栈,下一次该右孩子进了 */
            flags[stack_t - 1] = app_sys_tree_rbn_side_r;
            /* 左孩子存在,可以进栈 */
            if(child_l != NULL) {
                stack[stack_t++] = child_l;
                /* 如果左孩子是黑色的,计算其度 */
                if (app_sys_tree_rbn_get_color(child_l) == app_sys_tree_rbn_color_b)
                    len_cur_b++;
                flags[stack_t - 1] = app_sys_tree_rbn_side_l;//左孩子的左孩子未曾进栈
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
        if (flags[stack_t - 1] == app_sys_tree_rbn_side_r) {
            /* 3.2标记右孩子进过栈,下一次该退栈了 */
            flags[stack_t - 1] = app_sys_tree_rbn_error;
            /* 右孩子存在,可以进栈 */
            if (child_r != NULL) {
                stack[stack_t++] = child_r;
                /* 如果右孩子是黑色的,计算其度 */
                if (app_sys_tree_rbn_get_color(child_r) == app_sys_tree_rbn_color_b)
                    len_cur_b++;
                flags[stack_t - 1] = app_sys_tree_rbn_side_l;//右孩子的左孩子未曾进栈
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
        if (flags[stack_t - 1] == app_sys_tree_rbn_error) {
            if (app_sys_tree_rbn_get_color(node) == app_sys_tree_rbn_color_b)
                len_cur_b--; 
            stack_t--;
        }
        
        /* 堆栈太小,警告 */
        APP_SYS_TREE_RBT_CHECK(stack_t >= len, "overflow");
    } while (stack_t > 0);
    
    /* 6.退栈完毕了,验证完成 */
    return 1;
}
