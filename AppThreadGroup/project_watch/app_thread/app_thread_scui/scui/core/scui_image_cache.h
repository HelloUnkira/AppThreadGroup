#ifndef SCUI_IMAGE_CACHE_H
#define SCUI_IMAGE_CACHE_H

/* 内存图片节点信息项: */
typedef struct {
    app_sys_list_dln_t dl_node;     // 优先队列节点
    app_sys_table_rbsn_t ht_node;   // 哈希表节点
    scui_image_t *image;            // 图片数据
    uint8_t *data;                  // 图片数据源(内存地址)
    uint16_t count:7;
    uint16_t lock:7;
} scui_image_unit_t;

/* 内存图片节点信息表: */
typedef struct {
    app_sys_list_dll_t dl_list;
    app_sys_table_rbsl_t ht_list[SCUI_IMAGE_LIMIT_HASH];
    app_sys_table_rbst_t ht_table;
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} scui_image_cache_t;

/*@brief 图片资源缓存初始化
 */
void scui_image_cache_ready(void);

/*@brief 图片资源缓存清除
 */
void scui_image_cache_clear(void);

/*@brief 图片资源缓存卸载
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_unload(scui_image_unit_t *image_unit);

/*@brief 图片资源缓存加载
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_load(scui_image_unit_t *image_unit);

#endif