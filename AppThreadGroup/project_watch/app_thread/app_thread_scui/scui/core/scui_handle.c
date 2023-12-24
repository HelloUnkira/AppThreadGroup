/*实现目标:
 *    泛用句柄
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static void * scui_handle_table_s[SCUI_HANDLE_LIMIT_S] = {NULL};
static void * scui_handle_table_d[SCUI_HANDLE_LIMIT_D] = {NULL};

/*@brief 生成一个句柄
 *@param dynamic 动态句柄,静态句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_new(bool dynamic)
{
    if (dynamic) {
        for (uint32_t idx = 0; idx < SCUI_HANDLE_LIMIT_D; idx++)
            if (scui_handle_table_d[idx] == NULL) {
                scui_handle_table_d[idx]  = (void *)(~(uintptr_t)0);
                return SCUI_HANDLE_OFFSET_D + idx;
            }
        /* 句柄实例不足 */
        APP_SYS_ASSERT(false);
    } else {
        for (uint32_t idx = 0; idx < SCUI_HANDLE_LIMIT_S; idx++)
            if (scui_handle_table_s[idx] == NULL) {
                scui_handle_table_s[idx]  = (void *)(~(uintptr_t)0);
                return SCUI_HANDLE_OFFSET_S + idx;
            }
        /* 句柄实例不足 */
        APP_SYS_ASSERT(false);
    }
    return SCUI_HANDLE_INVALID;
}

/*@brief 销毁一个静态句柄
 *@param dynamic 动态句柄,静态句柄
 *@param handle 句柄
 *@retval 成功失败
 */
bool scui_handle_del(bool dynamic, scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return false;
    if (dynamic) {
        if (handle - SCUI_HANDLE_OFFSET_D < SCUI_HANDLE_LIMIT_D) {
            uint32_t idx = handle - SCUI_HANDLE_OFFSET_D;
            scui_handle_table_d[idx] = NULL;
            return true;
        }
    } else {
        if (handle - SCUI_HANDLE_OFFSET_S < SCUI_HANDLE_LIMIT_S) {
            uint32_t idx = handle - SCUI_HANDLE_OFFSET_S;
            scui_handle_table_s[idx] = NULL;
            return true;
        }
    }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return false;
}

/*@brief 静态句柄获得资源
 *@param handle 句柄
 *@retval 资源
 */
void * scui_handle_get(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return NULL;
    if (handle - SCUI_HANDLE_OFFSET_S < SCUI_HANDLE_LIMIT_S) {
        uint32_t idx = handle - SCUI_HANDLE_OFFSET_S;
        APP_SYS_ASSERT(scui_handle_table_s[idx] != NULL);
        if (scui_handle_table_s[idx] != (void *)(~(uintptr_t)0))
            return scui_handle_table_s[idx];
    }
    if (handle - SCUI_HANDLE_OFFSET_D < SCUI_HANDLE_LIMIT_D) {
        uint32_t idx = handle - SCUI_HANDLE_OFFSET_D;
        APP_SYS_ASSERT(scui_handle_table_d[idx] != NULL);
        if (scui_handle_table_d[idx] != (void *)(~(uintptr_t)0))
            return scui_handle_table_d[idx];
    }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return NULL;
}

/*@brief 静态句柄设置资源
 *@param handle 句柄
 *@param source 资源
 *@retval 成功失败
 */
bool scui_handle_set(scui_handle_t handle, void *source)
{
    if (handle == SCUI_HANDLE_INVALID)
        return false;
    if (handle - SCUI_HANDLE_OFFSET_S < SCUI_HANDLE_LIMIT_S) {
        uint32_t idx = handle - SCUI_HANDLE_OFFSET_S;
        scui_handle_table_s[idx] = source;
        return true;
    }
    if (handle - SCUI_HANDLE_OFFSET_D < SCUI_HANDLE_LIMIT_D) {
        uint32_t idx = handle - SCUI_HANDLE_OFFSET_D;
        scui_handle_table_d[idx] = source;
        return true;
    }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return false;
}
