/*实现目标:
 *    系统相关的功能组件
 *    全局的记录所有杂项数据资源
 *    注意:被注入到这里的数据资源是不需要内存常驻的
 *         这能有效节约足够的内存资源
 *
 *         仅需使用时加载并转储
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_list.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_cache.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_ext_src.h"
#include "app_sys_ext_src_table.h"
#include "app_sys_util.h"
#include "app_module_data_center.h"

static app_mutex_t app_module_data_center_mutex = {0};
static app_sys_ext_mem_cache_t app_module_data_center_cache = {0};
static app_module_data_center_t *app_module_data_center_handle = NULL;
static uint32_t app_module_data_center_crc32 = 0;
static uint32_t app_module_data_center_type = 0;

/* 数据中心类型与数据源映射表 */
static const struct {
    uint8_t type;
    const char *data_name;
    uint32_t data_size;
} app_module_data_center_type_table[] = {
    {app_module_data_center_module_source,      "module source",    sizeof(app_module_data_center_handle->module_source)},
    {app_module_data_center_system_profile,     "system profile",   sizeof(app_module_data_center_handle->system_profile)},
    {app_module_data_center_system_data,        "system data"   ,   sizeof(app_module_data_center_handle->system_data)},
    {app_module_data_center_user_profile,       "user profile"  ,   sizeof(app_module_data_center_handle->user_profile)},
    {app_module_data_center_user_data,          "user data"     ,   sizeof(app_module_data_center_handle->user_data)},
};

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
static void app_module_data_center_reset(void)
{
    APP_SYS_ASSERT(app_module_data_center_handle != NULL);
    APP_SYS_ASSERT(app_module_data_center_type > app_module_data_center_none);
    APP_SYS_ASSERT(app_module_data_center_type < app_module_data_center_num);
    /* 求字段大小,如sizeof(app_module_data_center_handle->module_source) */
    app_module_data_center_t *data_center = app_module_data_center_handle;
    
    switch (app_module_data_center_type) {
    case app_module_data_center_module_source:
        app_module_clock_reset(&data_center->module_source.system_clock);
        app_module_remind_alarm_reset(data_center->module_source.remind_alarm.remind_item,
                                      data_center->module_source.remind_alarm.alarm_info);
        app_module_remind_drink_reset(&data_center->module_source.remind_drink);
        app_module_remind_sedentary_reset(&data_center->module_source.remind_sedentary);
        app_module_do_not_disturb_reset(&data_center->module_source.do_not_disturb);
        break;
    case app_module_data_center_system_profile:
        break;
    case app_module_data_center_system_data:
        break;
    case app_module_data_center_user_profile:
        break;
    case app_module_data_center_user_data:
        break;
    default:
        break;
    }
}

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
static const app_sys_ext_src_t * app_module_data_center_find_ext_src_by_type(void)
{
    for (uint32_t idx = 0; idx < app_ext_arr_len(app_module_data_center_type_table); idx++)
        if (app_module_data_center_type_table[idx].type == app_module_data_center_type)
            return app_sys_ext_src_find_by_name("mix_chunk_small", app_module_data_center_type_table[idx].data_name);
    APP_SYS_LOG_ERROR("data center catch unknown type:%u", app_module_data_center_type);
    APP_SYS_ASSERT(NULL != NULL);
    return NULL;
}

/*@brief    更换目标数据类型并锁定数据中心
 *param[in] type 数据中心类型(app_module_data_center_t(type))
 *@retval   数据中心的缓存地址
 */
app_module_data_center_t * app_module_data_center_take(uint32_t type)
{
    APP_SYS_ASSERT(type > app_module_data_center_none);
    APP_SYS_ASSERT(type < app_module_data_center_num);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    app_module_data_center_type = type;
    
    APP_SYS_ASSERT(app_module_data_center_handle == NULL);
    const app_sys_ext_src_t *ext_src = app_module_data_center_find_ext_src_by_type();
    uintptr_t ofs  = app_ext_own_ofs(app_module_data_center_t, type, 0); ofs = -ofs;
    uintptr_t size = app_module_data_center_type_table[app_module_data_center_type - 1].data_size;
    app_sys_ext_mem_cache_take(&app_module_data_center_cache, ext_src->data_base, size + ofs, &app_module_data_center_handle);
    APP_SYS_LOG_INFO("data center:%d", app_module_data_center_type);
    APP_SYS_LOG_INFO("take[%p,%p]:%d", app_module_data_center_handle, app_module_data_center_handle + size + ofs, size + ofs);
    app_module_data_center_crc32 = app_sys_crc32(&app_module_data_center_handle->type, size);
    APP_SYS_ASSERT(app_module_data_center_handle != NULL);
    /* 如果当次校验与最开始加载时不一样表明数据无效化了 */
    if (app_module_data_center_handle->crc32 != app_module_data_center_crc32) {
        APP_SYS_LOG_WARN("load data center fail:%d", type);
        app_module_data_center_reset();
    }
    return app_module_data_center_handle;
}

/*@brief 解锁数据中心
 */
void app_module_data_center_give(void)
{
    APP_SYS_ASSERT(app_module_data_center_handle != NULL);
    const app_sys_ext_src_t *ext_src = app_module_data_center_find_ext_src_by_type();
    uintptr_t ofs  = app_ext_own_ofs(app_module_data_center_t, type, 0); ofs = -ofs;
    uintptr_t size = app_module_data_center_type_table[app_module_data_center_type - 1].data_size;
    app_module_data_center_handle->crc32 = app_sys_crc32(&app_module_data_center_handle->type, size);
    app_sys_ext_mem_cache_give(&app_module_data_center_cache, app_module_data_center_handle, app_module_data_center_handle->crc32 != app_module_data_center_crc32);
    APP_SYS_LOG_INFO("data center:%d", app_module_data_center_type);
    APP_SYS_LOG_INFO("give[%p,%p]:%d", app_module_data_center_handle, app_module_data_center_handle + size + ofs, size + ofs);
    app_module_data_center_handle = NULL;
    app_module_data_center_type = 0;
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
}

/*@brief     刷新数据中心
 *@param[in] force 强制刷新数据中心(不建议使用)
 */
void app_module_data_center_reflush(bool force)
{
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    app_sys_ext_mem_cache_reflush(&app_module_data_center_cache, force);
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
    for (uint32_t idx = 0; idx < app_ext_arr_len(app_module_data_center_type_table); idx++)
        data_size_all += app_module_data_center_type_table[idx].data_size;

    uint32_t unit  = sizeof(app_module_data_center_t);
    uint32_t total = sizeof(app_module_data_center_t) * (1 + data_size_all / unit);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_static);
    app_sys_ext_mem_cache_ready(&app_module_data_center_cache, app_sys_ext_mem_find_by_name("mix_chunk_small"), unit, total);
    
    APP_SYS_LOG_INFO("data_center size all:%d", data_size_all);
    APP_SYS_LOG_INFO("data_center:%d", sizeof(app_module_data_center_t));
    for (uint32_t idx = 0; idx < app_ext_arr_len(app_module_data_center_type_table); idx++)
        APP_SYS_LOG_INFO("%d:%s:%d", app_module_data_center_type_table[idx].type,
                                     app_module_data_center_type_table[idx].data_name,
                                     app_module_data_center_type_table[idx].data_size);
}
