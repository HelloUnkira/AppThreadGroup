/*实现目标:
 *    系统相关的功能组件
 *    全局的记录所有杂项数据资源
 *    注意:被注入到这里的数据资源是不需要内存常驻的
 *         这能有效节约足够的内存资源

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
static uint32_t app_module_data_center_type = 0;

/* 追加一个静态断言,如果缓存块超出目标,则迫使它编译失败 */
static uint8_t app_module_data_center_static_assert[sizeof(app_module_data_center_t) <= 512 ? 1 : -1];

/*@brief      获得数据中心的数据源,直接本地修改
 *@param[out] data_center数据中心索引地址
 *@param[out] type 当前数据缓存类型
 */
void app_module_data_center_source(app_module_data_center_t **data_center, uint32_t *type)
{
    *data_center = &app_module_data_center;
    *type = app_module_data_center_type;
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
    
    APP_SYS_ASSERT(app_module_data_center_number > type);
    app_mutex_take(&app_module_data_center_mutex);
    memset(&data_center_data, 0, sizeof(data_center_data));
    app_module_data_center_type = type;
    switch (app_module_data_center_type)
    {
    case app_module_data_center_system_profile:
        app_sys_ext_src_read("mix_chunk_small", APP_MODULE_DATA_CENTER_SYSTEM_PROFILE,
                              data_center_data.buffer, sizeof(data_center_data));
        break;
    case app_module_data_center_user_profile:
        app_sys_ext_src_read("mix_chunk_small", APP_MODULE_DATA_CENTER_USER_PROFILE,
                              data_center_data.buffer, sizeof(data_center_data));
        break;
    default:
        break;
    }
    uint32_t crc32 = app_sys_crc32(&data_center_data.data_center, sizeof(app_module_data_center_t));
    if (crc32 != data_center_data.crc32) {
        memset(&data_center_data, 0, sizeof(data_center_data));
        APP_SYS_LOG_WARN("load data center fail:%d", type);
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
    
    data_center_data.crc32 = app_sys_crc32(&data_center_data.data_center, sizeof(app_module_data_center_t));
    switch (app_module_data_center_type)
    {
    case app_module_data_center_system_profile:
        app_sys_ext_src_write("mix_chunk_small", APP_MODULE_DATA_CENTER_SYSTEM_PROFILE,
                               data_center_data.buffer, sizeof(data_center_data));
        break;
    case app_module_data_center_user_profile:
        app_sys_ext_src_write("mix_chunk_small", APP_MODULE_DATA_CENTER_USER_PROFILE,
                               data_center_data.buffer, sizeof(data_center_data));
        break;
    default:
        break;
    }
    app_mutex_give(&app_module_data_center_mutex);
}

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void)
{
    app_mutex_process(&app_module_data_center_mutex);
}

