/*实现目标:
 *    图片资源缓存
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static scui_image_cache_t scui_image_record_cache = {0};

/*@brief 缓存带计数优先级排序入队列比较回调
 */
static bool scui_image_cache_sort(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    scui_image_unit_t *unit1 = app_sys_own_ofs(scui_image_unit_t, ht_node, node1);
    scui_image_unit_t *unit2 = app_sys_own_ofs(scui_image_unit_t, ht_node, node2);
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
static uint32_t scui_image_cache_fd_t(app_sys_table_rbsn_t *node)
{
    scui_image_unit_t *unit = app_sys_own_ofs(scui_image_unit_t, ht_node, node);
    /* 摘要的来源网络的Hash散列函数 */
    uint32_t app_sys_table_elf_hash(uint8_t *data, uint32_t length);
    return scui_image_cache_hash((void *)&unit->image->pixel.data, sizeof(uintptr_t));
}

/*@brief 哈希比较函数
 */
static uint8_t scui_image_cache_fc1_t(app_sys_table_rbsn_t *node1, app_sys_table_rbsn_t *node2)
{
    scui_image_unit_t *unit1 = app_sys_own_ofs(scui_image_unit_t, ht_node, node1);
    scui_image_unit_t *unit2 = app_sys_own_ofs(scui_image_unit_t, ht_node, node2);
    return unit1->image->pixel.data < unit2->image->pixel.data ? 1 : 0;
}

/*@brief 哈希比较函数
 */
static uint8_t scui_image_cache_fc2_t(app_sys_table_rbsn_t *node1, app_sys_table_rbsn_t *node2)
{
    scui_image_unit_t *unit1 = app_sys_own_ofs(scui_image_unit_t, ht_node, node1);
    scui_image_unit_t *unit2 = app_sys_own_ofs(scui_image_unit_t, ht_node, node2);
    return unit1->image->pixel.data == unit2->image->pixel.data ? 0 : 1;
}

/*@brief 哈希访问函数
 */
static void scui_image_cache_fv_t(app_sys_table_rbsn_t *node, uint32_t idx)
{
    scui_image_unit_t *unit = app_sys_own_ofs(scui_image_unit_t, ht_node, node);
    
    APP_SYS_LOG_INFO_RAW("< format:%x",         unit->image->format);
    APP_SYS_LOG_INFO_RAW("- status:%x",         unit->image->status);
    APP_SYS_LOG_INFO_RAW("- height:%x",         unit->image->pixel.height);
    APP_SYS_LOG_INFO_RAW("- width:%x",          unit->image->pixel.width);
    APP_SYS_LOG_INFO_RAW("- size_raw<src>:%x",  unit->image->pixel.size_raw);
    APP_SYS_LOG_INFO_RAW("- size_mem<src>:%x",  unit->image->pixel.size_mem);
    APP_SYS_LOG_INFO_RAW("- data<src>:%x",      unit->image->pixel.data);
    APP_SYS_LOG_INFO_RAW("- from:%x >",         unit->image->from);
}

/*@brief 图片初始化
 */
void scui_image_cache_ready(void)
{
    scui_image_cache_t *cache = &scui_image_record_cache;
    
    app_sys_list_dll_reset(&cache->dl_list);
    app_sys_table_rbst_fd_t digest  = scui_image_cache_fd_t;
    app_sys_table_rbst_fc_t compare = scui_image_cache_fc1_t;
    app_sys_table_rbst_fc_t confirm = scui_image_cache_fc2_t;
    app_sys_table_rbst_fv_t visit   = scui_image_cache_fv_t;
    app_sys_table_rbsl_reset(cache->ht_list, SCUI_IMAGE_LIMIT_HASH);
    app_sys_table_rbst_reset(&cache->ht_table, digest, compare, confirm, visit, cache->ht_list, SCUI_IMAGE_LIMIT_HASH);
    
    cache->usage     = 0;
    cache->total     = SCUI_IMAGE_LIMIT_TOTAL;
    cache->cnt_hit   = 0;
    cache->cnt_unhit = 0;
}

/*@brief 图片资源清除
 */
void scui_image_cache_clear(void)
{
    scui_image_cache_t *cache = &scui_image_record_cache;
    
    app_sys_table_rbst_visit(&cache->ht_table);
    
    scui_image_unit_t *unit = NULL;
    
    /* 所有已解锁资源全部回收 */
    while (true) {
        /* 前向遍历,找已经解锁的资源 */
        app_sys_list_dll_ftra(&cache->dl_list, curr) {
            unit = app_sys_own_ofs(scui_image_unit_t, dl_node, curr);
            if (unit->lock == 0)
                break;
            unit = NULL;
        }
        if (unit == NULL)
            return;
        app_sys_list_dll_remove(&cache->dl_list, &unit->dl_node);
        app_sys_table_rbst_remove(&cache->ht_table, &unit->ht_node);
        
        /* 约减使用率 */
        cache->usage -= unit->image->pixel.size_mem;
        /* 卸载图像资源 */
        scui_image_src_unload(unit);
        SCUI_MEM_FREE(unit);
        unit = NULL;
    }
}

/*@brief 图片资源卸载
 */
void scui_image_cache_unload(scui_image_unit_t *image_unit)
{
    scui_image_cache_t *cache = &scui_image_record_cache;
    
    if (image_unit == NULL) {
        APP_SYS_LOG_WARN("image unit is empty");
        return;
    }
    
    scui_image_unit_t *unit = NULL;
    app_sys_table_dln_t *unit_node = NULL;
    
    if ((unit_node = app_sys_table_rbst_search(&cache->ht_table, &image_unit->ht_node)) != NULL)
        unit = app_sys_own_ofs(scui_image_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL)
        unit->lock--;
}

/*@brief 图片资源加载
 */
void scui_image_cache_load(scui_image_unit_t *image_unit)
{
    scui_image_cache_t *cache = &scui_image_record_cache;
    
    if (image_unit == NULL) {
        APP_SYS_LOG_WARN("image info is empty");
        return;
    }
    
    scui_image_unit_t *unit = NULL;
    app_sys_table_dln_t *unit_node = NULL;
    
    if ((unit_node = app_sys_table_rbst_search(&cache->ht_table, &image_unit->ht_node)) != NULL)
        unit = app_sys_own_ofs(scui_image_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (unit != NULL) {
        /* 上锁 */
        unit->lock++;
        /* 命中缓存资源计数加 */
        if (unit->count != 0 && unit->count < 100) {
            unit->count++;
            /* 重新带计数优先级加入 */
            app_sys_list_dll_remove(&cache->dl_list, &unit->dl_node);
            app_sys_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_image_cache_sort);
        }
        cache->cnt_hit++;
        *image_unit = *unit;
        return;
    }
    /* 对缓存计数器进行一次重衰减(rewind),老化它 */
    app_sys_list_dln_t *node = NULL;
    if ((node = app_sys_list_dll_tail(&cache->dl_list)) != NULL) {
         unit = app_sys_own_ofs(scui_image_unit_t, dl_node, node);
         uint8_t count = unit->count;
         if (count > 3) {
             // app_sys_list_dll_ftra(&cache->dl_list, curr)
                app_sys_list_dll_btra(&cache->dl_list, curr) {
                unit = app_sys_own_ofs(scui_image_unit_t, dl_node, curr);
                unit->count -= count;
             }
         }
         node = NULL;
         unit = NULL;
    }
    /* 如果缓存未命中时 */
    if (unit == NULL) {
        /* 如果缓存空间不足时,老化资源回收 */
        while (cache->usage + image_unit->image->pixel.size_mem > cache->total) {
            /* 前向遍历,找已经解锁的资源 */
            app_sys_list_dll_ftra(&cache->dl_list, curr) {
                unit = app_sys_own_ofs(scui_image_unit_t, dl_node, curr);
                if (unit->lock == 0)
                    break;
                unit = NULL;
            }
            if (unit == NULL) {
                APP_SYS_LOG_ERROR("cache legacy excess");
                return;
            }
            app_sys_list_dll_remove(&cache->dl_list, &unit->dl_node);
            app_sys_table_rbst_remove(&cache->ht_table, &unit->ht_node);
            
            /* 约减使用率 */
            cache->usage -= unit->image->pixel.size_mem;
            /* 卸载图像资源 */
            scui_image_src_unload(unit);
            SCUI_MEM_FREE(unit);
            unit = NULL;
        }
        /* 为数据区申请新资源 */
        unit = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_image_unit_t));
        unit->image   = image_unit->image;
        unit->count   = 1;
        unit->lock    = 1;
        cache->usage += unit->image->pixel.size_mem;
        /* 图片资源加载 */
        scui_image_src_load(unit);
        *image_unit = *unit;
        /* 带计数优先级加入 */
        app_sys_list_dln_reset(&unit->dl_node);
        app_sys_queue_dlpq_enqueue(&cache->dl_list, &unit->dl_node, scui_image_cache_sort);
        app_sys_table_rbsn_reset(&unit->ht_node);
        app_sys_table_rbst_insert(&cache->ht_table, &unit->ht_node);
        cache->cnt_unhit++;
    }
}
