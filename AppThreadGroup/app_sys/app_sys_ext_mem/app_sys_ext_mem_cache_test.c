/*实现目标:
 *    抽象外存资源的缓存管理测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_list.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_cache.h"
#include "app_sys_ext_mem_table.h"


/*@brief 抽象外存资源的缓存管理测试
 */
void app_sys_ext_mem_cache_test(void)
{
    /* 开辟缓存地址 */
    uint8_t *buffer = NULL;
    app_sys_ext_mem_cache_t static_memory_cache = {0};
    app_sys_ext_mem_cache_ready(&static_memory_cache, app_sys_ext_mem_find_by_name("static_memory"), 1024, 10 * 1024);
    
    uint32_t unit_cnt  = 10000;
    uint32_t wheel_cnt = 3;
    uint32_t total_cnt = unit_cnt * wheel_cnt;
    uint32_t hit_cnt = 0;
    uint32_t unhit_cnt = 0;
    /* 循环测试 */
    for (uint32_t cnt = 0; cnt <= total_cnt; cnt++) {
        /*  */
        uintptr_t block_ofs = (50 / (wheel_cnt + 1)) * (1 + cnt / unit_cnt);
        /* 缓存命中测试 */
        uint32_t retval = app_sys_ext_mem_cache_take(&static_memory_cache, rand() % block_ofs * 1024, 1024, &buffer);
        hit_cnt   += retval == app_sys_ext_mem_cache_hit ? 1 : 0;
        unhit_cnt += retval == app_sys_ext_mem_cache_unhit ? 1 : 0;
        for (uint32_t idx = 0; idx < 1024; buffer[idx] += 1, idx++);
        app_sys_ext_mem_cache_give(&static_memory_cache, buffer, true);
        
        if (cnt != 0 && cnt % unit_cnt == 0) {
            APP_SYS_LOG_INFO("cache unit:%u",  static_memory_cache.unit);
            APP_SYS_LOG_INFO("cache total:%u", static_memory_cache.total);
            APP_SYS_LOG_INFO("cache usage:%u", static_memory_cache.usage);
            app_sys_ext_mem_cache_reflush(&static_memory_cache, false);
            APP_SYS_LOG_INFO("cache usage:%u", static_memory_cache.usage);
            APP_SYS_LOG_INFO("hit_cnt:%d,   predict:(%d,%d)%f, reality:%f",   hit_cnt, 10, block_ofs,       (float)10 / block_ofs, (float)  hit_cnt / (hit_cnt + unhit_cnt));
            APP_SYS_LOG_INFO("unhit_cnt:%d, predict:(%d,%d)%f, reality:%f", unhit_cnt, 10, block_ofs, 1.0 - (float)10 / block_ofs, (float)unhit_cnt / (hit_cnt + unhit_cnt));
            hit_cnt = 0;
            unhit_cnt = 0;
        }
    }
    APP_SYS_LOG_INFO("recycle");
    APP_SYS_LOG_INFO("cache unit:%u",  static_memory_cache.unit);
    APP_SYS_LOG_INFO("cache total:%u", static_memory_cache.total);
    APP_SYS_LOG_INFO("cache usage:%u", static_memory_cache.usage);
    app_sys_ext_mem_cache_reflush(&static_memory_cache, true);
    APP_SYS_LOG_INFO("cache usage:%u", static_memory_cache.usage);
}
