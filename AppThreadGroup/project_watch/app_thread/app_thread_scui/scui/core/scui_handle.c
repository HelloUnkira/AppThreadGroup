/*实现目标:
 *    泛用句柄
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static scui_handle_table_t scui_handle_table[SCUI_HANDLE_TABLE_LIMIT] = {0};
static void * scui_handle_table_share[SCUI_HANDLE_SHARE_LIMIT] = {NULL};

/*@brief 句柄表注册
 *@param table 句柄表
 */
void scui_handle_table_register(scui_handle_table_t *table)
{
    APP_SYS_ASSERT(table->source != NULL);
    APP_SYS_ASSERT(table->number != 0);
    APP_SYS_ASSERT(table->offset != 0);
    
    /* 检查offset是否有重复 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++)
        if (scui_handle_table[ofs].source != NULL) {
            if (scui_handle_table[ofs].offset == table->offset) {
                APP_SYS_LOG_ERROR("duplicate list");
                APP_SYS_ASSERT(false);
            }
            if (!(table->offset >= scui_handle_table[ofs].offset + scui_handle_table[ofs].number ||
                  table->offset + table->number <= scui_handle_table[ofs].offset)) {
                  APP_SYS_LOG_ERROR("overlay list");
                  APP_SYS_ASSERT(false);
            }
        }
    /* 检查当前表不在动态句柄范围内 */
    if (!(table->offset >= SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT ||
          table->offset + table->number <= SCUI_HANDLE_SHARE_OFFSET)) {
          APP_SYS_LOG_ERROR("overlay list");
          APP_SYS_ASSERT(false);
    }
    /* 注册句柄: */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++)
        if (scui_handle_table[ofs].source == NULL)
            scui_handle_table[ofs] = *table;
}

/*@brief 生成一个句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_new(void)
{
    for (uint32_t idx = 0; idx < SCUI_HANDLE_SHARE_LIMIT; idx++)
        if (scui_handle_table_share[idx] == NULL) {
            scui_handle_table_share[idx]  = (void *)(~(uintptr_t)0);
            return SCUI_HANDLE_SHARE_OFFSET + idx;
        }
    /* 句柄实例不足 */
    APP_SYS_ASSERT(false);
    return SCUI_HANDLE_INVALID;
}

/*@brief 销毁一个句柄
 *@param handle 句柄
 *@retval 成功失败
 */
bool scui_handle_del(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return false;
    if (handle - SCUI_HANDLE_SHARE_OFFSET < SCUI_HANDLE_SHARE_LIMIT) {
        uint32_t idx = handle - SCUI_HANDLE_SHARE_OFFSET;
        scui_handle_table_share[idx] = NULL;
        return true;
    }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return false;
}

/*@brief 句柄获得资源
 *@param handle 句柄
 *@retval 资源
 */
void * scui_handle_get(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return NULL;
    /* 检查是否是动态句柄中的句柄 */
    if (handle - SCUI_HANDLE_SHARE_OFFSET < SCUI_HANDLE_SHARE_LIMIT) {
        uint32_t idx = handle - SCUI_HANDLE_SHARE_OFFSET;
        if (scui_handle_table_share[idx] != (void *)(~(uintptr_t)0))
            return scui_handle_table_share[idx];
            return NULL;
    }
    /* 检查是否是静态句柄表中的句柄 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++)
        if (scui_handle_table[ofs].source != NULL)
        if (handle - scui_handle_table[ofs].offset < scui_handle_table[ofs].number) {
            uint32_t idx = handle - scui_handle_table[ofs].offset;
            APP_SYS_ASSERT(scui_handle_table[ofs].source[idx] != NULL);
            return scui_handle_table[ofs].source[idx];
        }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return NULL;
}

/*@brief 句柄设置资源
 *@param handle 句柄
 *@param source 资源
 *@retval 成功失败
 */
bool scui_handle_set(scui_handle_t handle, void *source)
{
    if (handle == SCUI_HANDLE_INVALID)
        return false;
    /* 检查是否是动态句柄中的句柄 */
    if (handle - SCUI_HANDLE_SHARE_OFFSET < SCUI_HANDLE_SHARE_LIMIT) {
        uint32_t idx = handle - SCUI_HANDLE_SHARE_OFFSET;
        scui_handle_table_share[idx] = source;
        return true;
    }
    APP_SYS_LOG_ERROR("handle %u is unknown", handle);
    return false;
}
