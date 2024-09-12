/*实现目标:
 *    系统滴答器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "app_thread_group.h"

static uint64_t   scui_tick_cnt_elapse = 0;
static uint64_t (*scui_tick_us_cb)(void) = NULL;

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void)
{
    return scui_tick_cnt_elapse;
}

/*@brief 滴答器耗时
 *@retval 耗时
 */
uint64_t scui_tick_us(void)
{
    return app_execute_us();
}

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint64_t ms)
{
    scui_tick_cnt_elapse += ms;
    /* 更新动画即可,动画更新了自己会产生事件调度 */
    if (scui_engine_execute_status_get())
        scui_anima_elapse(ms);
}

/*@brief 滴答器计算时间
 *@param state  0:开始;1:结束;2:结果;3:清除
 *@retval 最近统计时间
 */
uint64_t scui_tick_calc(uint8_t state)
{
    static uint64_t stat_rcd = 0;
    static uint64_t stat_sum = 0;
    uint64_t retval = 0;
    
    switch (state) {
    case 0:
        stat_rcd = scui_tick_us();
        break;
    case 1:
        stat_sum += scui_tick_us() - stat_rcd;
        break;
    case 2:
        retval = stat_sum;
        break;
    case 3:
        stat_sum = 0;
        break;
    default:
        SCUI_LOG_ERROR("");
        break;
    }
    
    return retval;
}
