#ifndef APP_SYS_EXT_MEM_CACHE_H
#define APP_SYS_EXT_MEM_CACHE_H

typedef enum {
    app_sys_ext_mem_cache_none = 0,
    app_sys_ext_mem_cache_overflow,     /* 获取的资源超出缓存单元门限 */
    app_sys_ext_mem_cache_hit,          /* 缓存命中 */
    app_sys_ext_mem_cache_unhit,        /* 缓存未命中 */
    app_sys_ext_mem_cache_overlay,
} app_sys_ext_mem_cache_status_t;

typedef struct {
    app_sys_list_dn_t dl_node;
    app_sys_table_dn_t ht_node;
    uintptr_t   offset;
    uint8_t    *buffer;
    uintptr_t   size;
    uint8_t     count:7;
    uint8_t     dirty:1;
    uint8_t     lock:7;
} app_sys_ext_mem_cache_unit_t;

typedef struct {
    app_mutex_t mutex;
    app_sys_list_dl_t dl_list;
    app_sys_table_dl_t *ht_list;
    app_sys_table_dt_t  ht_table;
    uint32_t ht_list_num;
    const app_sys_ext_mem_t *ext_mem;
    uint32_t unit;      /* 缓存对内存资源使用单元门限 */
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} app_sys_ext_mem_cache_t;

/*@brief     缓存就绪,配置参数
 *@param[in] cache 缓存实例
 *@param[in] ext_mem 外存静态实例
 *@param[in] unit  缓存单元门限
 *@param[in] total 缓存总门限
 */
void app_sys_ext_mem_cache_ready(app_sys_ext_mem_cache_t *cache, const app_sys_ext_mem_t *ext_mem, uint32_t unit, uint32_t total);

/*@brief     缓存刷新(清理内存)
 *@param[in] cache 缓存实例
 *@param[in] force 强制刷新模式(仅在完全不使用它时使用)
 */
void app_sys_ext_mem_cache_reflush(app_sys_ext_mem_cache_t *cache, bool force);

/*@brief      缓存资源获取
 *@param[in]  cache  缓存实例
 *@param[in]  offset 外存数据偏移
 *@param[in]  size   外存数据大小
 *@param[out] buffer 内存数据地址
 *@retval     操作结果
 */
uint32_t app_sys_ext_mem_cache_take(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uintptr_t size, uint8_t **buffer);

/*@brief     缓存资源获取
 *@param[in] cache  缓存实例
 *@param[in] offset 外存数据偏移
 *@param[in] buffer 内存数据地址
 *@param[in] dirty  内存数据是否修改
 *@retval    操作结果
 */
uint32_t app_sys_ext_mem_cache_give(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uint8_t *buffer, bool dirty);

#endif
