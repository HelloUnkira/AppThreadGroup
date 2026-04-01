#ifndef SCUI_CACHE_GLYPH_H
#define SCUI_CACHE_GLYPH_H

/*@brief 内存文字节点信息项
 */
typedef struct {
    scui_cache_lru_unit_t lru_unit;
    
    /* LRU缓存数据: */
    uint16_t          size;     /* 字库尺寸信息 */
    scui_handle_t     name;     /* 字库名字句柄 */
    scui_handle_t     font;     /* 字库信息句柄 */
    scui_font_glyph_t glyph;    /* 文字数据 */
} scui_cache_glyph_unit_t;

/*@brief 内存文字节点信息表
 */
typedef struct {
    scui_cache_lru_table_t lru_table;
    
} scui_cache_glyph_t;

/*@brief 文字资源缓存初始化
 */
void scui_cache_glyph_ready(void);

/*@brief 文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_cache_glyph_rectify(void);

/*@brief 文字资源检查
 */
void scui_cache_glyph_visit(void);

/*@brief 文字资源使用
 *@param usage 文字资源使用
 */
void scui_cache_glyph_usage(uint32_t *usage);

/*@brief 文字资源数量
 *@param nodes 文字资源数量
 */
void scui_cache_glyph_nodes(uint32_t *nodes);

/*@brief 文字资源缓存清除
 */
void scui_cache_glyph_clear(void);

/*@brief 文字资源缓存卸载
 *@brief font_unit 文字资源缓存节点
 */
void scui_cache_glyph_unload(scui_cache_glyph_unit_t *font_unit);

/*@brief 文字资源缓存加载
 *@brief font_unit 文字资源缓存节点
 */
void scui_cache_glyph_load(scui_cache_glyph_unit_t *font_unit);

#endif
