/*实现目标:
 *    超时模组
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_overtime_t scui_overtime = {0};

/*@brief scui 超时模组动画响应回调
 */
static void scui_overtime_anima_expire(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 距上次系统活跃不足1秒: 视为仍在操作, 重置超时等待 */
    uint64_t inactive_tick = 0;
    scui_tick_inactive(&inactive_tick);
    if (inactive_tick < 1000 && scui_overtime.use)
        scui_overtime.tick = scui_overtime.tick_bak;
    
    /* 只取起始点位 */
    if (anima->value_c != 0)
        return;
    
    /* 未使用不响应 */
    if (!scui_overtime.use)
        return;
    
    /* 约减超时等待 */
    if (scui_overtime.tick != 0) {
        scui_overtime.tick--;
        return;
    }
    
    /* 超时等待结束: 置空并产生超时事件 */
    SCUI_LOG_WARN("overtime");
    scui_overtime.use = false;
    scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
        scui_event_overtime, NULL);
    scui_event_notify(&event_ui);
}

/*@brief scui 超时模组 重置(设定超时时长, 并从当前时刻重新计时)
 *@param tick 超时时长(单位: tick, 1tick = 1秒)
 *             SCUI_OVERTIME_UNUSE: 不使用超时
 */
void scui_overtime_reset(uint32_t tick)
{
    /* 哨兵: 该界面不触发超时 */
    if (tick >= SCUI_OVERTIME_UNUSE) {
        scui_overtime.use = false;
        return;
    }
    
    scui_overtime.use = true;
    scui_overtime.tick = scui_overtime.tick_bak;
    scui_overtime.tick_bak = tick;
}

/*@brief scui 超时模组
 */
void scui_overtime_ready(void)
{
    scui_anima_t anima = {0};
    anima.expire  = scui_overtime_anima_expire;
    anima.value_s = 0;
    anima.value_e = 9;
    anima.period  = 1000;
    anima.reload  = SCUI_ANIMA_INFINITE;
    scui_anima_create(&anima, &scui_overtime.anima);
    
    scui_overtime.use = false;
}

/*@brief scui 超时模组启停
 *@param work 启停
 */
void scui_overtime_work(bool work)
{
    if (work) scui_anima_start(scui_overtime.anima);
    else scui_anima_stop(scui_overtime.anima);
}
