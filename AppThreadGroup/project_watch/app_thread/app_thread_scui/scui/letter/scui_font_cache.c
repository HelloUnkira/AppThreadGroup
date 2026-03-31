/*实现目标:
 *    字库资源缓存
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_font_cache_t scui_font_cache = {0};
static scui_font_glyph_cache_t scui_font_glyph_cache = {0};

/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t scui_font_cache_fd_t(scui_table_dln_t *node)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_font_unit_t *font_unit = scui_own_ofs(scui_font_unit_t, lru_unit, unit);
    
    return scui_cache_lru_hash(&font_unit->name, sizeof(scui_handle_t));
}

/*@brief 哈希比较函数
 */
static bool scui_font_cache_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_cache_lru_unit_t *unit1 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node1);
    scui_cache_lru_unit_t *unit2 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node2);
    scui_font_unit_t *font_unit1 = scui_own_ofs(scui_font_unit_t, lru_unit, unit1);
    scui_font_unit_t *font_unit2 = scui_own_ofs(scui_font_unit_t, lru_unit, unit2);
    
    scui_handle_t name1 = font_unit1->name;
    scui_handle_t name2 = font_unit2->name;
    uint16_t size1 = font_unit1->size;
    uint16_t size2 = font_unit2->size;
    
    return name1 == name2 && size1 == size2 ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_font_cache_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_font_unit_t *font_unit = scui_own_ofs(scui_font_unit_t, lru_unit, unit);
    
    SCUI_LOG_INFO("- name:%s",  scui_handle_source(font_unit->name));
    SCUI_LOG_INFO("- size:%u",  font_unit->size);
    SCUI_LOG_INFO("- font:%x",  font_unit->font);
    SCUI_LOG_INFO("- count:%x", unit->count);
    SCUI_LOG_INFO("- lock:%x",  unit->lock);
}

/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t scui_font_glyph_cache_fd_t(scui_table_dln_t *node)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_font_glyph_unit_t *glyph_unit = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit);
    
    uint32_t letter = glyph_unit->glyph.unicode_letter;
    return scui_cache_lru_hash((void *)&letter, sizeof(uint32_t));
}

/*@brief 哈希比较函数
 */
static uint8_t scui_font_glyph_cache_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_cache_lru_unit_t *unit1 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node1);
    scui_cache_lru_unit_t *unit2 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node2);
    scui_font_glyph_unit_t *glyph_unit1 = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit1);
    scui_font_glyph_unit_t *glyph_unit2 = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit2);
    
    scui_handle_t name1 = glyph_unit1->name;
    scui_handle_t name2 = glyph_unit2->name;
    uint32_t letter1 = glyph_unit1->glyph.unicode_letter;
    uint32_t letter2 = glyph_unit2->glyph.unicode_letter;
    uint16_t size1 = glyph_unit1->size;
    uint16_t size2 = glyph_unit2->size;
    
    return name1 == name2 && letter1 == letter2 && size1 == size2 ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_font_glyph_cache_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_font_glyph_unit_t *glyph_unit = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit);
    
    SCUI_LOG_INFO("- name:%s",   scui_handle_source(glyph_unit->name));
    SCUI_LOG_INFO("- size:%u",   glyph_unit->size);
    SCUI_LOG_INFO("- letter:%x", glyph_unit->glyph.unicode_letter);
    SCUI_LOG_INFO("- count:%x",  unit->count);
    SCUI_LOG_INFO("- lock:%x",   unit->lock);
}

/*@brief 节点占用回调
 */
static scui_multi_t scui_font_unit_get_size(scui_cache_lru_unit_t *unit)
{
    scui_font_unit_t *font_unit = scui_own_ofs(scui_font_unit_t, lru_unit, unit);
    return scui_font_size(font_unit->font);
}

/*@brief 节点销毁回调
 */
static void scui_font_unit_new_node(scui_cache_lru_unit_t **unit, scui_cache_lru_unit_t *unit_t)
{
    scui_font_unit_t *font_unit_t = scui_own_ofs(scui_font_unit_t, lru_unit, unit_t);
    scui_font_unit_t *font_unit = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_font_unit_t));
    font_unit->size = font_unit_t->size;
    font_unit->name = font_unit_t->name;
    font_unit->font = font_unit_t->font;
    /* 字库资源加载 */
    char *name = scui_handle_source_check(font_unit->name);
    scui_font_load(name, font_unit->size, &font_unit->font);
    SCUI_ASSERT(scui_handle_source(font_unit->font) != NULL);
    
    *unit = &font_unit->lru_unit;
}

/*@brief 节点销毁回调
 */
static void scui_font_unit_cpy_node(scui_cache_lru_unit_t *unit, scui_cache_lru_unit_t *unit_t)
{
    scui_font_unit_t *font_unit_t = scui_own_ofs(scui_font_unit_t, lru_unit, unit_t);
    scui_font_unit_t *font_unit   = scui_own_ofs(scui_font_unit_t, lru_unit, unit);
    *font_unit = *font_unit_t;
}

/*@brief 节点创建回调
 */
static void scui_font_unit_del_node(scui_cache_lru_unit_t  *unit)
{
    scui_font_unit_t *font_unit = scui_own_ofs(scui_font_unit_t, lru_unit, unit);
    /* 字库资源卸载 */
    scui_font_unload(font_unit->font);
    SCUI_MEM_FREE(font_unit);
}

/*@brief 节点占用回调
 */
static scui_multi_t scui_font_glyph_unit_get_size(scui_cache_lru_unit_t *unit)
{
    scui_font_glyph_unit_t *glyph_unit = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit);
    return glyph_unit->glyph.bitmap_size;
}

/*@brief 节点销毁回调
 */
static void scui_font_glyph_unit_new_node(scui_cache_lru_unit_t **unit, scui_cache_lru_unit_t *unit_t)
{
    scui_font_glyph_unit_t *glyph_unit_t = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit_t);
    scui_font_glyph_unit_t *glyph_unit   = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_font_glyph_unit_t));
    glyph_unit->size  = glyph_unit_t->size;
    glyph_unit->name  = glyph_unit_t->name;
    glyph_unit->font  = glyph_unit_t->font;
    glyph_unit->glyph = glyph_unit_t->glyph;
    
    /* 文字资源加载 */
    scui_font_unit_t font_unit = {0};
    font_unit.size = glyph_unit->size;
    font_unit.name = glyph_unit->name;
    scui_font_cache_load(&font_unit);
    glyph_unit->font = font_unit.font;
    glyph_unit->glyph.handle = font_unit.font;
    scui_font_glyph_load(&glyph_unit->glyph);
    scui_font_cache_unload(&font_unit);
    
    *unit = &glyph_unit->lru_unit;
}

/*@brief 节点销毁回调
 */
static void scui_font_glyph_unit_cpy_node(scui_cache_lru_unit_t *unit, scui_cache_lru_unit_t *unit_t)
{
    scui_font_glyph_unit_t *glyph_unit_t = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit_t);
    scui_font_glyph_unit_t *glyph_unit   = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit);
    *glyph_unit = *glyph_unit_t;
}

/*@brief 节点创建回调
 */
static void scui_font_glyph_unit_del_node(scui_cache_lru_unit_t  *unit)
{
    scui_font_glyph_unit_t *glyph_unit = scui_own_ofs(scui_font_glyph_unit_t, lru_unit, unit);
    /* 文字资源卸载 */
    scui_font_glyph_unload(&glyph_unit->glyph);
    SCUI_MEM_FREE(glyph_unit);
}

/*@brief 字库初始化
 */
void scui_font_cache_ready(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    
    cache->lru_table.dlt_fd = scui_font_cache_fd_t;
    cache->lru_table.dlt_fc = scui_font_cache_fc_t;
    cache->lru_table.dlt_fv = scui_font_cache_fv_t;
    cache->lru_table.ht_list_num = SCUI_CACHE_HASH_FONT;
    cache->lru_table.total = SCUI_CACHE_TOTAL_FONT;
    cache->lru_table.get_size = scui_font_unit_get_size;
    cache->lru_table.new_node = scui_font_unit_new_node;
    cache->lru_table.cpy_node = scui_font_unit_cpy_node;
    cache->lru_table.del_node = scui_font_unit_del_node;
    scui_cache_lru_ready(&cache->lru_table);
    #endif
}

/*@brief 字库资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_cache_rectify(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_rectify(&cache->lru_table);
    #endif
}

/*@brief 字库资源检查
 */
void scui_font_cache_visit(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_visit(&cache->lru_table);
    #endif
}

/*@brief 字库资源使用
 *@param usage 字库资源使用
 */
void scui_font_cache_usage(uint32_t *usage)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_usage(&cache->lru_table, usage);
    #endif
}

/*@brief 字库资源数量
 *@param nodes 字库资源数量
 */
void scui_font_cache_nodes(uint32_t *nodes)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_nodes(&cache->lru_table, nodes);
    #endif
}

/*@brief 字库资源清除
 */
void scui_font_cache_clear(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_clear(&cache->lru_table, false);
    #endif
}

/*@brief 字库资源卸载
 */
void scui_font_cache_unload(scui_font_unit_t *font_unit)
{
    SCUI_ASSERT(font_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_unload(&cache->lru_table, &font_unit->lru_unit);
    #else
    /* 卸载字库资源 */
    scui_font_unload(font_unit->font);
    #endif
}

/*@brief 字库资源加载
 */
void scui_font_cache_load(scui_font_unit_t *font_unit)
{
    SCUI_ASSERT(font_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT != 0
    scui_font_cache_t *cache = &scui_font_cache;
    scui_cache_lru_load(&cache->lru_table, &font_unit->lru_unit);
    #else
    
    /* 加载字库资源 */
    char *name = scui_handle_source_check(font_unit->name);
    scui_font_load(name, font_unit->size, &font_unit->font);
    SCUI_ASSERT(scui_handle_source(font_unit->font) != NULL);
    #endif
}

/*@brief 文字初始化
 */
void scui_font_glyph_cache_ready(void)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    cache->lru_table.dlt_fd = scui_font_glyph_cache_fd_t;
    cache->lru_table.dlt_fc = scui_font_glyph_cache_fc_t;
    cache->lru_table.dlt_fv = scui_font_glyph_cache_fv_t;
    cache->lru_table.ht_list_num = SCUI_CACHE_HASH_FONT_GLYPH;
    cache->lru_table.total = SCUI_CACHE_TOTAL_FONT_GLYPH;
    cache->lru_table.get_size = scui_font_glyph_unit_get_size;
    cache->lru_table.new_node = scui_font_glyph_unit_new_node;
    cache->lru_table.cpy_node = scui_font_glyph_unit_cpy_node;
    cache->lru_table.del_node = scui_font_glyph_unit_del_node;
    scui_cache_lru_ready(&cache->lru_table);
    #endif
}

/*@brief 文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_glyph_cache_rectify(void)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_rectify(&cache->lru_table);
    #endif
}

/*@brief 文字资源检查
 */
void scui_font_glyph_cache_visit(void)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_visit(&cache->lru_table);
    #endif
}

/*@brief 文字资源使用
 *@param usage 文字资源使用
 */
void scui_font_glyph_cache_usage(uint32_t *usage)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_usage(&cache->lru_table, usage);
    #endif
}

/*@brief 文字资源数量
 *@param nodes 文字资源数量
 */
void scui_font_glyph_cache_nodes(uint32_t *nodes)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_nodes(&cache->lru_table, nodes);
    #endif
}

/*@brief 文字资源清除
 */
void scui_font_glyph_cache_clear(void)
{
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_clear(&cache->lru_table, false);
    #endif
}

/*@brief 文字资源卸载
 */
void scui_font_glyph_cache_unload(scui_font_glyph_unit_t *glyph_unit)
{
    SCUI_ASSERT(glyph_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_unload(&cache->lru_table, &glyph_unit->lru_unit);
    #else
    /* 卸载文字资源 */
    scui_font_glyph_unload(&glyph_unit->glyph);
    #endif
}

/*@brief 文字资源加载
 */
void scui_font_glyph_cache_load(scui_font_glyph_unit_t *glyph_unit)
{
    SCUI_ASSERT(glyph_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT_GLYPH != 0
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    scui_cache_lru_load(&cache->lru_table, &glyph_unit->lru_unit);
    #else
    /* 加载文字资源 */
    scui_font_unit_t font_unit = {0};
    font_unit.size = glyph_unit->size;
    font_unit.name = glyph_unit->name;
    scui_font_cache_load(&font_unit);
    glyph_unit->font = font_unit.font;
    glyph_unit->glyph.handle = font_unit.font;
    scui_font_glyph_load(&glyph_unit->glyph);
    scui_font_cache_unload(&font_unit);
    #endif
}
