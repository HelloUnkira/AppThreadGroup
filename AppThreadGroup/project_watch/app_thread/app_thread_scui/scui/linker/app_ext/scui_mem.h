#ifndef SCUI_MEM_H
#define SCUI_MEM_H

/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK       (0)
#define SCUI_MEM_RECORD_ITEM_DEF    (128)
#define SCUI_MEM_RECORD_ITEM_TEXT   (256)
#define SCUI_MEM_RECORD_ITEM_GRAPH  (64)

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
    scui_mem_type_def  = scui_mem_type_none,
    scui_mem_type_text,
    scui_mem_type_graph,
    scui_mem_num,
} scui_mem_type_t;

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

/*@brief 内存模组就绪
 */
void scui_mem_ready(void);

/* 内存监控使用 */
#if SCUI_MEM_RECORD_CHECK

typedef struct {
    scui_mem_type_t type;
    const char *file;
    const char *func;
    uint32_t    line;
    uint32_t size;
    void *ptr;
} scui_mem_record_item_t;

/*@brief 内存记录检查
 */
void scui_mem_record_check(void);

#endif

#endif
