#ifndef SCUI_CHECK_TIME_H
#define SCUI_CHECK_TIME_H

typedef struct {
    scui_handle_t anima;
    scui_sbitfd_t over_tick:8;
    scui_sbitfd_t idle_tick:8;
    scui_sbitfd_t over_tick_tmp:8;
    scui_sbitfd_t idle_tick_tmp:8;
    scui_sbitfd_t over_tick_bak:8;
    scui_sbitfd_t idle_tick_bak:8;
    scui_sbitfd_t over_tick_def:8;
    scui_sbitfd_t idle_tick_def:8;
    scui_sbitfd_t over_temp:1;
    scui_sbitfd_t idle_temp:1;
    scui_sbitfd_t over:1;
    scui_sbitfd_t idle:1;
} scui_check_time_t;

/*@brief scui 超时/空闲模组 重置超时时间
 *@param over_tick 超时时间
 *@param over_temp 临时重置
 */
void scui_check_time_over_reset(uint8_t over_tick, bool over_temp);

/*@brief scui 超时/空闲模组 重置空闲时间
 *@param idle_tick 空闲时间
 *@param idle_temp 临时重置
 */
void scui_check_time_idle_reset(uint8_t idle_tick, bool idle_temp);

/*@brief scui 超时/空闲模组
 */
void scui_check_time_ready(void);

/*@brief scui 超时/空闲模组启停
 */
void scui_check_time_work(bool work);

#endif
