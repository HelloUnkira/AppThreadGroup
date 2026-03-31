#ifndef SCUI_CACHE_LRU_H
#define SCUI_CACHE_LRU_H

/*@brief 节点信息项
 */
typedef struct {
    scui_list_dln_t  dl_node;
    scui_table_dln_t ht_node;
    scui_sbitfd_t count:16; /* 衰减计数器 */
    scui_sbitfd_t lock:16;  /* 嵌套锁计数器 */
} scui_cache_lru_unit_t;

/*@brief 节点信息表
 */
typedef struct {
    scui_list_dll_t  dl_list;
    scui_table_dll_t ht_list[SCUI_CACHE_LRU_HASH_LIMIT];
    scui_table_dlt_t ht_table;
    scui_multi_t nodes;     /* 缓存单位数量 */
    scui_multi_t usage;     /* 缓存对内存资源占用情况 */
    scui_multi_t total;     /* 缓存对内存资源占用总门限 */
    scui_multi_t cnt_hit;   /* 命中次数 */
    scui_multi_t cnt_unhit; /* 非命中次数 */
    /* 表资源回调: */
    scui_table_dlt_fd_t dlt_fd;
    scui_table_dlt_fc_t dlt_fc;
    scui_table_dlt_fv_t dlt_fv;
    uint32_t ht_list_num;
    /* 节点资源回调 */
    scui_multi_t (*get_size)(scui_cache_lru_unit_t *lru_unit);
    void (*new_node)(scui_cache_lru_unit_t **lru_unit, scui_cache_lru_unit_t *lru_unit_t);
    void (*cpy_node)(scui_cache_lru_unit_t  *lru_unit, scui_cache_lru_unit_t *lru_unit_t);
    void (*del_node)(scui_cache_lru_unit_t  *lru_unit);
} scui_cache_lru_table_t;

/*@brief 摘要的来源网络的Hash散列函数
 *@param data 数据流
 *@param len  数据字节
 */
uint32_t scui_cache_lru_hash(uint8_t *data, uint32_t len);

/*@brief 资源缓存器就绪
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_ready(scui_cache_lru_table_t *lru_table);

/*@brief 资源重校正(计数器重衰减到0)
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_rectify(scui_cache_lru_table_t *lru_table);

/*@brief 资源检查
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_visit(scui_cache_lru_table_t *lru_table);

/*@brief 资源使用
 *@param lru_table 资源缓存表
 *@param usage     资源使用
 */
void scui_cache_lru_usage(scui_cache_lru_table_t *lru_table, uint32_t *usage);

/*@brief 资源数量
 *@param lru_table 资源缓存表
 *@param nodes     资源数量
 */
void scui_cache_lru_nodes(scui_cache_lru_table_t *lru_table, uint32_t *nodes);

/*@brief 资源缓存清除
 *@param lru_table 资源缓存表
 *@param force     强制清除
 */
void scui_cache_lru_clear(scui_cache_lru_table_t *lru_table, bool force);

/*@brief 资源缓存无效化(指定目标)
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_invalidate(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit);

/*@brief 资源缓存卸载
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_unload(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit);

/*@brief 资源缓存加载
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_load(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit);

#endif
