#ifndef SCUI_CACHE_FONT_H
#define SCUI_CACHE_FONT_H

/*@brief 内存字库节点信息项
 */
typedef struct {
    scui_cache_lru_unit_t lru_unit;
    
    /* LRU缓存数据: */
    uint16_t         size;     /* 字库尺寸信息 */
    scui_handle_t    name;     /* 字库名字句柄 */
    scui_handle_t    font;     /* 字库信息句柄 */
} scui_cache_font_unit_t;

/*@brief 内存字库节点信息表
 */
typedef struct {
    scui_cache_lru_table_t lru_table;
    
} scui_cache_font_t;

/*@brief 字库资源缓存初始化
 */
void scui_cache_font_ready(void);

/*@brief 字库资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_cache_font_rectify(void);

/*@brief 字库资源检查
 */
void scui_cache_font_visit(void);

/*@brief 字库资源使用
 *@param usage 字库资源使用
 */
void scui_cache_font_usage(uint32_t *usage);

/*@brief 字库资源数量
 *@param nodes 字库资源数量
 */
void scui_cache_font_nodes(uint32_t *nodes);

/*@brief 字库资源缓存清除
 */
void scui_cache_font_clear(void);

/*@brief 字库资源缓存卸载
 *@brief font_unit 字库资源缓存节点
 */
void scui_cache_font_unload(scui_cache_font_unit_t *font_unit);

/*@brief 字库资源缓存加载
 *@brief font_unit 字库资源缓存节点
 */
void scui_cache_font_load(scui_cache_font_unit_t *font_unit);

#endif
