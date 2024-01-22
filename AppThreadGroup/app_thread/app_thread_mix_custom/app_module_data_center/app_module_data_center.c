/*实现目标:
 *    带缓存机制的数据中心
 *    全局记录所有杂项数据资源
 *    被注入到这里的数据资源是不需要内存常驻的
 *    能有效节约足够的内存资源
 *    数据仅需使用时加载并转储
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_mutex_t app_module_data_center_mutex = {0};
static app_sys_ext_mem_cache_t app_module_data_center_cache = {0};
/* 数据中心类型与数据源映射表 */
extern uint32_t app_module_data_center_src_crc32;
extern app_module_data_center_src_t *app_module_data_center_src;
extern app_module_data_center_src_type_t app_module_data_center_src_type;
extern app_module_data_center_src_table_t app_module_data_center_src_table[];
extern uint32_t app_module_data_center_src_table_size;

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
void app_module_data_center_src_reset(void);

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
static const app_sys_ext_mem_src_t * app_module_data_center_find_ext_src_by_type(void)
{
    for (uint32_t idx = 0; idx < app_module_data_center_src_table_size; idx++)
        if (app_module_data_center_src_table[idx].type == app_module_data_center_src_type)
            return app_sys_ext_mem_src_find_by_name("mix_chunk_small", app_module_data_center_src_table[idx].data_name);
    APP_SYS_LOG_ERROR("data center catch unknown type:%u", app_module_data_center_src_type);
    APP_SYS_ASSERT(NULL != NULL);
    return NULL;
}

/*@brief 更换目标数据类型并锁定数据中心
 *@param type 数据中心类型(app_module_data_center_src_t(type))
 *@retval 数据中心的缓存地址
 */
app_module_data_center_src_t * app_module_data_center_take(uint32_t type)
{
    APP_SYS_ASSERT(type > app_module_data_center_src_none);
    APP_SYS_ASSERT(type < app_module_data_center_src_num);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    app_module_data_center_src_type = type;
    
    APP_SYS_ASSERT(app_module_data_center_src == NULL);
    const app_sys_ext_mem_src_t *ext_src = app_module_data_center_find_ext_src_by_type();
    uintptr_t ofs  = app_sys_own_ofs(app_module_data_center_src_t, offset, 0); ofs = -ofs;
    uintptr_t size = app_module_data_center_src_table[app_module_data_center_src_type - 1].data_size;
    app_sys_ext_mem_cache_take(&app_module_data_center_cache, ext_src->data_base, size + ofs, &app_module_data_center_src);
    APP_SYS_LOG_INFO("data center:%d", app_module_data_center_src_type);
    APP_SYS_LOG_INFO("take[%p,%p]:%d", app_module_data_center_src, app_module_data_center_src + size + ofs, size + ofs);
    app_module_data_center_src_crc32 = app_sys_crc32(&app_module_data_center_src->offset, size);
    APP_SYS_ASSERT(app_module_data_center_src != NULL);
    /* 如果当次校验与最开始加载时不一样表明数据无效化了 */
    if (app_module_data_center_src->crc32 != app_module_data_center_src_crc32) {
        APP_SYS_LOG_WARN("load data center fail:%d", type);
        app_module_data_center_src_reset();
    }
    return app_module_data_center_src;
}

/*@brief 解锁数据中心
 */
void app_module_data_center_give(void)
{
    APP_SYS_ASSERT(app_module_data_center_src != NULL);
    const app_sys_ext_mem_src_t *ext_src = app_module_data_center_find_ext_src_by_type();
    uintptr_t ofs  = app_sys_own_ofs(app_module_data_center_src_t, offset, 0); ofs = -ofs;
    uintptr_t size = app_module_data_center_src_table[app_module_data_center_src_type - 1].data_size;
    app_module_data_center_src->crc32 = app_sys_crc32(&app_module_data_center_src->offset, size);
    app_sys_ext_mem_cache_give(&app_module_data_center_cache, ext_src->data_base, app_module_data_center_src, app_module_data_center_src->crc32 != app_module_data_center_src_crc32);
    APP_SYS_LOG_INFO("data center:%d", app_module_data_center_src_type);
    APP_SYS_LOG_INFO("give[%p,%p]:%d", app_module_data_center_src, app_module_data_center_src + size + ofs, size + ofs);
    app_module_data_center_src = NULL;
    app_module_data_center_src_type = 0;
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
}

/*@brief 刷新数据中心
 */
void app_module_data_center_reflush(void)
{
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    app_sys_ext_mem_cache_reflush(&app_module_data_center_cache);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
    APP_SYS_LOG_INFO("data center cache usage:%d", app_module_data_center_cache.usage);
    APP_SYS_LOG_INFO("data center cache total:%d", app_module_data_center_cache.total);
    APP_SYS_LOG_INFO("data center cache hit:%d",   app_module_data_center_cache.cnt_hit);
    APP_SYS_LOG_INFO("data center cache unhit:%d", app_module_data_center_cache.cnt_unhit);
}

/*@brief 回收数据中心资源
 *@param force 强制回收数据中心资源(不建议使用)
 *       不建议使用,这会导致非正常的逻辑
 */
void app_module_data_center_recycle(bool force)
{
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    app_sys_ext_mem_cache_recycle(&app_module_data_center_cache, force);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
    APP_SYS_LOG_INFO("data center cache usage:%d", app_module_data_center_cache.usage);
    APP_SYS_LOG_INFO("data center cache total:%d", app_module_data_center_cache.total);
    APP_SYS_LOG_INFO("data center cache hit:%d",   app_module_data_center_cache.cnt_hit);
    APP_SYS_LOG_INFO("data center cache unhit:%d", app_module_data_center_cache.cnt_unhit);
}

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void)
{
    uint32_t data_size_all = 0;
    for (uint32_t idx = 0; idx < app_module_data_center_src_table_size; idx++)
        data_size_all += app_module_data_center_src_table[idx].data_size;

    uint32_t unit  = sizeof(app_module_data_center_src_t);
    uint32_t total = sizeof(app_module_data_center_src_t) * (1 + data_size_all / unit);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_static);
    app_sys_ext_mem_cache_ready(&app_module_data_center_cache, app_sys_ext_mem_find_by_name("mix_chunk_small"), unit, total);
    
    APP_SYS_LOG_INFO("data_center_src size all:%d", data_size_all);
    APP_SYS_LOG_INFO("data_center_src:%d", sizeof(app_module_data_center_src_t));
    for (uint32_t idx = 0; idx < app_module_data_center_src_table_size; idx++)
        APP_SYS_LOG_INFO("%02d:%s:%d", app_module_data_center_src_table[idx].type,
                                       app_module_data_center_src_table[idx].data_name,
                                       app_module_data_center_src_table[idx].data_size);
}
