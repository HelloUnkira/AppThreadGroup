/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_home_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        return scui_event_retval_keep;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 窗口属性参数配置 */
        scui_window_cfg_t window_cfg = {0};
        scui_window_cfg_get(event->object, &window_cfg);
        window_cfg.sibling[0] = SCUI_UI_SCENE_FLOAT_1;
        window_cfg.sibling[1] = SCUI_UI_SCENE_FLOAT_2;
        window_cfg.sibling[2] = SCUI_UI_SCENE_FLOAT_3;
        window_cfg.sibling[3] = SCUI_UI_SCENE_FLOAT_4;
        window_cfg.sibling_attr[0].floating = true;
        window_cfg.sibling_attr[1].floating = true;
        window_cfg.sibling_attr[2].floating = true;
        window_cfg.sibling_attr[3].floating = true;
        window_cfg.sibling_attr[0].preload = false;
        window_cfg.sibling_attr[1].preload = false;
        window_cfg.sibling_attr[2].preload = false;
        window_cfg.sibling_attr[3].preload = false;
        scui_window_cfg_set(event->object, &window_cfg);
        
        return scui_event_retval_keep;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        return scui_event_retval_keep;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        return scui_event_retval_keep;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        return scui_event_retval_keep;
    case scui_event_key_click:
        scui_window_jump(SCUI_UI_SCENE_1, scui_window_switch_normal, scui_event_dir_to_d);
        return scui_event_retval_over;
    default:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        return scui_event_retval_quit;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_home_c_event_proc(scui_event_t *event)
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
scui_event_retval_t scui_ui_scene_home_lu_event_proc(scui_event_t *event)
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
scui_event_retval_t scui_ui_scene_home_ru_event_proc(scui_event_t *event)
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
scui_event_retval_t scui_ui_scene_home_ld_event_proc(scui_event_t *event)
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
scui_event_retval_t scui_ui_scene_home_rd_event_proc(scui_event_t *event)
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
