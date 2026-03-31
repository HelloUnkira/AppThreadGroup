#ifndef SCUI_FONT_CACHE_H
#define SCUI_FONT_CACHE_H

/*@brief 内存字库节点信息项
 */
typedef struct {
    scui_cache_lru_unit_t lru_unit;
    
    /* LRU缓存数据: */
    uint16_t         size;     /* 字库尺寸信息 */
    scui_handle_t    name;     /* 字库名字句柄 */
    scui_handle_t    font;     /* 字库信息句柄 */
} scui_font_unit_t;

/*@brief 内存文字节点信息项
 */
typedef struct {
    scui_cache_lru_unit_t lru_unit;
    
    /* LRU缓存数据: */
    uint16_t          size;     /* 字库尺寸信息 */
    scui_handle_t     name;     /* 字库名字句柄 */
    scui_handle_t     font;     /* 字库信息句柄 */
    scui_font_glyph_t glyph;    /* 文字数据 */
} scui_font_glyph_unit_t;

/*@brief 内存字库节点信息表
 */
typedef struct {
    scui_cache_lru_table_t lru_table;
    
} scui_font_cache_t;

/*@brief 内存文字节点信息表
 */
typedef struct {
    scui_cache_lru_table_t lru_table;
    
} scui_font_glyph_cache_t;

/*@brief 字库文字资源缓存初始化
 */
void scui_font_cache_ready(void);
void scui_font_glyph_cache_ready(void);

/*@brief 字库文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_cache_rectify(void);
void scui_font_glyph_cache_rectify(void);

/*@brief 字库文字资源检查
 */
void scui_font_cache_visit(void);
void scui_font_glyph_cache_visit(void);

/*@brief 字库文字资源使用
 *@param usage 字库文字资源使用
 */
void scui_font_cache_usage(uint32_t *usage);
void scui_font_glyph_cache_usage(uint32_t *usage);

/*@brief 字库文字资源数量
 *@param nodes 字库文字资源数量
 */
void scui_font_cache_nodes(uint32_t *nodes);
void scui_font_glyph_cache_nodes(uint32_t *nodes);

/*@brief 字库文字资源缓存清除
 */
void scui_font_cache_clear(void);
void scui_font_glyph_cache_clear(void);

/*@brief 字库文字资源缓存卸载
 *@brief font_unit 字库文字资源缓存节点
 */
void scui_font_cache_unload(scui_font_unit_t *font_unit);
void scui_font_glyph_cache_unload(scui_font_glyph_unit_t *font_unit);

/*@brief 字库文字资源缓存加载
 *@brief font_unit 字库文字资源缓存节点
 */
void scui_font_cache_load(scui_font_unit_t *font_unit);
void scui_font_glyph_cache_load(scui_font_glyph_unit_t *font_unit);

#endif
