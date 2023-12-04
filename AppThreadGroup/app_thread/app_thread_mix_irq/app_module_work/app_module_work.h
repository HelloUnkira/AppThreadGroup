#ifndef APP_MODULE_WORK_H
#define APP_MODULE_WORK_H

typedef struct {
    uint32_t number;
    struct {
        void (*routine)(void *argument);
        void  *argument;
    } array[0];
} app_module_work_t;

typedef struct {
    app_sys_timer_t timer;
    uint32_t thread;
    void    *work;
} app_module_work_delay_t;

/*@brief 处理一份工作清单
 *@param works 工作实例
 */
void app_module_work_execute(app_module_work_t *works);

/*@brief 生成一份工作清单
 *@param number    工作数量
 *@param argv[0,1] 工作回调1, 工作回调1参数
 *@param argv[2,3] 工作回调2, 工作回调2参数...依此类推
 */
app_module_work_t * app_module_work_make(uint32_t num, ...);

/*@brief 迟延工作清单提交
 *@param work       一份工作清单(app_module_work_make生成)
 *@param delay_ms   延迟时间
 *@param thread     目标线程
 */
void app_module_work_delay_submit(app_module_work_t *work, uint32_t delay_ms, uint32_t thread);

#endif
