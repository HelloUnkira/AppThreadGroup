/*实现目标:
 *    引擎
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static bool scui_engine_execute_status = false;

/*@brief 引擎就绪
 */
void scui_engine_ready(void)
{
    scui_disp_set_hor_res(SCUI_DRV_HOR_RES);
    scui_disp_set_ver_res(SCUI_DRV_VER_RES);
    scui_disp_ready();
    scui_indev_ready();
    
    scui_mem_ready();
    scui_event_ready();
    scui_image_cache_ready();
    
    scui_event_register_before(scui_event_before);
    scui_event_register_after(scui_event_after);
    scui_event_register_custom(scui_event_custom);
    
    scui_handle_table_t table = {0};
    /* 句柄表(image) */
    table.offset = SCUI_HANDLE_OFFSET_IMAGE;
    table.source = scui_image_combine_table;
    table.number = app_sys_arr_len(scui_image_combine_table);
    scui_handle_table_register(&table);
    
}

/*@brief 引擎执行
 */
void scui_engine_execute(void)
{
    scui_event_sync_wait();
    scui_event_dispatch();
    scui_anima_update();
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
