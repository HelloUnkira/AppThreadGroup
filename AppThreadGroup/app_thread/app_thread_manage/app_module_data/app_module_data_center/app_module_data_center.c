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

static app_mutex_t app_module_data_center_mutex = {0};
static app_module_data_center_t app_module_data_center = {0};
static bool app_module_data_center_valid = false;
static uint32_t app_module_data_center_crc32 = 0;
static uint32_t app_module_data_center_type = 0;

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
static void app_module_data_center_get_ext_src_by_type(char **chunk_name, char **data_name, uint32_t *data_size)
{
    const app_module_data_center_t data_center = {0};
    /* 数据中心类型与数据源映射表 */
    const char *ext_mem_name = "mix_chunk_small";
    static const struct {
        uint8_t     type;
        const char *ext_src;
        uint32_t    ext_src_size;
    } app_module_data_center_type_table[] = {
        {app_module_data_center_system_profile,     "system profile",   sizeof(data_center.system_profile)},
        {app_module_data_center_system_data,        "system data"   ,   sizeof(data_center.system_data)},
        {app_module_data_center_user_profile,       "user profile"  ,   sizeof(data_center.user_profile)},
        {app_module_data_center_user_data,          "user data"     ,   sizeof(data_center.user_data)},
    };
    
    for (uint32_t idx = 0; idx < app_arr_len(app_module_data_center_type_table); idx++)
        if (app_module_data_center_type_table[idx].type == app_module_data_center_type) {
            *chunk_name = ext_mem_name;
            *data_name  = app_module_data_center_type_table[idx].ext_src;
            *data_size  = app_module_data_center_type_table[idx].ext_src_size;
            return;
        }
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
    memset(data_center, 0, sizeof(app_module_data_center_t));
    
    switch (app_module_data_center_type) {
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

/*@brief      获得数据中心的数据源,直接本地修改
 *@param[out] data_center数据中心索引地址
 *@retval     数据中心的数据是否无效
 */
bool app_module_data_center_source(app_module_data_center_t **data_center)
{
    *data_center = &app_module_data_center;
    return app_module_data_center_valid;
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
    memset(&data_center_data, 0, sizeof(data_center_data));
    
    APP_SYS_ASSERT(app_module_data_center_num > type);
    app_mutex_process(&app_module_data_center_mutex, app_mutex_take);
    /* 本地缓存命中时不要再继续加载了 */
    if (app_module_data_center_type == type)
        return;
    app_module_data_center_type = type;
    char    *chunk_name = NULL;
    char    *data_name  = NULL;
    uint32_t data_size  = 0;
    app_module_data_center_get_ext_src_by_type(&chunk_name, &data_name, &data_size);
    app_sys_ext_src_read(chunk_name, data_name, data_center_data.buffer,
                         sizeof(data_center_data) - sizeof(data_center_data.data_center) + data_size);
    app_module_data_center_crc32 = app_sys_crc32(&data_center_data.data_center, data_size);
    /* 如果当次校验与最开始加载时不一样表明数据无效化了 */
    if (data_center_data.crc32 == app_module_data_center_crc32) {
        app_module_data_center_valid = true;
        memcpy(&app_module_data_center, &data_center_data.data_center, sizeof(app_module_data_center_t));
    }
    if (data_center_data.crc32 != app_module_data_center_crc32) {
        app_module_data_center_valid = false;
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
    memset(&data_center_data, 0, sizeof(data_center_data));
    
    char    *chunk_name = NULL;
    char    *data_name  = NULL;
    uint32_t data_size  = 0;
    app_module_data_center_get_ext_src_by_type(&chunk_name, &data_name, &data_size);
    memcpy(&data_center_data.data_center, &app_module_data_center, sizeof(app_module_data_center_t));
    data_center_data.crc32 = app_sys_crc32(&data_center_data.data_center, data_size);
    /* 数据未修改,不做实际转存 */
    if (data_center_data.crc32 != app_module_data_center_crc32) {
        app_sys_ext_src_write(chunk_name, data_name, data_center_data.buffer,
                              sizeof(data_center_data) - sizeof(data_center_data.data_center) + data_size);
    }
    app_mutex_process(&app_module_data_center_mutex, app_mutex_give);
}

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void)
{
    app_mutex_process(&app_module_data_center_mutex, app_mutex_static);
    
    const app_module_data_center_t data_center = {0};
    /* 追加一个静态断言,如果缓存块超出目标,则迫使它编译失败 */
    app_expr_assert_static(app_module_data_center_system_profile,   sizeof(data_center.system_profile)  <= 512);
    app_expr_assert_static(app_module_data_center_system_data,      sizeof(data_center.system_data)     <= 1024);
    app_expr_assert_static(app_module_data_center_user_profile,     sizeof(data_center.user_profile)    <= 1024);
    app_expr_assert_static(app_module_data_center_user_data,        sizeof((data_center.user_data))     <= 4096);
    
    APP_SYS_LOG_WARN("data_center:%d", sizeof(app_module_data_center_t));
    APP_SYS_LOG_WARN("data_center.system_profile:%d",       sizeof(data_center.system_profile));
    APP_SYS_LOG_WARN("data_center.system_data:%d",          sizeof(data_center.system_data));
    APP_SYS_LOG_WARN("data_center.user_profile:%d",         sizeof(data_center.user_profile));
    APP_SYS_LOG_WARN("data_center.user_data:%d",            sizeof(data_center.user_data));
}
