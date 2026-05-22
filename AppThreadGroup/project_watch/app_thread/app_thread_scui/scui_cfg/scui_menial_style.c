/*实现目标:
 *    泛用样式:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 仆从控件样式应用
 *@param handle 仆从控件句柄
 */
void scui_menial_btn_style_apply(scui_handle_t handle)
{
}

/*@brief 仆从控件样式应用
 *@param handle 仆从控件句柄
 */
void scui_menial_arc_style_apply(scui_handle_t handle)
{
}

/*@brief 仆从控件样式应用
 *@param handle 仆从控件句柄
 */
void scui_menial_bar_style_apply(scui_handle_t handle)
{
}

/*@brief 仆从控件样式应用
 *@param handle 仆从控件句柄
 */
void scui_menial_cht_style_apply(scui_handle_t handle)
{
}

/*@brief 仆从控件样式应用
 *@param handle 仆从控件句柄
 */
void scui_menial_style_apply(scui_handle_t handle)
{
    scui_menial_type_t type = scui_menial_type_none;
    scui_menial_type_sub(handle, &type);
    
    switch (type) {
    default:SCUI_ASSERT(false);break;
    case scui_menial_type_btn:
        scui_menial_btn_style_apply(handle);
        break;
    case scui_menial_type_arc:
        scui_menial_arc_style_apply(handle);
        break;
    case scui_menial_type_bar:
        scui_menial_bar_style_apply(handle);
        break;
    case scui_menial_type_cht:
        scui_menial_cht_style_apply(handle);
        break;
    }
}
