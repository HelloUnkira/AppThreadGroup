#ifndef SCUI_TICK_H
#define SCUI_TICK_H

/*@brief 滴答器回调(建议1ms一调)
 *@param ms 过去时间
 */
void scui_tick_elapse(uint32_t ms);

/*@brief 滴答器当前计数
 *@retval 当前计数值
 */
uint64_t scui_tick_cnt(void);

/*@brief 滴答器标记
 *       标记程序某一个位置,差则为流逝滴答数,以us为单位
 *@retval 标记值
 */
uint64_t scui_tick_mark(void);

#endif
