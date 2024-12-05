/*实现目标:
 *    图片资源缓存
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_image_cache_t scui_image_cache = {0};

/*@brief 缓存带计数优先级排序入队列比较回调
 */
static bool scui_image_cache_sort(scui_list_dln_t *node1, scui_list_dln_t *node2)
{
    scui_image_unit_t *unit1 = scui_own_ofs(scui_image_unit_t, ht_node, node1);
    scui_image_unit_t *unit2 = scui_own_ofs(scui_image_unit_t, ht_node, node2);
    return unit1->count >= unit2->count;
}

/*@brief 摘要的来源网络的Hash散列函数
 */
static uint32_t scui_image_cache_hash(uint8_t *data, uint32_t length)
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
static uint32_t scui_image_cache_fd_t(scui_table_dln_t *node)
{
    scui_image_unit_t *unit = scui_own_ofs(scui_image_unit_t, ht_node, node);
    return scui_image_cache_hash((void *)&unit->image->pixel.data_bin, sizeof(uintptr_t));
}

/*@brief 哈希比较函数
 */
static bool scui_image_cache_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_image_unit_t *unit1 = scui_own_ofs(scui_image_unit_t, ht_node, node1);
    scui_image_unit_t *unit2 = scui_own_ofs(scui_image_unit_t, ht_node, node2);
    // 来源和地址同步匹配才可确认资源一致性
    bool cond_bin = false;
    bool cond_tar = false;
    
    if (unit1->image->pixel.data_bin ==
        unit2->image->pixel.data_bin)
        cond_bin = true;
    
    if (unit1->image->from == SCUI_HANDLE_INVALID &&
        unit2->image->from == SCUI_HANDLE_INVALID)
        cond_tar = true;
    if (unit1->image->from != SCUI_HANDLE_INVALID &&
        unit2->image->from != SCUI_HANDLE_INVALID) {
        const char *from_str1 = scui_handle_get(unit1->image->from);
        const char *from_str2 = scui_handle_get(unit2->image->from);
        if (strcmp(from_str1, from_str2) == 0)
            cond_tar = true;
    }
    
    return cond_bin && cond_tar ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_image_cache_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_image_unit_t *unit = scui_own_ofs(scui_image_unit_t, ht_node, node);
    
    static const char *unit__image__local = "local";
    char * unit__image__from = unit__image__local;
    if (unit->image->from != SCUI_HANDLE_INVALID)
        unit__image__from  = scui_handle_get(unit->image->from);
    
    SCUI_LOG_INFO("- width:%x",          unit->image->pixel.width);
    SCUI_LOG_INFO("- height:%x",         unit->image->pixel.height);
    SCUI_LOG_INFO("- data_bin<src>:%x",  unit->image->pixel.data_bin);
    SCUI_LOG_INFO("- size_bin<src>:%x",  unit->image->pixel.size_bin);
    SCUI_LOG_INFO("- type:%x",           unit->image->type);
    SCUI_LOG_INFO("- format:%x",         unit->image->format);
    SCUI_LOG_INFO("- from:%s",           unit__image__from);
    SCUI_LOG_INFO("- count:%x",          unit->count);
    SCUI_LOG_INFO("- lock:%x",           unit->lock);
}

/*@brief 图片初始化
 */
void scui_image_cache_ready(void)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    scui_list_dll_reset(&cache->dl_list);
    scui_table_dlt_fd_t digest  = scui_image_cache_fd_t;
    scui_table_dlt_fc_t confirm = scui_image_cache_fc_t;
    scui_table_dlt_fv_t visit   = scui_image_cache_fv_t;
    scui_table_dll_reset(cache->ht_list, SCUI_CACHE_IMAGE_HASH);
    scui_table_dlt_reset(&cache->ht_table, digest, confirm, visit, cache->ht_list, SCUI_CACHE_IMAGE_HASH);
    
    cache->usage     = 0;
    cache->total     = SCUI_CACHE_IMAGE_TOTAL;
    cache->cnt_hit   = 0;
    cache->cnt_unhit = 0;
}

/*@brief 图片资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_image_cache_rectify(void)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    scui_image_unit_t *unit = NULL;
    
    /* 所有资源全部重衰减 */
    scui_list_dll_ftra(&cache->dl_list, curr) {
        unit = scui_own_ofs(scui_image_unit_t, dl_node, curr);
        unit->count = 0;
    }
}

/*@brief 图片资源检查
 */
void scui_image_cache_visit(void)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    SCUI_LOG_WARN("usage:%u", cache->usage);
    scui_table_dlt_visit(&cache->ht_table);
}

/*@brief 图片资源清除
 */
void scui_image_cache_clear(void)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    // SCUI_LOG_WARN("");
    SCUI_LOG_WARN("usage:%u", cache->usage);
    uint32_t cnt_hit = cache->cnt_hit;
    uint32_t cnt_unhit = cache->cnt_unhit;
    SCUI_LOG_WARN("hit:%u unhit:%u pct:%.02f",
        cnt_hit, cnt_unhit, 1.0f * cnt_hit / (cnt_hit + cnt_unhit));
    cache->cnt_hit = 0;
    cache->cnt_unhit = 0;
    
    scui_image_unit_t *unit = NULL;
    
    /* 所有已解锁资源全部回收 */
    while (true) {
        /* 前向遍历,找已经解锁的资源 */
        scui_list_dll_ftra(&cache->dl_list, curr) {
            unit = scui_own_ofs(scui_image_unit_t, dl_node, curr);
            if (unit->lock == 0)
                break;
            unit = NULL;
        }
        if (unit == NULL)
            break;
        scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
        scui_table_dlt_remove(&cache->ht_table, &unit->ht_node);
        
        /* 约减使用率 */
        cache->usage -= scui_image_size(unit->image);
        /* 卸载图像资源 */
        SCUI_MEM_FREE(unit->data);
        SCUI_MEM_FREE(unit);
        unit = NULL;
    }
}

/*@brief 图片资源缓存无效化(指定目标)
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_invalidate(scui_image_unit_t *image_unit)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    if (image_unit == NULL) {
        SCUI_LOG_WARN("image unit is empty");
        return;
    }
    
    // 内存图片直达即可(不走缓存管理)
    if (image_unit->image->type == scui_image_type_mem)
        return;
    
    scui_image_unit_t *unit = NULL;
    scui_table_dln_t *unit_node = NULL;
    
    if ((unit_node = scui_table_dlt_search(&cache->ht_table, &image_unit->ht_node)) != NULL)
        unit = scui_own_ofs(scui_image_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL) {
        scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
        scui_table_dlt_remove(&cache->ht_table, &unit->ht_node);
        
        /* 约减使用率 */
        cache->usage -= scui_image_size(unit->image);
        /* 卸载图像资源 */
        SCUI_MEM_FREE(unit->data);
        SCUI_MEM_FREE(unit);
    }
}

/*@brief 图片资源卸载
 *@brief image_unit 图片资源缓存节点
 */
void scui_image_cache_unload(scui_image_unit_t *image_unit)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    if (image_unit == NULL) {
        SCUI_LOG_WARN("image unit is empty");
        return;
    }
    
    // 内存图片直达即可(不走缓存管理)
    if (image_unit->image->type == scui_image_type_mem)
        return;
    
    scui_image_unit_t *unit = NULL;
    scui_table_dln_t *unit_node = NULL;
    
    if ((unit_node = scui_table_dlt_search(&cache->ht_table, &image_unit->ht_node)) != NULL)
        unit = scui_own_ofs(scui_image_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL)
        unit->lock--;
}

/*@brief 图片资源加载
 */
void scui_image_cache_load(scui_image_unit_t *image_unit)
{
    scui_image_cache_t *cache = &scui_image_cache;
    
    if (image_unit == NULL) {
        SCUI_LOG_WARN("image info is empty");
        return;
    }
    
    // 内存图片直达即可(不走缓存管理)
    if (image_unit->image->type == scui_image_type_mem) {
        image_unit->data = image_unit->image->pixel.data_bin;
        return;
    }
    
    scui_image_unit_t *unit = NULL;
    scui_table_dln_t *unit_node = NULL;
    
    if ((unit_node = scui_table_dlt_search(&cache->ht_table, &image_unit->ht_node)) != NULL)
        unit = scui_own_ofs(scui_image_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL) {
        /* 上锁 */
        unit->lock++;
        /* 命中缓存资源计数加 */
        if (unit->count != 0 && unit->count < 100) {
            unit->count++;
            /* 重新带计数优先级加入 */
            scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
            scui_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_image_cache_sort);
        }
        cache->cnt_hit++;
        *image_unit = *unit;
        return;
    }
    /* 对缓存计数器进行一次重衰减(rewind),老化它 */
    scui_list_dln_t *node = NULL;
    if ((node = scui_list_dll_tail(&cache->dl_list)) != NULL) {
         unit = scui_own_ofs(scui_image_unit_t, dl_node, node);
         uint8_t count = unit->count;
         if (count > 3) {
             // scui_list_dll_ftra(&cache->dl_list, curr)
                scui_list_dll_btra(&cache->dl_list, curr) {
                unit = scui_own_ofs(scui_image_unit_t, dl_node, curr);
                unit->count -= count;
             }
         }
         node = NULL;
         unit = NULL;
    }
    /* 如果缓存未命中时 */
    if (unit == NULL) {
        uintptr_t image_size = scui_image_size(image_unit->image);
        /* 如果缓存空间不足时,老化资源回收 */
        while (cache->usage + image_size > cache->total) {
            SCUI_ASSERT(cache->usage <= cache->total);
            /* 前向遍历,找已经解锁的资源 */
            scui_list_dll_ftra(&cache->dl_list, curr) {
                unit = scui_own_ofs(scui_image_unit_t, dl_node, curr);
                if (unit->lock == 0)
                    break;
                unit = NULL;
            }
            if (unit == NULL) {
                SCUI_LOG_ERROR("cache legacy excess");
                scui_image_cache_visit();
                scui_image_cache_clear();
                break;
            }
            scui_list_dll_remove(&cache->dl_list, &unit->dl_node);
            scui_table_dlt_remove(&cache->ht_table, &unit->ht_node);
            
            /* 约减使用率 */
            cache->usage -= scui_image_size(unit->image);
            /* 卸载图像资源 */
            SCUI_MEM_FREE(unit->data);
            SCUI_MEM_FREE(unit);
            unit = NULL;
        }
        /* 为数据区申请新资源 */
        unit          = SCUI_MEM_ALLOC(scui_mem_type_mix,   sizeof(scui_image_unit_t));
        unit->data    = SCUI_MEM_ALLOC(scui_mem_type_graph, image_size);
        unit->image   = image_unit->image;
        unit->count   = 1;
        unit->lock    = 1;
        cache->usage += image_size;
        
        /* 图片资源加载 */
        scui_image_src_read(unit->image, unit->data);
        *image_unit = *unit;
        /* 带计数优先级加入 */
        scui_list_dln_reset(&unit->dl_node);
        scui_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_image_cache_sort);
        scui_table_dln_reset(&unit->ht_node);
        scui_table_dlt_insert(&cache->ht_table, &unit->ht_node);
        cache->cnt_unhit++;
    }
}
