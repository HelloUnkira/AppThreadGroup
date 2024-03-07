/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_scroll);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建自定义控件实例 */
    scui_scroll_t *scroll = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_scroll_t));
    memset(scroll, 0, sizeof(scui_scroll_t));
    
    /* 滚动控件必须设置事件响应标记 */
    SCUI_ASSERT(maker->widget.style.indev_ptr);
    SCUI_ASSERT(maker->widget.style.indev_enc);
    
    /* 创建基础控件实例 */
    scui_widget_create(&scroll->widget, &maker->widget, handle, layout);
    
    /* 状态初始化 */
    scroll->dir         = maker->dir;
    scroll->pos         = maker->pos;
    scroll->space       = maker->space;
    scroll->springback  = maker->springback;
    scroll->loop        = maker->loop;
    
    scroll->layout = true;
    
    /* 为滚动控件添加指定的事件回调 */
    scui_widget_event_t event = {0};
    event.order    = scui_widget_order_current;
    event.event_cb = scui_scroll_event;
    
    /* 事件默认全局接收 */
    event.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_ptr_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_enc_all;
    scui_widget_event_add(*handle, &event);
}

/*@brief 滚动控件销毁
 *@param handle 可滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_scroll);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&scroll->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(scroll);
}

/*@brief 滚动控件布局更新
 *@param handle 可滚动控件句柄
 */
void scui_scroll_layout(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_scroll);
    
    scroll->layout = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件子控件位置调整
 *@param handle 窗口控件句柄
 *@param point  窗口移动坐标
 */
void scui_scroll_adjust_offset(scui_handle_t handle, scui_point_t *offset)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_area_t clip_child = child->clip;
            clip_child.x += offset->x;
            clip_child.y += offset->y;
            scui_widget_repos(handle, &clip_child);
        }
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_handle_t  handle = anima->handle;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    int32_t value_s = anima->value_s;
    int32_t value_e = anima->value_e;
    int32_t value_c = anima->value_c;
    scui_point_t point_s = scroll->point_s;
    scui_point_t point_e = scroll->point_e;
    
    scui_point_t offset = {
        .x = scui_map(value_c, value_s, value_e, point_s.x, point_e.x),
        .y = scui_map(value_c, value_s, value_e, point_s.y, point_e.y),
    };
    
    scui_scroll_adjust_offset(handle, &offset);
}

/*@brief 滚动控件动画自动化
 */
void scui_scroll_anima_auto(scui_handle_t handle, int32_t value_s, int32_t value_e, uint32_t peroid)
{
    scui_anima_t anima = {0};
    anima.start   = scui_scroll_anima_start;
    anima.ready   = scui_scroll_anima_ready;
    anima.expired = scui_scroll_anima_expired;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.peroid  = peroid != 0 ? peroid : scui_abs(anima.value_e - anima.value_s) / 2;
    anima.handle  = handle;
    
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_window_float_anima_start(&anima);
        scui_window_float_anima_expired(&anima);
        scui_window_float_anima_ready(&anima);
        return;
    }
    scui_anima_create(&anima, &scroll->anima);
    scui_anima_start(scroll->anima);
}

/*@brief 滚动控件更新布局回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scroll_update_layout(scui_event_t *event)
{
    scui_event_retval_t ret = scui_event_retval_quit;
    
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 仅标记布局触发布局更新 */
    if (scroll->layout)
        scroll->layout = false;
    else
        return ret;
    
    /* 计算布局内容 */
    scui_area_t clip = widget->clip;
    
    if (widget->child_num == 0) {
        scroll->clip = clip;
        return ret;
    }
    
    /* 仅指定布局触发布局更新 */
    /* 粘性布局,与自由布局类似,只需要边界自动对齐即可 */
    if (scroll->dir == scui_event_dir_none) {
        /* 布局内容为所有子控件的全集 */
        for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
            if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
                scui_handle_t handle = widget->child_list[idx];
                scui_widget_t *child = scui_handle_get(handle);
                scui_area_t clip_child = child->clip;
                if (widget->parent != SCUI_HANDLE_INVALID) {
                    clip_child.x -= widget->clip.x;
                    clip_child.y -= widget->clip.y;
                }
                scui_area_t clip_inter = {0};
                scui_area_merge(&clip_inter, &clip_child, &clip);
                clip = clip_inter;
            }
        scroll->clip = clip;
        return ret;
    }
    
    // 水平自动布局会调整所有子控件的(x,y,h)
    // 垂直自动布局会调整所有子控件的(x,y,w)
    scui_point_t pos = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    
    /* 根据布局方向调整子控件信息 */
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            /* 被隐藏的控件忽略 */
            if (scui_widget_style_is_hide(handle))
                continue;
            
            /* 更新子控件尺寸 */
            if (scroll->dir == scui_event_dir_hor)
                scui_widget_resize(handle, child->clip.w, widget->clip.h);
            if (scroll->dir == scui_event_dir_ver)
                scui_widget_resize(handle, widget->clip.w, child->clip.h);
            /* 更新子控件位置 */
            scui_widget_repos(handle, &pos);
            /* 迭代到下一子控件 */
            if (scroll->dir == scui_event_dir_hor)
                pos.x += child->clip.w + scroll->space;
            if (scroll->dir == scui_event_dir_ver)
                pos.y += child->clip.h + scroll->space;
        }
    
    if (scui_widget_style_is_show(handle))
        scui_widget_draw(handle, NULL, false);
    
    return ret;
}

/*@brief 滚动控件事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scroll_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_event_retval_t ret = scui_event_retval_quit;
    
    switch (event->type) {
    case scui_event_show:
        ret |= scui_scroll_update_layout(event);
        break;
    case scui_event_focus_get:
        ret |= scui_scroll_update_layout(event);
        break;
    default:
        break;
    }
    
    return ret;
}
