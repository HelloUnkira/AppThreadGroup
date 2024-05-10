/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static inline void scui_ui_scene_window_float_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = SCUI_UI_SCENE_SOCCER;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_SOCCER, &window_cfg);
    window_cfg.sibling[0] = SCUI_UI_SCENE_2;
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[3] = SCUI_HANDLE_INVALID;
    scui_window_cfg_set(SCUI_UI_SCENE_SOCCER, &window_cfg);
}

static struct {
    void *none;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_soccer_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_widget_event_mask_keep(event);
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_widget_event_mask_keep(event);
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        scui_ui_scene_window_float_cfg(event);
        scui_ui_scene_window_list_cfg(event);
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click:
        scui_widget_event_mask_keep(event);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_soccer_custom_event_proc(scui_event_t *event)
{
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        
        break;
    case scui_event_draw:
        scui_widget_event_mask_keep(event);
        
        /* 绘制流程准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
        }
        
        /* 绘制流程进行 */
        if (scui_widget_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
        }
        break;
    case scui_event_ptr_move:
        scui_widget_event_mask_over(event);
        
        break;
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        scui_widget_event_mask_keep(event);
        
        break;
    break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
