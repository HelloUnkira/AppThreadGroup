/*实现目标:
 *    抽象外存资源的缓存管理
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_list.h"
#include "app_sys_hashtable.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_cache.h"

/*@brief 缓存带计数优先级排序入队列比较回调
 */
static bool app_sys_ext_mem_cache_sort(app_sys_list_dn_t *node1, app_sys_list_dn_t *node2)
{
    app_sys_ext_mem_cache_unit_t *unit1 = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, node1);
    app_sys_ext_mem_cache_unit_t *unit2 = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, node2);
    return unit1->count >= unit2->count;
}

/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t app_sys_ext_mem_cache_fd_t(app_sys_hashtable_dn_t *node)
{
    app_sys_ext_mem_cache_unit_t *unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node);
    /* 摘要的来源网络的Hash散列函数 */
    uint32_t app_sys_hashtable_elf_hash(uint8_t *data, uint32_t length);
    return app_sys_hashtable_elf_hash((void *)&unit->offset, sizeof(uintptr_t));
}

/*@brief 哈希比较函数
 */
static bool app_sys_ext_mem_cache_fc_t(app_sys_hashtable_dn_t *node1, app_sys_hashtable_dn_t *node2)
{
    app_sys_ext_mem_cache_unit_t *unit1 = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node1);
    app_sys_ext_mem_cache_unit_t *unit2 = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node2);
    return unit1->offset == unit2->offset;
}

/*@brief 哈希访问函数
 */
static void app_sys_ext_mem_cache_fv_t(app_sys_hashtable_dn_t *node,  uint32_t idx)
{
    app_sys_ext_mem_cache_unit_t *unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node);
    APP_SYS_LOG_INFO("%u: <%x, %x, %p, %u, %u, %u>", idx,
                      unit->offset, unit->size,  unit->buffer,
                      unit->count,  unit->dirty, unit->lock);
}

/*@brief     缓存就绪,配置参数
 *@param[in] cache 缓存实例
 *@param[in] ext_mem 外存静态实例
 *@param[in] unit  缓存单元门限
 *@param[in] total 缓存总门限
 */
void app_sys_ext_mem_cache_ready(app_sys_ext_mem_cache_t *cache, const app_sys_ext_mem_t *ext_mem, uint32_t unit, uint32_t total)
{
    APP_SYS_ASSERT(ext_mem != NULL);
    APP_SYS_ASSERT(unit < total);
    app_mutex_process(&cache->mutex, app_mutex_static);
    app_sys_list_dl_reset(&cache->dl_list);
    cache->ht_list_num = total / unit + 1;
    cache->ht_list = app_mem_alloc(sizeof(app_sys_hashtable_dl_t) * cache->ht_list_num);
    app_sys_hashtable_dt_fd_t digest  = app_sys_ext_mem_cache_fd_t;
    app_sys_hashtable_dt_fc_t confirm = app_sys_ext_mem_cache_fc_t;
    app_sys_hashtable_dt_fv_t visit   = app_sys_ext_mem_cache_fv_t;
    app_sys_hashtable_dl_reset(cache->ht_list, cache->ht_list_num);
    app_sys_hashtable_dt_reset(&cache->ht_table, digest, confirm, visit, cache->ht_list, cache->ht_list_num);
    cache->ext_mem   = ext_mem;
    cache->unit      = unit;    /* 单元资源门限 */
    cache->usage     = 0;       /* 内存资源占用 */
    cache->total     = total;   /* 内存资源总占用门限 */
    cache->cnt_hit   = 0;
    cache->cnt_unhit = 0;
}

/*@brief     缓存刷新(清理内存)
 *@param[in] cache 缓存实例
 *@param[in] force 强制刷新模式(仅在完全不使用它时使用)
 */
void app_sys_ext_mem_cache_reflush(app_sys_ext_mem_cache_t *cache, bool force)
{
    app_mutex_process(&cache->mutex, app_mutex_take);
    /* 输出一次缓存布局(如果刷新了信息) */
    app_sys_hashtable_dt_visit(&cache->ht_table);
    /* 所有已解锁资源全部回收 */
    app_sys_ext_mem_cache_unit_t *unit = NULL;
    while (true) {
        /* 前向遍历,找已经解锁的资源 */
        app_sys_list_dl_ftra(&cache->dl_list, curr) {
            unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, curr);
            if (force)
                break;
            if (unit->lock == 0)
                 break;
            unit = NULL;
        }
        if (unit == NULL)
            goto over;
        app_sys_list_dl_remove(&cache->dl_list, &unit->dl_node);
        app_sys_hashtable_dt_remove(&cache->ht_table, &unit->ht_node);
        /* 污染标记,数据回写 */
        if (unit->dirty)
        if (!app_sys_ext_mem_write(cache->ext_mem, unit->offset, unit->buffer, unit->size))
             APP_SYS_LOG_ERROR("data write fail");
        /* 约减使用率 */
        cache->usage -= unit->size;
        /* 回收数据区 */
        app_mem_free(unit->buffer);
        app_mem_free(unit);
        unit = NULL;
    }
    over:
    app_mutex_process(&cache->mutex, app_mutex_give);
}

/*@brief      缓存资源获取
 *@param[in]  cache  缓存实例
 *@param[in]  offset 外存数据偏移
 *@param[in]  size   外存数据大小
 *@param[out] buffer 内存数据地址
 *@retval     操作结果
 */
uint32_t app_sys_ext_mem_cache_take(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uintptr_t size, uint8_t **buffer)
{
    uint32_t retval = 0;
    app_mutex_process(&cache->mutex, app_mutex_take);
    if (cache->unit < size) {
       *buffer = NULL;
        APP_SYS_LOG_WARN("cache legacy excess");
        retval = app_sys_ext_mem_cache_overflow;
        goto over;
    }
    /* 迭代缓存链表,尝试命中目标缓存 */
    app_sys_ext_mem_cache_unit_t *unit = NULL;
    app_sys_hashtable_dn_t *node_match = NULL;
    app_sys_ext_mem_cache_unit_t unit_match = {.offset = offset};
    if ((node_match = app_sys_hashtable_dt_search(&cache->ht_table, &unit_match.ht_node)) != NULL)
        unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node_match);
    /* 如果缓存命中时 */
    if (unit  != NULL) {
       *buffer = unit->buffer;
        /* 上锁 */
        unit->lock++;
        /* 命中缓存资源计数加 */
        if (unit->count != 0 && unit->count < 100) {
            unit->count++;
            /* 重新带计数优先级加入 */
            app_sys_list_dl_remove(&cache->dl_list, &unit->dl_node);
            app_sys_queue_dpq_enqueue(&cache->dl_list, &unit->dl_node, app_sys_ext_mem_cache_sort);
        }
        cache->cnt_hit++;
        retval = app_sys_ext_mem_cache_hit;
        goto over;
    }
    /* 对缓存计数器进行一次重衰减(rewind),老化它 */
    app_sys_list_dn_t *node = NULL;
    if ((node = app_sys_list_dl_tail(&cache->dl_list)) != NULL) {
         unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, node);
         uint8_t count = unit->count;
         if (count != 0) {
             // app_sys_list_dl_ftra(&cache->dl_list, curr)
                app_sys_list_dl_btra(&cache->dl_list, curr) {
                unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, curr);
                unit->count -= count;
             }
         }
         node = NULL;
         unit = NULL;
    }
    /* 如果缓存未命中时 */
    if (unit == NULL) {
        /* 如果缓存空间不足时,老化资源回收 */
        while (cache->usage + size > cache->total) {
            /* 前向遍历,找已经解锁的资源 */
            app_sys_list_dl_ftra(&cache->dl_list, curr) {
                unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, dl_node, curr);
                if (unit->lock == 0)
                     break;
                unit = NULL;
            }
            if (unit  == NULL) {
               *buffer = NULL;
                APP_SYS_LOG_WARN("cache legacy excess");
                retval = app_sys_ext_mem_cache_overflow;
                goto over;
            }
            app_sys_list_dl_remove(&cache->dl_list, &unit->dl_node);
            app_sys_hashtable_dt_remove(&cache->ht_table, &unit->ht_node);
            /* 污染标记,数据回写 */
            if (unit->dirty)
            if (!app_sys_ext_mem_write(cache->ext_mem, unit->offset, unit->buffer, unit->size))
                 APP_SYS_LOG_ERROR("data write fail");
            /* 约减使用率 */
            cache->usage -= unit->size;
            /* 回收数据区 */
            app_mem_free(unit->buffer);
            app_mem_free(unit);
            unit = NULL;
        }
        /* 为数据区申请新资源 */
        unit = app_mem_alloc(sizeof(app_sys_ext_mem_cache_unit_t));
        unit->offset  = offset;
        unit->buffer  = app_mem_alloc(size);
        unit->size    = size;
        unit->count   = 1;
        unit->dirty   = false;
        unit->lock    = 1;
        app_sys_list_dn_reset(&unit->dl_node);
        cache->usage += size;
       *buffer = unit->buffer;
        /* 数据读取 */
        if (!app_sys_ext_mem_read(cache->ext_mem, unit->offset, unit->buffer, unit->size))
             APP_SYS_LOG_ERROR("data read fail");
        /* 带计数优先级加入 */
        app_sys_queue_dpq_enqueue(&cache->dl_list, &unit->dl_node, app_sys_ext_mem_cache_sort);
        app_sys_hashtable_dt_insert(&cache->ht_table, &unit->ht_node);
        cache->cnt_unhit++;
        retval = app_sys_ext_mem_cache_unhit;
        goto over;
    }
    over:
    app_mutex_process(&cache->mutex, app_mutex_give);
    return retval;
}

/*@brief     缓存资源获取
 *@param[in] cache  缓存实例
 *@param[in] offset 外存数据偏移
 *@param[in] buffer 内存数据地址
 *@param[in] dirty  内存数据是否修改
 *@retval    操作结果
 */
uint32_t app_sys_ext_mem_cache_give(app_sys_ext_mem_cache_t *cache, uintptr_t offset, uint8_t *buffer, bool dirty)
{
    uint32_t retval = 0;
    app_mutex_process(&cache->mutex, app_mutex_take);
    /* 迭代缓存链表,尝试命中目标缓存 */
    app_sys_ext_mem_cache_unit_t *unit = NULL;
    app_sys_hashtable_dn_t *node_match = NULL;
    app_sys_ext_mem_cache_unit_t unit_match = {.offset = offset};
    if ((node_match = app_sys_hashtable_dt_search(&cache->ht_table, &unit_match.ht_node)) != NULL)
        unit = app_ext_own_ofs(app_sys_ext_mem_cache_unit_t, ht_node, node_match);
    /* 如果缓存未命中时 */
    if (unit == NULL) {
        /* 资源可能被强制回收了,警告 */
        APP_SYS_LOG_WARN("cache not record");
        retval = app_sys_ext_mem_cache_unhit;
        goto over;
    }
    /* 如果缓存命中时 */
    if (unit != NULL) {
        unit->lock--;
        unit->dirty = dirty ? dirty : unit->dirty;
        retval = app_sys_ext_mem_cache_hit;
        goto over;
    }
    over:
    app_mutex_process(&cache->mutex, app_mutex_give);
    return retval;
}
