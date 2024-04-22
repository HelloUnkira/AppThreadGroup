/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->parent != SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->parent;
}

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->child_num;
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_index(scui_handle_t handle, scui_handle_t index)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 其他范围映射到取值范围[0:num) */
    index = scui_mabs(index, widget->child_num);
    
    if (index < widget->child_num)
        return widget->child_list[index];
    return SCUI_HANDLE_INVALID;
}

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_attr_type(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->type;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_attr_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->clip;
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID)
    if (!scui_widget_style_is_show(widget->parent))
         return false;
    
    /* 它自己的显示状态 */
    if (widget->style.state)
        return true;
    
    return false;
}

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_style_is_hide(scui_handle_t handle)
{
    return !scui_widget_style_is_show(handle);
}

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@retval 控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->surface.alpha;
}

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->image;
}

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_gradient_t scui_widget_color_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->color;
}

/*@brief 控件透明度设置
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->surface.alpha = alpha;
    
    /* 待定中 */
    return;
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_alpha_set(widget->child_list[idx], alpha);
}

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image)
{
    SCUI_LOG_INFO("widget %u image %u set", handle, image);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->image = image;
}

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_gradient_t color)
{
    SCUI_LOG_INFO("widget %u color %x set", handle, color.color.full);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->color = color;
}
