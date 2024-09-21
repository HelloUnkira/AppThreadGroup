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
 *@param state 0x00:refr回调计数;
 *             0x10:主调度开始;
 *             0x11:主调度结束;
 *             0x20:绘制开始;
 *             0x21:绘制结束;
 *             0xAA:结果;
 *             0xFF:复位;
 *@param tick_fps 结果:refr回调计数
 *@param sched_us 结果:主调度时间
 *@param draw_us  结果:绘制时间
 */
void scui_tick_calc(uint8_t state, uint32_t *tick_fps, uint32_t *sched_us, uint32_t *draw_us)
{
    static uint64_t stat_rcd = 0;
    static uint64_t stat_sum = 0;
    static uint64_t draw_rcd = 0;
    static uint64_t draw_sum = 0;
    static uint64_t stat_fps = 0;
    uint64_t retval = 0;
    
    switch (state) {
    case 0x00:
        stat_fps++;
        break;
    case 0x10:
        stat_rcd = scui_tick_us();
        break;
    case 0x11:
        stat_sum += scui_tick_us() - stat_rcd;
        break;
    case 0x20:
        draw_rcd = scui_tick_us();
        break;
    case 0x21:
        draw_sum += scui_tick_us() - draw_rcd;
        break;
    case 0xAA:
        *tick_fps = stat_fps;
        *sched_us = stat_sum;
        *draw_us  = draw_sum;
        break;
    case 0xFF:
        stat_fps = 0;
        stat_sum = 0;
        draw_sum = 0;
        break;
    default:
        SCUI_LOG_ERROR("");
        break;
    }
}
