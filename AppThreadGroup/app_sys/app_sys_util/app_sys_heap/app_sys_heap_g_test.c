/*实现目标:
 *    泛型大根堆和小根堆测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

typedef struct {
    uint8_t  data1;
    uint16_t data2;
    uint32_t data4;
    uint64_t data8;
    double   data_d;
    float    data_f;
} app_sys_heap_g_test_t;

bool app_sys_heap_g_test_fc(void *node1, void *node2)
{
    return ((app_sys_heap_g_test_t *)node1)->data4 >
           ((app_sys_heap_g_test_t *)node2)->data4 ? true : false;
}

void app_sys_heap_g_test(void)
{
    #define APP_SYS_HEAP_G_LEN  100
    
    app_sys_heap_g_t heap = {0};
    void *arr[APP_SYS_HEAP_G_LEN] = {NULL};
    
    app_sys_heap_g_reset(&heap, arr, APP_SYS_HEAP_G_LEN, app_sys_heap_g_test_fc);
    
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    for (uint32_t I = 0; I < APP_SYS_HEAP_G_LEN; I++) {
        app_sys_heap_g_test_t *data = app_mem_alloc(sizeof(app_sys_heap_g_test_t));
        data->data4 = rand() % 100;
        APP_SYS_LOG_INFO_RAW("%d ", data->data4);
        app_sys_heap_g_push(&heap, data);
    }
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    app_sys_heap_g_test_t *data = NULL;
    while (app_sys_heap_g_pop(&heap, &data) == true) {
        APP_SYS_LOG_INFO_RAW("%d ", data->data4);
        app_mem_free(data);
    }
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}
