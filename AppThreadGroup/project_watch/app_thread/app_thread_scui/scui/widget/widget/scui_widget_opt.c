/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件吸收
 */
static bool scui_widget_draw_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, bool sync)
{
    SCUI_LOG_INFO("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 只为当前控件及其子控件添加剪切域,但是绘制是从窗口开始的 */
    scui_widget_clip_reset(widget, true);
    scui_widget_clip_update(widget);
    
    scui_event_t event = {
        .type   = scui_event_draw,
        .object = scui_widget_root(widget->myself),
        .absorb = scui_widget_draw_absorb,
    };
    if (sync)
        event.style = scui_event_style_sync;
    scui_event_notify(&event);
}

/*@brief 事件吸收
 */
static bool scui_widget_refr_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync)
{
    SCUI_LOG_INFO("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_event_t event = {
        .type   = scui_event_refr,
        .object = scui_widget_root(widget->myself),
        .absorb = scui_widget_refr_absorb,
    };
    if (sync)
        event.style = scui_event_style_sync;
    scui_event_notify(&event);
}

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_repos(scui_handle_t handle, scui_point_t *point)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->clip.x == point->x && widget->clip.y == point->y)
        return;
    
    SCUI_LOG_DEBUG("");
    /* 如果移动的是窗口,只需要改变窗口位置 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        widget->surface.clip.x = point->x;
        widget->surface.clip.y = point->y;
        SCUI_LOG_DEBUG("<x:%d, y:%d>", point->x, point->y);
        return;
    }
    
    SCUI_LOG_DEBUG("");
    /* 计算移动偏移量 */
    scui_point_t offset = {
        .x = point->x - widget->clip.x,
        .y = point->y - widget->clip.y,
    };
    
    SCUI_LOG_DEBUG("");
    
    /* 移动自己 */
    widget->clip.x = point->x;
    widget->clip.y = point->y;
    
    SCUI_LOG_DEBUG("");
    /* 更新画布剪切域 */
    scui_widget_surface_refr(widget, false);
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_point_t point = {
                .x = offset.x + child->clip.x,
                .y = offset.y + child->clip.y,
            };
            scui_widget_repos(handle, &point);
        }
}

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_resize(scui_handle_t handle, scui_coord_t width, scui_coord_t height)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(width  >= 0);
    SCUI_ASSERT(height >= 0);
    
    if (widget->parent == SCUI_HANDLE_INVALID) {
        SCUI_LOG_ERROR("unsupport");
        return;
    }
    
    widget->clip.w = width;
    widget->clip.h = height;
    /* 更新画布剪切域 */
    scui_widget_surface_refr(widget, false);
}

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    scui_widget_cb_create(handle);
    
    if (scui_widget_style_is_show(handle)) {
        SCUI_LOG_DEBUG("is show");
        return;
    }
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    /* 设置控件状态为显示 */
    widget->style.state = true;
    
    SCUI_LOG_DEBUG("");
    scui_event_t event = {
        .object = scui_widget_root(widget->myself),
        .type   = scui_event_show,
        .style  = scui_event_style_sync,
    };
    scui_event_notify(&event);
    
    bool only = scui_widget_surface_only(widget);
    scui_widget_draw(widget->myself, only);
    
    /* 将该显示窗口加入到场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_add(widget->myself);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    
    if (scui_handle_unmap(handle))
        return;
    
    if (scui_widget_style_is_hide(handle)) {
        SCUI_LOG_DEBUG("is hide");
        return;
    }
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    /* 设置控件状态为隐藏 */
    widget->style.state = false;
    
    SCUI_LOG_DEBUG("");
    scui_event_t event = {
        .object = scui_widget_root(widget->myself),
        .type   = scui_event_hide,
        .style  = scui_event_style_sync,
    };
    scui_event_notify(&event);
    
    /* 通知父窗口重绘 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        bool only = scui_widget_surface_only(widget);
        scui_widget_draw(widget->parent, only);
    }
    
    /* 将该显示窗口移除出场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_del(widget->myself);
    
    scui_widget_cb_destroy(widget->myself);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_hide_without(scui_handle_t handle, scui_handle_t child)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID &&
            widget->child_list[idx] != child)
            scui_widget_hide(widget->child_list[idx]);
}

/*@brief 控件树镜像(相对父控件)
 *@param handle 控件句柄
 */
void scui_widget_mirror(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* @等待适配,要用的时候再去实现 */
    SCUI_LOG_ERROR("not finish yet");
    return;
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++) {
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle_child = widget->child_list[idx];
        scui_widget_t *widget_child = scui_handle_get(handle);
        scui_point_t    point_child = {0};
        
        point_child.x -= widget->clip.x;
        point_child.y -= widget->clip.y;
        
        /// ...
        
        scui_widget_repos(handle_child, &point_child);
    }
}
