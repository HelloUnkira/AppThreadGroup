/*实现目标:
 *    嵌入式红黑树测试
 */

#define APP_SYS_LOG_LIMIT_RECORD    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

//这是我要获得红黑树语义的数据结构
typedef struct {
    int32_t key;
    /* 将红黑树侵入到这个容器中 */
    app_sys_tree_rbsn_t rbsn_node;
    /*  */
    int32_t rand_val;
} app_sys_tree_rbs_test_t;

uint8_t app_sys_tree_rbs_test_compare(app_sys_tree_rbsn_t *node1, app_sys_tree_rbsn_t *node2)
{
    app_sys_tree_rbs_test_t *test1 = app_sys_own_ofs(app_sys_tree_rbs_test_t, rbsn_node, node1);
    app_sys_tree_rbs_test_t *test2 = app_sys_own_ofs(app_sys_tree_rbs_test_t, rbsn_node, node2);
    
    return test1->key < test2->key ? 1 : 0;
}

uint8_t app_sys_tree_rbs_test_confirm(app_sys_tree_rbsn_t *node1, app_sys_tree_rbsn_t *node2)
{
    app_sys_tree_rbs_test_t *test1 = app_sys_own_ofs(app_sys_tree_rbs_test_t, rbsn_node, node1);
    app_sys_tree_rbs_test_t *test2 = app_sys_own_ofs(app_sys_tree_rbs_test_t, rbsn_node, node2);
    
    return test1->key == test2->key ? 0 : 1;
}

void app_sys_tree_rbs_test_visit(app_sys_tree_rbsn_t *node, uint8_t color)
{
    app_sys_tree_rbs_test_t *test = app_sys_own_ofs(app_sys_tree_rbs_test_t, rbsn_node, node);
    /* 暴力测试时不全部打印 */
    APP_SYS_LOG_DEBUG("key:%d val:%d color:%s", test->key, test->rand_val,
                      color == app_sys_tree_rbsn_color_b ? "BLACK" :
                      color == app_sys_tree_rbsn_color_r ? "RED"   : "ERROR");
}

void app_sys_tree_rbs_test(void)
{
    #if APP_SYS_LOG_LOCAL_LEVEL > 1
    return;
    #endif
    
    #define APP_SYS_TREE_RB_MAX         (1000000)
    #define APP_SYS_TREE_RB_MAX_HALF    (APP_SYS_TREE_RB_MAX / 2)
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("tree stack depth max:%u", APP_SYS_TREE_RBS_DEPTH);
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    
    app_sys_tree_rbst_t *tree = app_mem_alloc(sizeof(app_sys_tree_rbst_t));
    app_sys_tree_rbst_reset(tree);
    app_sys_tree_rbst_config(tree, app_sys_tree_rbs_test_compare, app_sys_tree_rbs_test_confirm, app_sys_tree_rbs_test_visit);
    
    app_sys_tree_rbsn_t **queue = app_mem_alloc(sizeof(app_sys_tree_rbsn_t *)  * APP_SYS_TREE_RB_MAX);
    app_sys_tree_rbsn_t **stack = app_mem_alloc(sizeof(app_sys_tree_rbsn_t *)  * APP_SYS_TREE_RB_MAX);
    int32_t *flags = app_mem_alloc(sizeof(int32_t) * APP_SYS_TREE_RB_MAX);
    
    //随机插入节点
    for (int32_t idx = 0; idx < APP_SYS_TREE_RB_MAX; idx++)
    {
        app_sys_tree_rbs_test_t *test = app_mem_alloc(sizeof(app_sys_tree_rbs_test_t));
        test->key      = rand() % 100;
        test->rand_val = rand() % 10000;
        app_sys_tree_rbsn_reset(&test->rbsn_node);
        app_sys_tree_rbst_insert(tree, &test->rbsn_node);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_sys_tree_rbst_seq_tra(tree, queue, APP_SYS_TREE_RB_MAX);//插入检查
    APP_SYS_LOG_INFO("check:%d", app_sys_tree_rbst_check_valid(tree, stack, flags, APP_SYS_TREE_RB_MAX));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    //随机删除一半节点 (事实上基于key的红黑树理论上不支持重复key)
    //这里的删除是删除最先遇到的项(即不是首项也不是末项)
    //因为对于红黑树来说,项相同的节点是完全等价的,它们的数据并不是关心的点
    for (int32_t idx = 0; idx < APP_SYS_TREE_RB_MAX_HALF; idx++)
    {
        app_sys_tree_rbs_test_t *test = app_mem_alloc(sizeof(app_sys_tree_rbs_test_t));
        test->key      = rand() % 100;
        test->rand_val = rand() % 10000;
        app_sys_tree_rbsn_reset(&test->rbsn_node);
        //从树中删除,先查找再删除
        app_sys_tree_rbsn_t *node = app_sys_tree_rbst_search(tree, &test->rbsn_node);
        //从树中删除,先查找再删除
        app_sys_tree_rbst_remove(tree, node);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_sys_tree_rbst_seq_tra(tree, queue, APP_SYS_TREE_RB_MAX);//删除检查
    APP_SYS_LOG_INFO("check:%d", app_sys_tree_rbst_check_valid(tree, stack, flags, APP_SYS_TREE_RB_MAX));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    //更新 == 删除 + 插入
    //因为删除指定的节点顶多只是将其移除出树
    //其数据是不会被做任何修改的,可以只修改关键字或其他,重新插入
    //如果出现一堆重复key,那么它们可以被视作为一个集合
    //反复的移除可以获得指定集合中全部的数据项
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    //最小的数核对
    if (app_sys_tree_rbst_iter_min(tree) != app_sys_tree_rbst_search_min(tree))
        APP_SYS_LOG_INFO("tree is error");
    app_delay_ms(2000);
    //从树的最左叶子节点遍历到最右叶子节点,打印该节点(color是无效值)
    app_sys_tree_rbst_iter_btra(tree, node)
    app_sys_tree_rbs_test_visit(node, app_sys_tree_rbsn_error);
    if (app_sys_tree_rbst_iter_used(tree, true))
        APP_SYS_LOG_INFO("tree iter is not recycle yet");
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    //最大的数核对
    if (app_sys_tree_rbst_iter_max(tree) != app_sys_tree_rbst_search_max(tree))
        APP_SYS_LOG_INFO("tree is error");
    app_delay_ms(2000);
    //从树的最右叶子节点遍历到最左叶子节点,打印该节点(color是无效值)
    app_sys_tree_rbst_iter_ftra(tree, node)
    app_sys_tree_rbs_test_visit(node, app_sys_tree_rbsn_error);
    if (app_sys_tree_rbst_iter_used(tree, true))
        APP_SYS_LOG_INFO("tree iter is not recycle yet");
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}
