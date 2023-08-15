/*实现目标:
 *    勿扰模式相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_module_clock.h"
#include "app_module_data_center.h"
#include "app_module_do_not_disturb.h"

static app_mutex_t app_module_do_not_disturb_mutex = {0};
static app_module_do_not_disturb_t app_module_do_not_disturb = {0};

/*@brief     勿扰模式设置
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_set(app_module_do_not_disturb_t *do_not_disturb)
{
    app_mutex_process(&app_module_do_not_disturb_mutex, app_mutex_take);
    app_module_do_not_disturb = *do_not_disturb;
    app_mutex_process(&app_module_do_not_disturb_mutex, app_mutex_give);
}

/*@brief      勿扰模式获取
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_get(app_module_do_not_disturb_t *do_not_disturb)
{
    app_mutex_process(&app_module_do_not_disturb_mutex, app_mutex_take);
    *do_not_disturb = app_module_do_not_disturb;
    app_mutex_process(&app_module_do_not_disturb_mutex, app_mutex_give);
}

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
    app_module_do_not_disturb_t do_not_disturb;
    app_module_do_not_disturb_get(&do_not_disturb);
    /* 今天需要勿扰 */
    if ((do_not_disturb.week & (1 << clock.week)) != 0)
    for (uint8_t idx = 0; idx < APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM; idx++)
    if ((do_not_disturb.valid_bits & (1 << idx)) != 0)
    for (uint8_t idx = 0; idx < APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM; idx++) {
        uint16_t c_secs   = clock.hour * 60 * 60 + clock.minute * 60 + clock.second;
        uint16_t t_secs_s = do_not_disturb.table[idx].time_s[0] * 60 * 60 +
                            do_not_disturb.table[idx].time_s[1] * 60 +
                            do_not_disturb.table[idx].time_s[2];
        uint16_t t_secs_e = do_not_disturb.table[idx].time_e[0] * 60 * 60 +
                            do_not_disturb.table[idx].time_e[1] * 60 +
                            do_not_disturb.table[idx].time_e[2];
        if (t_secs_s <= c_secs && c_secs <= t_secs_e) {
            retval = true;
            break;
        }
    }
    return retval;
}

/*@brief      勿扰模式默认设置
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_reset(app_module_do_not_disturb_t *do_not_disturb)
{
    /* 9:00~11:30;13:00~18:00;workday */
    do_not_disturb->table[0].time_s[0] = 9;
    do_not_disturb->table[0].time_s[1] = 0;
    do_not_disturb->table[0].time_s[2] = 0;
    do_not_disturb->table[0].time_e[0] = 11;
    do_not_disturb->table[0].time_e[1] = 30;
    do_not_disturb->table[0].time_e[2] = 0;
    do_not_disturb->table[1].time_s[0] = 13;
    do_not_disturb->table[1].time_s[1] = 0;
    do_not_disturb->table[1].time_s[2] = 0;
    do_not_disturb->table[1].time_e[0] = 18;
    do_not_disturb->table[1].time_e[1] = 0;
    do_not_disturb->table[1].time_e[2] = 0;
    do_not_disturb->valid_bits = 0b11;
    do_not_disturb->week = 0b0111110;
}

/*@brief 勿扰状态转储到外存
 */
void app_module_do_not_disturb_dump(void)
{
    app_module_do_not_disturb_t do_not_disturb = {0};
    app_module_do_not_disturb_get(&do_not_disturb);
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_source);
    memcpy(&data_center->module_source.do_not_disturb, &do_not_disturb, sizeof(app_module_do_not_disturb_t));
    app_module_data_center_give();
}

/*@brief 勿扰状态加载到内存
 */
void app_module_do_not_disturb_load(void)
{
    app_module_do_not_disturb_t do_not_disturb = {0};
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_source);
    memcpy(&do_not_disturb, &data_center->module_source.do_not_disturb, sizeof(app_module_do_not_disturb_t));
    app_module_do_not_disturb_set(&do_not_disturb);
    app_module_data_center_give();
}

/*@brief 勿扰模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_do_not_disturb_ready(void)
{
    app_mutex_process(&app_module_do_not_disturb_mutex, app_mutex_static);
}
