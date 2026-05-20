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
        
        /* 必须标记anima事件 */
        menial_maker->widget.style.sched_anima = true;
    } else {
        
        /* 未配置使用默认值 */
        if (SCUI_IS_ZERO_VAL_F(menial->data.arc.angle_s) &&
            SCUI_IS_ZERO_VAL_F(menial->data.arc.angle_e)) {
            menial->data.arc.angle_s = 0.0f;
            menial->data.arc.angle_e = 360.0f;
        }
        
        if (menial->data.arc.spinner) {
            menial->data.arc.angle_s = 0.0f;
            menial->data.arc.angle_e = 360.0f;
            SCUI_ASSERT(menial->data.arc.width != 0);
        }
        
        /* 未配置使用默认值 */
        if (menial->data.arc.time == 0)
            menial->data.arc.time  = SCUI_WIDGET_MENIAL_ARC_TIME;
        
        scui_coord3_t angle_s = menial->data.arc.angle_s;
        scui_coord3_t angle_e = menial->data.arc.angle_e;
        scui_coord3_t angle_d = scui_dist(angle_s, angle_e);
        menial->data.arc.angle_d = angle_d;
        
        if (!menial->data.arc.spinner) {
            /* spinner的time不做计算, 默认按360度给入 */
            menial->data.arc.time = menial->data.arc.time * angle_d / 360.0f;
        }
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_burn(scui_menial_t *menial)
{
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
    
    if (menial->type != scui_menial_type_arc) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    if (menial->data.arc.spinner)
        return;
    
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
        scui_coord3_t angle_d = menial->data.arc.angle_d;
        scui_coord3_t val_dif = scui_dist(tran_def.data_p.number, tran_def.data_n.number);
        tran_def.time = scui_map(val_dif, 0, angle_d, 0, menial->data.bar.time);
        
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
 *@param angle  目标角度
 */
void scui_menial_arc_current_angle(scui_handle_t handle, scui_coord3_t *angle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_arc) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    *angle = menial->data.arc.angle_c;
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
    
    if (menial->type != scui_menial_type_arc) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    /*  */
    value = scui_clamp(value, 0.0f, 100.0f);
    scui_coord3_t angle_s = menial->data.arc.angle_s;
    scui_coord3_t angle_e = menial->data.arc.angle_e;
    scui_menial_arc_update_angle(handle, menial->data.arc.anti ?
        scui_map(value, 100.0f, 0.0f, angle_s, angle_e) :
        scui_map(value, 0.0f, 100.0f, angle_s, angle_e), anim);
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
        
        if (menial->data.arc.spinner) {
            scui_coord3_t angle_d = menial->data.arc.angle_d;
            menial->data.arc.angle_c += scui_map(event->tick, 0, menial->data.arc.time, 0.0f, angle_d);
            if (menial->data.arc.angle_c > 360) menial->data.arc.angle_c -= 360;
            
            /* 加载圆环前景(angle_s, angle_e) */
            scui_coord3_t angle_s = menial->data.arc.angle_s;
            scui_coord3_t angle_e = menial->data.arc.angle_e;
            scui_coord_t  angle_w = menial->data.arc.anti ? -1 : +1;
            scui_coord_t  angle_c = menial->data.arc.angle_c;
            angle_c = scui_map(angle_c, 0, angle_d, 0, 360);
            /* angle_c映射到[0, 360]度中去(有精度损失但不影响) */
            scui_map_cb_t path_map = scui_map_ease_out;
            if (scui_mabs(angle_c / 180, 2) == 1)
                path_map = scui_map_ease_in;
            
            SCUI_LOG_INFO("angle:%d", angle_c);
            scui_coord_t  angle_p = scui_mabs(angle_c % 180, 180);
            angle_s += angle_w * path_map(angle_p, 0, 180, 0, 360);
            angle_e += angle_w * scui_map(angle_p, 0, 180, 0, 360) + angle_w * angle_d;
            
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
    default:
        break;
    }
}

/*@brief 事件处理回调(子类型)(样板)
 *@param event 事件
 */
void scui_menial_arc_event_cb(scui_event_t *event)
{
    scui_menial_data_t *data = NULL;
    scui_menial_data_inst(event->object, &data);
    
    switch (event->type) {
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        scui_object_arc_t   arc = {
            .alpha[0] = scui_alpha_cover,
            .alpha[1] = scui_alpha_cover,
            .color[0] = data->arc.color[0],
            .color[1] = data->arc.color[1],
            .width    = data->arc.width,
            .center.x = widget_clip.w / 2,
            .center.y = widget_clip.h / 2,
            .angle_s  = 0,
            .angle_e  = 360,
            .radius   = data->arc.radius,
            .round    = data->arc.round,
            .grad_w   = data->arc.gradw,
            .grad     = data->arc.grad,
        };
        
        arc.state = scui_object_state_def;
        arc.index = 0; scui_object_prop_arc(event->object, &arc);
        arc.index = 1; scui_object_prop_arc(event->object, &arc);
        
        scui_menial_arc_update_value(event->object, 0.0f, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_object_prop_t prop = {0};
        prop.part = scui_object_part_arc_bg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_arc(event->object,  &prop);
        
        prop.part = scui_object_part_arc_fg;
        scui_object_state_get(event->object, &prop.state);
        scui_object_draw_arc(event->object,  &prop);
        break;
    }
    }
}
