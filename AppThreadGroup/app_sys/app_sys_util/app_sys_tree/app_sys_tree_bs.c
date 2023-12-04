/*实现目标:
 *    B*树
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 条件检查语义 */
#define APP_SYS_TREE_BST_CHECK(cond, str)  if (cond) APP_SYS_LOG_ERROR(str)

/* 动态内存分配释放语义适配 */
#define APP_SYS_TREE_BST_ALLOC  app_mem_alloc
#define APP_SYS_TREE_BST_FREE   app_mem_free

/*@brief B*树(设置语义)
 *@param tree B*树实例
 *@param root B*树根
 */
static inline void app_sys_tree_bst_set_root(app_sys_tree_bst_t *tree, app_sys_tree_bsn_t *root)
{
    tree->root = root;
}

/*@brief B*树(设置语义)
 *@param tree B*树实例
 *@param key  B*树关键字获取语义
 */
static inline void app_sys_tree_bst_set_key(app_sys_tree_bst_t *tree, app_sys_tree_bst_key_t key)
{
    tree->key = key;
}

/*@brief B*树(设置语义)
 *@param tree  B*树实例
 *@param visit B*树节点访问语义
 */
static inline void app_sys_tree_bst_set_visit(app_sys_tree_bst_t *tree, app_sys_tree_bst_visit_t visit)
{
    tree->visit = visit;
}

/*@brief B*树(设置语义)
 *@param tree   B*树实例
 *@param length B*树节点最大负载
 */
static inline void app_sys_tree_bst_set_length(app_sys_tree_bst_t *tree, uint32_t length)
{
    tree->length = length;
}

/*@brief B*树(设置语义)
 *@param tree   B*树实例
 *@param criter B*树节点分裂值
 */
static inline void app_sys_tree_bst_set_criter(app_sys_tree_bst_t *tree, uint32_t criter)
{
    tree->criter = criter;
}

/*@brief B*树(设置语义)
 *@param tree   B*树实例
 *@param number B*树节点当前值
 */
static inline void app_sys_tree_bst_set_number(app_sys_tree_bst_t *tree, uint32_t number)
{
    tree->number = number;
}

/*@brief B*树(获取语义)
 *@param tree B*树实例
 *@retval B*树根
 */
static inline app_sys_tree_bsn_t * app_sys_tree_bst_get_root(app_sys_tree_bst_t *tree)
{
    return tree->root;
}

/*@brief B*树(获取语义)
 *@param tree B*树实例
 *@retval B*树关键字获取语义
 */
static inline app_sys_tree_bst_key_t app_sys_tree_bst_get_key(app_sys_tree_bst_t *tree)
{
    return tree->key;
}

/*@brief B*树(设置语义)
 *@param tree  B*树实例
 *@retval B*树节点访问语义
 */
static inline app_sys_tree_bst_visit_t app_sys_tree_bst_get_visit(app_sys_tree_bst_t *tree)
{
    return tree->visit;
}

/*@brief B*树(获取语义)
 *@param tree B*树实例
 *@retval B*树节点最大负载
 */
static inline uint32_t app_sys_tree_bst_get_length(app_sys_tree_bst_t *tree)
{
    return tree->length;
}

/*@brief B*树(获取语义)
 *@param tree B*树实例
 *@retval B*树节点分裂值
 */
static inline uint32_t app_sys_tree_bst_get_criter(app_sys_tree_bst_t *tree)
{
    return tree->criter;
}

/*@brief B*树(获取语义)
 *@param tree B*树实例
 *@retval B*树节点当前值
 */
static inline uint32_t app_sys_tree_bst_get_number(app_sys_tree_bst_t *tree)
{
    return tree->number;
}

/*@brief B*节点(设置语义)
 *@param node   B*节点实例
 *@param parent B*节点实例
 */
static inline void app_sys_tree_bsn_set_parent(app_sys_tree_bsn_t *node, app_sys_tree_bsn_t *parent)
{
    node->parent = parent;
}

/*@brief B*节点(设置语义)
 *@param node    B*节点实例
 *@param brother B*节点实例
 */
static inline void app_sys_tree_bsn_set_brother_l(app_sys_tree_bsn_t *node, app_sys_tree_bsn_t *brother)
{
    node->brother_l = brother;
}

/*@brief B*节点(设置语义)
 *@param node    B*节点实例
 *@param brother B*节点实例
 */
static inline void app_sys_tree_bsn_set_brother_r(app_sys_tree_bsn_t *node, app_sys_tree_bsn_t *brother)
{
    node->brother_r = brother;
}

/*@brief B*节点(设置语义)
 *@param node       B*节点实例
 *@param child_data 数据元集或节点集
 */
static inline void app_sys_tree_bsn_set_child_data(app_sys_tree_bsn_t *node, void **child_data)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->child_data_state);
    retval &= (uintptr_t)(1);
    retval |= (uintptr_t)(child_data);
    
    node->child_data_state = (void *)retval;
}

/*@brief B*节点(设置语义)
 *@param node  B*节点实例
 *@param state B*节点状态
 */
static inline void app_sys_tree_bsn_set_state(app_sys_tree_bsn_t *node, app_sys_tree_bsn_status_t state)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->child_data_state);
    retval &= (uintptr_t)(~1);
    
    if (state  == app_sys_tree_bsn_state_leaves)
        retval |= (uintptr_t)app_sys_tree_bsn_state_leaves;
    if (state  == app_sys_tree_bsn_state_common)
        retval |= (uintptr_t)app_sys_tree_bsn_state_common;
    
    node->child_data_state = (void *)retval;
}

/*@brief B*节点(设置语义)
 *@param node B*节点实例
 *@param key  关键字集
 */
static inline void app_sys_tree_bsn_set_key(app_sys_tree_bsn_t *node, uint32_t *key)
{
    node->key = key;
}

/*@brief B*节点(设置语义)
 *@param node   B*节点实例
 *@param number 节点数量
 */
static inline void app_sys_tree_bsn_set_number(app_sys_tree_bsn_t *node, uint32_t number)
{
    node->number = number;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval B*节点实例
 */
static inline app_sys_tree_bsn_t * app_sys_tree_bsn_get_parent(app_sys_tree_bsn_t *node)
{
    return node->parent;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval B*节点实例
 */
static inline app_sys_tree_bsn_t * app_sys_tree_bsn_get_brother_l(app_sys_tree_bsn_t *node)
{
    return node->brother_l;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval B*节点实例
 */
static inline app_sys_tree_bsn_t * app_sys_tree_bsn_get_brother_r(app_sys_tree_bsn_t *node)
{
    return node->brother_r;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval 数据元集或节点集
 */
static inline void ** app_sys_tree_bsn_get_child_data(app_sys_tree_bsn_t *node)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->child_data_state);
    retval &= (uintptr_t)(~1);
    
    return (void **)retval;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval B*节点状态
 */
static inline app_sys_tree_bsn_status_t app_sys_tree_bsn_get_state(app_sys_tree_bsn_t *node)
{
    uintptr_t retval = 0;
    
    retval |= (uintptr_t)(node->child_data_state);
    retval &= (uintptr_t)(1);
    
    if (retval == app_sys_tree_bsn_state_leaves)
        return app_sys_tree_bsn_state_leaves;
    if (retval == app_sys_tree_bsn_state_common)
        return app_sys_tree_bsn_state_common;
    
    return app_sys_tree_bsn_error;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval 关键字集
 */
static inline uint32_t * app_sys_tree_bsn_get_key(app_sys_tree_bsn_t *node)
{
    return node->key;
}

/*@brief B*节点(获取语义)
 *@param node B*节点实例
 *@retval 节点数量
 */
static inline uint32_t app_sys_tree_bsn_get_number(app_sys_tree_bsn_t *node)
{
    return node->number;
}

/* 辅助定制宏,美化代码,剔除边缘逻辑 */
#define APP_SYS_TREE_RBT_CHECK_RETURN(expr)         if (expr) return;
#define APP_SYS_TREE_RBT_CHECK_RETURN_ZERO(expr)    if (expr) return 0;
#define APP_SYS_TREE_RBT_CHECK_RETURN_NULL(expr)    if (expr) return NULL;

/*@brief 创建一个B*节点实例(辅助功能)
 *@param tree  B*树实例
 *@param state B*节点状态
 *@retval B*节点实例
 */
static app_sys_tree_bsn_t * app_sys_tree_bst_create_node(app_sys_tree_bst_t *tree, app_sys_tree_bsn_status_t state)
{
    APP_SYS_TREE_BST_CHECK(tree == NULL, "tree");
    APP_SYS_TREE_BST_CHECK(app_sys_tree_bst_get_length(tree) < 3, "tree zone");
    /* 分配空间计算 */
    uint32_t zone1 = sizeof(app_sys_tree_bsn_t);
    uint32_t zone2 = app_sys_tree_bst_get_length(tree) * sizeof(void *);
    uint32_t zone3 = app_sys_tree_bst_get_length(tree) * sizeof(uint32_t);
    /* 1.创建节点,创建关键字集,创建数据元集或节点集 */
    app_sys_tree_bsn_t *node = NULL; void *child = NULL; uint32_t *key = NULL;
    node  = APP_SYS_TREE_BST_ALLOC(zone1);
    child = APP_SYS_TREE_BST_ALLOC(zone2);
    key   = APP_SYS_TREE_BST_ALLOC(zone3);
    /* 2.检查分配空间 */
    if (node == NULL || key == NULL || child == NULL) {
        APP_SYS_TREE_BST_FREE(key);
        APP_SYS_TREE_BST_FREE(child);
        APP_SYS_TREE_BST_FREE(node);
        APP_SYS_TREE_BST_CHECK(true, "alloc node fail");
        return NULL;
    }
    /* 3.初始化该节点,直接清零 */
    for (uint32_t idx = 0; idx < zone1; idx++) ((uint8_t *) node)[idx] = 0;
    for (uint32_t idx = 0; idx < zone2; idx++) ((uint8_t *)child)[idx] = 0;
    for (uint32_t idx = 0; idx < zone3; idx++) ((uint8_t *)  key)[idx] = 0;
    /* 4.关键字集绑定,数据元集或节点集绑定 */
    app_sys_tree_bsn_set_child_data(node, child);
    app_sys_tree_bsn_set_state(node, state);
    app_sys_tree_bsn_set_key(node, key);
    return node;
}

/*@brief 销毁一个B*节点实例(辅助功能)
 *@param node B*节点实例
 */
static void app_sys_tree_bst_destroy_node(app_sys_tree_bsn_t *node)
{
    APP_SYS_TREE_BST_FREE(app_sys_tree_bsn_get_child_data(node));
    APP_SYS_TREE_BST_FREE(app_sys_tree_bsn_get_key(node));
    APP_SYS_TREE_BST_FREE(node);
}

/*@brief 二分搜索(辅助功能)
 *@param key_val 目标关键字
 *@param key     关键字集
 *@param edge    边界亲和度(0:(l,r];1:[l,r);)
 *@param left    匹配左边界
 *@param right   匹配右边界
 */
static void app_sys_tree_bst_binary(uint32_t key_val, uint32_t *key, uint32_t edge, int32_t *left, int32_t *right)
{
    APP_SYS_TREE_BST_CHECK(edge >= 2, "edge");
    APP_SYS_TREE_BST_CHECK(key == NULL, "key");
    APP_SYS_TREE_BST_CHECK(*left > *right, "left>right");
    APP_SYS_TREE_RBT_CHECK_RETURN(key == NULL || *left > *right || edge >= 2);
    
    int32_t center = 0;
    while (*left <= *right) {
        center = *left + (*right - *left) / 2;
        if (key_val > key[center])
            *left  = center + 1;
        if (key_val < key[center])
            *right = center - 1;
        /* 根据参数确定,左右边界 */
        if (key_val == key[center]) {
            if (edge == 0)
                *right = center - 1;
            if (edge == 1)
                *left  = center + 1;
        }
    }
    /* 此时交换left与right的值 */
    int32_t temp1 = *left;
    int32_t temp2 = *right;
    *left  = temp2;
    *right = temp1;
    /* 不检查左右边界溢出(调用者需要自己检查) */
}

/*@brief 迭代更新从指定节点到根节点的关键字(辅助功能)
 *@param node        B*节点实例
 *@param key_val_new 新关键字
 *@param key_val_old 旧关键字
 */
static void app_sys_tree_bst_key_update(app_sys_tree_bsn_t *node, uint32_t key_val_new, uint32_t key_val_old)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL || key_val_new == key_val_old);
    /* 迭代向上更新对应数据元,它是去往最近最小的项 */
    while ((node = app_sys_tree_bsn_get_parent(node)) != NULL && key_val_new != key_val_old) {
        /* 遍历当前数据关键字集,定位到恰好遇到一项比它大或退出 */
        uint32_t *key    = app_sys_tree_bsn_get_key(node);
        uint32_t  number = app_sys_tree_bsn_get_number(node);
        int32_t left = 0, right = (int32_t)number - 1, idx = -1;
        /* 查找更新parent中对应数据 */
        if (key_val_new > key_val_old) {
            app_sys_tree_bst_binary(key_val_old, key, 1, &left, &right);
            idx = (left < 0) ? 0 : left;
        }
        if (key_val_new < key_val_old) {
            app_sys_tree_bst_binary(key_val_old, key, 0, &left, &right);
            idx = (right > number - 1) ? number - 1 : right;
        }
        APP_SYS_TREE_BST_CHECK(key[idx] != key_val_old, "key error");
        /* 注意:必须要保证更新正确的父亲的关键字索引 */
        key_val_old = key[idx];
        key[idx] = key_val_new;
        /* 迭代到下一层,如果可以的话 */
        APP_SYS_TREE_RBT_CHECK_RETURN(idx != 0);
    }
}

/*@brief 从指定节点迁移批量数据到另一节点(辅助功能)
 *@param tree   B*树实例
 *@param node1  B*节点实例
 *@param node2  B*节点实例
 *@param number 迁移数据量
 *@param state  迁移节点状态
 */
static void app_sys_tree_bst_migrate(app_sys_tree_bst_t *tree,
                                     app_sys_tree_bsn_t *node1,
                                     app_sys_tree_bsn_t *node2, uint32_t number,
                                     app_sys_tree_bsn_status_t state)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL || node1 == NULL || node2 == NULL);
    /* 指定将其进行数据迁移:data1 ==> data2 */
    /* state == 0,将data1后number数据加入到data2的前面 */
    /* state == 1,将data1前number数据加入到data2的后面 */
    /* 注意:跨节点迁移时其对父亲的索引要一并迁移 */
    uint32_t  length = app_sys_tree_bst_get_length(tree);
    uint32_t *key1 = app_sys_tree_bsn_get_key(node1);
    uint32_t *key2 = app_sys_tree_bsn_get_key(node2);
    void **child1 = app_sys_tree_bsn_get_child_data(node1);
    void **child2 = app_sys_tree_bsn_get_child_data(node2);
    uint32_t number1 = app_sys_tree_bsn_get_number(node1);
    uint32_t number2 = app_sys_tree_bsn_get_number(node2);
    app_sys_tree_bsn_status_t state1 = app_sys_tree_bsn_get_state(node1);
    app_sys_tree_bsn_status_t state2 = app_sys_tree_bsn_get_state(node2);
    //要迁移migrate项数据(极限检测) */
    APP_SYS_TREE_BST_CHECK(state >= 2, "state");
    APP_SYS_TREE_BST_CHECK(state1 != state2, "state");
    APP_SYS_TREE_BST_CHECK(number1 - number < 0, "1");
    APP_SYS_TREE_BST_CHECK(number2 + number > length, "2");
    
    /* 迁移所用到的索引 */
    int32_t idx = 0, tmp = 0;
    /* 根据实际情况继续不同的迁移动作 */
    if (state == 0) {
        /* 1.先漂移data2的数据 */
        if (number2 != 0)
            for (idx = number2 - 1; idx >= 0; idx--) {
                key2[idx + number]   = key2[idx];
                child2[idx + number] = child2[idx];
            }
        /* 2.data1的数据迁移到data2 */
        for (idx = 0; idx < number; idx++) {
            key2[idx]   = key1[idx + number1 - number];
            child2[idx] = child1[idx + number1 - number];
            tmp = idx;
            if (state2 != app_sys_tree_bsn_state_leaves && child2[tmp] != NULL)
                app_sys_tree_bsn_set_parent(child2[tmp], node2);
        }
    }
    if (state == 1) {
        /* 1.data1的数据迁移到data2 */
        for (idx = 0; idx < number; idx++) {
            key2[idx + number2]    = key1[idx];
            child2[idx + number2] = child1[idx];
            tmp = idx + number2;
            if (state1 != app_sys_tree_bsn_state_leaves && child2[tmp] != NULL)
                app_sys_tree_bsn_set_parent(child2[tmp], node2);
        }
        /* 2.后漂移data1的数据 */
        if (number1 - number != 0)
            for (idx = 0; idx < number1 - number; idx++) {
                key1[idx]   = key1[idx + number];
                child1[idx] = child1[idx + number];
            }
    }
    /* 更新data1和data2的计数量 */
    app_sys_tree_bsn_set_number(node1, number1 - number);
    app_sys_tree_bsn_set_number(node2, number2 + number);
}

/*@brief 根据关键字迭代搜索到叶节点(辅助功能)
 *@param tree    B*树实例
 *@param node    B*节点实例
 *@param key_val 关键字
 */
static void app_sys_tree_bst_search_node_only(app_sys_tree_bst_t  *tree,
                                              app_sys_tree_bsn_t **node, uint32_t key_val)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL || node == NULL);
    app_sys_tree_bsn_t *root = app_sys_tree_bst_get_root(tree);
    /* 从根节点一直走到叶子节点即可 */
    while (root != NULL && app_sys_tree_bsn_get_state(root) == app_sys_tree_bsn_state_common) {
        /* 根节点至少是有元素,此时需要向下搜索 */
        void **child = NULL; uint32_t *key = NULL; uint32_t number = 0;
        child  = app_sys_tree_bsn_get_child_data(root);
        key    = app_sys_tree_bsn_get_key(root);
        number = app_sys_tree_bsn_get_number(root);
        /* 遍历当前数据关键字集,定位到恰好遇到一项比它大或退出 */
        int32_t left = 0, right = (int32_t)number - 1;
        /* 二分搜索,查找时有效项是left */
        app_sys_tree_bst_binary(key_val, key, 1, &left, &right);
        left = (left < 0) ? 0 : left;
        /* 判断有效元,因为查找时可能出现溢出的情况 */
        root = child[left];
    }
    *node = root;
}

/*@brief 在一个指定节点中插入一个项(辅助功能)
 *@param node    B*节点实例
 *@param state   节点状态
 *@param idx     插入索引
 *@param key_val 关键字
 *@param data    数据项
 */
static void app_sys_tree_bst_insert_data_only(app_sys_tree_bsn_t *node,
                                              app_sys_tree_bsn_status_t state,
                                              uint32_t *idx, uint32_t key_val, void *data)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL || idx == NULL || data == NULL || state >= 2);
    void **child = NULL; uint32_t *key = NULL; uint32_t number = 0;
    child  = app_sys_tree_bsn_get_child_data(node);
    key    = app_sys_tree_bsn_get_key(node);
    number = app_sys_tree_bsn_get_number(node);
    /* 1.遍历当前数据关键字集,定位到恰好遇到一项比它大或退出 */
    int32_t left = 0, right = (int32_t)number - 1;
    /* 二分搜索,插入时有效项是right */
    app_sys_tree_bst_binary(key_val, key, state, &left, &right);
    *idx = (right < 0) ? 0 : right;
    /* 2.需要将后续的部分迁移到下一项,为新入项腾出一个空间 */
    if (*idx < number)
        for (int32_t idx1 = number - 1; idx1 >= (int32_t)*idx; idx1--) {
            /* 当前项覆盖后一项,直到==index覆盖完毕 */
            key[idx1 + 1]   = key[idx1];
            child[idx1 + 1] = child[idx1];
        }
    /* 3.将数据加入到指定位置即可 */
    child[*idx] = data;
    key[*idx]   = key_val;
    app_sys_tree_bsn_set_number(node, number + 1);
}

/*@brief 从一个指定节点中移除一个项(辅助功能)
 *@param node    B*节点实例
 *@param state   节点状态
 *@param idx     插入索引
 *@param key_val 关键字
 *@param data    数据项
 */
static void app_sys_tree_bst_remove_data_only(app_sys_tree_bsn_t *node,
                                              app_sys_tree_bsn_status_t state,
                                              uint32_t *idx, uint32_t key_val, void **data)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL || idx == NULL || data == NULL || state >= 2);
    void **child = NULL; uint32_t *key = NULL; uint32_t number = 0;
    child  = app_sys_tree_bsn_get_child_data(node);
    key    = app_sys_tree_bsn_get_key(node);
    number = app_sys_tree_bsn_get_number(node);
    /* 如果是无效的删除的话,直接回退即可 */
    *data == NULL;
    APP_SYS_TREE_RBT_CHECK_RETURN(number == 0);
    int32_t left = 0, right = (int32_t)number - 1;
    /* 1.遍历当前数据关键字集,定位到恰好遇到一项比它大或退出 */
    app_sys_tree_bst_binary(key_val, key, state, &left, &right);
    *idx = state == 1 ? left < 0 ? 0 : left : state == 0 ? left + 1 : 0;
    /* 删除时,有效项是left */
    APP_SYS_TREE_BST_CHECK(key[*idx] != key_val, "key_val");
    /* 2.记录其数据 */
    *data = child[*idx];
    /* 3.需要将后续的部分迁移到前一项,为其覆盖一个空间 */
    if (number != 1 && *idx < number)
        for (int32_t idx1 = *idx; idx1 <= (int32_t)number; idx1++) {
            /* 当前项覆盖后一项,直到==index覆盖完毕 */
            key[idx1]   = key[idx1 + 1];
            child[idx1] = child[idx1 + 1];
        }
    app_sys_tree_bsn_set_number(node, number - 1);
}

/*@brief 从一个指定节点中搜索一个项(辅助功能)
 *@param node    B*节点实例
 *@param state   节点状态
 *@param idx     插入索引
 *@param key_val 关键字
 *@param data    数据项
 */
static void app_sys_tree_bst_search_data_only(app_sys_tree_bsn_t *node,
                                              app_sys_tree_bsn_status_t state,
                                              uint32_t *idx, uint32_t key_val, void **data)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL || idx == NULL || data == NULL || state >= 2);
    void **child = NULL; uint32_t *key = NULL; uint32_t number = 0;
    child  = app_sys_tree_bsn_get_child_data(node);
    key    = app_sys_tree_bsn_get_key(node);
    number = app_sys_tree_bsn_get_number(node);
    /* 保证搜索时,内部至少有一个项 */
    *data = NULL;
    APP_SYS_TREE_RBT_CHECK_RETURN(number == 0);
    /* 遍历当前数据关键字集,定位到恰好遇到一项比它大或退出 */
    int32_t left = 0, right = (int32_t)number - 1;
    app_sys_tree_bst_binary(key_val, key, state, &left, &right);
    *idx = state == 1 ? left < 0 ? 0 : left : state == 0 ? left + 1 : 0;
    /* 搜索时,有效项是left */
    APP_SYS_TREE_BST_CHECK(key[*idx] != key_val, "key_val");
    /* 记录其数据 */
    *data = child[*idx];
}

/*@brief 单次插入函数
 *@param tree B*树实例
 *@param node B*节点实例
 *@param data 数据项
 */
static void app_sys_tree_bst_insert_only(app_sys_tree_bst_t *tree, app_sys_tree_bsn_t **node, void *data)
{
    APP_SYS_TREE_BST_CHECK(tree == NULL || node == NULL || data == NULL, "");
    /* 1.计算关键字 */
    uint32_t key_val = app_sys_tree_bst_get_key(tree)(data);
    /* 2.如果是插入到空树中时或者插入到叶子节点时 */
    *node = app_sys_tree_bst_get_root(tree);
    /* 3.确认是否是初次插入,不存在项 */
    if (*node == NULL) {
        /* 为其生成一个空间,设置其为叶子节点 */
        app_sys_tree_bst_set_root(tree, app_sys_tree_bst_create_node(tree, app_sys_tree_bsn_state_leaves));
        *node = app_sys_tree_bst_get_root(tree);
        APP_SYS_TREE_RBT_CHECK_RETURN(*node == NULL);
    }
    /* 4.迭代搜索到叶子节点 */
    app_sys_tree_bst_search_node_only(tree, node, key_val);
    /* 5.数据元加入到叶子节点 */
    int32_t idx = -1;
    app_sys_tree_bst_insert_data_only(*node, 1, &idx, key_val, data);
    /* 6.插入的节点是叶子节点的首节点,修复它所产生的影响 */
    if (idx == 0) {
        uint32_t *key = app_sys_tree_bsn_get_key(*node);
        app_sys_tree_bst_key_update(*node, key[0], key[1]);
    }
}

/*@brief 插入调整函数
 *@param tree B*树实例
 *@param node B*节点实例
 */
static void app_sys_tree_bst_insert_adjust(app_sys_tree_bst_t *tree, app_sys_tree_bsn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL || node == NULL);
    uint32_t idx = 0;
    app_sys_tree_bsn_status_t state = 0;
    uint32_t *key1 = NULL, key_val1 = 0;
    uint32_t *key2 = NULL, key_val2 = 0;
    uint32_t *key3 = NULL, key_val3 = 0;
    uint32_t length = app_sys_tree_bst_get_length(tree);
    uint32_t criter = app_sys_tree_bst_get_criter(tree);
    uint32_t number = app_sys_tree_bsn_get_number(node);
    app_sys_tree_bsn_t *parent = NULL;
    app_sys_tree_bsn_t *brother_l = NULL;
    app_sys_tree_bsn_t *brother_r = NULL;
    /* 插入时导致的节点超出 */
    /* 如果左右兄弟只要有一个存在盈余空间就无需分裂 */
    /* 如果左右兄弟都负重不堪,那么往左边分裂和往右边分裂(这是一个问题)
    /* 这里直接按正常习惯往右边分裂,但是到底需要有向左边分裂的必要性,是否 */
    /* 是叶节点分裂还是非叶子节点分裂(情况是一致的) */
    while (number >= criter) {
        /* 先获取它与周围节点的关系 */
        parent = app_sys_tree_bsn_get_parent(node);
        brother_l = app_sys_tree_bsn_get_brother_l(node);
        brother_r = app_sys_tree_bsn_get_brother_r(node);
        state = app_sys_tree_bsn_get_state(node);
        /* 情况1:如果右兄弟有盈余空间 */
        if (brother_r != NULL &&
            app_sys_tree_bsn_get_number(brother_r) < criter) {
            /* 1.获取右兄弟在父亲那的记录 */
            key3 = app_sys_tree_bsn_get_key(brother_r);
            key_val3 = key3[0];
            /* 2.将一部分数据迁移至右兄弟 */
            app_sys_tree_bst_migrate(tree, node, brother_r, length - criter, 0);
            /* 3.迭代更新从指定节点到根节点的索引 */
            node = brother_r;
            app_sys_tree_bst_key_update(node, key3[0], key_val3);
            return;
        }
        
        /* 情况2:如果左兄弟有盈余空间(它应该只做少部分数据承担者) */
        /* 添加该种情况的话可以更大化的利用空间 */
        if (brother_l != NULL &&
            app_sys_tree_bsn_get_number(brother_l) < criter) {
            /* 1.获取自己在父亲那的记录 */
            key1 = app_sys_tree_bsn_get_key(node);
            key_val1 = key1[0];
            /* 2.将一部分数据迁移至左兄弟 */
            app_sys_tree_bst_migrate(tree, node, brother_l, length - criter, 1);
            /* 3.迭代更新从指定节点到根节点的索引 */
            app_sys_tree_bst_key_update(node, key1[0], key_val1);
            return;
        }
        
        /* 情况3:如果没有兄弟节点时 */
        if (brother_r == NULL) {
            /* 1.创建兄弟节点 */
            brother_r = app_sys_tree_bst_create_node(tree, state);
            APP_SYS_TREE_BST_CHECK(brother_r == NULL, "brother_r");
            /* 2.为自己设置兄弟,为兄弟设置兄弟 */
            app_sys_tree_bsn_set_brother_r(node, brother_r);
            app_sys_tree_bsn_set_brother_l(brother_r, node);
            /* 3.将本节点部分数据拷贝到右兄弟中去 */
            app_sys_tree_bst_migrate(tree, node, brother_r, length - criter, 0);
            /* 4.如果没有父亲要生成父亲,如果有父亲要添加兄弟节点 */
            if (parent == NULL) {
                /* 1.创建一个父亲 */
                parent = app_sys_tree_bst_create_node(tree, app_sys_tree_bsn_state_common);
                APP_SYS_TREE_BST_CHECK(parent == NULL, "parent");
                /* 2.为自己设置新父亲 */
                app_sys_tree_bsn_set_parent(node, parent);
                /* 3.新生成父亲时需要更新根节点 */
                app_sys_tree_bst_set_root(tree, parent);
                /* 4.将自己加入到新父亲中 */
                key1 = app_sys_tree_bsn_get_key(node);
                app_sys_tree_bst_insert_data_only(parent, 1, &idx, key1[0], (void *)node);
            }
            /* 5.为兄弟设置父亲 */
            app_sys_tree_bsn_set_parent(brother_r, parent);
            /* 6.将兄弟插入到父亲中 */
            key3 = app_sys_tree_bsn_get_key(brother_r);
            app_sys_tree_bst_insert_data_only(parent, 1, &idx, key3[0], (void *)brother_r);
            /* 7.迭代到上层调整 */
            number = app_sys_tree_bsn_get_number(parent);
            node = parent;
            continue;
        }
        
        /* 情况4:自己和左右兄弟都是负重不堪 */
        /* 1.创建一个新节点 */
        app_sys_tree_bsn_t *tmp = app_sys_tree_bst_create_node(tree, state);
        /* 2.为其设置它的父亲 */
        app_sys_tree_bsn_set_parent(tmp, parent);
        /* 3.先迁移自己的数据 */
        app_sys_tree_bst_migrate(tree, node, tmp, length - criter, 0);
        /* 4.获取右兄弟在父亲那的记录 */
        key3 = app_sys_tree_bsn_get_key(brother_r);
        key_val3 = key3[0];
        /* 5.将右兄弟一部分数据迁移 */
        app_sys_tree_bst_migrate(tree, brother_r, tmp, length - criter, 1);
        /* 6.迭代更新从指定节点到根节点的索引 */
        app_sys_tree_bst_key_update(brother_r, key3[0], key_val3);
        /* 7.更新兄弟关系 */
        app_sys_tree_bsn_set_brother_l(brother_r, tmp);
        app_sys_tree_bsn_set_brother_l(tmp, node);
        app_sys_tree_bsn_set_brother_r(node, tmp);
        app_sys_tree_bsn_set_brother_r(tmp, brother_r);
        /* 9.新兄弟加入到父亲中 */
        key1 = app_sys_tree_bsn_get_key(node);
        key2 = app_sys_tree_bsn_get_key(tmp);
        /* 注意:默认是在其最后产生分裂 */
        /* 但新兄弟在父亲的位置应该只能出现在当前节点之后 */
        state = key1[0] <  key2[0] ? 0 : key1[0] >= key2[0] ? 1 : 0;
        app_sys_tree_bst_insert_data_only(parent, state, &idx, key2[0], (void *)tmp);
        /* 10.迭代到上层调整 */
        number = app_sys_tree_bsn_get_number(parent);
        node = parent;
        continue;
    }
}

/*@brief 单次删除函数
 *@param tree    B*树实例
 *@param node    B*节点实例
 *@param key_val 关键字
 *@param data    数据项
 */
static void app_sys_tree_bst_remove_only(app_sys_tree_bst_t *tree, app_sys_tree_bsn_t **node, uint32_t key_val, void **data)
{
    APP_SYS_TREE_BST_CHECK(tree == NULL || data == NULL || node == NULL, "");
    *node = app_sys_tree_bst_get_root(tree);
    /* 确认是否是初次删除,不存在项 */
    APP_SYS_TREE_RBT_CHECK_RETURN(*node == NULL);
    int32_t idx;
    /* 从根节点开始向下迭代 */
    app_sys_tree_bst_search_node_only(tree, node, key_val);
    /* 数据元从叶子节点中移除 */
    app_sys_tree_bst_remove_data_only(*node, 1, &idx, key_val, data);
    /* 调整一下删除该数所造成的影响 */
    uint32_t *key = app_sys_tree_bsn_get_key(*node);
    if (idx != 0)
        return;
    app_sys_tree_bst_key_update(*node, key[0], key_val);
}

/*@brief 删除调整函数
 *@param tree B*树实例
 *@param node B*节点实例
 */
static void app_sys_tree_bst_remove_adjust(app_sys_tree_bst_t *tree, app_sys_tree_bsn_t *node)
{
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL || node == NULL);
    /* 先获取它与周围节点的关系 */
    app_sys_tree_bsn_t *parent = app_sys_tree_bsn_get_parent(node);
    app_sys_tree_bsn_t *brother_l = app_sys_tree_bsn_get_brother_l(node);
    app_sys_tree_bsn_t *brother_r = app_sys_tree_bsn_get_brother_r(node);
    app_sys_tree_bsn_status_t state = app_sys_tree_bsn_get_state(node);
    uint32_t number = app_sys_tree_bsn_get_number(node);
    /* 情况1:节点被清空(正常情况下,节点被清空之前会做合并的动作) */
    if (number == 0 && state == app_sys_tree_bsn_state_leaves) {
        /* 添加必要的检查 */
        APP_SYS_TREE_BST_CHECK(parent != NULL, "parent");
        APP_SYS_TREE_BST_CHECK(brother_l != NULL, "brother_l");
        APP_SYS_TREE_BST_CHECK(brother_r != NULL, "brother_r");
        /* 它仅会出现在一种情况,就是清空树时,直接删除掉该节点就好 */
        app_sys_tree_bst_set_root(tree, NULL);
        app_sys_tree_bst_destroy_node(node);
        return;
    }
    
    void    *data = NULL;
    uint32_t idx = -1;
    uint32_t length = app_sys_tree_bst_get_length(tree);
    uint32_t criter = app_sys_tree_bst_get_criter(tree);
    /* 情况2:删除导致的节点缺失,可能需要合并 */
    /* 合并仅向右方向合并,向左兄弟合并无法很好的迭代更新 */
    /* 而数据的提供优先向左兄弟索要,它更容易被溢出 */
    /* 是叶节点合并还是非叶子节点合并(情况是一致的) */
    while (node != NULL && number < criter) {
        APP_SYS_TREE_BST_CHECK(number == 0, "number");
        APP_SYS_TREE_RBT_CHECK_RETURN(number == 0);
        uint32_t *key1 = NULL, key_val1 = 0;
        uint32_t *key2 = NULL, key_val2 = 0;
        uint32_t *key3 = NULL, key_val3 = 0;
        /* 获取需要调整的节点的兄弟节点和父节点,以及孩子数量 */
        parent = app_sys_tree_bsn_get_parent(node);
        brother_l = app_sys_tree_bsn_get_brother_l(node);
        brother_r = app_sys_tree_bsn_get_brother_r(node);
        
        /* 情况2.1:没有左右兄弟(孤立的节点) */
        if (brother_l == NULL && brother_r == NULL) {
            /* 合并时产生的孤立节点,如果它的数据量只有一个 */
            if (number == 1 && state == app_sys_tree_bsn_state_common) {
                void **child = app_sys_tree_bsn_get_child_data(node);
                /* 删除本体,让其孩子上位 */
                app_sys_tree_bst_set_root(tree, (app_sys_tree_bsn_t *)(child[0]));
                app_sys_tree_bst_destroy_node(node);
                /* 新的根节点被设置为空 */
                app_sys_tree_bsn_set_parent(app_sys_tree_bst_get_root(tree), NULL);
            }
            return;
        }
        
        /* 情况2.2:左兄弟数据充足 */
        if (brother_l != NULL && number +
            app_sys_tree_bsn_get_number(brother_l) >= length) {
            /* 1.向左兄弟索要一个数据并插入到自己中 */
            app_sys_tree_bst_migrate(tree, brother_l, node, 1, 0);
            /* 2.迭代更新从指定节点到根节点的索引 */
            key1 = app_sys_tree_bsn_get_key(node);
            app_sys_tree_bst_key_update(node, key1[0], key1[1]);
            return;
        }
        
        /* 情况2.3:右兄弟数据充足 */
        if (brother_r != NULL && number +
            app_sys_tree_bsn_get_number(brother_r) >= length) {
            /* 迁移前获取节点信息 */
            key3 = app_sys_tree_bsn_get_key(brother_r);
            key_val3 = key3[0];
            /* 1.向右兄弟索要一个数据并插入到自己中 */
            app_sys_tree_bst_migrate(tree, brother_r, node, 1, 1);
            /* 2.迭代更新从指定节点到根节点的索引 */
            node = brother_r;
            app_sys_tree_bst_key_update(node, key3[0], key_val3);
            return;
        }
        
        /* 情况2.4:自己和右兄弟都贫困 */
        if (brother_r != NULL && number +
            app_sys_tree_bsn_get_number(brother_r) < length) {
            /* 迁移前获取节点信息 */
            key1 = app_sys_tree_bsn_get_key(node);
            key3 = app_sys_tree_bsn_get_key(brother_r);
            key_val1 = key1[0];
            key_val3 = key3[0];
            /* 多多少少有些尴尬,因为对于多key的处理仅关系首项与末项 */
            /* 这里在向上迭代调整时会,使得node可能成为第二项 */
            /* 这很显然是设计上存在的缺失,不能将俩种情况合并 */
            if (key_val1 != key_val3) {
                /* 1.将右兄弟的数据迁移到自己 */
                number = app_sys_tree_bsn_get_number(brother_r);
                app_sys_tree_bst_migrate(tree, brother_r, node, number, 1);
                /* 2.删除右兄弟 */
                parent = app_sys_tree_bsn_get_parent(brother_r);
                brother_r = app_sys_tree_bsn_get_brother_r(brother_r);
                app_sys_tree_bst_destroy_node(app_sys_tree_bsn_get_brother_r(node));
                /* 3.更新兄弟关系 */
                app_sys_tree_bsn_set_brother_r(node, brother_r);
                if (brother_r != NULL)
                    app_sys_tree_bsn_set_brother_l(brother_r, node);
            }
            if (key_val1 == key_val3) {
                /* 1.将自己的数据迁移到右兄弟 */
                app_sys_tree_bst_migrate(tree, node, brother_r, number, 0);
                /* 2.删除自己 */
                app_sys_tree_bst_destroy_node(node);
                /* 3.更新兄弟关系 */
                app_sys_tree_bsn_set_brother_l(brother_r, brother_l);
                if (brother_l != NULL)
                    app_sys_tree_bsn_set_brother_r(brother_l, brother_r);
            }
            /* 4.删除在父亲中的记录 */
            app_sys_tree_bst_remove_data_only(parent, 0, &idx, key_val3, &data);
            /* 5.清除父亲节点因删除照成的影响 */
            if (idx == 0) {
                key3 = app_sys_tree_bsn_get_key(parent);
                app_sys_tree_bst_key_update(parent, key3[0], key_val3);
            }
            /* 6.迭代到上层调整 */
            number = app_sys_tree_bsn_get_number(parent);
            state = app_sys_tree_bsn_get_state(parent);
            node = parent;
            continue;
        }
    
        /* 情况2.5:自己和左兄弟都贫困 */
        if (brother_l != NULL && number +
            app_sys_tree_bsn_get_number(brother_l) < length) {
            /* 迁移前获取节点信息 */
            key1 = app_sys_tree_bsn_get_key(node);
            key_val1 = key1[0];
            /* 1.将自己的数据迁移到左兄弟 */
            app_sys_tree_bst_migrate(tree, node, brother_l, number, 1);
            /* 2.删除自己 */
            app_sys_tree_bst_destroy_node(node);
            /* 3.更新兄弟关系 */
            app_sys_tree_bsn_set_brother_r(brother_l, brother_r);
            if (brother_r != NULL)
                app_sys_tree_bsn_set_brother_l(brother_r, brother_l);
            /* 4.删除自己在父亲中的记录 */
            app_sys_tree_bst_remove_data_only(parent, 1, &idx, key_val1, &data);
            /* 5.清除父亲节点因删除照成的影响 */
            if (idx == 0) {
                key1 = app_sys_tree_bsn_get_key(parent);
                app_sys_tree_bst_key_update(parent, key1[0], key_val1);
            }
            /* 6.迭代到上层调整 */
            number = app_sys_tree_bsn_get_number(parent);
            state = app_sys_tree_bsn_get_state(parent);
            node = parent;
            continue;
        }
        
        /* 也许会存在导致其节点亏空的情况,意味着它没有兄弟为其解决苦难 */
        /* 但我们依然相信,我们希望的美好的事情会发生 */
        break;
    }
}

/*@brief 单次查找函数
 *@param tree    B*树实例
 *@param key_val 关键字
 *@param data    数据项
 */
static void app_sys_tree_bst_search_only(app_sys_tree_bst_t *tree, uint32_t key_val, void **data)
{
    APP_SYS_TREE_BST_CHECK(tree == NULL || data == NULL, "");
    app_sys_tree_bsn_t *node = app_sys_tree_bst_get_root(tree);
    /* 确认是否是初次删除,不存在项 */
    *data = NULL;
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    int32_t idx;
    /* 从根节点开始向下迭代 */
    app_sys_tree_bst_search_node_only(tree, &node, key_val);
    APP_SYS_TREE_RBT_CHECK_RETURN(node == NULL);
    /* 找到了目标节点,确定目标节点是否存在目标 */
    app_sys_tree_bst_search_data_only(node, 0, &idx, key_val, data);
}

/*@brief 插入函数
 *@param tree B*树实例
 *@param data 数据项
 */
void app_sys_tree_bst_insert(app_sys_tree_bst_t *tree, void *data)
{
    app_sys_tree_bsn_t *node = NULL;
    app_sys_tree_bst_insert_only(tree, &node, data);
    app_sys_tree_bst_insert_adjust(tree, node);
    app_sys_tree_bst_set_number(tree, app_sys_tree_bst_get_number(tree) + 1);
}

/*@brief 删除函数
 *@param tree    B*树实例
 *@param data    数据项
 *@param key_val 关键字
 */
void app_sys_tree_bst_remove(app_sys_tree_bst_t *tree, void **data, uint32_t key_val)
{
    app_sys_tree_bsn_t *node = NULL;
    app_sys_tree_bst_remove_only(tree, &node, key_val, data);
    app_sys_tree_bst_remove_adjust(tree, node);
    APP_SYS_TREE_RBT_CHECK_RETURN(*data == NULL);
    app_sys_tree_bst_set_number(tree, app_sys_tree_bst_get_number(tree) - 1);
}

/*@brief 查找函数
 *@param tree    B*树实例
 *@param data    数据项
 *@param key_val 关键字
 */
void app_sys_tree_bst_search(app_sys_tree_bst_t *tree, void **data, uint32_t key_val)
{
    app_sys_tree_bst_search_only(tree, key_val, data);
}

/*@brief 配置函数
 *@param tree    B*树实例
 *@param length 数据项
 *@param key    关键字获取语义
 *@param visit  节点访问语义
 */
void app_sys_tree_bst_config(app_sys_tree_bst_t *tree, uint32_t length, app_sys_tree_bst_key_t key, app_sys_tree_bst_visit_t visit)
{
    /* 一旦创建便会固定,所以需要容错处理 */
    APP_SYS_TREE_RBT_CHECK_RETURN(tree == NULL || key == NULL || length < 3);
    APP_SYS_TREE_BST_CHECK(length < 4, "");
    APP_SYS_TREE_BST_CHECK(length / 3 * 2 + 1 >= length, "");
    app_sys_tree_bst_set_root(tree, NULL);
    app_sys_tree_bst_set_key(tree, key);
    app_sys_tree_bst_set_visit(tree, visit);
    app_sys_tree_bst_set_length(tree, length);
    app_sys_tree_bst_set_criter(tree, length / 3 * 2 + 1);
    app_sys_tree_bst_set_number(tree, 0);
}

/*@brief 数据的层序遍历并访问(窥探)
 *@param tree    B*树实例
 *@param destroy 是否销毁
 */
uint32_t app_sys_tree_bst_seq_tra(app_sys_tree_bst_t *tree, bool destroy)
{
    APP_SYS_TREE_BST_CHECK(tree == NULL,  "tree");
    APP_SYS_TREE_BST_CHECK(app_sys_tree_bst_get_visit(tree) == NULL, "visit");
    //层序遍历(这里使用循环队列):
    //创建一个队列(这里使用投机取巧的方法,队列极限情况下存在
    //数量最大时顶多应为满B*树的全部叶子节点数大小+1
    uint32_t length = app_sys_tree_bst_get_number(tree);
    uint32_t zone   = sizeof(app_sys_tree_bsn_t *) * length;
    app_sys_tree_bsn_t **queue = APP_SYS_TREE_BST_ALLOC(zone);
    APP_SYS_TREE_RBT_CHECK_RETURN_ZERO(queue == NULL);
    //获取插入的起始位置
    int32_t queue_h = 0;
    int32_t queue_t = 0;
    int32_t node_number = 0;
    int32_t all_number  = 0;
    
    queue[queue_t++] = app_sys_tree_bst_get_root(tree);
    queue_t %= length;
    node_number++;
    //这是一个循环队列
    do {
        //节点出队列
        app_sys_tree_bsn_t *node = queue[queue_h++];
        queue_h %= length;
        node_number--;
        APP_SYS_TREE_RBT_CHECK_RETURN_ZERO(node == NULL);
        uint32_t *key = NULL; void **child = NULL; uint32_t number = 0;
        child  = app_sys_tree_bsn_get_child_data(node);
        key    = app_sys_tree_bsn_get_key(node);
        number = app_sys_tree_bsn_get_number(node);
        //节点打印
        if (app_sys_tree_bsn_get_state(node) == app_sys_tree_bsn_state_common)
            app_sys_tree_bst_get_visit(tree)(app_sys_tree_bsn_state_common, number, key, NULL,  destroy);
        if (app_sys_tree_bsn_get_state(node) == app_sys_tree_bsn_state_leaves)
            app_sys_tree_bst_get_visit(tree)(app_sys_tree_bsn_state_leaves, number, key, child, destroy);
        
        //子节点入队列
        if (app_sys_tree_bsn_get_state(node) == app_sys_tree_bsn_state_common) {
            int32_t idx = 0;
            for (idx = 0; idx < number; idx++) {
                queue[queue_t++] = (app_sys_tree_bsn_t *)(child[idx]);
                queue_t %= length;
                node_number++;
            }
        }
        
        //计算元节点个数
        if (app_sys_tree_bsn_get_state(node) == app_sys_tree_bsn_state_leaves)
            all_number += number;
        
        if (destroy != 0)
            app_sys_tree_bst_destroy_node(node);
        
        //队列太小,警告
        APP_SYS_TREE_BST_CHECK(node_number >= (int32_t)length, "overflow");
        APP_SYS_TREE_RBT_CHECK_RETURN_ZERO(node_number >= (int32_t)length);
    } while (node_number > 0);
    
    return all_number;
}
