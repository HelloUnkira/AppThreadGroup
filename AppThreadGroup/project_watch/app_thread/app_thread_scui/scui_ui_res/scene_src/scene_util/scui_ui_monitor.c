/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_handle_t monitor_anima = SCUI_HANDLE_INVALID;
scui_handle_t monitor_refr  = SCUI_HANDLE_INVALID;
scui_handle_t monitor_mem   = SCUI_HANDLE_INVALID;

/*@brief scui 监控器常驻动画响应回调
 */
void scui_ui_scene_monitor_anima_expired(void *instance)
{
    static uint64_t refr_us_last = 0;
    if (scui_tick_us() - refr_us_last > 1000 * 1000) {
        refr_us_last = scui_tick_us();
        
        static uint64_t rcd_refr_us = 0;
        uint64_t refr_us = scui_tick_calc(2);
        if (rcd_refr_us != refr_us) {
            rcd_refr_us  = refr_us;
            uint8_t str_refr[30] = {0};
            snprintf(str_refr, sizeof(str_refr) - 1, "refr %03d.%03d, fps %03d",
                     refr_us / 1000, refr_us % 1000, 1000 / (refr_us / 1000));
            
            scui_string_update_str(monitor_refr, str_refr);
        }
        scui_tick_calc(3);
    }
    
    // scui_string_update_str(monitor_mem,  "");
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_monitor_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        if (scui_widget_event_check_prepare(event)) {
            
            if (monitor_anima == SCUI_HANDLE_INVALID) {
                scui_anima_t anima = {0};
                anima.expired = scui_ui_scene_monitor_anima_expired;
                anima.peroid  = SCUI_ANIMA_TICK;
                anima.reload  = SCUI_ANIMA_INFINITE;
                scui_anima_create(&anima, &monitor_anima);
                scui_anima_start(monitor_anima);
                
                /* 创建显示信息文本,都是英文 */
                scui_string_maker_t string_maker = {0};
                string_maker.widget.type                = scui_widget_type_string;
                string_maker.widget.style.trans         = true;
                string_maker.widget.parent              = SCUI_UI_SCENE_MONITOR;
                string_maker.font_idx                   = 0;
                string_maker.args.gap_none              = SCUI_STRING_SPACE_WIDTH;
                string_maker.args.color.color_s.full    = 0xFF808080;
                string_maker.args.color.color_e.full    = 0xFF808080;
                
                string_maker.widget.clip.x              = 10;
                string_maker.widget.clip.y              = 10;
                string_maker.widget.clip.w              = SCUI_DRV_HOR_RES;
                string_maker.widget.clip.h              = 40;
                scui_string_create(&string_maker, &monitor_mem, false);
                string_maker.widget.clip.x              = 10;
                string_maker.widget.clip.y              = 50;
                string_maker.widget.clip.w              = SCUI_DRV_HOR_RES;
                string_maker.widget.clip.h              = 40;
                scui_string_create(&string_maker, &monitor_refr, false);
                
                scui_string_update_str(monitor_mem,  "");
                scui_string_update_str(monitor_refr, "refr");
            }
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        if (scui_widget_event_check_finish(event)) {
            
            if (monitor_anima != SCUI_HANDLE_INVALID) {
                scui_anima_stop(monitor_anima);
                scui_anima_destroy(monitor_anima);
                monitor_anima = SCUI_HANDLE_INVALID;
            }
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
