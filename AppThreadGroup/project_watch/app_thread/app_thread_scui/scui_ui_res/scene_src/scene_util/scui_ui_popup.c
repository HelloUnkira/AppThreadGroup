/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_alpha_t  popup_alpha  = scui_alpha_cover;       // 淡出透明度
static scui_handle_t popup_string = SCUI_HANDLE_INVALID;

void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string)
{
    if (scui_widget_style_is_show(SCUI_UI_SCENE_POPUP))
        return;
    
    popup_alpha = scui_alpha_cover;
    scui_widget_show(SCUI_UI_SCENE_POPUP, false);
    scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, popup_alpha, true);
    scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
    
    if (text != SCUI_HANDLE_INVALID)
        scui_string_update_text(popup_string, text);
    else if (string != NULL)
        scui_string_update_str(popup_string, string);
    
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (scui_string_scroll_over(popup_string)) {
            /* 完全隐藏则回收控件 */
            if (popup_alpha == 0) {
                scui_widget_hide(SCUI_UI_SCENE_POPUP, true);
            }
            
            if (popup_alpha > 0) {
                scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_POPUP_FADE_TIME / SCUI_ANIMA_TICK);
                if (popup_alpha  > tick)
                    popup_alpha -= tick;
                else
                    popup_alpha  = 0;
                
                SCUI_LOG_INFO("popup alpha:%d", popup_alpha);
                scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, popup_alpha, true);
                scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
            }
        }
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_string_maker_t string_maker = {0};
            string_maker.widget.type        = scui_widget_type_string;
            string_maker.widget.style.trans = true;
            string_maker.widget.parent      = SCUI_UI_SCENE_POPUP_BG;
            string_maker.widget.clip.x      = 20;
            string_maker.widget.clip.y      = 30;
            string_maker.widget.clip.w      = 330;
            string_maker.widget.clip.h      = 40;
            string_maker.font_idx           = 0,
            string_maker.args.gap_none      = SCUI_STRING_SPACE_WIDTH;
            string_maker.args.color.color_s.full = 0xFFFFFFFF;
            string_maker.args.color.color_e.full = 0xFFFFFFFF;
            string_maker.unit_s             = true;     //单次滚动,结束标记
            scui_string_create(&string_maker, &popup_string, false);
        }
        
        scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, scui_alpha_trans, true);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip  = scui_widget_clip(event->object);
        scui_color_t color = {0};
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        scui_widget_draw_color(event->object, &clip, color);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_bg_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        // 这里需要填完全覆盖的纯透明色调
        scui_alpha_t alpha = scui_widget_alpha_get(event->object);
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        scui_widget_draw_color(event->object, NULL, (scui_color_t){0});
        scui_widget_alpha_set(event->object, alpha, false);
        
        scui_handle_t image = scui_image_prj_image_src_repeat_btn_01_card_mediunpng;
        scui_widget_draw_image(event->object, NULL, image, NULL, (scui_color_t){0});
        
        break;
    }
    break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
