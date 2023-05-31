/*实现目标:
 *    工作处理
 */
#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_work.h"

/*@brief 处理一份工作
 *@param[in] work 工作实例
 */
void app_sys_work_execute(app_sys_work_t *work)
{
    APP_SYS_ASSERT(work != NULL);
    work->routine(work->argument);
    app_mem_free(work);
}

/*@brief 生成一份工作
 *@param[in] routine  工作回调
 *@param[in] argument 工作回调参数
 */
app_sys_work_t * app_sys_work_make(void (*routine)(void *argument), void *argument)
{
    app_sys_work_t *work = app_mem_alloc(sizeof(app_sys_work_t));
    work->routine  = routine;
    work->argument = argument;
    return work;
}

/*@brief 处理一份工作序列
 *@param[in] works 工作实例
 */
void app_sys_works_execute(app_sys_works_t *works)
{
    APP_SYS_ASSERT(works != NULL);
    for (uint32_t idx = 0; idx < works->num; idx++)
        works->array[idx].routine(works->array[idx].argument);
    app_mem_free(works);
}

/*@brief 生成一份工作序列
 *@param[in] num       工作数量
 *@param[in] argv[0,1] 工作回调1, 工作回调1参数
 *@param[in] argv[2,3] 工作回调2, 工作回调2参数...依此类推
 */
app_sys_works_t * app_sys_works_make(uint32_t num, ...)
{
    va_list  list;
    va_start(list, num);
    
    APP_SYS_ASSERT(num != 0);
    app_sys_works_t * works = app_mem_alloc(sizeof(app_sys_works_t) + sizeof(app_sys_work_t) * num);
    for (uint32_t idx = 0; idx < (works->num = num); idx++)
        works->array[idx] = va_arg(list, app_sys_work_t);
    
    va_end(list);
}
