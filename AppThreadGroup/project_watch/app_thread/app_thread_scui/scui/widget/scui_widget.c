/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"










/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    if (widget->parent != SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件alpha获取
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    return widget->alpha;
}

/*@brief 控件alpha设置
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    scui_widget_list_alpha_update(widget, alpha);
    widget->alpha = alpha;
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *parent = scui_handle_get(widget->parent);
        if (!scui_widget_style_is_show(parent->handle))
             return false;
    }
    
    /* 它自己的显示状态 */
    if (widget->style.state)
        return true;
    
    return false;
}

