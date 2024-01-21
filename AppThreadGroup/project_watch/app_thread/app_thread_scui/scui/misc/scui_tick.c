/*实现目标:
 *    系统滴答器
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static uint64_t scui_tick_elapse_cnt = 0;

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint32_t ms)
{
    scui_tick_elapse_cnt += ms;
    /* 更新动画即可,动画更新了自己会产生事件调度 */
    if (scui_engine_execute_status_get())
        scui_anima_elapse(ms);
}

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void)
{
    return scui_tick_elapse_cnt;
}
