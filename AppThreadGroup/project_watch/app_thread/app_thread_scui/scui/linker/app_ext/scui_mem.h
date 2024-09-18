#ifndef SCUI_MEM_H
#define SCUI_MEM_H

/* 内存分配类型:
 * 1.默认类型
 * 特点:碎片化,不稳定生存期,大小不定
 * 建议:使用常规内存分配策略
 * 2.字体类型
 * 特点:碎片化,不稳定生存期,大小稳定
 * 建议:可使用默认类型同一分配器
 *      也可单独开设分配器
 * 3.图形类型
 * 特点:块状化,稳定生存期,大小不限
 * 建议:单独为其开设分配器,唯一化分配
 */

typedef enum {
    scui_mem_type_none = 0,
    scui_mem_type_mix,
    scui_mem_type_font,
    scui_mem_type_graph,
    scui_mem_type_num,
} scui_mem_type_t;

#if SCUI_MEM_RECORD_CHECK

typedef struct {
    scui_mem_type_t type;
    const char *file;
    const char *func;
    uint32_t    line;
    uint32_t    size;
    void *ptr;
} scui_mem_record_item_t;

typedef struct {
    scui_mem_record_item_t *item;
    uint32_t                num;
    uint32_t                size;
    bool                    update;
} scui_mem_record_t;


/*@brief 内存记录分析
 *@param force 强制检查
 */
#if SCUI_MEM_RECORD_ANALYSIS
void scui_mem_record_analysis(bool force);
#endif

/*@brief 内存记录统计
 *@param force 强制检查
 */
#if SCUI_MEM_RECORD_STATISTIC
void scui_mem_record_statistic(bool force);
#endif

#endif

typedef struct {
    scui_mutex_t mutex;
    uintptr_t    size_total[scui_mem_type_num];
    uintptr_t    size_used[scui_mem_type_num];
    
    app_sys_mem_olsf_t *mem_olsf[scui_mem_type_num];
    #if SCUI_MEM_RECORD_CHECK
    scui_mem_record_t   record[scui_mem_type_num];
    #endif
} scui_mem_t;

/* 宏转接,外部使用接口 */
#define SCUI_MEM_ALLOC(type, size)  scui_mem_alloc(__FILE__, __func__, __LINE__, type, size)
#define SCUI_MEM_FREE(ptr)          scui_mem_free( __FILE__, __func__, __LINE__, ptr)

/*@brief 内存分配(外部不直接调用)
 *@param file 内存分配点(文件名)
 *@param line 内存分配点(文件行数)
 *@param func 内存分配点(函数名称)
 *@param type 内存分配类型
 *@param size 内存大小
 *@retval 内存地址
 */
void * scui_mem_alloc(const char *file, const char *func, uint32_t line, scui_mem_type_t type, uint32_t size);

/*@brief 内存释放(外部不直接调用)
 *@param file 内存分配点(文件名)
 *@param line 内存分配点(文件行数)
 *@param func 内存分配点(函数名称)
 *@param ptr  内存地址
 */
void scui_mem_free(const char *file, const char *func, uint32_t line, void *ptr);

/*@brief 内存类型检查
 *@param ptr  内存地址
 *@param type 内存类型
 */
void scui_mem_type(void *ptr, scui_mem_type_t *type);

/*@brief 内存模组统计(消耗值)
 *@param type 内存类型
 *@retval 内存大小
 */
uint32_t scui_mem_size_used(scui_mem_type_t type);

/*@brief 内存模组统计(总计值)
 *@param type 内存类型
 *@retval 内存大小
 */
uint32_t scui_mem_size_total(scui_mem_type_t type);

/*@brief 内存模组检查
 *@param type 内存类型
 */
void scui_mem_check(scui_mem_type_t type);

/*@brief 内存模组就绪
 */
void scui_mem_ready(void);

#endif
