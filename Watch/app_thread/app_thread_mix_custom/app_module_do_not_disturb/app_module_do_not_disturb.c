/*实现目标:
 *    勿扰模式相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_module_clock.h"
#include "app_module_do_not_disturb.h"

static app_mutex_t app_module_do_not_disturb_mutex = {0};
static app_module_do_not_disturb_t app_module_do_not_disturb = {0};

/*@brief  勿扰模式状态检查
 *@retval 是否为勿扰态
 */
bool app_module_do_not_disturb_status(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    /* 单个秒内的多发访问 */
    static bool last_status  = false;
    static uint64_t last_utc = 0;
    if (last_utc == clock.utc)
        return last_status;
    last_utc = clock.utc;
    
    bool retval = false;    
    app_mutex_take(&app_module_do_not_disturb_mutex);
    /* 今天需要勿扰 */
    if ((app_module_do_not_disturb.week & (1 << clock.week)) != 0)
    for (uint8_t idx = 0; idx < APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM; idx++)
    if ((app_module_do_not_disturb.valid_bits & (1 << idx)) != 0)
    for (uint8_t idx = 0; idx < APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM; idx++) {
        uint16_t c_secs   = clock.hour * 60 * 60 + clock.minute * 60 + clock.second;
        uint16_t t_secs_s = app_module_do_not_disturb.table[idx].time_s[0] * 60 * 60 +
                            app_module_do_not_disturb.table[idx].time_s[1] * 60 +
                            app_module_do_not_disturb.table[idx].time_s[2];
        uint16_t t_secs_e = app_module_do_not_disturb.table[idx].time_e[0] * 60 * 60 +
                            app_module_do_not_disturb.table[idx].time_e[1] * 60 +
                            app_module_do_not_disturb.table[idx].time_e[2];
        if (t_secs_s <= c_secs && c_secs <= t_secs_e) {
            retval = true;
            break;
        }
    }
    app_mutex_give(&app_module_do_not_disturb_mutex);
    return retval;
}

/*@brief 勿扰模式默认设置
 */
void app_module_do_not_disturb_reset(void)
{
    /* 9:00~11:30;13:00~18:00;workday */
    app_mutex_take(&app_module_do_not_disturb_mutex);
    app_module_do_not_disturb.table[0].time_s[0] = 9;
    app_module_do_not_disturb.table[0].time_s[1] = 0;
    app_module_do_not_disturb.table[0].time_s[2] = 0;
    app_module_do_not_disturb.table[0].time_e[0] = 11;
    app_module_do_not_disturb.table[0].time_e[1] = 30;
    app_module_do_not_disturb.table[0].time_e[2] = 0;
    app_module_do_not_disturb.table[1].time_s[0] = 13;
    app_module_do_not_disturb.table[1].time_s[1] = 0;
    app_module_do_not_disturb.table[1].time_s[2] = 0;
    app_module_do_not_disturb.table[1].time_e[0] = 18;
    app_module_do_not_disturb.table[1].time_e[1] = 0;
    app_module_do_not_disturb.table[1].time_e[2] = 0;
    app_module_do_not_disturb.valid_bits = 0b11;
    app_module_do_not_disturb.week = 0b0111110;
    app_mutex_give(&app_module_do_not_disturb_mutex);
}

/*@brief     勿扰模式设置
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_set(app_module_do_not_disturb_t *do_not_disturb)
{
    app_mutex_take(&app_module_do_not_disturb_mutex);
    app_module_do_not_disturb = *do_not_disturb;
    app_mutex_give(&app_module_do_not_disturb_mutex);
}

/*@brief     勿扰模式获取
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_get(app_module_do_not_disturb_t *do_not_disturb)
{
    app_mutex_take(&app_module_do_not_disturb_mutex);
    *do_not_disturb = app_module_do_not_disturb;
    app_mutex_give(&app_module_do_not_disturb_mutex);
}

/*@brief 勿扰状态转储到外存
 */
void app_module_do_not_disturb_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_do_not_disturb_t do_not_disturb;
            uint32_t crc32;
        } data;
    } do_not_disturb_data = {.data.do_not_disturb = app_module_do_not_disturb};
    
    do_not_disturb_data.data.crc32 = app_sys_crc32(do_not_disturb_data.buffer, sizeof(app_module_do_not_disturb_t));
    app_sys_ext_src_write("mix_chunk_small", "do not disturb", do_not_disturb_data.buffer, sizeof(do_not_disturb_data));
}

/*@brief 勿扰状态加载到内存
 */
void app_module_do_not_disturb_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_do_not_disturb_t do_not_disturb;
            uint32_t crc32;
        } data;
    } do_not_disturb_data;
    
    app_sys_ext_src_read("mix_chunk_small", "do not disturb", do_not_disturb_data.buffer, sizeof(do_not_disturb_data));
    uint32_t crc32 = app_sys_crc32(do_not_disturb_data.buffer, sizeof(app_module_do_not_disturb_t));
    if (crc32 == do_not_disturb_data.data.crc32) {
        app_mutex_take(&app_module_do_not_disturb_mutex);
        app_module_do_not_disturb = do_not_disturb_data.data.do_not_disturb;
        app_mutex_give(&app_module_do_not_disturb_mutex);
    }
    if (crc32 != do_not_disturb_data.data.crc32) {
        app_module_do_not_disturb_reset();
        APP_SYS_LOG_WARN("load do not disturb fail");
    }
}

/*@brief 勿扰模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_do_not_disturb_ready(void)
{
    app_mutex_process(&app_module_do_not_disturb_mutex);
}



