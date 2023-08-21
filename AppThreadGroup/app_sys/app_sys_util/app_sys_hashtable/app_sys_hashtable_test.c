/*实现目标:
 *    哈希表测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_list.h"
#include "app_sys_hashtable.h"

/*@brief 摘要的来源网络的Hash散列函数
 */
uint32_t app_sys_hashtable_elf_hash(uint8_t *data, uint32_t length)
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
    app_sys_hashtable_dn_t node;
    uint32_t keep;
    uint32_t adding;
} app_sys_hashtable_test_t;

uint32_t app_sys_hashtable_test_fd(app_sys_hashtable_dn_t *target)
{
    app_sys_hashtable_test_t *test = app_ext_own_ofs(app_sys_hashtable_test_t, node, target);
    return app_sys_hashtable_elf_hash((uint8_t *)&test->key, sizeof(uint32_t));
}

bool app_sys_hashtable_test_fc(app_sys_hashtable_dn_t *node1, app_sys_hashtable_dn_t *node2)
{
    app_sys_hashtable_test_t *test1 = app_ext_own_ofs(app_sys_hashtable_test_t, node, node1);
    app_sys_hashtable_test_t *test2 = app_ext_own_ofs(app_sys_hashtable_test_t, node, node2);
    return test1->key == test2->key;
}

void app_sys_hashtable_test_fv(app_sys_hashtable_dn_t *target, uint32_t idx)
{
    app_sys_hashtable_test_t *test = app_ext_own_ofs(app_sys_hashtable_test_t, node, target);
    
    static uint32_t idx_ofs = -1;
    
    if (idx_ofs != idx) {
        idx_ofs  = idx;
        APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    }
    APP_SYS_LOG_INFO_RAW("<%2u, %2u>", test->key, test->data);
}

void app_sys_hashtable_test(void)
{
    #define APP_SYS_HASHTABLE_TEST_LENGTH 10
    app_sys_hashtable_dt_t test_table = {0};
    app_sys_hashtable_dl_t test_list[APP_SYS_HASHTABLE_TEST_LENGTH] = {0};
    
    app_sys_hashtable_dl_reset(test_list,   APP_SYS_HASHTABLE_TEST_LENGTH);
    app_sys_hashtable_dt_reset(&test_table, app_sys_hashtable_test_fd,
                                            app_sys_hashtable_test_fc,
                                            app_sys_hashtable_test_fv,
                                test_list,  APP_SYS_HASHTABLE_TEST_LENGTH);
    
    /* 随机生成100个键值对 */
    for (uint32_t idx = 0; idx < 100; idx++) {
        app_sys_hashtable_test_t *data = app_mem_alloc(sizeof(app_sys_hashtable_test_t));
        data->key  = idx;
        data->data = rand() % 100;
        app_sys_hashtable_dn_reset(&(data->node));
        app_sys_hashtable_dt_insert(&test_table, &(data->node));
    }
    
    APP_SYS_LOG_INFO("app_sys_hashtable_dt_insert:");
    app_sys_hashtable_dt_visit(&test_table);
    
    /* 随机移除一半的键值对 */
    for (uint32_t idx = 0; idx < 50; idx) {
        /* 查询 */
        app_sys_hashtable_test_t  data = {.key = rand() % 100};
        app_sys_hashtable_dn_t *target = app_sys_hashtable_dt_search(&test_table, &(data.node));
        if (target == NULL)
            continue;
        app_sys_hashtable_dt_remove(&test_table, target);
        app_mem_free(app_ext_own_ofs(app_sys_hashtable_test_t, node, target));
        idx++;
    }
    
    APP_SYS_LOG_INFO("app_sys_hashtable_dt_remove:");
    app_sys_hashtable_dt_visit(&test_table);
    
}
