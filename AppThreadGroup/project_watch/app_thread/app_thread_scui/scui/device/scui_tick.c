/*实现目标:
 *    系统滴答器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "app_thread_group.h"

static scui_tick_t scui_tick = {0};

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void)
{
    return scui_tick.cnt_passby;
}

/*@brief 滴答器耗时
 *@retval 耗时
 */
uint64_t scui_tick_us(void)
{
    return app_execute_us();
}

/*@brief 滴答器启停
 *@param work 启停状态
 */
void scui_tick_work(bool work)
{
    scui_tick.tag_work = work;
}

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint64_t ms)
{
    if (scui_tick.tag_work) {
        scui_tick.cnt_passby += ms;
        /* 更新动画即可, 它是调度心跳 */
        /* 动画更新了会自生成事件调度 */
        scui_anima_elapse_new(ms);
    }
}

/*@brief 嘀嗒统计
 *@param stat 统计状态
 */
void scui_tick_stat(scui_tick_stat_type_t stat_type)
{
    uint64_t elapse = 0;
    
    switch (stat_type) {
    case scui_tick_stat_refr:
        scui_tick.stat.refr_fps++;
        break;
    case scui_tick_stat_sched_rcd:
        scui_tick.stat.sched_rcd = scui_tick_us();
        break;
    case scui_tick_stat_sched_sum:
        elapse = scui_tick_us() - scui_tick.stat.sched_rcd;
        scui_tick.stat.sched_sum += elapse;
        break;
    case scui_tick_stat_draw_sw_rcd:
        scui_tick.stat.draw_rcd_sw = scui_tick_us();
        break;
    case scui_tick_stat_draw_sw_sum:
        elapse = scui_tick_us() - scui_tick.stat.draw_rcd_sw;
        scui_tick.stat.draw_sum_sw += elapse;
        break;
    case scui_tick_stat_draw_hw_rcd:
        scui_tick.stat.draw_rcd_hw = scui_tick_us();
        break;
    case scui_tick_stat_draw_hw_sum:
        elapse = scui_tick_us() - scui_tick.stat.draw_rcd_hw;
        scui_tick.stat.draw_sum_hw += elapse;
        break;
    case scui_tick_stat_draw_rcd:
        scui_tick.stat.draw_rcd = scui_tick_us();
        break;
    case scui_tick_stat_draw_sum:
        elapse = scui_tick_us() - scui_tick.stat.draw_rcd;
        scui_tick.stat.draw_sum += elapse;
        break;
    case scui_tick_stat_reset:
        scui_tick.stat.refr_fps = 0;
        scui_tick.stat.sched_sum = 0;
        scui_tick.stat.draw_sum_sw = 0;
        scui_tick.stat.draw_sum_hw = 0;
        scui_tick.stat.draw_sum = 0;
        break;
    default:
        SCUI_LOG_ERROR("%d", stat_type);
        break;
    }
}

/*@brief 嘀嗒统计结果
 *@param tick_stat 统计结果
 */
void scui_tick_stat_rcd(scui_tick_stat_t *tick_stat)
{
    *tick_stat = scui_tick.stat;
}
