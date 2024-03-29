/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static inline void scui_ui_scene_window_float_cfg(void)
{
    /* 窗口属性参数配置 */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = SCUI_UI_SCENE_2;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(void)
{
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_2, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_UI_SCENE_1;
    window_cfg.sibling[3] = SCUI_UI_SCENE_3;
    scui_window_cfg_set(SCUI_UI_SCENE_2, &window_cfg);
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        return scui_event_retval_keep;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        return scui_event_retval_keep;
    case scui_event_focus_get:
        scui_ui_scene_window_float_cfg();
        scui_ui_scene_window_list_cfg();
        SCUI_LOG_INFO("scui_event_focus_get");
        return scui_event_retval_keep;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        return scui_event_retval_keep;
    case scui_event_key_click:
        return scui_event_retval_over;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            return scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_c_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_lu_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_ru_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_ld_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_2_rd_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}
