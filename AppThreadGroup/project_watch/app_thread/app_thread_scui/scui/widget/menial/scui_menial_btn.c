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
        
        scui_multi_t scale_w = (scui_multi_t)widget_clip.w * menial->data.btn.lim / 100;
        scui_multi_t scale_h = (scui_multi_t)widget_clip.h * menial->data.btn.lim / 100;
        scui_multi_t default_w = widget_clip.w;
        scui_multi_t default_h = widget_clip.h;
        
        /* def<->pre */
        if (true) {
            /* color bg prop */
            scui_object_prop_new(event->object, rect_bg, rect_color, def, scui_object_data_color32(menial->data.btn.color[0].color_l));
            scui_object_prop_new(event->object, rect_bg, rect_color, pre, scui_object_data_color32(menial->data.btn.color[0].color_d));
            
            /* color bg tran */
            scui_object_tran_new2(event->object, rect_bg, rect_color, def, pre,
                scui_object_data_color32(menial->data.btn.color[0].color_l),
                scui_object_data_color32(menial->data.btn.color[0].color_d),
                NULL, menial->data.btn.time, 0);
            
            /* width && height prop */
            scui_object_prop_new(event->object, rect_bg, rect_width,  def, scui_object_data_number(scale_w));
            scui_object_prop_new(event->object, rect_bg, rect_width,  pre, scui_object_data_number(default_w));
            scui_object_prop_new(event->object, rect_bg, rect_height, def, scui_object_data_number(scale_h));
            scui_object_prop_new(event->object, rect_bg, rect_height, pre, scui_object_data_number(default_h));
            
            if (!menial->data.btn.fixed) {
                /* width && height tran */
                scui_object_tran_new2(event->object, rect_bg, rect_width,  def, pre, scui_object_data_number(scale_w),
                    scui_object_data_number(default_w), NULL, menial->data.btn.time, 0);
                scui_object_tran_new2(event->object, rect_bg, rect_height, def, pre, scui_object_data_number(scale_h),
                    scui_object_data_number(default_h), NULL, menial->data.btn.time, 0);
            }
        }
        
        /* chk<->pre */
        if (menial->data.btn.check) {
            /* color bg prop */
            scui_object_prop_new(event->object, rect_bg, rect_color, chk, scui_object_data_color32(menial->data.btn.color[1].color_l));
            scui_object_prop_new(event->object, rect_bg, rect_color, pre, scui_object_data_color32(menial->data.btn.color[1].color_d));
            
            /* color bg tran */
            scui_object_tran_new2(event->object, rect_bg, rect_color, chk, pre,
                scui_object_data_color32(menial->data.btn.color[1].color_l),
                scui_object_data_color32(menial->data.btn.color[1].color_d),
                NULL, menial->data.btn.time, 0);
            
            /* width && height prop */
            scui_object_prop_new(event->object, rect_bg, rect_width,  chk, scui_object_data_number(scale_w));
            scui_object_prop_new(event->object, rect_bg, rect_width,  pre, scui_object_data_number(default_w));
            scui_object_prop_new(event->object, rect_bg, rect_height, chk, scui_object_data_number(scale_h));
            scui_object_prop_new(event->object, rect_bg, rect_height, pre, scui_object_data_number(default_h));
            
            if (!menial->data.btn.fixed) {
                /* width && height tran */
                scui_object_tran_new2(event->object, rect_bg, rect_width,  chk, pre, scui_object_data_number(scale_w),
                    scui_object_data_number(default_w), NULL, menial->data.btn.time, 0);
                scui_object_tran_new2(event->object, rect_bg, rect_height, chk, pre, scui_object_data_number(scale_h),
                    scui_object_data_number(default_h), NULL, menial->data.btn.time, 0);
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
