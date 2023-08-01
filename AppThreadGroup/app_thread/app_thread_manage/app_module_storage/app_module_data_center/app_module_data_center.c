/*实现目标:
 *    系统相关的功能组件
 *    全局的记录所有杂项数据资源
 *    注意:被注入到这里的数据资源是不需要内存常驻的
 *         这能有效节约足够的内存资源
 *
 *         仅需使用时加载并转储
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_module_data_center.h"

/* 追加一个静态断言,如果缓存块超出目标,则迫使它编译失败 */
app_expr_assert_static(app_module_data_center_size, sizeof(app_module_data_center_t) <= 512);

static app_mutex_t app_module_data_center_mutex = {0};
static app_module_data_center_t app_module_data_center = {0};
static uint32_t app_module_data_center_crc32 = 0;
static uint32_t app_module_data_center_type = 0;

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
static const char * app_module_data_center_get_ext_src_by_type(void)
{
    /* 数据中心类型与数据源映射表 */
    static const struct {
        app_module_data_center_type_t type;
        const char *ext_src;
    } app_module_data_center_type_table[] = {
        {app_module_data_center_system_profile, "system profile"},
        {app_module_data_center_user_profile,   "user profile"},
        {app_module_data_center_user_goal,      "user goal"},
        {app_module_data_center_user_dev_info,  "user dev info"},
    };
    
    for (uint32_t idx = 0; idx < app_arr_len(app_module_data_center_type_table); idx++)
        if (app_module_data_center_type_table[idx].type == app_module_data_center_type)
            return app_module_data_center_type_table[idx].ext_src;
    APP_SYS_LOG_ERROR("data center catch unknown type:%u", app_module_data_center_type);
    APP_SYS_ASSERT(NULL != NULL);
}

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
void app_module_data_center_reset(void)
{
    APP_SYS_ASSERT(app_module_data_center_type > app_module_data_center_none);
    APP_SYS_ASSERT(app_module_data_center_type < app_module_data_center_num);
    
    app_module_data_center_t *data_center = &app_module_data_center;
    
    switch (app_module_data_center_type) {
    case app_module_data_center_system_profile:
        break;
    case app_module_data_center_user_profile:
        break;
    case app_module_data_center_user_goal:
        break;
    case app_module_data_center_user_dev_info:
        break;
    default:
        break;
    }
    
    app_module_data_center_crc32 = app_sys_crc32(&app_module_data_center, sizeof(app_module_data_center_t));
}

/*@brief      获得数据中心的数据源,直接本地修改
 *@param[out] data_center数据中心索引地址
 */
void app_module_data_center_source(app_module_data_center_t **data_center)
{
    *data_center = &app_module_data_center;
}

/*@brief 更换目标数据类型并锁定数据中心,静止更换其他类型数据
 */
void app_module_data_center_load(uint32_t type)
{
    union {
        uint8_t buffer[0];
        struct {
            uint32_t crc32;
            app_module_data_center_t data_center;
        };
    } data_center_data;
    
    APP_SYS_ASSERT(app_module_data_center_num > type);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    /* 本地缓存命中时不要再继续加载了 */
    if (app_module_data_center_type == type)
        return;
    app_module_data_center_type = type;
    const char *ext_src = app_module_data_center_get_ext_src_by_type();
    app_sys_ext_src_read("mix_chunk_small", ext_src, data_center_data.buffer, sizeof(data_center_data));
    app_module_data_center_crc32 = app_sys_crc32(&data_center_data.data_center, sizeof(app_module_data_center_t));
    /* 如果当次校验与最开始加载时不一样表明数据无效化了 */
    if (data_center_data.crc32 == app_module_data_center_crc32)
        memcpy(&app_module_data_center, &data_center_data.data_center, sizeof(app_module_data_center_t));
    if (data_center_data.crc32 != app_module_data_center_crc32) {
        APP_SYS_LOG_WARN("load data center fail:%d", type);
        app_module_data_center_reset();
    }
}

/*@brief 更新目标数据类型并锁定数据中心,允许换其他类型数据
 */
void app_module_data_center_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            uint32_t crc32;
            app_module_data_center_t data_center;
        };
    } data_center_data;
    
    memcpy(&data_center_data.data_center, &app_module_data_center, sizeof(app_module_data_center_t));
    data_center_data.crc32 = app_sys_crc32(&data_center_data.data_center, sizeof(app_module_data_center_t));
    /* 数据未修改,不做实际转存 */
    if (data_center_data.crc32 != app_module_data_center_crc32) {
        const char *ext_src = app_module_data_center_get_ext_src_by_type();
        app_sys_ext_src_write("mix_chunk_small", ext_src, data_center_data.buffer, sizeof(data_center_data));
    }
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
}

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void)
{
    app_mutex_process(&app_module_data_center_mutex, app_mutex_static);
}
