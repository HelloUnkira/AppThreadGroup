/*实现目标:
 *    简要的内存分配适配模组
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_mem_t scui_mem = {0};

#if SCUI_MEM_RECORD_CHECK

/*@brief 文件名去除路径
 *@param file 带路径的文件名称
 *@retval 去除路径后的文件名
 */
static const char * scui_mem_record_file_remove_path(const char *file)
{
    /* 文件名跳过路径,不需要使用时屏蔽即可 */
    /* 这里可以让编译器自优化 */
    int32_t separator = 0;
    int32_t file_len  = strlen(file);
    for (separator = file_len - 1; separator >= 0; separator--)
        if (file[separator] == '/' || file[separator] == '\\') {
            if (separator < file_len)
                file += separator + 1;
            break;
        }
    return file;
}

/*@brief 内存记录
 */
static bool scui_mem_record_item_add(scui_mem_record_item_t item)
{
    /* 不需要记录,默认已经记录 */
    if (scui_mem.record[item.type].item == NULL)
        return true;
    
    /* 去掉路径只保留文件名称即可 */
    item.file = scui_mem_record_file_remove_path(item.file);
    
    for (uint32_t idx = 0; idx < scui_mem.record[item.type].num; idx++)
        if (scui_mem.record[item.type].item[idx].ptr == NULL) {
            scui_mem.record[item.type].item[idx] = item;
            scui_mem.record[item.type].update = true;
            return true;
        }
    return false;
}

/*@brief 内存记录
 */
static bool scui_mem_record_item_del(scui_mem_record_item_t item)
{
    /* 不需要记录,默认已经记录 */
    if (scui_mem.record[item.type].item == NULL)
        return true;
    
    for (uint32_t idx = 0; idx < scui_mem.record[item.type].num; idx++)
        if (scui_mem.record[item.type].item[idx].ptr == item.ptr) {
            scui_mem.record[item.type].update = true;
            /* 整理:让后面的向前移动一个位置,最后一个位置填充空位 */
            for (idx = idx; idx + 1 < scui_mem.record[item.type].num; idx++) {
                scui_mem.record[item.type].item[idx] = scui_mem.record[item.type].item[idx + 1];
                if (scui_mem.record[item.type].item[idx + 1].ptr == NULL)
                    return true;
            }
            scui_mem.record[item.type].item[idx] = (scui_mem_record_item_t){0};
            return true;
        }
    
    return false;
}

/*@brief 内存记录分析
 *@param force 强制检查
 */
#if SCUI_MEM_RECORD_ANALYSIS
void scui_mem_record_analysis(bool force)
{
    bool update = force;
    for (uint32_t idx0 = scui_mem_type_none; idx0 < scui_mem_type_num; idx0++)
         update = update || scui_mem.record[idx0].update;
    if (!update)
         return;
    
    SCUI_LOG_INFO("scui mem record: --- <file,func,line,type,ptr,size>:");
    for (uint32_t idx0 = scui_mem_type_none; idx0 < scui_mem_type_num; idx0++) {
        if (!scui_mem.record[idx0].update && !force)
             continue;
             scui_mem.record[idx0].update = false;
        for (uint32_t idx1 = 0; idx1 < scui_mem.record[idx0].num; idx1++) {
            if (scui_mem.record[idx0].item[idx1].ptr == NULL)
                continue;
            SCUI_LOG_INFO("<%s,%s,%u,%u,%p,%u>",
                 scui_mem.record[idx0].item[idx1].file,
                 scui_mem.record[idx0].item[idx1].func,
                 scui_mem.record[idx0].item[idx1].line,
                 scui_mem.record[idx0].item[idx1].type,
                 scui_mem.record[idx0].item[idx1].ptr,
                 scui_mem.record[idx0].item[idx1].size);
            
            // 特殊问题特殊对待,查指定内容
            #if 1
            if (strcmp(scui_mem.record[idx0].item[idx1].func, "scui_clip_add") == 0) {
                scui_clip_unit_t *unit = scui_mem.record[idx0].item[idx1].ptr;
                SCUI_LOG_WARN("clip:%d-%d-%d-%d", unit->clip.x, unit->clip.y, unit->clip.w, unit->clip.h);
            }
            #endif
        }
    }
}
#endif

/*@brief 内存记录统计
 *@param force 强制检查
 */
#if SCUI_MEM_RECORD_STATISTIC
void scui_mem_record_statistic(bool force)
{
    bool update = force;
    for (uint32_t idx0 = scui_mem_type_none; idx0 < scui_mem_type_num; idx0++)
         update = update || scui_mem.record[idx0].update;
    if (!update)
         return;
    
    SCUI_LOG_INFO("scui mem record:");
    for (uint32_t idx0 = scui_mem_type_none; idx0 < scui_mem_type_num; idx0++) {
        if (!scui_mem.record[idx0].update && !force)
             continue;
             scui_mem.record[idx0].update = false;
        
        uint32_t size = 0;
        for (uint32_t idx1 = 0; idx1 < scui_mem.record[idx0].num; idx1++)
               if (scui_mem.record[idx0].item[idx1].ptr != NULL)
                   size += scui_mem.record[idx0].item[idx1].size;
        
        uint32_t total = scui_mem.record[idx0].size;
        SCUI_LOG_INFO("<type:%d, alloc:%u, free:%u, total:%u>", idx0, size, total - size, total);
    }
}
#endif

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
    
    scui_mutex_process(&scui_mem.mutex, scui_mutex_take);
    SCUI_ASSERT(type > scui_mem_type_none && type < scui_mem_type_num);
    ptr = app_sys_mem_olsf_alloc(scui_mem.mem_olsf[type], size);
    
    // notify回调:这里暂时写作就地调用.后续整理
    // 如果内存不够用,尝试清除缓存器后继续请求
    #if 1
    if (ptr == NULL) {
        // scui_mem_type_graph
        if (type == scui_mem_type_graph) {
            SCUI_LOG_WARN("memory graph deficit was caught");
            scui_mutex_process(&scui_mem.mutex, scui_mutex_give);
            
            // scui_image_cache_visit();
            scui_image_cache_clear();
            // scui_image_cache_visit();
            
            scui_mutex_process(&scui_mem.mutex, scui_mutex_take);
            ptr = app_sys_mem_olsf_alloc(scui_mem.mem_olsf[type], size);
            
            if (ptr == NULL)
                scui_image_cache_visit();
        }
        // scui_mem_type_font
        if (type == scui_mem_type_font) {
            SCUI_LOG_WARN("memory font deficit was caught");
            scui_mutex_process(&scui_mem.mutex, scui_mutex_give);
            
            // scui_font_glyph_cache_visit();
            scui_font_glyph_cache_clear();
            // scui_font_glyph_cache_visit();
            
            scui_mutex_process(&scui_mem.mutex, scui_mutex_take);
            ptr = app_sys_mem_olsf_alloc(scui_mem.mem_olsf[type], size);
            
            if (ptr == NULL)
                scui_font_glyph_cache_visit();
        }
    }
    #endif
    
    #if SCUI_MEM_RECORD_CHECK
    
    if (ptr == NULL) {
        scui_mem_check(type);
        
        #if SCUI_MEM_RECORD_STATISTIC
        scui_mem_record_statistic(true);
        #endif
        #if SCUI_MEM_RECORD_ANALYSIS
        scui_mem_record_analysis(true);
        #endif
    } else
    if (!scui_mem_record_item_add((scui_mem_record_item_t) {
        .file = file, .func = func, .line = line,
        .type = type, .ptr  = ptr,  .size = size,}))
         SCUI_LOG_WARN("record queue is full, item will be discard");
    
    #else
    
    if (ptr == NULL)
        scui_mem_check(type);
    
    #endif
    SCUI_ASSERT(ptr != NULL);
    
    scui_mutex_process(&scui_mem.mutex, scui_mutex_give);
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
    
    scui_mutex_process(&scui_mem.mutex, scui_mutex_take);
    scui_mem_type_t type = scui_mem_type_none;
    scui_mem_type(ptr, &type);
    
    SCUI_ASSERT(type > scui_mem_type_none && type < scui_mem_type_num);
    app_sys_mem_olsf_free(scui_mem.mem_olsf[type], ptr);
    
    #if SCUI_MEM_RECORD_CHECK
    
    if (!scui_mem_record_item_del((scui_mem_record_item_t){
        .type = type, .ptr = ptr,}))
         SCUI_LOG_WARN("record queue is not find, maybe discard");
    
    #endif
    
    scui_mutex_process(&scui_mem.mutex, scui_mutex_give);
}

/*@brief 内存类型检查
 *@param ptr  内存地址
 *@param type 内存类型
 */
void scui_mem_type(void *ptr, scui_mem_type_t *type)
{
    *type = scui_mem_type_none;
    
    for (uint32_t idx = scui_mem_type_none; idx < scui_mem_type_num; idx++)
        if (scui_mem.mem_olsf[idx] != NULL &&
            scui_mem.mem_olsf[idx]->addr_hdr <= (uintptr_t)ptr &&
            scui_mem.mem_olsf[idx]->addr_end >= (uintptr_t)ptr) {
            *type = idx;
             break;
        }
}

/*@brief 内存模组检查
 *@param type 内存类型
 */
void scui_mem_check(scui_mem_type_t type)
{
    if (type == scui_mem_type_mix)
        app_sys_mem_olsf_check(scui_mem.mem_olsf[scui_mem_type_mix]);
    if (type == scui_mem_type_font)
        app_sys_mem_olsf_check(scui_mem.mem_olsf[scui_mem_type_font]);
    if (type == scui_mem_type_graph)
        app_sys_mem_olsf_check(scui_mem.mem_olsf[scui_mem_type_graph]);
}

/*@brief 内存模组就绪
 */
void scui_mem_ready(void)
{
    scui_mutex_process(&scui_mem.mutex, scui_mutex_static);
    /* 这里使用自定义内存分配器, 用于查内存越界问题 */
    
    static uint8_t mem_olsf_buffer_mix[  SCUI_MEM_TYPE_SIZE_MIX]   = {0};
    static uint8_t mem_olsf_buffer_font[ SCUI_MEM_TYPE_SIZE_FONT]  = {0};
    static uint8_t mem_olsf_buffer_graph[SCUI_MEM_TYPE_SIZE_GRAPH] = {0};
    
    scui_mem.mem_olsf[scui_mem_type_mix]   = app_sys_mem_olsf_ready((void *)mem_olsf_buffer_mix,   SCUI_MEM_TYPE_SIZE_MIX);
    scui_mem.mem_olsf[scui_mem_type_font]  = app_sys_mem_olsf_ready((void *)mem_olsf_buffer_font,  SCUI_MEM_TYPE_SIZE_FONT);
    scui_mem.mem_olsf[scui_mem_type_graph] = app_sys_mem_olsf_ready((void *)mem_olsf_buffer_graph, SCUI_MEM_TYPE_SIZE_GRAPH);
    
    #if SCUI_MEM_RECORD_CHECK
    uint8_t *item = NULL;
    #if SCUI_MEM_RECORD_CHECK_MIX
    item = SCUI_MEM_ALLOC(scui_mem_type_mix,   sizeof(scui_mem_record_item_t) * SCUI_MEM_RECORD_ITEM_MIX);
    scui_mem_check(scui_mem_type_mix);
    scui_mem.record[scui_mem_type_mix].item    = item;
    scui_mem.record[scui_mem_type_mix].num     = SCUI_MEM_RECORD_ITEM_MIX;
    scui_mem.record[scui_mem_type_mix].size    = SCUI_MEM_TYPE_SIZE_MIX;
    #endif
    #if SCUI_MEM_RECORD_CHECK_FONT
    item = SCUI_MEM_ALLOC(scui_mem_type_font,  sizeof(scui_mem_record_item_t) * SCUI_MEM_RECORD_ITEM_FONT);
    scui_mem_check(scui_mem_type_font);
    scui_mem.record[scui_mem_type_font].item   = item;
    scui_mem.record[scui_mem_type_font].num    = SCUI_MEM_RECORD_ITEM_FONT;
    scui_mem.record[scui_mem_type_font].size   = SCUI_MEM_TYPE_SIZE_FONT;
    #endif
    #if SCUI_MEM_RECORD_CHECK_GRAPH
    item = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_mem_record_item_t) * SCUI_MEM_RECORD_ITEM_GRAPH);
    scui_mem_check(scui_mem_type_graph);
    scui_mem.record[scui_mem_type_graph].item  = item;
    scui_mem.record[scui_mem_type_graph].num   = SCUI_MEM_RECORD_ITEM_GRAPH;
    scui_mem.record[scui_mem_type_graph].size  = SCUI_MEM_TYPE_SIZE_GRAPH;
    #endif
    #endif
}
