/*实现目标:
 *    控件组织
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/* 控件组织表 */
static const scui_widget_cb_t scui_widget_cb[scui_widget_type_num] = {
    [scui_widget_type_window].event   = (scui_widget_cb_event_t)    NULL,
    [scui_widget_type_window].create  = (scui_widget_cb_create_t)   scui_window_create,
    [scui_widget_type_window].destroy = (scui_widget_cb_destroy_t)  scui_window_destroy,
};

/*@brief 控件函数映射表
 *@param type 控件类型
 *@retval 控件函数映射表
 */
scui_widget_cb_t * scui_widget_cb_link(uint32_t type)
{
    APP_SYS_ASSERT(type < scui_widget_type_num);
    return &scui_widget_cb[type];
}
