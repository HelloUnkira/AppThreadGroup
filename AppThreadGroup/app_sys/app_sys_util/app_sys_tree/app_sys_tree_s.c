/*实现目标:
 *    侵入式伸展树
 *    从当前树节点地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 条件检查语义 */
#define APP_SYS_TREE_ST_CHECK(cond, str)   if (cond) APP_SYS_LOG_ERROR(str)

/*@brief 伸展树(设置语义)
 *@param tree 伸展树实例
 *@param root 伸展树根
 */
static inline void app_sys_tree_st_set_root(app_sys_tree_st_t *tree, app_sys_tree_sn_t *root)
{
    tree->root = root;
}

/*@brief 伸展树(设置语义)
 *@param tree    伸展树实例
 *@param compare 伸展树比较语义
 */
static inline void app_sys_tree_st_set_compare(app_sys_tree_st_t *tree, app_sys_tree_st_compare_t compare)
{
    tree->compare = compare;
}

/*@brief 伸展树(设置语义)
 *@param tree    伸展树实例
 *@param compare 伸展树确认语义
 */
static inline void app_sys_tree_st_set_confirm(app_sys_tree_st_t *tree, app_sys_tree_st_confirm_t confirm)
{
    tree->confirm = confirm;
}

/*@brief 伸展树(设置语义)
 *@param tree    伸展树实例
 *@param compare 伸展树访问语义
 */
static inline void app_sys_tree_st_set_visit(app_sys_tree_st_t *tree, app_sys_tree_st_visit_t visit)
{
    tree->visit = visit;
}

/*@brief 伸展树(获取语义)
 *@param tree 伸展树实例
 *@retval 伸展树根
 */
static inline app_sys_tree_sn_t * app_sys_tree_st_get_root(app_sys_tree_st_t *tree)
{
    return tree->root;
}

/*@brief 伸展树(获取语义)
 *@param tree 伸展树实例
 *@retval 伸展树比较语义
 */
static inline app_sys_tree_st_compare_t app_sys_tree_st_get_compare(app_sys_tree_st_t *tree)
{
    return tree->compare;
}

/*@brief 伸展树(获取语义)
 *@param tree 伸展树实例
 *@retval 伸展树确认语义
 */
static inline app_sys_tree_st_confirm_t app_sys_tree_st_get_confirm(app_sys_tree_st_t *tree)
{
    return tree->confirm;
}

/*@brief 伸展树(获取语义)
 *@param tree 伸展树实例
 *@retval 伸展树访问语义
 */
static inline app_sys_tree_st_visit_t app_sys_tree_st_get_visit(app_sys_tree_st_t *tree)
{
    return tree->visit;
}

/*@brief 伸展节点(设置语义)
 *@param node   伸展节点实例
 *@param parent 伸展节点实例
 */
static inline void app_sys_tree_sn_set_parent(app_sys_tree_sn_t *node, app_sys_tree_sn_t *parent)
{
    node->parent = parent;
}

/*@brief 伸展节点(设置语义)
 *@param node  伸展节点实例
 *@param child 伸展节点实例
 *@param side  伸展节点方向
 */
static inline void app_sys_tree_sn_set_child(app_sys_tree_sn_t *node, app_sys_tree_sn_t *child, app_sys_tree_sn_status_t side)
{
    if (side == app_sys_tree_sn_side_l)
        node->child_l = child;
    if (side == app_sys_tree_sn_side_r)
        node->child_r = child;
}

/*@brief 伸展节点(获取语义)
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
static inline app_sys_tree_sn_t * app_sys_tree_sn_get_parent(app_sys_tree_sn_t *node)
{
    return node->parent;
}

/*@brief 伸展节点(获取语义)
 *@param node 伸展节点实例
 *@param side 伸展节点方向
 *@retval 伸展节点实例
 */
static inline app_sys_tree_sn_t * app_sys_tree_sn_get_child(app_sys_tree_sn_t *node, app_sys_tree_sn_status_t side)
{
    return side == app_sys_tree_sn_side_l ? node->child_l :
           side == app_sys_tree_sn_side_r ? node->child_r : NULL;
}

/*@brief 伸展节点(获取语义)
 *@param node   伸展节点实例
 *@param parent 伸展节点实例
 *@retval 伸展节点方向
 */
static inline app_sys_tree_sn_status_t app_sys_tree_sn_get_side(app_sys_tree_sn_t *node, app_sys_tree_sn_t *parent)
{
    return node == parent->child_l ? app_sys_tree_sn_side_l :
           node == parent->child_r ? app_sys_tree_sn_side_r : app_sys_tree_sn_error;
}

/*@brief 伸展节点(获取语义)
 *@param side 伸展节点方向
 *@retval 伸展节点方向
 */
static inline app_sys_tree_sn_status_t app_sys_tree_sn_get_side_other(app_sys_tree_sn_status_t side)
{
    return side == app_sys_tree_sn_side_l ? app_sys_tree_sn_side_r :
           side == app_sys_tree_sn_side_r ? app_sys_tree_sn_side_l : app_sys_tree_sn_error;
}

/* 辅助定制宏,美化代码,剔除边缘逻辑 */
#define APP_SYS_TREE_ST_CHECK_RETURN(target) if (target) return;
#define APP_SYS_TREE_ST_CHECK_RETURN_NULL(target)  if (target) return NULL;

/*@brief 伸展树核心动作:交换(左右旋转)
 *@param tree   伸展树实例
 *@param child  伸展节点实例
 *@param parent 伸展节点实例
 */
static void app_sys_tree_st_rotate(app_sys_tree_st_t *tree, app_sys_tree_sn_t *child, app_sys_tree_sn_t *parent)
{
    APP_SYS_TREE_ST_CHECK(tree    == NULL, "tree");
    APP_SYS_TREE_ST_CHECK(child   == NULL, "child");
    APP_SYS_TREE_ST_CHECK(parent  == NULL, "parent");
    /* 1.获得父子节点的对位关系,父亲与对顶位孙子的对位关系 */
    app_sys_tree_sn_status_t side       = app_sys_tree_sn_get_side(child, parent);
    app_sys_tree_sn_status_t side_other = app_sys_tree_sn_get_side_other(side);
    /* 2.获得祖父节点,获得父亲对顶位的孙子节点 */
    app_sys_tree_sn_t *ancestor = app_sys_tree_sn_get_parent(parent);
    app_sys_tree_sn_t *son      = app_sys_tree_sn_get_child(child, side_other);
    /* 3.原父亲获得新孩子,孙子获得新父亲 */
    app_sys_tree_sn_set_child(parent, son, side);
    if (son != NULL)
        app_sys_tree_sn_set_parent(son, parent);
    /* 4.交换父子节点位置 */
    app_sys_tree_sn_set_child(child, parent, side_other);
    app_sys_tree_sn_set_parent(child, ancestor);
    app_sys_tree_sn_set_parent(parent, child);
    /* 5.更新祖父节点 */
    if (ancestor == NULL)
        app_sys_tree_st_set_root(tree, child);
    if (ancestor != NULL) {
        side = app_sys_tree_sn_get_side(parent, ancestor);
        app_sys_tree_sn_set_child(ancestor, child, side);
    }
}

/*@brief 伸展树核心动作:伸展(Zig,Zag,ZigZig,ZagZag,ZigZag,ZagZig)
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
static void app_sys_tree_st_splay(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    /* 不断地将其旋转到根即可,这满足伸展树的六种语义 */
    while (app_sys_tree_sn_get_parent(node) != NULL)
        app_sys_tree_st_rotate(tree, node, app_sys_tree_sn_get_parent(node));
}

/*@brief 搜索函数(大小元)
 *@param node 伸展节点实例
 *@param side 伸展节点方向
 *@retval 伸展节点实例
 */
static app_sys_tree_sn_t * app_sys_tree_st_search_min_or_max(app_sys_tree_sn_t *node, app_sys_tree_sn_status_t side)
{
    APP_SYS_TREE_ST_CHECK(node == NULL, "node");
    APP_SYS_TREE_ST_CHECK(side == app_sys_tree_sn_error, "side");
    
    while (node != NULL && app_sys_tree_sn_get_child(node, side) != NULL)
        node = app_sys_tree_sn_get_child(node, side);
    
    return node;
}

/*@brief 搜索函数(前驱和后继)
 *@param node 伸展节点实例
 *@param side 伸展节点方向
 *@retval 伸展节点实例
 */
static app_sys_tree_sn_t * app_sys_tree_sn_search_prev_or_next(app_sys_tree_sn_t *node, app_sys_tree_sn_status_t side)
{
    APP_SYS_TREE_ST_CHECK(node == NULL, "node");
    APP_SYS_TREE_ST_CHECK(side == app_sys_tree_sn_error, "side");
    
    app_sys_tree_sn_t *child  = app_sys_tree_sn_get_child(node, side);
    app_sys_tree_sn_t *parent = app_sys_tree_sn_get_parent(node);
    app_sys_tree_sn_status_t side_other = app_sys_tree_sn_get_side_other(side);
    /* 先找孩子最大项 */
    if (child != NULL)
        return app_sys_tree_st_search_min_or_max(child, side_other);
    /* 节点的孩子不存在,找父亲 */
    while (parent != NULL) {
        /* 节点的孩子不存在,父亲与对应的孩子在同一边 */
        if (app_sys_tree_sn_get_side(node, parent) == side_other)
            return parent;
        /* 父亲与对应孩子不在同一边,找递归项到第一个相遇的同边 */
        node   = parent;
        parent = app_sys_tree_sn_get_parent(parent);
    }
    /* 退出可能: */
    /* 父亲一开始就不存在(根节点) */
    /* 祖父节点不存在(迭代到根节点,端点边界值没有对应的下一项) */
    return NULL;
}

/*@brief 查找函数
 *@param tree   伸展树实例
 *@param target 伸展节点实例
 *@retval 伸展节点实例
 */
static app_sys_tree_sn_t * app_sys_tree_st_search_only(app_sys_tree_st_t *tree, app_sys_tree_sn_t *target)
{
    APP_SYS_TREE_ST_CHECK(tree == NULL, ":tree");
    APP_SYS_TREE_ST_CHECK(tree->compare == NULL, "compare");
    APP_SYS_TREE_ST_CHECK(tree->confirm == NULL, "confirm");
    APP_SYS_TREE_ST_CHECK(target == NULL, "target");
    
    app_sys_tree_sn_t *node = app_sys_tree_st_get_root(tree);
    while (node != NULL) {
        /* 关键字匹配成功,返回 */
        if (app_sys_tree_st_get_confirm(tree)(target, node) == 0)
            return node;
        /* 该结点小于此本结点,到左子树去 */
        if (app_sys_tree_st_get_compare(tree)(target, node) != 0) {
            node = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_l);
            continue;
        }
        /* 该结点大于此本结点,到右子树去 */
        if (app_sys_tree_st_get_compare(tree)(target, node) == 0) {
            node = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_r);
            continue;
        }
    }
    return NULL;
}

/*@brief 单次插入函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
static void app_sys_tree_st_insert_only(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_ST_CHECK(node == NULL, "node");
    APP_SYS_TREE_ST_CHECK(tree->compare == NULL, "compare");
    
    /* 1.初始化要插入的节点 */
    app_sys_tree_sn_set_parent(node, NULL);
    app_sys_tree_sn_set_child(node,  NULL, app_sys_tree_sn_side_l);
    app_sys_tree_sn_set_child(node,  NULL, app_sys_tree_sn_side_r);
    
    /* 2.如果是插入到根节点 */
    if (app_sys_tree_st_get_root(tree) == NULL) {
        app_sys_tree_st_set_root(tree, node);
        return;
    }
    
    app_sys_tree_sn_t *insert_node = app_sys_tree_st_get_root(tree);
    app_sys_tree_sn_t *insert_node_l = NULL;
    app_sys_tree_sn_t *insert_node_r = NULL;
    /* 3.从根节点开始寻找插入的起始位置并插入 */
    while (insert_node != NULL) {
        /* 左子树为空且该结点小于此本结点,表示结点可以插到其左子树 */
        if (app_sys_tree_st_get_compare(tree)(node, insert_node) != 0) {
            /* 获得其左孩子 */
            insert_node_l = app_sys_tree_sn_get_child(insert_node, app_sys_tree_sn_side_l);
            /* 左孩子不为空继续迭代,左孩子为空插入 */
            if (insert_node_l != NULL) {
                insert_node  = insert_node_l;
                continue; 
            }
            app_sys_tree_sn_set_child(insert_node, node, app_sys_tree_sn_side_l);
            app_sys_tree_sn_set_parent(node, insert_node);
            return;
        }
        /* 右子树为空且该节点不小于此本节点,表示结点可以插到其右子树 */
        if (app_sys_tree_st_get_compare(tree)(node, insert_node) == 0) {
            /* 获得其右孩子 */
            insert_node_r = app_sys_tree_sn_get_child(insert_node, app_sys_tree_sn_side_r);
            /* 右孩子不为空继续迭代,右孩子为空插入 */
            if (insert_node_r != NULL) {
                insert_node   = insert_node_r;
                continue;
            }
            app_sys_tree_sn_set_child(insert_node, node, app_sys_tree_sn_side_r);
            app_sys_tree_sn_set_parent(node, insert_node);
            return;
        }
    }
}

/*@brief 单次删除函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
static void app_sys_tree_st_remove_only(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_ST_CHECK(node == NULL, "node");
    /* 找不到要删除的结点，就return */
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    /* 1.获得该节点的左右孩子 */
    app_sys_tree_sn_t *child_l = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_l);
    app_sys_tree_sn_t *child_r = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_r);
    /* 情况1:该结点是叶子结点,删除该结点 */
    /* 情况2:该结点仅有一个孩子,删除该节点 */
    /* 情况3:该结点有俩个孩子,找一个(前驱或后继)节点替换它 */
    if (child_l != NULL && child_r != NULL) {
        /* 1.获取其前驱和后继(这里使用最大前驱,也可以使用最小后继,反一下即可) */
        app_sys_tree_sn_t *prev = app_sys_tree_st_search_min_or_max(child_l, app_sys_tree_sn_side_r);
        /* 2.更新需要交换的俩个父亲的孩子信息 */
        app_sys_tree_sn_t *prev_parent = app_sys_tree_sn_get_parent(prev);
        app_sys_tree_sn_t *node_parent = app_sys_tree_sn_get_parent(node);
        /* 这里有一些特殊情况需要检查,原则上,这是通过在节点之间交换子指针 */
        /* 并将指向它们的节点从它们的父节点重新定位来实现的,但是: */
        /* (1)上面的节点可能是树的根,没有父节点 */
        if (node_parent == NULL)
            app_sys_tree_st_set_root(tree, prev);
        if (node_parent != NULL)
            app_sys_tree_sn_set_child(node_parent, prev, app_sys_tree_sn_get_side(node, node_parent));
        /* (2)下面的节点可能是上面节点的直接子节点 */
        if (prev_parent == node) {
            /* 交换俩个节点的左孩子 */
            app_sys_tree_sn_t *node_child_l = prev;
            app_sys_tree_sn_t *prev_child_l = app_sys_tree_sn_get_child(prev, app_sys_tree_sn_side_l);
            app_sys_tree_sn_set_child(node, prev_child_l, app_sys_tree_sn_side_l);
            app_sys_tree_sn_set_child(prev, node, app_sys_tree_sn_side_l);
            /* 左孩子父亲更新 */
            if (prev_child_l != NULL)
                app_sys_tree_sn_set_parent(prev_child_l, node);
            /* 交换俩个节点的父亲 */
            app_sys_tree_sn_set_parent(prev, node_parent);
            app_sys_tree_sn_set_parent(node, prev);
        }
        if (prev_parent != node) {
            /* 更新低父亲指向高孩子 */
            app_sys_tree_sn_set_child(prev_parent, node, app_sys_tree_sn_get_side(prev, prev_parent));
            /* 交换俩个节点的左孩子 */
            app_sys_tree_sn_t *node_child_l = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_l);
            app_sys_tree_sn_t *prev_child_l = app_sys_tree_sn_get_child(prev, app_sys_tree_sn_side_l);
            app_sys_tree_sn_set_child(node, prev_child_l, app_sys_tree_sn_side_l);
            app_sys_tree_sn_set_child(prev, node_child_l, app_sys_tree_sn_side_l);
            /* 左孩子父亲更新 */
            if (node_child_l != NULL)
                app_sys_tree_sn_set_parent(node_child_l, prev);
            if (prev_child_l != NULL)
                app_sys_tree_sn_set_parent(prev_child_l, node);
            /* 交换俩个节点的父亲 */
            app_sys_tree_sn_set_parent(prev, node_parent);
            app_sys_tree_sn_set_parent(node, prev_parent);
        }
        /* 交换俩个节点的右孩子 */
        app_sys_tree_sn_t *node_child_r = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_r);
        app_sys_tree_sn_t *prev_child_r = NULL;
        app_sys_tree_sn_set_child(prev, node_child_r, app_sys_tree_sn_side_r);
        app_sys_tree_sn_set_child(node, prev_child_r, app_sys_tree_sn_side_r);
        /* 右孩子父亲更新 */
        if (node_child_r != NULL)
            app_sys_tree_sn_set_parent(node_child_r, prev);
        /* 3.此时被删除元素变成其后继结点 */
        child_l = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_l);
        child_r = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_r);
    }
    /* 2.经过转化,变成获得可能存在的一个孩子 */
    app_sys_tree_sn_t *child = child_l == NULL ? child_r : child_l;
    /* 3.解除目标target在原集合的关联 */
    app_sys_tree_sn_t *parent = app_sys_tree_sn_get_parent(node);
    /* 4.如果是要删除根节点 */
    if (parent == NULL) {
        app_sys_tree_st_set_root(tree, child);
        return;
    }
    /* 5.父亲获得新孩子 */
    app_sys_tree_sn_status_t side = app_sys_tree_sn_get_side(node, parent);
    app_sys_tree_sn_set_child(parent, child, side);
    /* 6.为孩子设置新父亲 */
    if (child != NULL)
        app_sys_tree_sn_set_parent(child, parent);
}

/*@brief 插入函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_insert(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    
    app_sys_tree_st_insert_only(tree, node);
    app_sys_tree_st_splay(tree, node);
}

/*@brief 删除函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_remove(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    
    app_sys_tree_st_remove_only(tree, node);
    /* 注意:移除时是要伸展其父亲而不是自己 */
    node = app_sys_tree_sn_get_parent(node);
    app_sys_tree_st_splay(tree, node);
}

/*@brief 搜索函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(node == NULL);
    
    app_sys_tree_sn_t *target = app_sys_tree_st_search_only(tree, node);
    app_sys_tree_st_splay(tree, target);
    return target;
}

/*@brief 搜索函数(前驱节点)
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_prev(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(node == NULL);
    app_sys_tree_sn_t *target = app_sys_tree_sn_search_prev_or_next(node, app_sys_tree_sn_side_l);
    app_sys_tree_st_splay(tree, target);
    return target;
}

/*@brief 搜索函数(后继节点)
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_next(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(node == NULL);
    app_sys_tree_sn_t *target = app_sys_tree_sn_search_prev_or_next(node, app_sys_tree_sn_side_r);
    app_sys_tree_st_splay(tree, target);
    return target;
}

/*@brief 搜索函数(最小节点)
 *@param tree 伸展树实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_min(app_sys_tree_st_t *tree)
{
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(tree == NULL);
    app_sys_tree_sn_t *root = app_sys_tree_st_get_root(tree);
    app_sys_tree_sn_t *target = app_sys_tree_st_search_min_or_max(root, app_sys_tree_sn_side_l);
    app_sys_tree_st_splay(tree, target);
    return target;
}

/*@brief 搜索函数(最大节点)
 *@param tree 伸展树实例
 *@retval 伸展节点实例
 */
app_sys_tree_sn_t * app_sys_tree_st_search_max(app_sys_tree_st_t *tree)
{
    APP_SYS_TREE_ST_CHECK_RETURN_NULL(tree == NULL);
    app_sys_tree_sn_t *root = app_sys_tree_st_get_root(tree);
    app_sys_tree_sn_t *target = app_sys_tree_st_search_min_or_max(root, app_sys_tree_sn_side_r);
    app_sys_tree_st_splay(tree, target);
    return target;
}

/*@brief 伸展树合并
 *@param tree  伸展树实例
 *@param tree1 伸展树实例
 *@param tree2 伸展树实例
 */
void app_sys_tree_st_join(app_sys_tree_st_t *tree, app_sys_tree_st_t *tree1, app_sys_tree_st_t *tree2)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(tree1 == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(tree2 == NULL);
    
    app_sys_tree_sn_t *root = app_sys_tree_st_search_max(tree1);
    app_sys_tree_st_splay(tree1, root);
    app_sys_tree_sn_set_child(root, app_sys_tree_st_get_root(tree2), app_sys_tree_sn_side_r);
    app_sys_tree_st_set_root(tree,  root);
    app_sys_tree_st_set_root(tree1, NULL);
    app_sys_tree_st_set_root(tree2, NULL);
}

/*@brief 伸展树分裂
 *@param tree  伸展树实例
 *@param node  伸展节点实例
 *@param tree1 伸展树实例
 *@param tree2 伸展树实例
 */
void app_sys_tree_st_split(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node, app_sys_tree_st_t *tree1, app_sys_tree_st_t *tree2)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(tree1 == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(tree2 == NULL);
    
    app_sys_tree_sn_t *root1 = app_sys_tree_st_search_only(tree, node);
    app_sys_tree_st_splay(tree, root1);
    app_sys_tree_sn_t *root2 = app_sys_tree_sn_get_child(root1, app_sys_tree_sn_side_r);
    app_sys_tree_sn_set_child(root1, NULL, app_sys_tree_sn_side_r);
    app_sys_tree_st_set_root(tree, NULL);
    app_sys_tree_st_set_root(tree1, root1);
    app_sys_tree_st_set_root(tree2, root2);
}

/*@brief 复位函数
 *@param tree 伸展树实例
 */
void app_sys_tree_st_reset(app_sys_tree_st_t *tree)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    app_sys_tree_st_set_root(tree, NULL);
    app_sys_tree_st_set_compare(tree, NULL);
    app_sys_tree_st_set_confirm(tree, NULL);
    app_sys_tree_st_set_visit(tree, NULL);
}

/*@brief 复位函数
 *@param node 伸展节点实例
 */
void app_sys_tree_sn_reset(app_sys_tree_sn_t *node)
{
    app_sys_tree_sn_set_parent(node, NULL);
    app_sys_tree_sn_set_child(node, NULL, app_sys_tree_sn_side_l);
    app_sys_tree_sn_set_child(node, NULL, app_sys_tree_sn_side_r);
}

/*@brief 配置函数
 *@param tree 伸展树实例
 *@param compare 伸展树比较语义
 *@param confirm 伸展树确认语义
 *@param visit   伸展树访问语义
 */
void app_sys_tree_st_config(app_sys_tree_st_t *tree, app_sys_tree_st_compare_t compare, app_sys_tree_st_confirm_t confirm, app_sys_tree_st_visit_t visit)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    app_sys_tree_st_set_root(tree, NULL);
    app_sys_tree_st_set_compare(tree, compare);
    app_sys_tree_st_set_confirm(tree, confirm);
    app_sys_tree_st_set_visit(tree, visit);
}

/*@brief 根切换函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_root_set(app_sys_tree_st_t *tree, app_sys_tree_sn_t *node)
{
    APP_SYS_TREE_ST_CHECK_RETURN(tree == NULL);
    APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
    app_sys_tree_st_set_root(tree, node);
}

/*@brief 根切换函数
 *@param tree 伸展树实例
 *@param node 伸展节点实例
 */
void app_sys_tree_st_root_get(app_sys_tree_st_t *tree, app_sys_tree_sn_t **node)
{
    *node = app_sys_tree_st_get_root(tree);
}

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree  伸展树实例
 *@param queue 伸展树节点实例队列数组
 *@param len   伸展树节点实例队列数组长度
 */
void app_sys_tree_st_seq_tra(app_sys_tree_st_t *tree, app_sys_tree_sn_t **queue, int32_t len)
{
    /* 层序遍历(这里使用循环队列): */
    
    /* 获取插入的起始位置 */
    int32_t queue_h  = 0;
    int32_t queue_t  = 0;
    int32_t node_num = 1;
    queue[queue_t++] = app_sys_tree_st_get_root(tree);
    
    do {
        /* 节点出队列 */
        app_sys_tree_sn_t *node = queue[queue_h++];
        node_num--;
        APP_SYS_TREE_ST_CHECK_RETURN(node == NULL);
        /* 节点访问 */
        app_sys_tree_st_get_visit(tree)(node);
        /* 这是一个循环队列 */
        if (queue_h >= len)
            queue_h  = 0;
        if (queue_t >= len)
            queue_t  = 0; 
        
        /* 子节点入队列 */
        app_sys_tree_sn_t *child_l = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_l);
        app_sys_tree_sn_t *child_r = app_sys_tree_sn_get_child(node, app_sys_tree_sn_side_r);
        
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
        APP_SYS_TREE_ST_CHECK(node_num >= len, "overflow");
    } while (node_num > 0);
}
