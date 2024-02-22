/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        return scui_window_float_event_grasp_show(event);
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        return scui_window_float_event_grasp_hide(event);
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        return scui_event_retval_keep;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        return scui_event_retval_keep;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            return scui_window_float_event_grasp_ptr(event);
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e)
            return scui_window_float_event_grasp_key(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_c_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_lu_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_ru_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_ld_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_float_2_rd_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}
