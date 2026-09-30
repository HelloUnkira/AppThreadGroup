/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_btn_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t  *widget  = inst;
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t  *widget_maker  = inst_maker;
    scui_object_maker_t  *object_maker  = (void *)widget_maker;
    scui_obj_btn_maker_t *obj_btn_maker = (void *)widget_maker;
    
    /* 必须标记ptr,widget事件 */
    widget_maker->style.indev_ptr    = true;
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_btn));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_btn->fixed = obj_btn_maker->fixed;
    obj_btn->check = obj_btn_maker->check;
    obj_btn->click = false;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_btn_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_btn));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_btn_style(scui_handle_t handle, scui_obj_btn_res_t *res)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    
    scui_coord_t time = res->time;
    if (time == 0) time = SCUI_WIDGET_OBJ_BTN_TIME;
    scui_coord_t lim = res->lim;
    if (lim == 0) lim = SCUI_WIDGET_OBJ_BTN_PCT;
    
    /* 统一基准(form_rect_all): 几何+缩放tran */
    if (res->form == scui_object_form_rect_all) {
        /* 部件宽高 */
        scui_coord_t area_w = widget->clip.w;
        scui_coord_t area_h = widget->clip.h;
        scui_multi_t scale_w = (scui_multi_t)area_w * lim / 100;
        scui_multi_t scale_h = (scui_multi_t)area_h * lim / 100;
        
        scui_object_sub_t sub = {.part = res->part, .form = scui_object_form_rect_all};
        sub.rect.alpha.alpha   = res->alpha;
        sub.rect.align.align   = res->align;
        sub.rect.radius.number = res->radius;
        
        /* 几何(def=缩放; pre/chk=area; fixed恒等) */
        sub.state = scui_object_state_def;
        sub.rect.width.number  = obj_btn->fixed ? area_w : scale_w;
        sub.rect.height.number = obj_btn->fixed ? area_h : scale_h;
        scui_object_prop_rect(handle, &sub);
        sub.state = scui_object_state_pre;
        sub.rect.width.number  = area_w;
        sub.rect.height.number = area_h;
        scui_object_prop_rect(handle, &sub);
        if (obj_btn->check) {
            sub.state = scui_object_state_chk;
            sub.rect.width.number  = obj_btn->fixed ? area_w : scale_w;
            sub.rect.height.number = obj_btn->fixed ? area_h : scale_h;
            scui_object_prop_rect(handle, &sub);
        }
        
        /* def<->pre 缩放tran(fixed无) */
        if (!obj_btn->fixed) {
            scui_object_tran_add_s2(handle, res->part, scui_object_form_rect_all,
                scui_object_style_rect_width, scui_object_state_def, scui_object_state_pre,
                scui_object_data_number(scale_w), scui_object_data_number(area_w), NULL, time, 0);
            scui_object_tran_add_s2(handle, res->part, scui_object_form_rect_all,
                scui_object_style_rect_height, scui_object_state_def, scui_object_state_pre,
                scui_object_data_number(scale_h), scui_object_data_number(area_h), NULL, time, 0);
        }
        /* chk<->pre 缩放tran */
        if (obj_btn->check && !obj_btn->fixed) {
            scui_object_tran_add_s2(handle, res->part, scui_object_form_rect_all,
                scui_object_style_rect_width, scui_object_state_chk, scui_object_state_pre,
                scui_object_data_number(scale_w), scui_object_data_number(area_w), NULL, time, 0);
            scui_object_tran_add_s2(handle, res->part, scui_object_form_rect_all,
                scui_object_style_rect_height, scui_object_state_chk, scui_object_state_pre,
                scui_object_data_number(scale_h), scui_object_data_number(area_h), NULL, time, 0);
        }
        
        /* 同步time属性 */
        scui_object_prop_add_s(handle, scui_object_part_main, 0,
            scui_object_style_main_time, scui_object_state_def,
            scui_object_data_number(time));
        return;
    }
    
    /* 该层样式: alpha/color/stroke/grad+颜色tran(阴影仅sha) */
    bool shadow = (res->form == scui_object_form_rect_sha);
    
    scui_object_sub_t sub = {.part = res->part, .form = res->form};
    sub.rect.alpha.alpha        = res->alpha;
    sub.rect.stroke.number      = res->width;
    sub.rect.multi.multi.grad   = res->grad;
    sub.rect.multi.multi.grad_w = res->gradw;
    sub.rect.multi.multi.shadow = shadow;
    
    sub.rect.color.color32  = res->color[0].color_s;
    sub.rect.grad_c.color32 = res->color[0].color_e;
    sub.state = scui_object_state_def;
    scui_object_prop_rect(handle, &sub);
    sub.state = scui_object_state_pre;
    sub.rect.color.color32  = res->color[1].color_s;
    sub.rect.grad_c.color32 = res->color[1].color_e;
    scui_object_prop_rect(handle, &sub);
    
    if (obj_btn->check) {
        sub.state = scui_object_state_chk;
        sub.rect.color.color32  = res->color[2].color_s;
        sub.rect.grad_c.color32 = res->color[2].color_e;
        scui_object_prop_rect(handle, &sub);
        sub.state = scui_object_state_pre;
        sub.rect.color.color32  = res->color[3].color_s;
        sub.rect.grad_c.color32 = res->color[3].color_e;
        scui_object_prop_rect(handle, &sub);
    }
    
    /* 颜色tran(def<->pre) */
    scui_object_tran_add_s2(handle, res->part, res->form, scui_object_style_rect_color,
        scui_object_state_def, scui_object_state_pre,
        scui_object_data_color32(res->color[0].color_s),
        scui_object_data_color32(res->color[1].color_s), NULL, time, 0);
    if (res->grad) {
        scui_object_tran_add_s2(handle, res->part, res->form, scui_object_style_rect_grad_c,
            scui_object_state_def, scui_object_state_pre,
            scui_object_data_color32(res->color[0].color_e),
            scui_object_data_color32(res->color[1].color_e), NULL, time, 0);
    }
    /* 颜色tran(chk<->pre) */
    if (obj_btn->check) {
        scui_object_tran_add_s2(handle, res->part, res->form, scui_object_style_rect_color,
            scui_object_state_chk, scui_object_state_pre,
            scui_object_data_color32(res->color[2].color_s),
            scui_object_data_color32(res->color[3].color_s), NULL, time, 0);
        if (res->grad) {
            scui_object_tran_add_s2(handle, res->part, res->form, scui_object_style_rect_grad_c,
                scui_object_state_chk, scui_object_state_pre,
                scui_object_data_color32(res->color[2].color_e),
                scui_object_data_color32(res->color[3].color_e), NULL, time, 0);
        }
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_btn_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t  *widget  = scui_handle_source_check(event->object);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 动画完成才消费点击标记 */
        if (!obj_btn->click) break;
        
        scui_object_type_t state = scui_object_type_none;
        scui_object_state_get(event->object, &state);
        if (state == scui_object_state_def ||
            state == scui_object_state_chk) {
            
            /* 过渡动画未结束则不消费 */
            if (!scui_object_tran_idle(event->object)) break;
            
            obj_btn->click = false;
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
        
        if (obj_btn->check) {
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
        obj_btn->click = true;
        break;
    }
    
    case scui_event_draw_graph: {
        
        /* 统一基准: 刷part的form */
        scui_object_sub_t sub = {.part = scui_object_part_rect_bg};
        scui_object_state_get(event->object, &sub.state);
        /* 无基准几何 → 回退def */
        if (!scui_object_form_rect(event->object, &sub) &&
            sub.state != scui_object_state_def) {
            sub.state  = scui_object_state_def;
            scui_object_form_rect(event->object, &sub);
        }
        
        /* 默认绘制全部层级: 阴影->基础->边界->盒子 */
        static const scui_object_type_t form_table[] = {
            scui_object_form_rect_sha,
            scui_object_form_rect_base,
            scui_object_form_rect_edge,
            scui_object_form_rect_box,
        };
        for (uint8_t idx = 0; idx < scui_arr_len(form_table); idx++) {
            scui_object_prop_t prop = {.form = form_table[idx]};
            prop.part  = scui_object_part_rect_bg;
            prop.state = sub.state;
            /* 样式不全 → 回退def */
            if (!scui_object_draw_rect(event->object, &prop) &&
                prop.state != scui_object_state_def) {
                prop.state  = scui_object_state_def;
                scui_object_draw_rect(event->object, &prop);
            }
        }
        break;
    }
    default:
        break;
    }
}