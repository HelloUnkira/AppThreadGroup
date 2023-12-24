#ifndef APP_SYS_EXT_MEM_CACHE_H
#define APP_SYS_EXT_MEM_CACHE_H

/* DL:      双链表容器为载体的散列表 */
/* RBS:     红黑树容器为载体的散列表 */
#define APP_SYS_EXT_MEM_CACHE_USE_TABLE_DL      0
#define APP_SYS_EXT_MEM_CACHE_USE_TABLE_RBS     1

typedef enum {
    app_sys_ext_mem_cache_none = 0,
    app_sys_ext_mem_cache_overflow,     /* 获取的资源超出缓存单元门限 */
    app_sys_ext_mem_cache_hit,          /* 缓存命中 */
    app_sys_ext_mem_cache_unhit,        /* 缓存未命中 */
    app_sys_ext_mem_cache_overlay,
} app_sys_ext_mem_cache_status_t;

typedef struct {
    app_sys_list_dln_t dl_node;
    #if 0
    #elif APP_SYS_EXT_MEM_CACHE_USE_TABLE_DL
    app_sys_table_dln_t ht_node;
    #elif APP_SYS_EXT_MEM_CACHE_USE_TABLE_RBS
    app_sys_table_rbsn_t ht_node;
    #else
    #endif
    uintptr_t   offset;
    uint8_t    *buffer;
    uintptr_t   size;
    uint16_t    count:7;
    uint16_t    dirty:1;
    uint16_t    lock:7;
} app_sys_ext_mem_cache_unit_t;

typedef struct {
    app_mutex_t mutex;
    app_sys_list_dll_t dl_list;
    #if 0
    #elif APP_SYS_EXT_MEM_CACHE_USE_TABLE_DL
    app_sys_table_dll_t *ht_list;
    app_sys_table_dlt_t  ht_table;
    #elif APP_SYS_EXT_MEM_CACHE_USE_TABLE_RBS
    app_sys_table_rbsl_t *ht_list;
    app_sys_table_rbst_t  ht_table;
    #else
    #endif
    uint32_t ht_list_num;
    const app_sys_ext_mem_t *ext_mem;
    uint32_t unit;      /* 缓存对内存资源使用单元门限 */
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} app_sys_ext_mem_cache_t;

/*@brief 缓存就绪,配置参数
 *@param cache 缓存实例
 *@param ext_mem 外存静态实例
 *@param unit  缓存单元门限
 *@param total 缓存总门限
 */
void app_sys_ext_mem_cache_ready(app_sys_ext_mem_cache_t *cache, const app_sys_ext_mem_t *ext_mem, uint32_t unit, uint32_t total);

/*@brief 缓存刷新
 *@param cache 缓存实例
 */
void app_sys_ext_mem_cache_reflush(app_sys_ext_mem_cache_t *cache);

/*@brief 缓存回收(清理内存)
 *@param cache 缓存实例
 *@param force 强制回收模式(仅在完全不使用它时使用)
 */
void app_sys_ext_mem_cache_recycle(app_sys_ext_mem_cache_t *cache, bool force);

/*@brief 缓存资源获取
 *@param cache  缓存实例
 *@param offset 外存数据偏移
 *@param size   外存数据大小
 *@param buffer 内存数据地址
 *@retval 操作结果
 */
uint32_t app_sys_ext_mem_cache_take(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uintptr_t size, uint8_t **buffer);

/*@brief 缓存资源获取
 *@param cache  缓存实例
 *@param offset 外存数据偏移
 *@param buffer 内存数据地址
 *@param dirty  内存数据是否修改
 *@retval 操作结果
 */
uint32_t app_sys_ext_mem_cache_give(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uint8_t *buffer, bool dirty);

#endif
