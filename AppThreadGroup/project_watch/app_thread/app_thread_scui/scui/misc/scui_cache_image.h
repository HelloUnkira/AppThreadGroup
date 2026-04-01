#ifndef SCUI_CACHE_IMAGE_H
#define SCUI_CACHE_IMAGE_H

/*@brief 内存图片节点信息项
 */
typedef struct {
    scui_cache_lru_unit_t lru_unit;
    
    /* LRU缓存数据: */
    scui_image_t *image;        /* 图片数据 */
    uint8_t *data;              /* 图片数据源(内存地址) */
} scui_cache_image_unit_t;

/*@brief 内存图片节点信息表
 */
typedef struct {
    scui_cache_lru_table_t lru_table;
    
} scui_cache_image_t;

/*@brief 图片资源缓存初始化
 */
void scui_cache_image_ready(void);

/*@brief 图片资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_cache_image_rectify(void);

/*@brief 图片资源检查
 */
void scui_cache_image_visit(void);

/*@brief 图片资源使用
 *@param usage 图片资源使用
 */
void scui_cache_image_usage(uint32_t *usage);

/*@brief 图片资源数量
 *@param nodes 图片资源数量
 */
void scui_cache_image_nodes(uint32_t *nodes);

/*@brief 图片资源缓存清除
 */
void scui_cache_image_clear(void);

/*@brief 图片资源缓存无效化(指定目标)
 *@brief image_unit 图片资源缓存节点
 */
void scui_cache_image_invalidate(scui_cache_image_unit_t *image_unit);

/*@brief 图片资源缓存卸载
 *@brief image_unit 图片资源缓存节点
 */
void scui_cache_image_unload(scui_cache_image_unit_t *image_unit);

/*@brief 图片资源缓存加载
 *@brief image_unit 图片资源缓存节点
 */
void scui_cache_image_load(scui_cache_image_unit_t *image_unit);

#endif
