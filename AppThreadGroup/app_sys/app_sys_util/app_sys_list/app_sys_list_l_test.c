/*实现目标:
 *    链表测试
 */

#define APP_SYS_LOG_LIMIT_RECORD    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

typedef struct {
    uint64_t data8;
    uint32_t data4;
    uint16_t data2;
    uint8_t  data1;
    /* 为了简化将其合并书写 */
    union {
        /* 我们将双向链表侵入到该数据集合内部 */
        app_sys_list_dln_t dl_node;
        /* 我们将单向链表侵入到该数据集合内部 */
        app_sys_list_sln_t sl_node;
    };
    /*  */
    float    data_f;
    double   data_d;
    char     data_str[20];
    /* ...... */
} app_sys_list_test_t;

void app_sys_list_test_data_reset(app_sys_list_test_t *test, uint8_t value)
{
    test->data8 = value;
    test->data4 = value;
    test->data2 = value;
    test->data1 = value;
    test->data_f = value;
    test->data_d = value;
    sprintf(test->data_str,"idx:%u", value);
}

void app_sys_list_test_data_check(app_sys_list_test_t *test)
{
    APP_SYS_LOG_INFO_RAW("data8:%6llu,",  test->data8);
    APP_SYS_LOG_INFO_RAW("data4:%6llu,",  test->data4);
    APP_SYS_LOG_INFO_RAW("data2:%6llu,",  test->data2);
    APP_SYS_LOG_INFO_RAW("data1:%6llu,",  test->data1);
    APP_SYS_LOG_INFO_RAW("data_f:%6f,",   test->data_f);
    APP_SYS_LOG_INFO_RAW("data_d:%8f,",   test->data_d);
    APP_SYS_LOG_INFO_RAW("data_str:%s,",  test->data_str);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

/*@brief 侵入式双链表测试
 */
void app_sys_list_dll_test(void)
{
    uint32_t idx = 0;
    
    app_sys_list_dln_t *target = NULL;
    app_sys_list_dln_t *node = NULL;
    app_sys_list_dll_t *dl_list = app_mem_alloc(sizeof(app_sys_list_dll_t));
    /* 初始化双向链表体 */
    app_sys_list_dll_reset(dl_list);
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 批量生成并追加节点 */
    for (idx = 0, target = NULL; idx < 10; idx++) {
        /* 生成数据实体并初始化 */
        app_sys_list_test_t *test = app_mem_alloc(sizeof(app_sys_list_test_t));
        app_sys_list_test_data_reset(test, idx);
        node = &(test->dl_node);
        app_sys_list_dln_reset(node);
        app_sys_list_dll_ainsert(dl_list, target, node);
        target = node;
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 正向迭代检查 */
    app_sys_list_dll_btra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 逆向迭代检查 */
    app_sys_list_dll_ftra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    for (idx = 0; idx < 10; idx++) {
        /* 销毁所有节点并回收数据空间 */
        node = app_sys_list_dll_head(dl_list);
        app_sys_list_dll_remove(dl_list, node);
        app_mem_free(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 正向迭代检查 */
    app_sys_list_dll_btra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 逆向迭代检查 */
    app_sys_list_dll_ftra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 批量生成并追加节点 */
    for (idx = 0, target = NULL; idx < 10; idx++) {
        /* 生成数据实体并初始化 */
        app_sys_list_test_t *test = app_mem_alloc(sizeof(app_sys_list_test_t));
        app_sys_list_test_data_reset(test, idx);
        node = &(test->dl_node);
        app_sys_list_dln_reset(node);
        app_sys_list_dll_pinsert(dl_list, target, node);
        target = node;
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 正向迭代检查 */
    app_sys_list_dll_btra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 逆向迭代检查 */
    app_sys_list_dll_ftra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    for (idx = 0; idx < 10; idx++) {
        /* 销毁所有节点并回收数据空间 */
        node = app_sys_list_dll_head(dl_list);
        app_sys_list_dll_remove(dl_list, node);
        app_mem_free(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 正向迭代检查 */
    app_sys_list_dll_btra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 逆向迭代检查 */
    app_sys_list_dll_ftra(dl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, dl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}

/*@brief 侵入式单链表测试
 */
void app_sys_list_sll_test(void)
{
    uint32_t idx = 0;
    
    app_sys_list_sln_t *node = NULL;
    app_sys_list_sll_t *sl_list = app_mem_alloc(sizeof(app_sys_list_sll_t));
    /* 初始化单向链表体 */
    app_sys_list_sll_reset(sl_list);
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 批量生成并追加节点 */
    for (idx = 0; idx < 10; idx++) {
        /* 生成数据实体并初始化 */
        app_sys_list_test_t *test = app_mem_alloc(sizeof(app_sys_list_test_t));
        app_sys_list_test_data_reset(test, idx);
        node = &(test->sl_node);
        app_sys_list_sln_reset(node);
        app_sys_list_sll_pinsert(sl_list, node);
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 迭代检查 */
    app_sys_list_sll_tra(sl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    for (idx = 0; idx < 10; idx++) {
        /* 销毁所有节点并回收数据空间 */
        node = app_sys_list_sll_head(sl_list);
        app_sys_list_sll_remove(sl_list, NULL, node);
        app_mem_free(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 迭代检查 */
    app_sys_list_sll_tra(sl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 批量生成并追加节点 */
    for (idx = 0; idx < 10; idx++) {
        /* 生成数据实体并初始化 */
        app_sys_list_test_t *test = app_mem_alloc(sizeof(app_sys_list_test_t));
        app_sys_list_test_data_reset(test, idx);
        node = &(test->sl_node);
        app_sys_list_sln_reset(node);
        app_sys_list_sll_ainsert(sl_list, node);
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 迭代检查 */
    app_sys_list_sll_tra(sl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    for (idx = 0; idx < 10; idx++) {
        /* 销毁所有节点并回收数据空间 */
        node = app_sys_list_sll_head(sl_list);
        app_sys_list_sll_remove(sl_list, NULL, node);
        app_mem_free(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    /* 迭代检查 */
    app_sys_list_sll_tra(sl_list, node)
    app_sys_list_test_data_check(app_sys_own_ofs(app_sys_list_test_t, sl_node, node));
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}
