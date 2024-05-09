/*实现目标:
 *    简要的内存分配适配模组
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if SCUI_MEM_RECORD_CHECK
static scui_mem_record_item_t scui_mem_record_def[  SCUI_MEM_RECORD_ITEM_DEF]   = {0};
static scui_mem_record_item_t scui_mem_record_text[ SCUI_MEM_RECORD_ITEM_TEXT]  = {0};
static scui_mem_record_item_t scui_mem_record_graph[SCUI_MEM_RECORD_ITEM_GRAPH] = {0};

static scui_mem_record_item_t * const scui_mem_record_list[] = {
    scui_mem_record_def,
    scui_mem_record_text,
    scui_mem_record_graph,
};
static uint32_t const scui_mem_record_list_num[] = {
    SCUI_MEM_RECORD_ITEM_DEF,
    SCUI_MEM_RECORD_ITEM_TEXT,
    SCUI_MEM_RECORD_ITEM_GRAPH,
};

/*@brief 内存记录
 */
static bool scui_mem_record_item_add(scui_mem_record_item_t item)
{
    for (uint32_t idx = 0; idx < scui_mem_record_list_num[item.type]; idx++)
        if (scui_mem_record_list[item.type][idx].ptr == NULL) {
            scui_mem_record_list[item.type][idx] = item;
            return true;
        }
    return false;
}

/*@brief 内存记录
 */
static bool scui_mem_record_item_del(scui_mem_record_item_t item)
{
    for (uint32_t idx0 = 0; idx0 < scui_arr_len(scui_mem_record_list); idx0++)
    for (uint32_t idx1 = 0; idx1 < scui_mem_record_list_num[idx0]; idx1++)
        if (scui_mem_record_list[idx0][idx1].ptr == item.ptr) {
            /* 整理:让后面的向前移动一个位置,最后一个位置填充空位 */
            for (idx1 = idx1; idx1 + 1 < scui_mem_record_list_num[idx0]; idx1++) {
                scui_mem_record_list[idx0][idx1] = scui_mem_record_list[idx0][idx1 + 1];
                if (scui_mem_record_list[idx0][idx1 + 1].ptr == NULL)
                    return true;
            }
            scui_mem_record_list[idx0][idx1] = (scui_mem_record_item_t){0};
            return true;
        }
    return false;
}

/*@brief 内存记录分析
 */
void scui_mem_record_analysis(void)
{
    SCUI_LOG_INFO("scui mem record:");
    SCUI_LOG_INFO("<file,func,line,type,ptr,size>:");
    for (uint32_t idx0 = 0; idx0 < scui_arr_len(scui_mem_record_list); idx0++)
    for (uint32_t idx1 = 0; idx1 < scui_mem_record_list_num[idx0]; idx1++) {
        if (scui_mem_record_list[idx0][idx1].ptr == NULL)
            continue;
        SCUI_LOG_INFO("<%s,%s,%u,%u,%p,%u>",
             scui_mem_record_list[idx0][idx1].file,
             scui_mem_record_list[idx0][idx1].func,
             scui_mem_record_list[idx0][idx1].line,
             scui_mem_record_list[idx0][idx1].type,
             scui_mem_record_list[idx0][idx1].ptr,
             scui_mem_record_list[idx0][idx1].size);
    }
}

/*@brief 内存记录统计
 */
void scui_mem_record_statistic(void)
{
    SCUI_LOG_INFO("scui mem record:");
    for (uint32_t idx0 = 0; idx0 < scui_arr_len(scui_mem_record_list); idx0++) {
        
        uint32_t size = 0;
        
        for (uint32_t idx1 = 0; idx1 < scui_mem_record_list_num[idx0]; idx1++)
           if (scui_mem_record_list[idx0][idx1].ptr != NULL)
               size += scui_mem_record_list[idx0][idx1].size;
        
        SCUI_LOG_INFO("<type:%d, total size:%u>", idx0, size);
    }
}

#endif

/*@brief 内存分配(外部不直接调用)
 *@param file 内存分配点(文件名)
 *@param line 内存分配点(文件行数)
 *@param func 内存分配点(函数名称)
 *@param type 内存分配类型
 *@param size 内存大小
 *@retval 内存地址
 */
void * scui_mem_alloc(const char *file, const char *func, uint32_t line, scui_mem_type_t type, uint32_t size)
{
    void *ptr = NULL;
    
    if (size == 0)
        return NULL;
    
    #if 0
    #elif APP_ARCH_IS_PC
    ptr = app_mem_alloc(size);
    #else
    #endif
    
    #if SCUI_MEM_RECORD_CHECK
    if (ptr == NULL)
        scui_mem_record_statistic();
    else
    if (!scui_mem_record_item_add((scui_mem_record_item_t) {
        .file = file, .func = func, .line = line,
        .type = type, .ptr  = ptr,  .size = size,})) {
         SCUI_LOG_WARN("record queue is full, item will be discard");
         scui_mem_record_statistic();
    }
    #endif
    
    return ptr;
}

/*@brief 内存释放(外部不直接调用)
 *@param file 内存分配点(文件名)
 *@param func 内存分配点(函数名称)
 *@param line 内存分配点(文件行数)
 *@param ptr  内存地址
 */
void scui_mem_free(const char *file, const char *func, uint32_t line, void *ptr)
{
    if (ptr == NULL)
        return;
    
    #if 0
    #elif APP_ARCH_IS_PC
    app_mem_free(ptr);
    #else
    #endif
    
    #if SCUI_MEM_RECORD_CHECK
    if (!scui_mem_record_item_del((scui_mem_record_item_t){.ptr = ptr,}))
         SCUI_LOG_WARN("record queue is not find, maybe discard");
    #endif
}

/*@brief 内存模组就绪
 */
void scui_mem_ready(void)
{
}
