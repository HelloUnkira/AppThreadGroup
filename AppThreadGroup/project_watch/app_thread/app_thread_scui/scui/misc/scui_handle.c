/*实现目标:
 *    泛用句柄
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        3   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_handle_table_t scui_handle_table[SCUI_HANDLE_TABLE_LIMIT] = {0};
static void * scui_handle_table_share[SCUI_HANDLE_SHARE_LIMIT] = {NULL};

/*@brief 句柄表获取
 *@param handle 句柄
 *@retval 句柄表
 */
scui_handle_table_t * scui_handle_table_find(scui_handle_t handle)
{
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++) {
        if (scui_handle_table[ofs].source == NULL)
            continue;
        scui_handle_t betw_l = scui_handle_table[ofs].offset;
        scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
        if (scui_betw_lx(handle, betw_l, betw_r))
            return &scui_handle_table[ofs];
    }
    
    SCUI_LOG_ERROR("handle %u is unknown", handle);
    return NULL;
}

/*@brief 句柄表注册
 *@param table 句柄表
 */
void scui_handle_table_register(scui_handle_table_t *table)
{
    SCUI_ASSERT(table->source != NULL);
    SCUI_ASSERT(table->number != 0);
    SCUI_ASSERT(table->offset != 0);
    
    /* 检查offset是否有重复 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++)
        if (scui_handle_table[ofs].source != NULL) {
            if (scui_handle_table[ofs].offset == table->offset) {
                SCUI_LOG_ERROR("duplicate list");
                SCUI_ASSERT(false);
            }
            scui_handle_t betw_l = scui_handle_table[ofs].offset;
            scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
            if (!(table->offset >= betw_r || table->offset + table->number <= betw_l)) {
                  SCUI_LOG_ERROR("overlay list");
                  SCUI_ASSERT(false);
            }
        }
    /* 检查当前表不在动态句柄范围内 */
    scui_handle_t betw_l = SCUI_HANDLE_SHARE_OFFSET;
    scui_handle_t betw_r = SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT;
    if (!(table->offset >= betw_r || table->offset + table->number <= betw_l)) {
          SCUI_LOG_ERROR("overlay list");
          SCUI_ASSERT(false);
    }
    /* 注册句柄: */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++)
        if (scui_handle_table[ofs].source == NULL) {
            scui_handle_table[ofs] = *table;
            break;
        }
    
    SCUI_LOG_INFO("handle table register:");
    SCUI_LOG_INFO("handle table source:%p", table->source);
    SCUI_LOG_INFO("handle table number:%u", table->number);
    SCUI_LOG_INFO("handle table offset:%u", table->offset);
}

/*@brief 通过句柄判断属于哪一个偏移
 *       通过这种方式可以确认句柄所在类型
 *@param handle 句柄
 */
scui_handle_t scui_handle_check(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return SCUI_HANDLE_INVALID;
    /* 检查是否是动态句柄中的句柄 */
    scui_handle_t betw_l = SCUI_HANDLE_SHARE_OFFSET;
    scui_handle_t betw_r = SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT;
    if (scui_betw_lx(handle, betw_l, betw_r))
        return SCUI_HANDLE_SHARE_OFFSET;
    /* 检查是否是静态句柄表中的句柄 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++) {
        if (scui_handle_table[ofs].source == NULL)
            continue;
        scui_handle_t betw_l = scui_handle_table[ofs].offset;
        scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
        if (scui_betw_lx(handle, betw_l, betw_r))
            return scui_handle_table[ofs].offset;
    }
    
    SCUI_LOG_ERROR("handle %u is unknown", handle);
    return SCUI_HANDLE_INVALID;
}

/*@brief 寻找一个动态句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_find(void)
{
    for (scui_handle_t idx = 0; idx < SCUI_HANDLE_SHARE_LIMIT; idx++)
        if (scui_handle_table_share[idx] == NULL) {
            scui_handle_table_share[idx]  = (void *)(~(uintptr_t)NULL);
            return SCUI_HANDLE_SHARE_OFFSET + idx;
        }
    
    /* 句柄实例不足 */
    SCUI_ASSERT(false);
    return SCUI_HANDLE_INVALID;
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
    scui_handle_t betw_l = SCUI_HANDLE_SHARE_OFFSET;
    scui_handle_t betw_r = SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT;
    if (scui_betw_lx(handle, betw_l, betw_r)) {
        scui_handle_t idx = handle - SCUI_HANDLE_SHARE_OFFSET;
        if (scui_handle_table_share[idx] != (void *)(~(uintptr_t)NULL))
            return scui_handle_table_share[idx];
            return NULL;
    }
    /* 检查是否是静态句柄表中的句柄 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++) {
        if (scui_handle_table[ofs].source == NULL)
            continue;
        scui_handle_t betw_l = scui_handle_table[ofs].offset;
        scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
        if (scui_betw_lx(handle, betw_l, betw_r)) {
            scui_handle_t idx = handle - scui_handle_table[ofs].offset;
            /* 优先进行重映射,如果存在重映射的话 */
            if (scui_handle_table[ofs].source_remap != NULL)
            if (scui_handle_table[ofs].source_remap[idx] != NULL)
                return scui_handle_table[ofs].source_remap[idx];
            /* 映射到原来的位置,如果需要 */
            SCUI_ASSERT(scui_handle_table[ofs].source[idx] != NULL);
            return scui_handle_table[ofs].source[idx];
        }
    }
    
    SCUI_LOG_WARN("handle %u is unknown", handle);
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
    scui_handle_t betw_l = SCUI_HANDLE_SHARE_OFFSET;
    scui_handle_t betw_r = SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT;
    if (scui_betw_lx(handle, betw_l, betw_r)) {
        scui_handle_t idx = handle - SCUI_HANDLE_SHARE_OFFSET;
        scui_handle_table_share[idx] = source;
        return true;
    }
    /* 检查是否是静态句柄表中的句柄 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++) {
        if (scui_handle_table[ofs].source == NULL)
            continue;
        scui_handle_t betw_l = scui_handle_table[ofs].offset;
        scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
        if (scui_betw_lx(handle, betw_l, betw_r)) {
            scui_handle_t idx = handle - scui_handle_table[ofs].offset;
            /* 优先进行重映射,如果存在重映射的话 */
            if (scui_handle_table[ofs].source_remap != NULL) {
                scui_handle_table[ofs].source_remap[idx] = source;
                return true;
            }
        }
    }
    
    SCUI_LOG_WARN("handle %u is unknown", handle);
    return false;
}

/*@brief 句柄是否重映射
 *@param handle 句柄
 *@retval 是否重映射
 */
bool scui_handle_remap(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID)
        return false;
    /* 检查是否是静态句柄表中的句柄 */
    for (uint32_t ofs = 0; ofs < SCUI_HANDLE_TABLE_LIMIT; ofs++) {
        if (scui_handle_table[ofs].source == NULL)
            continue;
        scui_handle_t betw_l = scui_handle_table[ofs].offset;
        scui_handle_t betw_r = scui_handle_table[ofs].offset + scui_handle_table[ofs].number;
        if (scui_betw_lx(handle, betw_l, betw_r)) {
            scui_handle_t idx = handle - scui_handle_table[ofs].offset;
            /* 如果存在重映射的话 */
            if (scui_handle_table[ofs].source_remap != NULL)
            if (scui_handle_table[ofs].source_remap[idx] != NULL)
                return true;
            return false;
        }
    }
    /* 检查是否是动态句柄中的句柄 */
    scui_handle_t betw_l = SCUI_HANDLE_SHARE_OFFSET;
    scui_handle_t betw_r = SCUI_HANDLE_SHARE_OFFSET + SCUI_HANDLE_SHARE_LIMIT;
    if (scui_betw_lx(handle, betw_l, betw_r))
        return true;
    
    SCUI_LOG_ERROR("handle %u is unknown", handle);
    return false;
}

/*@brief 句柄是否重映射
 *@param handle 句柄
 *@retval 是否重映射
 */
bool scui_handle_unmap(scui_handle_t handle)
{
    return !scui_handle_remap(handle);
}
