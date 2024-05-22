/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static bool scui_engine_execute_status = false;

/*@brief 引擎就绪
 */
void scui_engine_ready(void)
{
    scui_mem_ready();
    scui_disp_ready();
    scui_indev_ready();
    scui_event_ready();
    
    scui_image_cache_ready();
    scui_surface_fb_ready();
}

/*@brief 引擎等待
 */
void scui_engine_wait(void)
{
    scui_event_wait();
}

/*@brief 引擎执行
 */
void scui_engine_execute(void)
{
    scui_event_dispatch();
}

/*@biref 设置引擎执行状态
 *@param 引擎是否执行
 */
void scui_engine_execute_status_set(bool execute)
{
    scui_engine_execute_status = execute;
}

/*@biref 获得引擎执行状态
 *@retval 引擎是否执行
 */
bool scui_engine_execute_status_get(void)
{
    return scui_engine_execute_status;
}
