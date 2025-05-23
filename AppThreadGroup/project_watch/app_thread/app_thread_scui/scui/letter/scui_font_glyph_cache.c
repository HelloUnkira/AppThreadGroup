/*实现目标:
 *    文字资源缓存
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if SCUI_CACHE_HASH_FONT_GLYPH != 0

static scui_font_glyph_cache_t scui_font_glyph_cache = {0};

/*@brief 缓存带计数优先级排序入队列比较回调
 */
static bool scui_font_glyph_cache_sort(scui_list_dln_t *node1, scui_list_dln_t *node2)
{
    scui_font_glyph_unit_t *unit1 = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node1);
    scui_font_glyph_unit_t *unit2 = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node2);
    return unit1->count >= unit2->count;
}

/*@brief 摘要的来源网络的Hash散列函数
 */
static uint32_t scui_font_glyph_cache_hash(uint8_t *data, uint32_t length)
{
    uint64_t h = 0, g = 0;
    
    for (uint32_t idx = 0; idx < length; idx++) {
        h = (h << 4) + data[idx];
        if (g = h & 0xf0000000)
            h ^= g >> 24;
        h &= ~g;
    }
    
    /* 散列不均匀时需要适当加点盐 */
    const uint32_t salt = 13;
    
    return (uint32_t)(h >> salt);
}

/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t scui_font_glyph_cache_fd_t(scui_table_dln_t *node)
{
    scui_font_glyph_unit_t *unit = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node);
    
    uint32_t letter = unit->glyph.unicode_letter;
    return scui_font_glyph_cache_hash((void *)&letter, sizeof(uint32_t));
}

/*@brief 哈希比较函数
 */
static uint8_t scui_font_glyph_cache_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_font_glyph_unit_t *unit1 = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node1);
    scui_font_glyph_unit_t *unit2 = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node2);
    
    scui_handle_t name1 = unit1->name;
    scui_handle_t name2 = unit2->name;
    uint32_t letter1 = unit1->glyph.unicode_letter;
    uint32_t letter2 = unit2->glyph.unicode_letter;
    uint16_t size1 = unit1->size;
    uint16_t size2 = unit2->size;
    
    return name1 == name2 && letter1 == letter2 && size1 == size2 ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_font_glyph_cache_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_font_glyph_unit_t *unit = scui_own_ofs(scui_font_glyph_unit_t, ht_node, node);
    
    SCUI_LOG_INFO("- name:%s",   scui_handle_source(unit->name));
    SCUI_LOG_INFO("- size:%u",   unit->size);
    SCUI_LOG_INFO("- letter:%x", unit->glyph.unicode_letter);
    SCUI_LOG_INFO("- count:%x",  unit->count);
    SCUI_LOG_INFO("- lock:%x",   unit->lock);
}

/*@brief 文字初始化
 */
void scui_font_glyph_cache_ready(void)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    scui_list_dll_reset(&cache->dl_list);
    scui_table_dlt_fd_t digest  = scui_font_glyph_cache_fd_t;
    scui_table_dlt_fc_t confirm = scui_font_glyph_cache_fc_t;
    scui_table_dlt_fv_t visit   = scui_font_glyph_cache_fv_t;
    scui_table_dll_reset(cache->ht_list, SCUI_CACHE_HASH_FONT_GLYPH);
    scui_table_dlt_reset(&cache->ht_table, digest, confirm, visit, cache->ht_list, SCUI_CACHE_HASH_FONT_GLYPH);
    
    cache->usage     = 0;
    cache->total     = SCUI_CACHE_TOTAL_FONT_GLYPH;
    cache->cnt_hit   = 0;
    cache->cnt_unhit = 0;
}

/*@brief 文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_glyph_cache_rectify(void)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    scui_font_glyph_unit_t *unit = NULL;
    
    /* 所有资源全部重衰减 */
    scui_list_dll_ftra(&cache->dl_list, curr) {
        unit = scui_own_ofs(scui_font_glyph_unit_t, dl_node, curr);
        unit->count = 0;
    }
}

/*@brief 文字资源检查
 */
void scui_font_glyph_cache_visit(void)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    SCUI_LOG_WARN("usage:%u", cache->usage);
    scui_table_dlt_visit(&cache->ht_table);
}

/*@brief 文字资源清除
 */
void scui_font_glyph_cache_clear(void)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    // SCUI_LOG_WARN("");
    SCUI_LOG_WARN("usage:%u", cache->usage);
    uint32_t cnt_hit = cache->cnt_hit;
    uint32_t cnt_unhit = cache->cnt_unhit;
    SCUI_LOG_WARN("hit:%u unhit:%u pct:%.02f",
        cnt_hit, cnt_unhit, 1.0f * cnt_hit / (cnt_hit + cnt_unhit));
    cache->cnt_hit = 0;
    cache->cnt_unhit = 0;
    
    scui_font_glyph_unit_t *unit = NULL;
    
    /* 所有已解锁资源全部回收 */
    while (true) {
        /* 前向遍历,找已经解锁的资源 */
        scui_list_dll_ftra(&cache->dl_list, curr) {
            unit = scui_own_ofs(scui_font_glyph_unit_t, dl_node, curr);
            if (unit->lock == 0)
                break;
            unit = NULL;
        }
        if (unit == NULL)
            return;
        scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
        scui_table_dlt_remove(&cache->ht_table, &unit->ht_node);
        
        /* 约减使用率 */
        cache->usage -= unit->glyph.bitmap_size;
        /* 卸载文字资源 */
        scui_font_glyph_unload(&unit->glyph);
        SCUI_MEM_FREE(unit);
        unit = NULL;
    }
}

/*@brief 文字资源卸载
 */
void scui_font_glyph_cache_unload(scui_font_glyph_unit_t *glyph_unit)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    if (glyph_unit == NULL) {
        SCUI_LOG_WARN("font unit is empty");
        return;
    }
    
    scui_font_glyph_unit_t *unit = NULL;
    scui_table_dln_t *unit_node = NULL;
    
    if ((unit_node = scui_table_dlt_search(&cache->ht_table, &glyph_unit->ht_node)) != NULL)
        unit = scui_own_ofs(scui_font_glyph_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL)
    if (unit->lock != 0)
        unit->lock--;
}

/*@brief 文字资源加载
 */
void scui_font_glyph_cache_load(scui_font_glyph_unit_t *glyph_unit)
{
    scui_font_glyph_cache_t *cache = &scui_font_glyph_cache;
    
    if (glyph_unit == NULL) {
        SCUI_LOG_WARN("font info is empty");
        return;
    }
    
    scui_font_glyph_unit_t *unit = NULL;
    scui_table_dln_t *unit_node = NULL;
    
    if ((unit_node = scui_table_dlt_search(&cache->ht_table, &glyph_unit->ht_node)) != NULL)
        unit = scui_own_ofs(scui_font_glyph_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL) {
        /* 上锁 */
        unit->lock++;
        if (unit->lock > 0x7A)
            SCUI_LOG_INFO("lock num will be overflow");
        /* 命中缓存资源计数加 */
        if (unit->count != 0 && unit->count < 100) {
            unit->count++;
            /* 重新带计数优先级加入 */
            scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
            scui_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_font_glyph_cache_sort);
        }
        cache->cnt_hit++;
        *glyph_unit = *unit;
        return;
    }
    /* 对缓存计数器进行一次重衰减(rewind),老化它 */
    scui_list_dln_t *node = NULL;
    if ((node = scui_list_dll_tail(&cache->dl_list)) != NULL) {
         unit = scui_own_ofs(scui_font_glyph_unit_t, dl_node, node);
         uint8_t count = unit->count;
         if (count > 3) {
             // scui_list_dll_ftra(&cache->dl_list, curr)
                scui_list_dll_btra(&cache->dl_list, curr) {
                unit = scui_own_ofs(scui_font_glyph_unit_t, dl_node, curr);
                unit->count -= count;
             }
         }
         node = NULL;
         unit = NULL;
    }
    /* 如果缓存未命中时 */
    if (unit == NULL) {
        /* 先加载字库 */
        scui_font_unit_t font_unit = {0};
        font_unit.size = glyph_unit->size;
        font_unit.name = glyph_unit->name;
        scui_font_cache_load(&font_unit);
        glyph_unit->font = font_unit.font;
        glyph_unit->glyph.handle = font_unit.font;
        scui_font_glyph_load(&glyph_unit->glyph);
        scui_font_cache_unload(&font_unit);
        /* 如果缓存空间不足时,老化资源回收 */
        while (cache->usage + glyph_unit->glyph.bitmap_size > cache->total) {
            /* 前向遍历,找已经解锁的资源 */
            scui_list_dll_ftra(&cache->dl_list, curr) {
                unit = scui_own_ofs(scui_font_glyph_unit_t, dl_node, curr);
                if (unit->lock == 0)
                    break;
                unit = NULL;
            }
            if (unit == NULL) {
                SCUI_LOG_ERROR("cache legacy excess");
                return;
            }
            scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
            scui_table_dlt_remove(&cache->ht_table, &unit->ht_node);
            
            /* 约减使用率 */
            cache->usage -= unit->glyph.bitmap_size;
            /* 卸载文字资源 */
            scui_font_glyph_unload(&unit->glyph);
            SCUI_MEM_FREE(unit);
            unit = NULL;
        }
        /* 为数据区申请新资源 */
        unit = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(scui_font_glyph_unit_t));
        unit->size    = glyph_unit->size;
        unit->name    = glyph_unit->name;
        unit->font    = glyph_unit->font;
        unit->glyph   = glyph_unit->glyph;
        unit->count   = 1;
        unit->lock    = 1;
        cache->usage += glyph_unit->glyph.bitmap_size;
        *glyph_unit = *unit;
        /* 带计数优先级加入 */
        scui_list_dln_reset(&unit->dl_node);
        scui_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_font_glyph_cache_sort);
        scui_table_dln_reset(&unit->ht_node);
        scui_table_dlt_insert(&cache->ht_table, &unit->ht_node);
        cache->cnt_unhit++;
    }
}

#else

/*@brief 文字初始化
 */
void scui_font_glyph_cache_ready(void)
{
}

/*@brief 文字资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_font_glyph_cache_rectify(void)
{
}

/*@brief 文字资源检查
 */
void scui_font_glyph_cache_visit(void)
{
}

/*@brief 文字资源清除
 */
void scui_font_glyph_cache_clear(void)
{
}

/*@brief 文字资源卸载
 */
void scui_font_glyph_cache_unload(scui_font_glyph_unit_t *glyph_unit)
{
    /* 卸载文字资源 */
    scui_font_glyph_unload(&glyph_unit->glyph);
}

/*@brief 文字资源加载
 */
void scui_font_glyph_cache_load(scui_font_glyph_unit_t *glyph_unit)
{
    /* 先加载字库 */
    scui_font_unit_t font_unit = {0};
    font_unit.size = glyph_unit->size;
    font_unit.name = glyph_unit->name;
    scui_font_cache_load(&font_unit);
    glyph_unit->font = font_unit.font;
    glyph_unit->glyph.handle = font_unit.font;
    scui_font_glyph_load(&glyph_unit->glyph);
    scui_font_cache_unload(&font_unit);
}

#endif
