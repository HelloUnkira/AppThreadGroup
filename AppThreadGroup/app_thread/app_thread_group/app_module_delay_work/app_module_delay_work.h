#ifndef APP_MODULE_DELAY_WORK_H
#define APP_MODULE_DELAY_WORK_H

typedef struct {
    void *work_or_works;
    app_sys_timer_t timer;
    uint32_t thread;
    uint32_t event;
} app_module_delay_work_or_works_t;

/*@brief     迟延工作提交
 *@param[in] work       一份工作(生成)
 *@param[in] delay_ms   延迟时间
 *@param[in] thread     目标线程
 */
void app_module_delay_work_submit(app_sys_work_t *work, uint32_t delay_ms, uint32_t thread);

/*@brief     迟延工作清单提交
 *@param[in] work       一份工作清单(生成)
 *@param[in] delay_ms   延迟时间
 *@param[in] thread     目标线程
 */
void app_module_delay_works_submit(app_sys_works_t *works, uint32_t delay_ms, uint32_t thread);

#endif
