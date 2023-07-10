#ifndef APP_MODULE_DELAY_WORK_H
#define APP_MODULE_DELAY_WORK_H

typedef struct {
    app_sys_timer_t timer;  /* 迟延软件定时器 */
    app_sys_work_t  work;   /* 一份迟延工作 */
    uint32_t thread_id;     /* 要交付的线程 */
} app_module_delay_work_t;

/*@brief     迟延工作提交
 *@param[in] delay_work 一份延迟工作
 *@param[in] delay_ms   延迟时间
 */
void app_module_delay_work_submit(app_module_delay_work_t *delay_work, uint32_t delay_ms);

#endif

