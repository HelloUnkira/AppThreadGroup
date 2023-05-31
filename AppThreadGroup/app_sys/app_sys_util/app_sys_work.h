#ifndef APP_SYS_WORK_H
#define APP_SYS_WORK_H

typedef struct {
    void (*routine)(void *argument);
    void  *argument;
} app_sys_work_t;

typedef struct {
    uint32_t       num;
    app_sys_work_t array[0];
} app_sys_works_t;

/*@brief 处理一份工作
 *@param[in] work 工作实例
 */
void app_sys_work_execute(app_sys_work_t *work);

/*@brief 生成一份工作
 *@param[in] routine  工作回调
 *@param[in] argument 工作回调参数
 */
app_sys_work_t * app_sys_work_make(void (*routine)(void *argument), void *argument);

/*@brief 处理一份工作清单
 *@param[in] works 工作实例
 */
void app_sys_works_execute(app_sys_works_t *works);

/*@brief 生成一份工作清单
 *@param[in] num       工作数量
 *@param[in] argv[0,1] 工作回调1, 工作回调1参数
 *@param[in] argv[2,3] 工作回调2, 工作回调2参数...依此类推
 */
app_sys_works_t * app_sys_works_make(uint32_t num, ...);

#endif
