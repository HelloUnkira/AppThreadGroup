/*实现目标:
 *    超时模组
 *    空闲模组
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_check_time_t scui_check_time = {0};

/*@brief scui 超时/空闲模组动画响应回调
 */
void scui_check_time_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    uint64_t inactive_tick = 0;
    scui_tick_inactive(&inactive_tick);
    SCUI_LOG_INFO("incative time:%u", inactive_tick);
    
    /* 同步非活跃时间到模组 */
    if (inactive_tick < 1000) {
        if (scui_check_time.over) {
            scui_check_time.over_tick = scui_check_time.over_temp ?
            scui_check_time.over_tick_tmp : scui_check_time.over_tick_bak;
        }
        
        if (scui_check_time.idle) {
            scui_check_time.idle_tick = scui_check_time.idle_temp ?
            scui_check_time.idle_tick_tmp : scui_check_time.idle_tick_bak;
        }
    }
    
    /* 只取起始点位 */
    if (anima->value_c != 0)
        return;
    
    /* 约减超时等待 */
    if (scui_check_time.over_tick != 0 &&
        scui_check_time.over_tick != SCUI_CHECK_TIME_OVER_UNUSE) {
        SCUI_LOG_WARN("check time over:%u", scui_check_time.over_tick);
        scui_check_time.over_tick--;
        return;
    }
    /* 超时等待结束发送事件 */
    if (scui_check_time.over) {
        scui_check_time.over = false;
        scui_check_time.over_temp = false;
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_check_time_over, NULL);
        scui_event_notify(&event_ui);
        return;
    }
    
    /* 约减空闲等待 */
    if (scui_check_time.idle_tick != 0 &&
        scui_check_time.idle_tick != SCUI_CHECK_TIME_IDLE_UNUSE) {
        SCUI_LOG_WARN("check time idle:%u", scui_check_time.idle_tick);
        scui_check_time.idle_tick--;
        return;
    }
    /* 空闲等待结束发送事件 */
    if (scui_check_time.idle) {
        scui_check_time.idle = false;
        scui_check_time.idle_temp = false;
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_check_time_idle, NULL);
        scui_event_notify(&event_ui);
        return;
    }
}

/*@brief scui 超时/空闲模组 重置超时时间
 *@param over_tick 超时时间
 *@param over_temp 临时重置
 */
void scui_check_time_over_reset(uint8_t over_tick, bool over_temp)
{
    scui_check_time.over = true;
    scui_check_time.over_temp = over_temp;
    if (scui_check_time.over_temp) {
        scui_check_time.over_tick_tmp = scui_check_time.over_tick_def;
        if (over_tick != 0) scui_check_time.over_tick_tmp = over_tick;
        scui_check_time.over_tick = scui_check_time.over_tick_tmp;
    } else {
        scui_check_time.over_tick_bak = scui_check_time.over_tick_def;
        if (over_tick != 0) scui_check_time.over_tick_bak = over_tick;
        scui_check_time.over_tick = scui_check_time.over_tick_bak;
    }
}

/*@brief scui 超时/空闲模组 重置空闲时间
 *@param idle_tick 空闲时间
 *@param idle_temp 临时重置
 */
void scui_check_time_idle_reset(uint8_t idle_tick, bool idle_temp)
{
    scui_check_time.idle = true;
    scui_check_time.idle_temp = idle_temp;
    if (scui_check_time.idle_temp) {
        scui_check_time.idle_tick_tmp = scui_check_time.idle_tick_def;
        if (idle_tick != 0) scui_check_time.idle_tick_tmp = idle_tick;
        scui_check_time.idle_tick = scui_check_time.idle_tick_tmp;
    } else {
        scui_check_time.idle_tick_bak = scui_check_time.idle_tick_def;
        if (idle_tick != 0) scui_check_time.idle_tick_bak = idle_tick;
        scui_check_time.idle_tick = scui_check_time.idle_tick_bak;
    }
}

/*@brief scui 超时/空闲模组
 */
void scui_check_time_ready(void)
{
    scui_anima_t anima = {0};
    anima.expire  = scui_check_time_anima_expire;
    anima.value_s = 0;
    anima.value_e = 9;
    anima.period  = 1000;
    anima.reload  = SCUI_ANIMA_INFINITE;
    scui_anima_create(&anima, &scui_check_time.anima);
    
    scui_check_time.over_tick_def = SCUI_CHECK_TIME_OVER_DEF;
    scui_check_time.idle_tick_def = SCUI_CHECK_TIME_IDLE_DEF;
    scui_check_time.over_tick_bak = scui_check_time.over_tick_def;
    scui_check_time.idle_tick_bak = scui_check_time.idle_tick_def;
    scui_check_time.over_tick_tmp = scui_check_time.over_tick_def;
    scui_check_time.idle_tick_tmp = scui_check_time.idle_tick_def;
    
    scui_check_time_over_reset(0, false);
    scui_check_time_idle_reset(0, false);
}

/*@brief scui 超时/空闲模组启停
 *@param work 启停
 */
void scui_check_time_work(bool work)
{
    if (work) scui_anima_start(scui_check_time.anima);
    else scui_anima_stop(scui_check_time.anima);
}
