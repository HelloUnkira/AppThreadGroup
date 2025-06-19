/*实现目标:
 *    哈希表测试
 */

#define APP_SYS_LOG_LIMIT_RECORD    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 摘要的来源网络的Hash散列函数
 */
static uint32_t app_sys_table_rbs_hash(uint8_t *data, uint32_t length)
{
    uint64_t h = 0, g = 0;
    
    for (uint32_t idx = 0; idx < length; idx++) {
        h = (h << 4) + data[idx];
        if (g = h & 0xf0000000)
            h ^= g >> 24;
        h &= ~g;
    }
    
    /* 散列不均匀时需要适当加点盐 */
    const uint32_t salt = 13;
    
    return (uint32_t)(h >> salt);
}

typedef struct {
    uint32_t key;
    uint32_t data;
    /* 我们将哈希表侵入到该数据集合内部 */
    app_sys_table_rbsn_t node;
    uint32_t keep;
    uint32_t adding;
} app_sys_table_rbs_test_t;

static uint32_t app_sys_table_rbs_test_fd(app_sys_table_rbsn_t *target)
{
    app_sys_table_rbs_test_t *test = app_sys_own_ofs(app_sys_table_rbs_test_t, node, target);
    return app_sys_table_rbs_hash((uint8_t *)&test->key, sizeof(uint32_t));
}

static uint8_t app_sys_table_rbs_test_fc1(app_sys_table_rbsn_t *node1, app_sys_table_rbsn_t *node2)
{
    app_sys_table_rbs_test_t *test1 = app_sys_own_ofs(app_sys_table_rbs_test_t, node, node1);
    app_sys_table_rbs_test_t *test2 = app_sys_own_ofs(app_sys_table_rbs_test_t, node, node2);
    return test1->key < test2->key ? 1 : 0;
}

static uint8_t app_sys_table_rbs_test_fc2(app_sys_table_rbsn_t *node1, app_sys_table_rbsn_t *node2)
{
    app_sys_table_rbs_test_t *test1 = app_sys_own_ofs(app_sys_table_rbs_test_t, node, node1);
    app_sys_table_rbs_test_t *test2 = app_sys_own_ofs(app_sys_table_rbs_test_t, node, node2);
    return test1->key == test2->key ? 0 : 1;
}

static void app_sys_table_rbs_test_fv(app_sys_table_rbsn_t *target, uint32_t idx)
{
    app_sys_table_rbs_test_t *test = app_sys_own_ofs(app_sys_table_rbs_test_t, node, target);
    
    static uint32_t idx_ofs = -1;
    
    if (idx_ofs != idx) {
        idx_ofs  = idx;
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO_RAW("<%2u, %2u>", test->key, test->data);
}

void app_sys_table_rbs_test(void)
{
    #define APP_SYS_TABLE_RBS_LENGTH    10
    #define APP_SYS_TABLE_RBS_MAX       (10000000)
    
    app_sys_table_rbst_t test_table = {0};
    app_sys_table_rbsl_t test_list[APP_SYS_TABLE_RBS_LENGTH] = {0};
    
    app_sys_table_rbsl_reset( test_list,  APP_SYS_TABLE_RBS_LENGTH);
    app_sys_table_rbst_reset(&test_table, app_sys_table_rbs_test_fd,
                                          app_sys_table_rbs_test_fc1,
                                          app_sys_table_rbs_test_fc2,
                                          app_sys_table_rbs_test_fv,
                              test_list,  APP_SYS_TABLE_RBS_LENGTH);
    
    uint8_t pct_insert_rcd = 0;
    APP_SYS_LOG_INFO("insert s:");
    /* 随机生成键值对 */
    for (uint32_t idx = 0; idx < APP_SYS_TABLE_RBS_MAX; idx++) {
        app_sys_table_rbs_test_t *data = app_mem_alloc(sizeof(app_sys_table_rbs_test_t));
        data->key  = rand() % 100;
        data->data = idx;
        app_sys_table_rbsn_reset(&(data->node));
        app_sys_table_rbst_insert(&test_table, &(data->node));
        
        uint8_t pct_insert =  app_sys_map(idx, 0, APP_SYS_TABLE_RBS_MAX, 0, 100);
        if (pct_insert_rcd != pct_insert) {
            pct_insert_rcd  = pct_insert;
            APP_SYS_LOG_INFO("pct_insert:%d", pct_insert);
        }
    }
    
    APP_SYS_LOG_INFO("insert e:");
    app_sys_table_rbst_visit(&test_table);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    uint8_t pct_remove_rcd = 0;
    APP_SYS_LOG_INFO("remove s:");
    /* 随机移除键值对 */
    for (uint32_t idx = 0; idx < APP_SYS_TABLE_RBS_MAX; idx) {
        /* 查询 */
        app_sys_table_rbs_test_t data = {.key = rand() % 100};
        app_sys_table_rbsn_t *target = app_sys_table_rbst_search(&test_table, &(data.node));
        if (target == NULL)
            continue;
        app_sys_table_rbst_remove(&test_table, target);
        app_mem_free(app_sys_own_ofs(app_sys_table_rbs_test_t, node, target));
        idx++;
        
        uint8_t pct_remove =  app_sys_map(idx, 0, APP_SYS_TABLE_RBS_MAX, 0, 100);
        if (pct_remove_rcd != pct_remove) {
            pct_remove_rcd  = pct_remove;
            APP_SYS_LOG_INFO("pct_remove:%d", pct_remove);
        }
    }
    
    APP_SYS_LOG_INFO("remove e:");
    app_sys_table_rbst_visit(&test_table);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}
