#ifndef SCUI_TICK_H
#define SCUI_TICK_H

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void);

/*@brief 滴答器耗时
 *@retval 耗时
 */
uint64_t scui_tick_us(void);

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint64_t ms);

/*@brief 滴答器计算时间
 *@param state  0:开始;1:结束;2:结果;3:清除
 *@retval 最近统计时间
 */
uint64_t scui_tick_calc(uint8_t state);

#endif
