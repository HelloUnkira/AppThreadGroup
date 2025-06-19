/*实现目标:
 *    泛用系统句柄模组测试
 */

#define APP_SYS_LOG_LIMIT_RECORD    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"


void app_sys_handle_test(void)
{
    #define APP_SYS_HANDLE_TEST_MAX     15
    uint32_t  idx = 0;
    uint32_t *ptr = NULL;
    uint32_t  arr[APP_SYS_HANDLE_TEST_MAX] = {0};
    
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++) {
        app_sys_handle_take(&arr[idx]);
        app_sys_handle_src_set(arr[idx], &arr[idx]);
    }
    
    /* 打乱 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++) {
        uint32_t tidx = rand() % APP_SYS_HANDLE_TEST_MAX;
        uint32_t temp = arr[idx];
        arr[idx]  = arr[tidx];
        arr[tidx] = temp;
    }

    APP_SYS_LOG_INFO_RAW("arr:");
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++)
         APP_SYS_LOG_INFO_RAW("%d ", arr[idx]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());

    /* 获取句柄资源 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++) {
        app_sys_handle_src_get(arr[idx], &ptr);
        APP_SYS_LOG_INFO("arr[%d]:%d", arr[idx], *ptr);
    }

    /* 释放 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++)
        app_sys_handle_give(arr[idx]);

    /* 再次获取,验证 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++) {
        app_sys_handle_take(&arr[idx]);
        app_sys_handle_src_set(arr[idx], &arr[idx]);
    }
    
    /* 获取句柄资源 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++) {
        app_sys_handle_src_get(arr[idx], &ptr);
        APP_SYS_LOG_INFO("arr[%d]:%d", arr[idx], *ptr);
    }

    /* 释放 */
    for (idx = 0; idx < APP_SYS_HANDLE_TEST_MAX; idx++)
        app_sys_handle_give(arr[idx]);
}
