/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 回收控件(销毁回收,主动调用)
 *@param handle 控件句柄
 */
void scui_widget_recycle(scui_handle_t handle)
{
    scui_widget_hide(handle);
    scui_widget_cb_destroy(handle);
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
    /* 更新画布剪切域 */
    widget->surface.clip = widget->clip;
    
    SCUI_LOG_DEBUG("");
    /* 画布的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = NULL;
        scui_handle_t  handle_parent = widget->parent;
        widget_parent = scui_handle_get(handle_parent);
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_merge = {0};
        scui_area_inter(&clip_merge, &widget->surface.clip, &widget_parent->surface.clip);
        widget->surface.clip = clip_merge;
    }
    
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
            scui_widget_move(handle, &point);
        }
    
    SCUI_LOG_DEBUG("");
    /* 重新追加剪切域 */
    if (!scui_area_empty(&widget->surface_clip))
         widget->surface_clip = widget->clip;
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
    
    /* 将该显示窗口加入到场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_add(widget->myself);
}

/*@brief 控件隐藏
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
    
    /* 将该显示窗口移除出场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_del(widget->myself);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_hide_without(scui_handle_t handle, scui_handle_t child)
{
    SCUI_LOG_INFO("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++) {
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID)
            continue;
        if (widget->child_list[idx] == child)
            scui_widget_show(widget->child_list[idx]);
        else
            scui_widget_hide(widget->child_list[idx]);
    }
}

/*@brief 控件树镜像(相对父控件)
 *@param handle 控件句柄
 */
void scui_widget_mirror(scui_handle_t handle)
{
    SCUI_LOG_INFO("");
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
        
        scui_widget_move(handle_child, &point_child);
    }
}
