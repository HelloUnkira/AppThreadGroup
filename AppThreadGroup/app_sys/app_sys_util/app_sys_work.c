/*实现目标:
 *    工作处理
 */
#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_work.h"

/*@brief 处理一份工作清单
 *@param[in] work 工作实例
 */
void app_sys_work_execute(app_sys_work_t *work)
{
    APP_SYS_ASSERT(work != NULL);
    for (uint32_t idx = 0; idx < work->number; idx++)
        work->array[idx].routine(work->array[idx].argument);
    app_mem_free(work);
}

/*@brief 生成一份工作清单
 *@param[in] number    工作数量
 *@param[in] argv[0,1] 工作回调1, 工作回调1参数
 *@param[in] argv[2,3] 工作回调2, 工作回调2参数...依此类推
 */
app_sys_work_t * app_sys_work_make(uint32_t number, ...)
{
    va_list  list;
    va_start(list, number);
    
    uint32_t work_zone = sizeof(app_sys_work_t) + sizeof(uint32_t) + sizeof(void *) * 2 * number;
    app_sys_work_t * work = app_mem_alloc(work_zone);
    for (uint32_t idx = 0; idx < (work->number = number); idx++) {
        work->array[idx].routine  = va_arg(list, void *);
        work->array[idx].argument = va_arg(list, void *);
    }
    
    va_end(list);
    
    return work;
}
