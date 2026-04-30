#ifndef SCUI_TICK_H
#define SCUI_TICK_H

typedef enum {
    scui_tick_stat_refr,
    scui_tick_stat_sched_rcd,
    scui_tick_stat_sched_sum,
    scui_tick_stat_draw_sw_rcd,
    scui_tick_stat_draw_sw_sum,
    scui_tick_stat_draw_hw_rcd,
    scui_tick_stat_draw_hw_sum,
    scui_tick_stat_draw_rcd,
    scui_tick_stat_draw_sum,
    scui_tick_stat_reset,
} scui_tick_stat_type_t;

typedef struct {
    uint64_t refr_fps;
    uint64_t sched_rcd;
    uint64_t sched_sum;
    uint64_t draw_rcd_sw;
    uint64_t draw_sum_sw;
    uint64_t draw_rcd_hw;
    uint64_t draw_sum_hw;
    uint64_t draw_rcd;
    uint64_t draw_sum;
} scui_tick_stat_t;

typedef struct {
    uint64_t cnt_passby;
    uint64_t tag_work:1;
    scui_tick_stat_t stat;
} scui_tick_t;

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void);

/*@brief 滴答器耗时
 *@retval 耗时
 */
uint64_t scui_tick_us(void);

/*@brief 滴答器启停
 *@param work 启停状态
 */
void scui_tick_work(bool work);

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint64_t ms);

/*@brief 嘀嗒统计
 *@param stat 统计状态
 */
void scui_tick_stat(scui_tick_stat_type_t stat_type);

/*@brief 嘀嗒统计结果
 *@param tick_stat 统计结果
 */
void scui_tick_stat_rcd(scui_tick_stat_t *tick_stat);

#endif
