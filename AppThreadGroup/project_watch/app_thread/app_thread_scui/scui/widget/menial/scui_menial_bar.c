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
void scui_menial_bar_make(bool maker, void *inst)
{
    scui_menial_t *menial = inst;
    scui_menial_maker_t *menial_maker = inst;
    
    if (maker) {
    } else {
        /* 不能同时开启slider和switch */
        bool ext_slider = menial->data.bar.ext_slider;
        bool ext_switch = menial->data.bar.ext_switch;
        SCUI_ASSERT(!(ext_slider && ext_switch));
        
        /* 运行时默认(未配置补默认), 样式默认由 apply 应用常规 res */
        if (SCUI_IS_ZERO_VAL_F(menial->data.bar.value_lim))
            menial->data.bar.value_lim = 100.0f;
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_bar_burn(scui_menial_t *menial)
{
}

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_bar_style(scui_handle_t handle, scui_menial_bar_res_t *res)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    scui_coord_t idx = 0;
    if (res->part == scui_object_part_rect_bg) idx = 0;
    if (res->part == scui_object_part_rect_fg) idx = 1;
    
    scui_object_sub_t sub = {.part = res->part};
    sub.rect.alpha.alpha           = scui_alpha_cover;
    sub.rect.color.color32         = res->color[idx].color_s;
    sub.rect.align.align           = scui_opt_pos_l | scui_opt_pos_u;
    sub.rect.width.number          = widget->clip.w;
    sub.rect.height.number         = widget->clip.h;
    sub.rect.radius.number         = res->radius;
    sub.rect.multi.multi.grad_w    = menial->data.bar.way;
    sub.rect.multi.multi.grad      = res->grad;
    sub.rect.grad_c.color32        = res->color[idx].color_e;
    
    if (res->part == scui_object_part_rect_fg) {
        sub.rect.width.number  = menial->data.bar.way ? widget->clip.w : 0;
        sub.rect.height.number = menial->data.bar.way ? 0 : widget->clip.h;
    }
    
    sub.state = scui_object_state_def;
    scui_object_prop_rect(handle, &sub);
    
    /* 同步全局time属性(默认值/可覆盖) */
    scui_coord_t time = res->time;
    if (time == 0) time = SCUI_WIDGET_MENIAL_BAR_TIME;
    scui_object_prop_add_s(handle, scui_object_part_main,
        scui_object_style_main_time, scui_object_state_def,
        scui_object_data_number(time));
    
    /* 样式修改复位到默认值 */
    scui_menial_bar_update_value(handle, 0.0f, false);
}

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_menial_bar_current_value(scui_handle_t handle, scui_coord3_t *value)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_bar);
    
    *value = menial->data.bar.value_cur;
}

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_menial_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_bar);
    
    /* 这可以实现丝滑到分段效果 */
    value = scui_clamp(value, 0.0f, menial->data.bar.value_lim);
    if (menial->data.bar.value_int) value = (scui_coord_t)value;
    
    /* 可选:slider无动画 */
    /* 可选:switch有动画,端点值 */
    if (menial->data.bar.ext_slider) anim = false;
    if (menial->data.bar.ext_switch) {
        scui_coord3_t value_d = menial->data.bar.value_lim;
        value = (value < value_d / 2) ? 0.0f : value_d;
    }
    menial->data.bar.value_cur = value;
    scui_event_define(event, widget->myself, true, scui_event_update_value, NULL);
    scui_event_notify(&event);
    
    bool way = menial->data.bar.way;
    scui_object_prop_t prop_def = {0};
    scui_object_tran_t tran_def = {0};
    prop_def.part  = scui_object_part_rect_fg;
    prop_def.state = scui_object_state_def;
    if (way) prop_def.style = scui_object_style_rect_height;
    else prop_def.style = scui_object_style_rect_width;
    scui_object_prop_sync(handle, &prop_def);
    
    #if 1
    /* 计算宽高值 */
    scui_area_t  dst_part = widget->clip;
    scui_coord3_t value_d = menial->data.bar.value_lim;
    scui_coord3_t value_c = menial->data.bar.value_cur;
    scui_object_data_t value_m = {0};
    scui_object_prop_sync_s(handle, scui_object_part_rect_bg,
        scui_object_style_rect_radius, scui_object_state_def, value_m);
    
    value_m.number *= 2;
    scui_coord3_t value_l = scui_min(dst_part.w, dst_part.h);
    if (value_m.number < 0) value_m.number = value_l;
    value_m.number = scui_clamp(value_m.number, 0, value_l);
    
    scui_coord_t size_max = way ? dst_part.h : dst_part.w;
    scui_coord_t size_min = scui_map(value_c, 0.0f, value_d, 0, size_max - value_m.number);
    size_min = scui_clamp(value_m.number + size_min, value_m.number, size_max);
    #endif
    
    tran_def.part    = prop_def.part;
    tran_def.state_p = prop_def.state;
    tran_def.state_n = prop_def.state;
    tran_def.style   = prop_def.style;
    tran_def.data_p.number = prop_def.data.number;
    tran_def.data_n.number = scui_map(value, 0.0f, value_d, size_min, size_max);
    SCUI_LOG_INFO("tran(%d->%d)", tran_def.data_p.number, tran_def.data_n.number);
    
    if (anim) {
        /* 同步time属性 */
        scui_object_data_t main_time = {0};
        scui_object_prop_sync_s(handle, scui_object_part_main,
            scui_object_style_main_time, scui_object_state_def, main_time);
        
        scui_coord_t  val_dif = scui_dist(tran_def.data_p.number, tran_def.data_n.number);
        scui_coord3_t value_d = menial->data.bar.value_lim;
        tran_def.time = scui_map(val_dif, 0, size_max, 0,
            main_time.number * value_d / 100.0f);
        
        /* 过渡动画更新 */
        scui_object_tran_add(handle, &tran_def);
        scui_object_tran_work(handle, &tran_def);
    } else {
        /* 直接更新(过渡动画移除) */
        scui_object_tran_del(handle, &tran_def);
        
        prop_def.data.number = tran_def.data_n.number;
        scui_object_prop_add(handle, &prop_def);
    }
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_bar_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_ptr_click: {
        if (!menial->data.bar.ext_switch)
             break;
        
        scui_coord3_t value_c = menial->data.bar.value_cur;
        scui_coord3_t value_d = menial->data.bar.value_lim;
        value_c = (value_c > value_d / 2) ? 0.0f : value_d;
        
        scui_menial_bar_update_value(widget->myself, value_c, true);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_move: {
        if (!menial->data.bar.ext_slider)
             break;
        
        scui_point_t ptr_c = event->ptr_e;
        scui_area_t  dst_part = widget->clip;
        scui_area_m_to_s(&dst_part, &dst_part);
        
        scui_coord3_t value_c = 0.0f;
        scui_coord3_t value_d = menial->data.bar.value_lim;
        /* 取最后的落点计算百分比值就地更新 */
        if (menial->data.bar.way) {
            value_c = scui_map(ptr_c.y, dst_part.y1, dst_part.y2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        } else {
            value_c = scui_map(ptr_c.x, dst_part.x1, dst_part.x2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        }
        
        scui_menial_bar_update_value(widget->myself, value_c, false);
        scui_event_mask_over(event);
        break;
    }
    
    case scui_event_draw_graph: {
        
        scui_object_prop_t prop = {0};
        prop.part = scui_object_part_rect_bg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_rect(event->object, &prop);
        
        prop.part = scui_object_part_rect_fg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_rect(event->object, &prop);
        break;
    }
    default:
        break;
    }
}
