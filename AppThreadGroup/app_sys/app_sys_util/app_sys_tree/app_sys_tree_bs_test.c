/*实现目标:
 *    B*树测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

typedef struct {
    uint32_t key;
} app_sys_tree_bs_test_t;

void app_sys_tree_bs_test_visit(uint8_t state, uint32_t number, uint32_t *key, void **child, bool destroy)
{
    if (state == app_sys_tree_bsn_state_common) {
    for (uint32_t idx = 0; idx < number; idx++)
        APP_SYS_LOG_DEBUG_RAW("%d ", key[idx]);
        APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
    }
    for (uint32_t idx = 0; idx < number; idx++)
        if (destroy && child != NULL)
            app_mem_free(child[idx]);
}

uint32_t app_sys_tree_bs_test_key(void *data)
{
    return ((app_sys_tree_bs_test_t *)data)->key;
}

void app_sys_tree_bs_test(void)
{
    #if APP_SYS_LOG_LOCAL_LEVEL > 1
    return;
    #endif
    
    #define APP_SYS_TREE_BS_MAX         (1000000)
    #define APP_SYS_TREE_BS_MAX_HALF    (APP_SYS_TREE_BS_MAX / 2)
    
    //1.创建一个容器集合(树)
    app_sys_tree_bst_t *tree = app_mem_alloc(sizeof(app_sys_tree_bst_t));
    //2.初始化一个容器集合(树)
    app_sys_tree_bst_config(tree, 10, app_sys_tree_bs_test_key, app_sys_tree_bs_test_visit);
    
    //测试:随机生成key
    for (int32_t idx = 0; idx < APP_SYS_TREE_BS_MAX; idx++) {
        //插入到树中
        void *data = app_mem_alloc(sizeof(app_sys_tree_bs_test_t));
        ((app_sys_tree_bs_test_t *)data)->key = rand() % 100;
        app_sys_tree_bst_insert(tree, data);
        // APP_SYS_LOG_DEBUG_RAW("%u ", ((app_sys_tree_bs_test_t *)data)->key);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("%u", app_sys_tree_bst_seq_tra(tree, 0));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    //测试:随机生成key
    for (int32_t idx = 0; idx < APP_SYS_TREE_BS_MAX_HALF; idx++) {
        //移除出树中
        void *data = NULL;
        app_sys_tree_bst_remove(tree, &data, rand() % 100);
        if (data == NULL)
            continue;
        // APP_SYS_LOG_DEBUG_RAW("%u ", ((app_sys_tree_bs_test_t *)data)->key);
    }
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("%u", app_sys_tree_bst_seq_tra(tree, 0));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    app_delay_ms(2000);
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("%u", app_sys_tree_bst_seq_tra(tree, 1));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}