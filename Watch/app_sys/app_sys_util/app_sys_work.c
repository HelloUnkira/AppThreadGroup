/*实现目标:
 *    工作处理
 */
#include "app_std_lib.h"
#include "app_os_adaptor.h"
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
