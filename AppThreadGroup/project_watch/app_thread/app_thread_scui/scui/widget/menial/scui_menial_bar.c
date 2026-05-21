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
        
        /* 不能同时开启slider和switch */
        bool ext_slider = menial_maker->data.bar.ext_slider;
        bool ext_switch = menial_maker->data.bar.ext_switch;
        SCUI_ASSERT(!(ext_slider && ext_switch));
        
        /* 必须标记widget事件 */
        /* 条件标记ptr事件 */
        menial_maker->widget.style.sched_widget = true;
        menial_maker->widget.style.indev_ptr    = ext_slider || ext_switch;
    } else {
        
        /* 未配置使用默认值 */
        if (SCUI_IS_ZERO_VAL_F(menial->data.bar.value_lim))
            menial->data.bar.value_lim = 100.0f;
        
        /* 未配置使用默认值 */
        if (menial->data.bar.time == 0 && menial->data.bar.ext_slider)
            menial->data.bar.time  = SCUI_WIDGET_MENIAL_BAR_EXT_SLIDER_TIME;
        if (menial->data.bar.time == 0 && menial->data.bar.ext_switch)
            menial->data.bar.time  = SCUI_WIDGET_MENIAL_BAR_EXT_SWITCH_TIME;
        if (menial->data.bar.time == 0)
            menial->data.bar.time  = SCUI_WIDGET_MENIAL_BAR_TIME;
        
        scui_coord3_t value_d = menial->data.bar.value_lim;
        menial->data.bar.time = menial->data.bar.time * value_d / 100.0f;
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_bar_burn(scui_menial_t *menial)
{
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
    scui_coord3_t value_m = menial->data.bar.radius * 2;
    scui_coord3_t value_l = scui_min(dst_part.w, dst_part.h);
    if (value_m < 0) value_m = value_l;
    value_m = scui_clamp(value_m, 0, value_l);
    
    scui_coord_t size_max = way ? dst_part.h : dst_part.w;
    scui_coord_t size_min = scui_map(value_c, 0.0f, value_d, 0, size_max - value_m);
    size_min = scui_clamp(value_m + size_min, value_m, size_max);
    #endif
    
    tran_def.part    = prop_def.part;
    tran_def.state_p = prop_def.state;
    tran_def.state_n = prop_def.state;
    tran_def.style   = prop_def.style;
    tran_def.data_p.number = prop_def.data.number;
    tran_def.data_n.number = scui_map(value, 0.0f, value_d, size_min, size_max);
    SCUI_LOG_INFO("tran(%d->%d)", tran_def.data_p.number, tran_def.data_n.number);
    
    if (anim) {
        scui_coord_t val_dif = scui_dist(tran_def.data_p.number, tran_def.data_n.number);
        tran_def.time = scui_map(val_dif, 0, size_max, 0, menial->data.bar.time);
        
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
        if (!scui_widget_event_inside(event))
             break;
        
        scui_event_mask_over(event);
        
        scui_coord3_t value_c = menial->data.bar.value_cur;
        scui_coord3_t value_d = menial->data.bar.value_lim;
        value_c = (value_c > value_d / 2) ? 0.0f : value_d;
        
        scui_menial_bar_update_value(widget->myself, value_c, true);
        break;
    }
    case scui_event_ptr_move: {
        if (!menial->data.bar.ext_slider)
             break;
        if (!scui_widget_event_inside(event) &&
            !widget->state.indev_ptr_hold)
             break;
        
        scui_event_mask_over(event);
        scui_point_t ptr_c = event->ptr_e;
        scui_area_t  dst_part = widget->clip;
        scui_area_m_to_s(&dst_part, &dst_part);
        widget->state.indev_ptr_hold = true;
        
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
        break;
    }
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        widget->state.indev_ptr_hold = false;
        break;
    default:
        break;
    }
}

/*@brief 事件处理回调(子类型)(样板)
 *@param event 事件
 */
void scui_menial_bar_event_cb(scui_event_t *event)
{
    scui_menial_data_t *data = NULL;
    scui_menial_data_inst(event->object, &data);
    
    switch (event->type) {
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        scui_object_rect_t rect = {
            .area.w   = widget_clip.w,
            .area.h   = widget_clip.h,
            .alpha[0] = scui_alpha_cover,
            .alpha[1] = scui_alpha_cover,
            .color[0] = data->bar.color[0],
            .color[1] = data->bar.color[1],
            .radius   = data->bar.radius,
            .align    = scui_opt_pos_l | scui_opt_pos_u,
            .grad_w   = data->bar.way,
            .grad     = data->bar.grad,
        };
        
        rect.state = scui_object_state_def;
        rect.index = 0; scui_object_prop_rect(event->object, &rect);
        rect.index = 1; scui_object_prop_rect(event->object, &rect);
        
        /* 需要按下变色时考虑(需要过渡动画) */
        // rect.state = scui_object_state_pre;
        // rect.index = 0; scui_object_prop_rect(event->object, &rect);
        // rect.index = 1; scui_object_prop_rect(event->object, &rect);
        
        scui_menial_bar_update_value(event->object, 0.0f, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
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
