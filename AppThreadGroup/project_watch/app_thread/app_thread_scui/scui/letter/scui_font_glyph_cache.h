#ifndef SCUI_FONT_GLYPH_CACHE_H
#define SCUI_FONT_GLYPH_CACHE_H

/*@brief 内存文字节点信息项
 */
typedef struct {
    scui_list_dln_t   dl_node;
    scui_table_rbsn_t ht_node;
    uint16_t          count:7;  // 衰减计数器
    uint16_t          lock:7;   // 嵌套锁计数器
    /* LRU缓存数据: */
    scui_handle_t     name;     // 字库名字句柄
    scui_handle_t     font;     // 字库信息句柄
    scui_font_glyph_t glyph;    // 文字数据
} scui_font_glyph_unit_t;

/*@brief 内存文字节点信息表
 */
typedef struct {
    scui_list_dll_t   dl_list;
    scui_table_rbsl_t ht_list[SCUI_FONT_GLYPH_LIMIT_HASH];
    scui_table_rbst_t ht_table;
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} scui_font_glyph_cache_t;

/*@brief 文字资源缓存初始化
 */
void scui_font_glyph_cache_ready(void);

/*@brief 文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_glyph_cache_rectify(void);

/*@brief 文字资源检查
 */
void scui_font_glyph_cache_visit(void);

/*@brief 文字资源缓存清除
 */
void scui_font_glyph_cache_clear(void);

/*@brief 文字资源缓存卸载
 *@brief font_unit 文字资源缓存节点
 */
void scui_font_glyph_cache_unload(scui_font_glyph_unit_t *font_unit);

/*@brief 文字资源缓存加载
 *@brief font_unit 文字资源缓存节点
 */
void scui_font_glyph_cache_load(scui_font_glyph_unit_t *font_unit);

#endif
