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
void scui_menial_arc_make(bool maker, void *inst)
{
    scui_menial_t *menial = inst;
    scui_menial_maker_t *menial_maker = inst;
    
    if (maker) {
    } else {
        /* 不能同时开启touch和spinner */
        bool ext_touch   = menial->data.arc.ext_touch;
        bool ext_spinner = menial->data.arc.ext_spinner;
        SCUI_ASSERT(!(ext_touch && ext_spinner));
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_burn(scui_menial_t *menial)
{
}

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_arc_style(scui_handle_t handle, scui_menial_arc_res_t *res)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    scui_coord3_t angle_s = res->angle_s;
    scui_coord3_t angle_e = res->angle_e;
    if (menial->data.arc.ext_spinner) {
        angle_s = 0.0f;  angle_e = 360.0f;
    }
    
    /* 补充一个容错的默认参数值 */
    if (!menial->data.arc.ext_spinner &&
        SCUI_IS_ZERO_VAL_F(scui_dist(angle_s, angle_e))) {
        angle_s = 0.0f; angle_e = 360.0f;
    }
    
    scui_coord_t idx = 0;
    if (res->part == scui_object_part_arc_bg) idx = 0;
    if (res->part == scui_object_part_arc_fg) idx = 1;
    
    scui_object_sub_t sub = {.part = res->part};
    sub.arc.alpha.alpha        = scui_alpha_cover;
    sub.arc.color.color32      = res->color[idx].color_s;
    sub.arc.angle_s.number     = angle_s;
    sub.arc.angle_e.number     = angle_e;
    sub.arc.center.point       = res->center;
    sub.arc.radius.number      = res->radius;
    sub.arc.stroke.number      = res->width;
    sub.arc.multi.multi.round  = res->round;
    sub.arc.multi.multi.grad_w = res->gradw;
    sub.arc.multi.multi.grad   = res->grad;
    sub.arc.grad_c.color32     = res->color[idx].color_e;
    
    sub.state = scui_object_state_def;
    scui_object_prop_arc(handle, &sub);
    
    if (res->part == scui_object_part_arc_bg);
    if (res->part == scui_object_part_arc_fg) {
        
        /* 同步全局time属性(默认值/可覆盖) */
        scui_coord_t time = res->time;
        if (time == 0) time = SCUI_WIDGET_MENIAL_ARC_TIME;
        scui_coord3_t angle_d = scui_dist(angle_s, angle_e);
        time = time * angle_d / 360.0f;
        
        scui_object_prop_add_s(handle, scui_object_part_main,
            scui_object_style_main_time, scui_object_state_def,
            scui_object_data_number(time));
        
        /* 样式修改复位到默认值 */
        scui_menial_arc_update_value(handle, 0.0f, false);
    }
}

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 */
void scui_menial_arc_current_angle(scui_handle_t handle, scui_coord3_t *angle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_arc);
    if (menial->data.arc.ext_spinner) return;
    /* spinner不使用此接口 */
    
    *angle = menial->data.arc.angle_c;
}

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 *@param anim   动画更新
 */
void scui_menial_arc_update_angle(scui_handle_t handle, scui_coord3_t angle, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_arc);
    if (menial->data.arc.ext_spinner) return;
    /* spinner不使用此接口 */
    
    menial->data.arc.angle_c = angle;
    scui_event_define(event, widget->myself, true, scui_event_update_value, NULL);
    scui_event_notify(&event);
    
    bool anti = menial->data.arc.anti;
    scui_object_prop_t prop_def = {0};
    scui_object_tran_t tran_def = {0};
    prop_def.part  = scui_object_part_arc_fg;
    prop_def.state = scui_object_state_def;
    if (anti) prop_def.style = scui_object_style_arc_angle_s;
    else prop_def.style = scui_object_style_arc_angle_e;
    scui_object_prop_sync(handle, &prop_def);
    
    tran_def.part    = prop_def.part;
    tran_def.state_p = prop_def.state;
    tran_def.state_n = prop_def.state;
    tran_def.style   = prop_def.style;
    tran_def.data_p.number = prop_def.data.number;
    tran_def.data_n.number = menial->data.arc.angle_c;
    SCUI_LOG_INFO("tran(%d->%d)", tran_def.data_p.number, tran_def.data_n.number);
    
    if (anim) {
        /* 同步time属性 */
        scui_object_data_t main_time = {0};
        scui_object_prop_sync_s(handle, scui_object_part_main,
            scui_object_style_main_time, scui_object_state_def, main_time);
        
        scui_object_data_t angle_s = {0};
        scui_object_data_t angle_e = {0};
        scui_object_prop_sync_s(handle, scui_object_part_arc_bg,
            scui_object_style_arc_angle_s, scui_object_state_def, angle_s);
        scui_object_prop_sync_s(handle, scui_object_part_arc_bg,
            scui_object_style_arc_angle_e, scui_object_state_def, angle_e);
        
        scui_coord3_t angle_d = scui_dist(angle_s.number, angle_e.number);
        scui_coord3_t val_dif = scui_dist(tran_def.data_p.number, tran_def.data_n.number);
        tran_def.time = scui_map(val_dif, 0, angle_d, 0,
            main_time.number * angle_d / 360.0f);
        
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

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, 100.0f]
 *@param anim   动画更新
 */
void scui_menial_arc_update_value(scui_handle_t handle, scui_coord3_t value, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_arc);
    if (menial->data.arc.ext_spinner) return;
    /* spinner不使用此接口 */
    
    /* 端点基准从bg取(稳定), fg为动态进度 */
    scui_object_data_t angle_s = {0};
    scui_object_data_t angle_e = {0};
    scui_object_prop_sync_s(handle, scui_object_part_arc_bg,
        scui_object_style_arc_angle_s, scui_object_state_def, angle_s);
    scui_object_prop_sync_s(handle, scui_object_part_arc_bg,
        scui_object_style_arc_angle_e, scui_object_state_def, angle_e);
    
    value = scui_clamp(value, 0.0f, 100.0f);
    scui_menial_arc_update_angle(handle, menial->data.arc.anti ?
        scui_map(value, 100.0f, 0.0f, angle_s.number, angle_e.number) :
        scui_map(value, 0.0f, 100.0f, angle_s.number, angle_e.number), anim);
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_arc_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        if (menial->data.arc.ext_spinner) {
            /* 同步time属性(旋转速度) */
            scui_object_data_t main_time = {0};
            scui_object_prop_sync_s(event->object, scui_object_part_main,
                scui_object_style_main_time, scui_object_state_def, main_time);
            
            /* spinner端点基准固化 */
            scui_coord3_t angle_s = 0.0f;
            scui_coord3_t angle_e = 360.0f;
            scui_coord3_t angle_d = 360.0f;
            menial->data.arc.angle_c += scui_map(event->tick, 0, main_time.number, 0.0f, angle_d);
            if (menial->data.arc.angle_c > 360) menial->data.arc.angle_c -= 360;
            
            scui_coord_t  angle_w = menial->data.arc.anti ? -1 : +1;
            scui_coord_t  angle_c = menial->data.arc.angle_c;
            angle_c = scui_map(angle_c, 0, angle_d, 0, 360);
            /* angle_c映射到[0, 360]度中去(有精度损失但不影响) */
            scui_map_cb_t path_map = scui_map_ease_out;
            if (scui_mabs(angle_c / 180, 2) == 1)
                path_map = scui_map_ease_in;
            
            SCUI_LOG_INFO("angle:%d", angle_c);
            scui_coord_t  angle_p = scui_mabs(angle_c % 180, 180);
            angle_s += angle_w * path_map(angle_p, 0, 180, 0, angle_d);
            angle_e += angle_w * scui_map(angle_p, 0, 180, 0, angle_d) + angle_w * angle_d;
            
            /* 加载圆环背景(0, 360) */
            scui_object_prop_t prop_def = {0};
            prop_def.part  = scui_object_part_arc_bg;
            prop_def.state = scui_object_state_def;
            
            prop_def.data.number = 0;
            prop_def.style = scui_object_style_arc_angle_s;
            scui_object_prop_add(event->object, &prop_def);
            
            prop_def.data.number = 360;
            prop_def.style = scui_object_style_arc_angle_e;
            scui_object_prop_add(event->object, &prop_def);
            
            /* 加载圆环前景(angle_s, angle_e) */
            prop_def.part  = scui_object_part_arc_fg;
            prop_def.state = scui_object_state_def;
            
            prop_def.data.number = angle_s;
            prop_def.style = scui_object_style_arc_angle_s;
            scui_object_prop_add(event->object, &prop_def);
            
            prop_def.data.number = angle_e;
            prop_def.style = scui_object_style_arc_angle_e;
            scui_object_prop_add(event->object, &prop_def);
        }
        break;
    }
    case scui_event_ptr_move: {
        if (!menial->data.arc.ext_touch)
             break;
        if (!widget->state.indev_ptr_hold)
             break;
        
        scui_point_t point = event->ptr_e;
        scui_widget_switch_point(event->object, &point);
        
        scui_object_data_t center = {0};
        scui_object_prop_sync_s(event->object, scui_object_part_arc_bg,
            scui_object_style_arc_center, scui_object_state_def, center);
        
        scui_coord_t cx = center.point.x;
        scui_coord_t cy = center.point.y;
        scui_coord_t x = +(point.x - cx);
        scui_coord_t y = -(point.y - cy);
        if (x == 0 && y == 0) break;
        
        /* 从bg读基准端点(稳定) */
        scui_object_data_t angle_s = {0};
        scui_object_data_t angle_e = {0};
        scui_object_prop_sync_s(event->object, scui_object_part_arc_bg,
            scui_object_style_arc_angle_s, scui_object_state_def, angle_s);
        scui_object_prop_sync_s(event->object, scui_object_part_arc_bg,
            scui_object_style_arc_angle_e, scui_object_state_def, angle_e);
        
        scui_coord_t angle_d = scui_dist(angle_s.number, angle_e.number);
        scui_coord_t angle_n_down = (menial->data.arc.angle_down - angle_s.number + 360) % 360;
        /* 值从落点(角)起始: 落点归一化到bg范围得value_base, 再加指针增量, 卡在[0,100] */
        if (angle_n_down > angle_d) angle_n_down = angle_d;
        
        scui_coord3_t value_base = menial->data.arc.anti ?
            (angle_d - angle_n_down) * 100.0f / angle_d :
            angle_n_down * 100.0f / angle_d;
        
        scui_coord_t angle = (scui_atan2(x, y) - 90 + 360) % 360;
        scui_coord_t delta = angle - menial->data.arc.angle_down;
        if (delta > +180) delta -= 360;
        if (delta < -180) delta += 360;
        
        scui_coord3_t value = value_base + (menial->data.arc.anti ?
            -delta : delta) * 100.0f / angle_d;
        
        value = scui_clamp(value, 0.0f, 100.0f);
        scui_menial_arc_update_value(event->object, value, false);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_down: {
        if (!menial->data.arc.ext_touch)
             break;
        
        scui_point_t point = event->ptr_c;
        scui_widget_switch_point(event->object, &point);
        
        scui_object_data_t center = {0};
        scui_object_prop_sync_s(event->object, scui_object_part_arc_bg,
            scui_object_style_arc_center, scui_object_state_def, center);
        
        scui_coord_t cx = center.point.x;
        scui_coord_t cy = center.point.y;
        scui_coord_t x = +(point.x - cx);
        scui_coord_t y = -(point.y - cy);
        if (x == 0 && y == 0) break;
        
        scui_coord_t angle = (scui_atan2(x, y) - 90 + 360) % 360;
        menial->data.arc.angle_down = angle;
        
        widget->state.indev_ptr_hold = true;
        scui_menial_arc_update_angle(event->object, angle, false);
        break;
    }
    case scui_event_ptr_up:
        widget->state.indev_ptr_hold = false;
        break;
    case scui_event_draw_graph: {
        
        scui_object_prop_t prop = {0};
        prop.part = scui_object_part_arc_bg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_arc(event->object,  &prop);
        
        prop.part = scui_object_part_arc_fg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_arc(event->object,  &prop);
        break;
    }
    default:
        break;
    }
}
