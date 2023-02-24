#ifndef APP_MODULE_WORK_H
#define APP_MODULE_WORK_H

typedef struct {
    void (*routine)(void *argument);
    void  *argument;
} app_module_work_t;

/*@brief 处理一份工作
 *@param[in] work 工作实例
 */
void app_module_work_execute(app_module_work_t *work);

/*@brief 生成一份工作
 *@param[in] routine  工作回调
 *@param[in] argument 工作回调参数
 */
app_module_work_t * app_module_work_make(void (*routine)(void *argument), void *argument);

#endif
