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
void scui_tick_calc(uint8_t state, uint32_t *tick_fps, uint32_t *sched_us, uint32_t *draw_us);

#endif
