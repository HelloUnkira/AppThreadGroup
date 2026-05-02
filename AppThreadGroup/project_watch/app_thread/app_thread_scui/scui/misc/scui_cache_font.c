/*实现目标:
 *    字库资源缓存
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_cache_font_t scui_cache_font = {0};
/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t scui_cache_font_fd_t(scui_table_dln_t *node)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_cache_font_unit_t *font_unit = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit);
    
    return scui_cache_lru_hash(&font_unit->name, sizeof(scui_handle_t));
}

/*@brief 哈希比较函数
 */
static bool scui_cache_font_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_cache_lru_unit_t *unit1 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node1);
    scui_cache_lru_unit_t *unit2 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node2);
    scui_cache_font_unit_t *font_unit1 = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit1);
    scui_cache_font_unit_t *font_unit2 = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit2);
    
    scui_handle_t name1 = font_unit1->name;
    scui_handle_t name2 = font_unit2->name;
    uint16_t size1 = font_unit1->size;
    uint16_t size2 = font_unit2->size;
    
    return name1 == name2 && size1 == size2 ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_cache_font_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_cache_font_unit_t *font_unit = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit);
    
    SCUI_LOG_INFO("- name:%s",  scui_handle_source(font_unit->name));
    SCUI_LOG_INFO("- size:%u",  font_unit->size);
    SCUI_LOG_INFO("- font:%x",  font_unit->font);
    SCUI_LOG_INFO("- count:%x", unit->count);
    SCUI_LOG_INFO("- lock:%x",  unit->lock);
}

/*@brief 节点占用回调
 */
static scui_multi_t scui_cache_font_unit_get_size(scui_cache_lru_unit_t *unit)
{
    scui_cache_font_unit_t *font_unit = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit);
    return scui_font_size(font_unit->font);
}

/*@brief 节点销毁回调
 */
static void scui_cache_font_unit_new_node(scui_cache_lru_unit_t **unit, scui_cache_lru_unit_t *unit_t)
{
    scui_cache_font_unit_t *font_unit_t = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit_t);
    scui_cache_font_unit_t *font_unit = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(scui_cache_font_unit_t));
    font_unit->size = font_unit_t->size;
    font_unit->name = font_unit_t->name;
    font_unit->font = font_unit_t->font;
    /* 字库资源加载 */
    scui_font_t *font = scui_handle_source_check(font_unit->name);
    scui_font_t font_src = *font;
    font_src.font_size = font_unit->size;
    scui_font_load(&font_src, &font_unit->font);
    SCUI_ASSERT(scui_handle_source(font_unit->font) != NULL);
    
    *unit = &font_unit->lru_unit;
}

/*@brief 节点销毁回调
 */
static void scui_cache_font_unit_cpy_node(scui_cache_lru_unit_t *unit, scui_cache_lru_unit_t *unit_t)
{
    scui_cache_font_unit_t *font_unit_t = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit_t);
    scui_cache_font_unit_t *font_unit   = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit);
    *font_unit = *font_unit_t;
}

/*@brief 节点创建回调
 */
static void scui_cache_font_unit_del_node(scui_cache_lru_unit_t  *unit)
{
    scui_cache_font_unit_t *font_unit = scui_own_ofs(scui_cache_font_unit_t, lru_unit, unit);
    /* 字库资源卸载 */
    scui_font_unload(font_unit->font);
    SCUI_MEM_FREE(font_unit);
}

/*@brief 字库初始化
 */
void scui_cache_font_ready(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    
    cache->lru_table.dlt_fd = scui_cache_font_fd_t;
    cache->lru_table.dlt_fc = scui_cache_font_fc_t;
    cache->lru_table.dlt_fv = scui_cache_font_fv_t;
    cache->lru_table.ht_list_num = SCUI_CACHE_HASH_FONT;
    cache->lru_table.total = SCUI_CACHE_TOTAL_FONT;
    cache->lru_table.get_size = scui_cache_font_unit_get_size;
    cache->lru_table.new_node = scui_cache_font_unit_new_node;
    cache->lru_table.cpy_node = scui_cache_font_unit_cpy_node;
    cache->lru_table.del_node = scui_cache_font_unit_del_node;
    scui_cache_lru_ready(&cache->lru_table);
    #endif
}

/*@brief 字库资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_cache_font_rectify(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_rectify(&cache->lru_table);
    #endif
}

/*@brief 字库资源检查
 */
void scui_cache_font_visit(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_visit(&cache->lru_table);
    #endif
}

/*@brief 字库资源使用
 *@param usage 字库资源使用
 */
void scui_cache_font_usage(uint32_t *usage)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_usage(&cache->lru_table, usage);
    #endif
}

/*@brief 字库资源数量
 *@param nodes 字库资源数量
 */
void scui_cache_font_nodes(uint32_t *nodes)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_nodes(&cache->lru_table, nodes);
    #endif
}

/*@brief 字库资源清除
 */
void scui_cache_font_clear(void)
{
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_clear(&cache->lru_table, false);
    #endif
}

/*@brief 字库资源卸载
 */
void scui_cache_font_unload(scui_cache_font_unit_t *font_unit)
{
    SCUI_ASSERT(font_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_unload(&cache->lru_table, &font_unit->lru_unit);
    #else
    /* 卸载字库资源 */
    scui_font_unload(font_unit->font);
    #endif
}

/*@brief 字库资源加载
 */
void scui_cache_font_load(scui_cache_font_unit_t *font_unit)
{
    SCUI_ASSERT(font_unit != NULL);
    
    #if SCUI_CACHE_HASH_FONT != 0
    scui_cache_font_t *cache = &scui_cache_font;
    scui_cache_lru_load(&cache->lru_table, &font_unit->lru_unit);
    #else
    
    /* 加载字库资源 */
    scui_font_t *font = scui_handle_source_check(font_unit->name);
    scui_font_t font_src = *font;
    font_src.font_size = font_unit->size;
    scui_font_load(&font_src, &font_unit->font);
    SCUI_ASSERT(scui_handle_source(font_unit->font) != NULL);
    #endif
}
