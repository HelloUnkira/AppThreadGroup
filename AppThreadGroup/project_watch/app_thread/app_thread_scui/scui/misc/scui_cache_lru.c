/*实现目标:
 *    资源缓存器:LRU
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 缓存带计数优先级排序入队列比较回调
 *@param node1 侵入式列表节点
 *@param node2 侵入式列表节点
 */
static bool scui_cache_lru_sort(scui_list_dln_t *node1, scui_list_dln_t *node2)
{
    scui_cache_lru_unit_t *lru_unit1 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node1);
    scui_cache_lru_unit_t *lru_unit2 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node2);
    return lru_unit1->count >= lru_unit2->count;
}

/*@brief 摘要的来源网络的Hash散列函数
 *@param data 数据流
 *@param len  数据字节
 */
uint32_t scui_cache_lru_hash(uint8_t *data, uint32_t len)
{
    uint64_t h = 0, g = 0;
    
    for (uint32_t idx = 0; idx < len; idx++) {
        h = (h << 4) + data[idx];
        if (g = h & 0xf0000000)
            h ^= g >> 24;
        h &= ~g;
    }
    
    /* 散列不均匀时需要适当加点盐 */
    const uint32_t salt = 13;
    
    return (uint32_t)(h >> salt);
}

/*@brief 资源缓存器就绪
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_ready(scui_cache_lru_table_t *lru_table)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(lru_table->total != 0);
    SCUI_ASSERT(lru_table->dlt_fd != NULL);
    SCUI_ASSERT(lru_table->dlt_fc != NULL);
    SCUI_ASSERT(lru_table->dlt_fv != NULL);
    SCUI_ASSERT(lru_table->ht_list_num != 0);
    SCUI_ASSERT(lru_table->ht_list_num <= SCUI_CACHE_LRU_HASH_LIMIT);
    
    scui_list_dll_reset(&lru_table->dl_list);
    scui_table_dll_reset(lru_table->ht_list, lru_table->ht_list_num);
    scui_table_dlt_reset(&lru_table->ht_table, lru_table->dlt_fd, lru_table->dlt_fc,
        lru_table->dlt_fv, lru_table->ht_list, lru_table->ht_list_num);
    
    lru_table->nodes = 0;
    lru_table->usage = 0;
    lru_table->cnt_hit = 0;
    lru_table->cnt_unhit = 0;
}

/*@brief 资源重校正(计数器重衰减到0)
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_rectify(scui_cache_lru_table_t *lru_table)
{
    SCUI_ASSERT(lru_table != NULL);
    
    /* 所有资源全部重衰减 */
    scui_cache_lru_unit_t *lru_unit_t = NULL;
    scui_list_dll_ftra(&lru_table->dl_list, curr) {
        lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, dl_node, curr);
        lru_unit_t->count = 0;
    }
}

/*@brief 资源检查
 *@param lru_table 资源缓存表
 */
void scui_cache_lru_visit(scui_cache_lru_table_t *lru_table)
{
    SCUI_ASSERT(lru_table != NULL);
    
    SCUI_LOG_WARN("nodes:%u, usage:%u", lru_table->nodes, lru_table->usage);
    scui_table_dlt_visit(&lru_table->ht_table);
}

/*@brief 资源使用
 *@param lru_table 资源缓存表
 *@param usage     资源使用
 */
void scui_cache_lru_usage(scui_cache_lru_table_t *lru_table, uint32_t *usage)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(usage != NULL);
    
    *usage = lru_table->usage;
}

/*@brief 资源数量
 *@param lru_table 资源缓存表
 *@param nodes     资源数量
 */
void scui_cache_lru_nodes(scui_cache_lru_table_t *lru_table, uint32_t *nodes)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(nodes != NULL);
    
    *nodes = lru_table->nodes;
}

/*@brief 资源缓存清除
 *@param lru_table 资源缓存表
 *@param force     强制清除
 */
void scui_cache_lru_clear(scui_cache_lru_table_t *lru_table, bool force)
{
    SCUI_ASSERT(lru_table != NULL);
    
    SCUI_LOG_WARN("nodes:%u, usage:%u", lru_table->nodes, lru_table->usage);
    SCUI_LOG_WARN("hit:%u unhit:%u pct:%.02f", lru_table->cnt_hit, lru_table->cnt_unhit,
        1.0f * lru_table->cnt_hit / (lru_table->cnt_hit + lru_table->cnt_unhit));
    
    /* 所有已解锁资源全部回收 */
    while (true) {
        /* 前向遍历,找已经解锁的资源 */
        scui_cache_lru_unit_t *lru_unit_t = NULL;
        scui_list_dll_ftra(&lru_table->dl_list, curr) {
            lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, dl_node, curr);
            if (lru_unit_t->lock == 0 || force)
                break;
            lru_unit_t = NULL;
        }
        if (lru_unit_t == NULL)
            break;
        scui_list_dll_remove(&lru_table->dl_list, &lru_unit_t->dl_node);
        scui_table_dlt_remove(&lru_table->ht_table, &lru_unit_t->ht_node);
        
        /* 约减使用率 */
        lru_table->usage -= lru_table->get_size(lru_unit_t);
        lru_table->nodes --;
        /* 卸载节点资源 */
        lru_table->del_node(lru_unit_t);
        lru_unit_t = NULL;
    }
    
    lru_table->cnt_hit = 0;
    lru_table->cnt_unhit = 0;
    SCUI_LOG_INFO("nodes:%u, usage:%u", lru_table->nodes, lru_table->usage);
}

/*@brief 资源缓存无效化(指定目标)
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_invalidate(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(lru_unit != NULL);
    
    scui_table_dln_t *unit_node = NULL;
    scui_cache_lru_unit_t *lru_unit_t = NULL;
    if ((unit_node = scui_table_dlt_search(&lru_table->ht_table, &lru_unit->ht_node)) != NULL)
        lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (lru_unit_t != NULL) {
        scui_list_dll_remove(&lru_table->dl_list, &lru_unit_t->dl_node);
        scui_table_dlt_remove(&lru_table->ht_table, &lru_unit_t->ht_node);
        SCUI_ASSERT(lru_unit_t->lock == 0);
        
        /* 约减使用率 */
        lru_table->usage -= lru_table->get_size(lru_unit_t);
        lru_table->nodes --;
        /* 卸载节点资源 */
        lru_table->del_node(lru_unit_t);
        lru_unit_t = NULL;
    }
}

/*@brief 资源缓存卸载
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_unload(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(lru_unit != NULL);
    
    scui_table_dln_t *unit_node = NULL;
    scui_cache_lru_unit_t *lru_unit_t = NULL;
    if ((unit_node = scui_table_dlt_search(&lru_table->ht_table, &lru_unit->ht_node)) != NULL)
        lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (lru_unit_t != NULL)
    if (lru_unit_t->lock != 0)
        lru_unit_t->lock--;
}

/*@brief 资源缓存加载
 *@brief lru_table 资源缓存表
 *@brief lru_unit  资源缓存节点
 */
void scui_cache_lru_load(scui_cache_lru_table_t *lru_table, scui_cache_lru_unit_t *lru_unit)
{
    SCUI_ASSERT(lru_table != NULL);
    SCUI_ASSERT(lru_unit != NULL);
    
    scui_table_dln_t *unit_node = NULL;
    scui_cache_lru_unit_t *lru_unit_t = NULL;
    if ((unit_node = scui_table_dlt_search(&lru_table->ht_table, &lru_unit->ht_node)) != NULL)
        lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, ht_node, unit_node);
    
    /* 如果缓存命中时 */
    if (lru_unit_t != NULL) {
        /* 上锁 */
        lru_unit_t->lock++;
        /* 正常情况下缓存锁不可能嵌套太多 */
        SCUI_ASSERT(lru_unit_t->lock < 0x64);
        /* 命中缓存资源计数加 */
        if (lru_unit_t->count != 0 && lru_unit_t->count < 0x64) {
            lru_unit_t->count++;
            /* 重新带计数优先级加入 */
            /* scui_list_dll_remove(&lru_table->dl_list, &lru_unit_t->dl_node); */
            /* scui_queue_dlpq_enqueue(&lru_table->dl_list, &lru_unit_t->dl_node, scui_cache_lru_sort); */
            scui_queue_dlpq_adjust(&lru_table->dl_list, &lru_unit_t->dl_node, true, scui_cache_lru_sort);
        }
        
        lru_table->cpy_node(lru_unit, lru_unit_t);
        lru_table->cnt_hit++;
        return;
    }
    
    /* 对缓存计数器进行一次重衰减(rewind),老化它 */
    if ((unit_node = scui_list_dll_tail(&lru_table->dl_list)) != NULL) {
         lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, dl_node, unit_node);
         scui_multi_t count = lru_unit_t->count;
         if (count > 0x07) {
             /* scui_list_dll_ftra(&lru_table->dl_list, curr) */
                scui_list_dll_btra(&lru_table->dl_list, curr) {
                lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, dl_node, curr);
                lru_unit_t->count -= count;
             }
         }
         unit_node = NULL;
         lru_unit_t = NULL;
    }
    /* 如果缓存未命中时 */
    if (lru_unit_t == NULL) {
        /* 申请节点资源 */
        lru_table->new_node(&lru_unit_t, lru_unit);
        SCUI_ASSERT(lru_unit_t != NULL);
        /* 更新节点状态 */
        lru_unit_t->count = 1;
        lru_unit_t->lock  = 1;
        lru_table->usage += lru_table->get_size(lru_unit_t);
        lru_table->nodes ++;
        lru_table->cpy_node(lru_unit, lru_unit_t);
        lru_table->cnt_unhit++;
        lru_unit = lru_unit_t;
        
        scui_multi_t unit_size = lru_table->get_size(lru_unit_t);
        /* 如果缓存空间不足时,老化资源回收 */
        while (lru_table->usage + unit_size > lru_table->total) {
            /* SCUI_ASSERT(lru_table->usage <= lru_table->total); */
            /* 前向遍历,找已经解锁的资源 */
            scui_list_dll_ftra(&lru_table->dl_list, curr) {
                lru_unit_t = scui_own_ofs(scui_cache_lru_unit_t, dl_node, curr);
                if (lru_unit_t->lock == 0)
                    break;
                lru_unit_t = NULL;
            }
            if (lru_unit_t == NULL) {
                SCUI_LOG_ERROR("cache legacy excess");
                scui_cache_lru_visit(lru_table);
                scui_cache_lru_clear(lru_table, false);
                break;
            }
            scui_list_dll_remove(&lru_table->dl_list, &lru_unit_t->dl_node);
            scui_table_dlt_remove(&lru_table->ht_table, &lru_unit_t->ht_node);
            
            /* 约减使用率 */
            lru_table->usage -= lru_table->get_size(lru_unit_t);
            lru_table->nodes --;
            /* 卸载节点资源 */
            lru_table->del_node(lru_unit_t);
            lru_unit_t = NULL;
        }
        lru_unit_t = lru_unit;
        /* 带计数优先级加入 */
        scui_list_dln_reset(&lru_unit_t->dl_node);
        scui_queue_dlpq_enqueue(&lru_table->dl_list, &lru_unit_t->dl_node, scui_cache_lru_sort);
        /* 哈希表保存查询记录 */
        scui_table_dln_reset(&lru_unit_t->ht_node);
        scui_table_dlt_insert(&lru_table->ht_table, &lru_unit_t->ht_node);
    }
}
