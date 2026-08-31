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
    } else {
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_burn(scui_menial_t *menial)
{
}

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_btn_style(scui_handle_t handle, scui_menial_btn_res_t *res)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    /* 部件宽高 */
    scui_coord_t area_w = res->area.w ? res->area.w : widget->clip.w;
    scui_coord_t area_h = res->area.h ? res->area.h : widget->clip.h;
    
    /* 几何属性(def<->pre) */
    scui_object_sub_t sub = {.part = res->part};
    sub.rect.alpha.alpha        = scui_alpha_cover;
    sub.rect.align.align        = scui_opt_pos_c;
    sub.rect.width.number       = area_w;
    sub.rect.height.number      = area_h;
    sub.rect.radius.number      = res->radius;
    sub.rect.stroke.number      = res->width;
    sub.rect.multi.multi.grad   = res->grad;
    sub.rect.multi.multi.grad_w = res->gradw;
    sub.rect.multi.multi.shadow = res->shadow;
    
    sub.state = scui_object_state_def;
    scui_object_prop_rect(handle, &sub);
    sub.state = scui_object_state_pre;
    scui_object_prop_rect(handle, &sub);
    
    if (menial->data.btn.check) {
        sub.state = scui_object_state_chk;
        scui_object_prop_rect(handle, &sub);
    }
    
    scui_coord_t time = res->time;
    if (time == 0) time = SCUI_WIDGET_MENIAL_BTN_TIME;
    
    scui_coord_t lim = res->lim;
    if (lim == 0) lim = SCUI_WIDGET_MENIAL_BTN_PCT;
    scui_multi_t scale_w = (scui_multi_t)area_w * lim / 100;
    scui_multi_t scale_h = (scui_multi_t)area_h * lim / 100;
    
    /* def<->pre: 颜色/缩放/动画 */
    {
        /* color prop(def<->pre): color_s状态色, color_e渐变 */
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_color,
            scui_object_state_def, scui_object_data_color32(res->color[0].color_s));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_color,
            scui_object_state_pre, scui_object_data_color32(res->color[1].color_s));
        /* color tran(def<->pre) */
        scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_color,
            scui_object_state_def, scui_object_state_pre,
            scui_object_data_color32(res->color[0].color_s),
            scui_object_data_color32(res->color[1].color_s), NULL, time, 0);
        
        if (res->grad) {
            /* grad_c prop(def<->pre): 渐变终点 */
            scui_object_prop_add_s(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_def, scui_object_data_color32(res->color[0].color_e));
            scui_object_prop_add_s(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_pre, scui_object_data_color32(res->color[1].color_e));
            /* grad_c tran(def<->pre) */
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_def, scui_object_state_pre,
                scui_object_data_color32(res->color[0].color_e),
                scui_object_data_color32(res->color[1].color_e), NULL, time, 0);
        }
        
        /* width && height prop */
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_width,
            scui_object_state_def, scui_object_data_number(scale_w));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_width,
            scui_object_state_pre, scui_object_data_number(area_w));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_height,
            scui_object_state_def, scui_object_data_number(scale_h));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_height,
            scui_object_state_pre, scui_object_data_number(area_h));
        
        if (!menial->data.btn.fixed) {
            /* width && height tran(def<->pre) */
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_width,
                scui_object_state_def, scui_object_state_pre, scui_object_data_number(scale_w),
                scui_object_data_number(area_w), NULL, time, 0);
            
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_height,
                scui_object_state_def, scui_object_state_pre, scui_object_data_number(scale_h),
                scui_object_data_number(area_h), NULL, time, 0);
        }
    }
    
    /* chk<->pre: 颜色/缩放/动画(选中时) */
    if (menial->data.btn.check) {
        /* color prop(chk<->pre): color_s状态色, color_e渐变 */
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_color,
            scui_object_state_chk, scui_object_data_color32(res->color[2].color_s));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_color,
            scui_object_state_pre, scui_object_data_color32(res->color[3].color_s));
        /* color tran(chk<->pre) */
        scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_color,
            scui_object_state_chk, scui_object_state_pre,
            scui_object_data_color32(res->color[2].color_s),
            scui_object_data_color32(res->color[3].color_s), NULL, time, 0);
        
        if (res->grad) {
            /* grad_c prop(chk<->pre): 渐变终点 */
            scui_object_prop_add_s(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_chk, scui_object_data_color32(res->color[2].color_e));
            scui_object_prop_add_s(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_pre, scui_object_data_color32(res->color[3].color_e));
            /* grad_c tran(chk<->pre) */
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_grad_c,
                scui_object_state_chk, scui_object_state_pre, scui_object_data_color32(res->color[2].color_e),
                scui_object_data_color32(res->color[3].color_e), NULL, time, 0);
        }
        
        /* width && height prop */
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_width,
            scui_object_state_chk, scui_object_data_number(scale_w));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_width,
            scui_object_state_pre, scui_object_data_number(area_w));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_height,
            scui_object_state_chk, scui_object_data_number(scale_h));
        scui_object_prop_add_s(handle, res->part, scui_object_style_rect_height,
            scui_object_state_pre, scui_object_data_number(area_h));
        
        if (!menial->data.btn.fixed) {
            /* width && height tran(chk<->pre) */
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_width,
                scui_object_state_chk, scui_object_state_pre, scui_object_data_number(scale_w),
                scui_object_data_number(area_w), NULL, time, 0);
            
            scui_object_tran_add_s2(handle, res->part, scui_object_style_rect_height,
                scui_object_state_chk, scui_object_state_pre, scui_object_data_number(scale_h),
                scui_object_data_number(area_h), NULL, time, 0);
        }
    }
    
    /* 同步全局time属性(默认值/可覆盖) */
    scui_object_prop_add_s(handle, scui_object_part_main,
        scui_object_style_main_time, scui_object_state_def,
        scui_object_data_number(time));
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
    case scui_event_anima_elapse: {
        /* 动画完成才消费点击标记 */
        if (!menial->data.btn.click) break;
        
        scui_object_type_t state = scui_object_type_none;
        scui_object_state_get(event->object, &state);
        if (state == scui_object_state_def ||
            state == scui_object_state_chk) {
            
            /* 过渡动画未结束则不消费 */
            if (!scui_object_tran_idle(event->object)) break;
            
            menial->data.btn.click = false;
            scui_event_define(event, widget->myself, true, scui_event_button_click, NULL);
            scui_event_notify(&event);
        }
        break;
    }
    case scui_event_ptr_down:{
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
        menial->data.btn.click = true;
        break;
    }
    
    case scui_event_create: {
        
        /* 运行状态初始化(运行时初值已由构造器拷贝) */
        scui_object_press_set(event->object, true);
        scui_object_check_set(event->object, menial->data.btn.check);
        break;
    }
    
    case scui_event_draw_graph: {
        
        /* 默认绘制全部 part: 阴影->外部线->边框->背景 */
        static const scui_object_type_t part_table[] = {
            scui_object_part_rect_sha,
            scui_object_part_rect_box,
            scui_object_part_rect_edge,
            scui_object_part_rect_bg,
        };
        for (uint8_t idx = 0; idx < scui_arr_len(part_table); idx++) {
            scui_object_prop_t prop = {.part = part_table[idx]};
            scui_object_state_get(event->object, &prop.state);
            scui_object_draw_rect(event->object, &prop);
        }
        break;
    }
    default:
        break;
    }
}
