#ifndef SCUI_TICK_H
#define SCUI_TICK_H

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint64_t ms);

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void);

/*@brief 滴答器总计计数
 *@retval 总计计数值
 */
uint64_t scui_tick_all(void);

/*@brief 滴答器耗时回调注册
 *@param cb 回调
 */
void scui_tick_elapse_us_register(uint64_t (*cb)(bool run));

/*@brief 滴答器耗时
 *@param run 计时开始或者结束
 *@retval 耗时
 */
uint64_t scui_tick_elapse_us(bool run);

#endif
