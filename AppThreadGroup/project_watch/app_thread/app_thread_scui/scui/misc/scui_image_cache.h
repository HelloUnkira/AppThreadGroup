#ifndef SCUI_IMAGE_CACHE_H
#define SCUI_IMAGE_CACHE_H

/*@brief 内存图片节点信息项
 */
typedef struct {
    scui_list_dln_t  dl_node;
    scui_table_dln_t ht_node;
    uint16_t count:7;           // 衰减计数器
    uint16_t lock:7;            // 嵌套锁计数器
    /* LRU缓存数据: */
    scui_image_t *image;        // 图片数据
    uint8_t *data;              // 图片数据源(内存地址)
} scui_image_unit_t;

/*@brief 内存图片节点信息表
 */
typedef struct {
    scui_list_dll_t  dl_list;
    scui_table_dll_t ht_list[SCUI_CACHE_HASH_IMAGE + 1];
    scui_table_dlt_t ht_table;
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} scui_image_cache_t;

/*@brief 图片资源缓存初始化
 */
void scui_image_cache_ready(void);

/*@brief 图片资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_image_cache_rectify(void);

/*@brief 图片资源检查
 */
void scui_image_cache_visit(void);

/*@brief 图片资源缓存清除
 */
void scui_image_cache_clear(void);

/*@brief 图片资源缓存无效化(指定目标)
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_invalidate(scui_image_unit_t *image_unit);

/*@brief 图片资源缓存卸载
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_unload(scui_image_unit_t *image_unit);

/*@brief 图片资源缓存加载
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_load(scui_image_unit_t *image_unit);

#endif
