/*实现目标:
 *    一些小混合资源加载与存储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_checksum.h"
#include "app_module_source.h"
#include "app_module_clock.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"

/*@brief 系统时钟转储到外存
 */
void app_module_clock_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t crc32;
        };
    } clock_data = {};
    
    app_module_clock_get_system_clock(&clock_data.clock);
    clock_data.crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    app_module_source_write("mix_chunk_small", "system clock", clock_data.buffer, sizeof(clock_data));
}

/*@brief 系统时钟加载到内存
 */
void app_module_clock_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t crc32;
        };
    } clock_data = {};
    
    app_module_source_read("mix_chunk_small", "system clock", clock_data.buffer, sizeof(clock_data));
    uint32_t crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    if (crc32 == clock_data.crc32)
        app_module_clock_set_system_clock(&clock_data.clock);
    if (crc32 != clock_data.crc32) {
        app_module_clock_t clock = {.year = 2020, .month = 1, .day = 1,};
        app_module_clock_to_week(&clock);
        app_module_clock_to_utc(&clock);
        app_module_clock_set_system_clock(&clock);
        APP_SYS_LOG_WARN("load system clock fail");
    }
}

