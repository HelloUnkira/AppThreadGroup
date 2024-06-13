/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_coord_t  popup_wait   = 0;                      // 暂停时间
static scui_alpha_t  popup_alpha  = scui_alpha_cover;       // 淡出透明度
static scui_handle_t popup_string = SCUI_HANDLE_INVALID;

void scui_ui_scene_popup_text(scui_handle_t text, uint8_t *string)
{
    if (text != SCUI_HANDLE_INVALID) {
        scui_string_update_text(popup_string, text);
        return;
    }
    if (string != NULL) {
        scui_string_update_str(popup_string, string);
        return;
    }
    SCUI_ASSERT(false);
}

void scui_ui_scene_popup_exec(void)
{
    popup_wait  = 0;
    popup_alpha = scui_alpha_cover;
    scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, popup_alpha, true);
    scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (popup_wait  < SCUI_UI_POPUP_STOP_TIME)
            popup_wait += SCUI_ANIMA_TICK;
        else
        if (popup_alpha > 0) {
            scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_POPUP_FADE_TIME / SCUI_ANIMA_TICK);
            if (popup_alpha  > tick)
                popup_alpha -= tick;
            else
                popup_alpha  = 0;
            
            SCUI_LOG_WARN("popup alpha:%d", popup_alpha);
            scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, popup_alpha, true);
            scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
        }
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_widget_event_mask_keep(event);
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_string_maker_t string_maker = {0};
            string_maker.widget.type        = scui_widget_type_string;
            string_maker.widget.style.trans = true;
            string_maker.widget.parent      = SCUI_UI_SCENE_POPUP_BG;
            string_maker.widget.clip.x      = 20;
            string_maker.widget.clip.y      = 30;
            string_maker.widget.clip.w      = 330;
            string_maker.widget.clip.h      = 40;
            string_maker.name               = SCUI_FONT_TYPE_32_ZH,
            string_maker.args.margin        = SCUI_STRING_MARGIN;
            string_maker.args.gap_none      = SCUI_STRING_SPACE_WIDTH;
            string_maker.args.color.color_s.full = 0xFFFFFFFF;
            string_maker.args.color.color_e.full = 0xFFFFFFFF;
            scui_string_create(&string_maker, &popup_string, false);
        }
        
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_widget_event_mask_keep(event);
        
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
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
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t   clip = scui_widget_surface_clip(event->object);
        scui_color_t  color = {0};
        scui_handle_t image = scui_image_prj_image_src_repeat_btn_01_card_mediunpng;
        scui_widget_surface_draw_image(event->object, &clip, image, NULL, color);
        
        break;
    }
    break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
