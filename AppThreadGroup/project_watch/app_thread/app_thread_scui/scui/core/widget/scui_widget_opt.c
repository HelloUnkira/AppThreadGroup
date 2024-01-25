/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

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
    scui_widget_clip_reset(widget);
    
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

/*@brief 控件移动到目标坐标
 *@param handle 控件句柄
 */
void scui_widget_move(scui_handle_t handle, scui_point_t *point)
{
    SCUI_LOG_INFO("");
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
        /* 移动要清除剪切域 */
        widget->surface_clip = (scui_area_t){0};
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
    widget->surface.clip.x = point->x;
    widget->surface.clip.y = point->y;
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_point_t point = {
                .x = offset.x + child->clip.x,
                .y = offset.y + child->clip.y,
            };
            scui_widget_move(handle, &point);
        }
    
    SCUI_LOG_DEBUG("");
    /* 重新追加剪切域 */
    if (!scui_area_empty(&widget->surface_clip)) {
         widget->surface_clip = (scui_area_t){0};
         scui_widget_clip_reset(widget);
    }
}

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle)
{
    SCUI_LOG_INFO("");
    /* 资源还未加载时,加载该资源 */
    if (!scui_handle_remap(handle)) {
         SCUI_LOG_INFO("not load yet");
         scui_widget_cb_load(handle);
    }
    
    SCUI_LOG_DEBUG("");
    if (scui_widget_style_is_show(handle)) {
        SCUI_LOG_INFO("is show");
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
    
    scui_widget_draw(widget->myself, false);
}

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle)
{
    SCUI_LOG_INFO("");
    
    SCUI_LOG_DEBUG("");
    if (scui_widget_style_is_hide(handle)) {
        SCUI_LOG_INFO("is hide");
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
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_draw(widget->parent, false);
}
