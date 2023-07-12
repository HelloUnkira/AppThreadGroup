#ifndef APP_MODULE_DELAY_WORK_H
#define APP_MODULE_DELAY_WORK_H

typedef struct {
    uint32_t thread;
    app_sys_work_t *work;
    app_sys_timer_t timer;
} app_module_delay_work_t;

/*@brief     迟延工作清单提交
 *@param[in] work       一份工作清单(app_sys_work_list_make生成)
 *@param[in] delay_ms   延迟时间
 *@param[in] thread     目标线程
 */
void app_module_delay_work_submit(app_sys_work_t *work, uint32_t delay_ms, uint32_t thread);

#endif
