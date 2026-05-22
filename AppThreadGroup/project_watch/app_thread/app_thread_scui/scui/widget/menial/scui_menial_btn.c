/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造(子类型)
 *@param maker inst是构造器
 *@param inst  构造器或实例
 */
void scui_menial_btn_make(bool maker, void *inst)
{
    scui_menial_t *menial = inst;
    scui_menial_maker_t *menial_maker = inst;
    
    if (maker) {
        
        /* 必须标记widget事件 */
        menial_maker->widget.style.sched_widget = true;
    } else {
        
        /* 未配置使用默认值 */
        if (menial->data.btn.time == 0) menial->data.btn.time = SCUI_WIDGET_MENIAL_BTN_TIME;
        if (menial->data.btn.lim  == 0) menial->data.btn.lim  = SCUI_WIDGET_MENIAL_BTN_PCT;
        if (menial->data.btn.fixed)     menial->data.btn.lim  = 100;
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_burn(scui_menial_t *menial)
{
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_btn_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_ptr_down:{
        if (!scui_widget_event_inside(event))
             break;
        
        scui_object_state_set(event->object, scui_object_state_pre);
        break;
    }
    case scui_event_ptr_up: {
        scui_object_type_t state = scui_object_type_none;
        scui_object_state_get(event->object, &state);
        if (state != scui_object_state_pre) break;
        
        if (menial->data.btn.check) {
            scui_object_type_t state_l = scui_object_type_none;
            scui_object_state_l_get(event->object, &state_l);
            
            if (state_l == scui_object_state_def) {
                scui_object_state_set(event->object, scui_object_state_chk);
                break;
            }
            if (state_l == scui_object_state_chk) {
                scui_object_state_set(event->object, scui_object_state_def);
                break;
            }
        }
        
        scui_object_state_set(event->object, scui_object_state_def);
        break;
    }
    case scui_event_ptr_move: {
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        
        /* 这里是直接响应的,要不要延迟到抬起? */
        scui_event_define(event, widget->myself, true, scui_event_button_click, NULL);
        scui_event_notify(&event);
        break;
    }
    
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        scui_object_sub_t sub = {0};
        
        sub.rect.alpha.alpha       = scui_alpha_cover;
        sub.rect.align.align       = scui_opt_pos_c;
        sub.rect.width.number      = widget_clip.w;
        sub.rect.height.number     = widget_clip.h;
        sub.rect.radius.number     = menial->data.btn.radius;
        sub.rect.side_width.number = menial->data.btn.width;
        
        scui_object_press_set(event->object, true);
        scui_object_check_set(event->object, menial->data.btn.check);
        
        /* def<->pre */
        if (true) {
            sub.part  = scui_object_part_rect_bg;
            sub.state = scui_object_state_def;
            scui_object_prop_rect(event->object, &sub);
            
            sub.part  = scui_object_part_rect_bg;
            sub.state = scui_object_state_pre;
            scui_object_prop_rect(event->object, &sub);
        }
        /* chk */
        if (menial->data.btn.check) {
            
            sub.part  = scui_object_part_rect_bg;
            sub.state = scui_object_state_chk;
            scui_object_prop_rect(event->object, &sub);
        }
        
        /* def<->pre */
        if (true) {
            scui_object_prop_t prop_def = {0};
            scui_object_prop_t prop_pre = {0};
            scui_object_tran_t tran_def = {0};
            scui_object_tran_t tran_pre = {0};
            prop_def.part  = scui_object_part_rect_bg;
            prop_pre.part  = scui_object_part_rect_bg;
            prop_def.state = scui_object_state_def;
            prop_pre.state = scui_object_state_pre;
            tran_def.state_p = scui_object_state_pre;
            tran_def.state_n = scui_object_state_def;
            tran_pre.state_p = scui_object_state_def;
            tran_pre.state_n = scui_object_state_pre;
            tran_def.part = scui_object_part_rect_bg;
            tran_pre.part = scui_object_part_rect_bg;
            tran_def.time = menial->data.btn.time;
            tran_pre.time = menial->data.btn.time;
            
            /* color bg prop */
            prop_def.style = scui_object_style_rect_color;
            prop_pre.style = scui_object_style_rect_color;
            prop_def.data.color32 = menial->data.btn.color[0].color_l;
            prop_pre.data.color32 = menial->data.btn.color[0].color_d;
            scui_object_prop_add(event->object, &prop_def);
            scui_object_prop_add(event->object, &prop_pre);
            /* color bg tran */
            tran_def.style = scui_object_style_rect_color;
            tran_pre.style = scui_object_style_rect_color;
            scui_object_tran_add_by(event->object, &tran_def);
            scui_object_tran_add_by(event->object, &tran_pre);
            
            /* width && height prop */
            prop_def.style = scui_object_style_rect_width;
            prop_pre.style = scui_object_style_rect_width;
            prop_def.data.number = (scui_multi_t)widget_clip.w * menial->data.btn.lim / 100;
            prop_pre.data.number = (scui_multi_t)widget_clip.w * 100 / 100;
            scui_object_prop_add(event->object, &prop_def);
            scui_object_prop_add(event->object, &prop_pre);
            prop_def.style = scui_object_style_rect_height;
            prop_pre.style = scui_object_style_rect_height;
            prop_def.data.number = (scui_multi_t)widget_clip.h * menial->data.btn.lim / 100;
            prop_pre.data.number = (scui_multi_t)widget_clip.h * 100 / 100;
            scui_object_prop_add(event->object, &prop_def);
            scui_object_prop_add(event->object, &prop_pre);
            
            if (!menial->data.btn.fixed) {
                /* width && height tran */
                tran_def.style = scui_object_style_rect_width;
                tran_pre.style = scui_object_style_rect_width;
                scui_object_tran_add_by(event->object, &tran_def);
                scui_object_tran_add_by(event->object, &tran_pre);
                tran_def.style = scui_object_style_rect_height;
                tran_pre.style = scui_object_style_rect_height;
                scui_object_tran_add_by(event->object, &tran_def);
                scui_object_tran_add_by(event->object, &tran_pre);
            }
        }
        
        /* chk<->pre */
        if (menial->data.btn.check) {
            scui_object_prop_t prop_chk = {0};
            scui_object_prop_t prop_pre = {0};
            scui_object_tran_t tran_chk = {0};
            scui_object_tran_t tran_pre = {0};
            prop_chk.part  = scui_object_part_rect_bg;
            prop_pre.part  = scui_object_part_rect_bg;
            prop_chk.state = scui_object_state_chk;
            prop_pre.state = scui_object_state_pre;
            tran_chk.state_p = scui_object_state_pre;
            tran_chk.state_n = scui_object_state_chk;
            tran_pre.state_p = scui_object_state_chk;
            tran_pre.state_n = scui_object_state_pre;
            tran_chk.part = scui_object_part_rect_bg;
            tran_pre.part = scui_object_part_rect_bg;
            tran_chk.time = menial->data.btn.time;
            tran_pre.time = menial->data.btn.time;
            
            /* color bg prop */
            prop_chk.style = scui_object_style_rect_color;
            prop_pre.style = scui_object_style_rect_color;
            prop_chk.data.color32 = menial->data.btn.color[1].color_l;
            prop_pre.data.color32 = menial->data.btn.color[1].color_d;
            scui_object_prop_add(event->object, &prop_chk);
            scui_object_prop_add(event->object, &prop_pre);
            /* color bg tran */
            tran_chk.style = scui_object_style_rect_color;
            tran_pre.style = scui_object_style_rect_color;
            scui_object_tran_add_by(event->object, &tran_chk);
            scui_object_tran_add_by(event->object, &tran_pre);
            
            /* width && height prop */
            prop_chk.style = scui_object_style_rect_width;
            prop_pre.style = scui_object_style_rect_width;
            prop_chk.data.number = (scui_multi_t)widget_clip.w * menial->data.btn.lim / 100;
            prop_pre.data.number = (scui_multi_t)widget_clip.w * 100 / 100;
            scui_object_prop_add(event->object, &prop_chk);
            scui_object_prop_add(event->object, &prop_pre);
            prop_chk.style = scui_object_style_rect_height;
            prop_pre.style = scui_object_style_rect_height;
            prop_chk.data.number = (scui_multi_t)widget_clip.h * menial->data.btn.lim / 100;
            prop_pre.data.number = (scui_multi_t)widget_clip.h * 100 / 100;
            scui_object_prop_add(event->object, &prop_chk);
            scui_object_prop_add(event->object, &prop_pre);
            
            if (!menial->data.btn.fixed) {
                /* width && height tran */
                tran_chk.style = scui_object_style_rect_width;
                tran_pre.style = scui_object_style_rect_width;
                scui_object_tran_add_by(event->object, &tran_chk);
                scui_object_tran_add_by(event->object, &tran_pre);
                tran_chk.style = scui_object_style_rect_height;
                tran_pre.style = scui_object_style_rect_height;
                scui_object_tran_add_by(event->object, &tran_chk);
                scui_object_tran_add_by(event->object, &tran_pre);
            }
        }
        break;
    }
    
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_object_prop_t prop = {0};
        prop.part = scui_object_part_rect_bg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_rect(event->object, &prop);
        break;
    }
    default:
        break;
    }
}
