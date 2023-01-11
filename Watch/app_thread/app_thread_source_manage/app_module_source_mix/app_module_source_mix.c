/*实现目标:
 *    一些小混合资源加载与存储相关的功能组件
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_thread_interface.h"

/*@brief 系统时钟转储到外存
 */
void app_module_clock_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } clock_data = {};
    
    app_module_clock_get_system_clock(&clock_data.clock);
    clock_data.crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    clock_data.checksum32 = app_sys_checksum32(clock_data.buffer, sizeof(app_module_clock_t));
    app_module_source_write("thread_mix_custom", "system clock", clock_data.buffer, sizeof(clock_data));
}

/*@brief 系统时钟加载到内存
 */
void app_module_clock_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } clock_data = {};
    
    app_module_source_read("thread_mix_custom", "system clock", clock_data.buffer, sizeof(clock_data));
    uint32_t checksum32 = app_sys_checksum32(clock_data.buffer, sizeof(app_module_clock_t));
    uint32_t crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    if (checksum32 == clock_data.checksum32 && crc32 == clock_data.crc32)
        app_module_clock_set_system_clock(&clock_data.clock);
    if (checksum32 != clock_data.checksum32 || crc32 != clock_data.crc32) {
        app_module_clock_t clock = {.year = 2020, .month = 1, .day = 1,};
        app_module_clock_to_week(&clock);
        app_module_clock_to_utc(&clock);
        app_module_clock_set_system_clock(&clock);
        APP_OS_LOG_WARN("app_module_clock_load: load system clock fail\n");
    }
}

/*@brief 秒表转储到外存
 */
void app_module_stopwatch_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_stopwatch_t stopwatch;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } stopwatch_data = {};
    
    app_module_stopwatch_get(&stopwatch_data.stopwatch);
    stopwatch_data.crc32 = app_sys_crc32(stopwatch_data.buffer, sizeof(app_module_stopwatch_t));
    stopwatch_data.checksum32 = app_sys_checksum32(stopwatch_data.buffer, sizeof(app_module_stopwatch_t));
    app_module_source_write("thread_mix_custom", "system stopwatch", stopwatch_data.buffer, sizeof(stopwatch_data));
}

/*@brief 秒表加载到内存
 */
void app_module_stopwatch_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_stopwatch_t stopwatch;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } stopwatch_data = {};
    
    app_module_source_read("thread_mix_custom", "system stopwatch", stopwatch_data.buffer, sizeof(stopwatch_data));
    uint32_t checksum32 = app_sys_checksum32(stopwatch_data.buffer, sizeof(app_module_stopwatch_t));
    uint32_t crc32 = app_sys_crc32(stopwatch_data.buffer, sizeof(app_module_stopwatch_t));
    if (checksum32 == stopwatch_data.checksum32 && crc32 == stopwatch_data.crc32)
        app_module_stopwatch_set(&stopwatch_data.stopwatch);
    if (checksum32 != stopwatch_data.checksum32 || crc32 != stopwatch_data.crc32)
        APP_OS_LOG_WARN("app_module_stopwatch_load: load stopwatch fail\n");
}

/*@brief 倒计时转储到外存
 */
void app_module_countdown_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_countdown_t countdown;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } countdown_data = {};
    
    app_module_countdown_get(&countdown_data.countdown);
    countdown_data.crc32 = app_sys_crc32(countdown_data.buffer, sizeof(app_module_countdown_t));
    countdown_data.checksum32 = app_sys_checksum32(countdown_data.buffer, sizeof(app_module_countdown_t));
    app_module_source_write("thread_mix_custom", "system countdown", countdown_data.buffer, sizeof(countdown_data));
}

/*@brief 倒计时加载到内存
 */
void app_module_countdown_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_countdown_t countdown;
            uint32_t checksum32;
            uint32_t crc32;
        };
    } countdown_data = {};
    
    app_module_source_read("thread_mix_custom", "system countdown", countdown_data.buffer, sizeof(countdown_data));
    uint32_t checksum32 = app_sys_checksum32(countdown_data.buffer, sizeof(app_module_countdown_t));
    uint32_t crc32 = app_sys_crc32(countdown_data.buffer, sizeof(app_module_countdown_t));
    if (checksum32 == countdown_data.checksum32 && crc32 == countdown_data.crc32)
        app_module_countdown_set(&countdown_data.countdown);
    if (checksum32 != countdown_data.checksum32 || crc32 != countdown_data.crc32)
        APP_OS_LOG_WARN("app_module_countdown_load: load countdown fail\n");
}
