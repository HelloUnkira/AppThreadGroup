/*实现目标:
 *    嵌入式伸展树测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

//这是我要获得伸展树语义的数据结构
typedef struct {
    int32_t key;
    /* 将伸展树侵入到这个容器中 */
    app_sys_tree_sn_t sn_node;
    /*  */
    int32_t rand_val;
} app_sys_tree_s_test_t;

uint8_t app_sys_tree_s_test_compare(app_sys_tree_sn_t *node1, app_sys_tree_sn_t *node2)
{
    app_sys_tree_s_test_t *test1 = app_sys_own_ofs(app_sys_tree_s_test_t, sn_node, node1);
    app_sys_tree_s_test_t *test2 = app_sys_own_ofs(app_sys_tree_s_test_t, sn_node, node2);
    
    return test1->key < test2->key ? 1 : 0;
}

uint8_t app_sys_tree_s_test_confirm(app_sys_tree_sn_t *node1, app_sys_tree_sn_t *node2)
{
    app_sys_tree_s_test_t *test1 = app_sys_own_ofs(app_sys_tree_s_test_t, sn_node, node1);
    app_sys_tree_s_test_t *test2 = app_sys_own_ofs(app_sys_tree_s_test_t, sn_node, node2);
    
    return test1->key == test2->key ? 0 : 1;
}

void app_sys_tree_s_test_visit(app_sys_tree_sn_t *node)
{
    app_sys_tree_s_test_t *test = app_sys_own_ofs(app_sys_tree_s_test_t, sn_node, node);
    /* 暴力测试时不全部打印 */
    APP_SYS_LOG_DEBUG("key:%d val:%d", test->key, test->rand_val);
}

void app_sys_tree_s_test(void)
{
    #if APP_SYS_LOG_LOCAL_LEVEL > 1
    return;
    #endif
    
    #define APP_SYS_TREE_S_MAX         (100)
    #define APP_SYS_TREE_S_MAX_HALF    (APP_SYS_TREE_S_MAX / 2)
    
    app_sys_tree_st_t *tree = app_mem_alloc(sizeof(app_sys_tree_st_t));
    app_sys_tree_st_config(tree, app_sys_tree_s_test_compare, app_sys_tree_s_test_confirm, app_sys_tree_s_test_visit);
    
    app_sys_tree_sn_t **queue = app_mem_alloc(sizeof(app_sys_tree_sn_t *)  * APP_SYS_TREE_S_MAX);
    
    //随机插入节点
    for (int32_t idx = 0; idx < APP_SYS_TREE_S_MAX; idx++)
    {
        app_sys_tree_s_test_t *test = app_mem_alloc(sizeof(app_sys_tree_s_test_t));
        test->key      = rand() % 100;
        test->rand_val = rand() % 10000;
        app_sys_tree_sn_reset(&test->sn_node);
        app_sys_tree_st_insert(tree, &test->sn_node);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_sys_tree_st_seq_tra(tree, queue, APP_SYS_TREE_S_MAX);//插入检查
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    //随机删除一半节点 (事实上基于key的伸展树理论上不支持重复key)
    //这里的删除是删除最先遇到的项(即不是首项也不是末项)
    //因为对于伸展树来说,项相同的节点是完全等价的,它们的数据并不是关心的点
    for (int32_t idx = 0; idx < APP_SYS_TREE_S_MAX_HALF; idx++)
    {
        app_sys_tree_s_test_t *test = app_mem_alloc(sizeof(app_sys_tree_s_test_t));
        test->key      = rand() % 100;
        test->rand_val = rand() % 10000;
        app_sys_tree_sn_reset(&test->sn_node);
        //从树中删除,先查找再删除
        app_sys_tree_sn_t *node = app_sys_tree_st_search(tree, &test->sn_node);
        //从树中删除,先查找再删除
        app_sys_tree_st_remove(tree, node);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_sys_tree_st_seq_tra(tree, queue, APP_SYS_TREE_S_MAX);//删除检查
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    //更新 == 删除 + 插入
    //因为删除指定的节点顶多只是将其移除出树
    //其数据是不会被做任何修改的,可以只修改关键字或其他,重新插入
    //如果出现一堆重复key,那么它们可以被视作为一个集合
    //反复的移除可以获得指定集合中全部的数据项
    app_sys_tree_sn_t *node = NULL;
    
    //先找到最小的数
    node = app_sys_tree_st_search_min(tree);
    //最小项没有前驱
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    if (app_sys_tree_st_search_prev(tree, node) != NULL)
        APP_SYS_LOG_INFO("tree is error");
    app_delay_ms(2000);
    //从树的最左叶子节点遍历到最右叶子节点
    while (node != NULL)
    {
        //打印该节点(color是无效值)
        app_sys_tree_s_test_visit(node);
        //迭代获得指定节点的后继
        node = app_sys_tree_st_search_next(tree, node);
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    
    //从剩下数中,先找到最大的数
    node = app_sys_tree_st_search_max(tree);
    //最大项没有后继
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    if (app_sys_tree_st_search_next(tree, node) != NULL)
        APP_SYS_LOG_INFO("tree is error");
    app_delay_ms(2000);
    //从树的最右叶子节点遍历到最左叶子节点
    while (node != NULL)
    {
        //打印该节点(color是无效值)
        app_sys_tree_s_test_visit(node);
        //迭代获得指定节点的前驱
        node = app_sys_tree_st_search_prev(tree, node);
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}
