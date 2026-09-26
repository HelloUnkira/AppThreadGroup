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
void scui_obj_bar_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_object_t *object = widget;
    scui_object_maker_t *object_maker = widget_maker;
    /* 本类对象 */
    scui_obj_bar_t *obj_bar = widget;
    scui_obj_bar_maker_t *obj_bar_maker = widget_maker;
    
    /* 必须标记ptr,anima,widget事件 */
    widget_maker->style.indev_ptr    = true;
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_bar));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_bar->way        = obj_bar_maker->way;
    obj_bar->rev        = obj_bar_maker->rev;
    obj_bar->value_cur  = obj_bar_maker->value_cur;
    obj_bar->value_lim  = obj_bar_maker->value_lim;
    obj_bar->value_int  = obj_bar_maker->value_int;
    
    /* 运行时默认(未配置补默认), 样式默认由 apply 应用常规 res */
    if (SCUI_IS_ZERO_VAL_F(obj_bar->value_lim))
        obj_bar->value_lim = 100.0f;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_bar_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_bar_style(scui_handle_t handle, scui_obj_bar_res_t *res)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 部件宽高 */
    scui_coord_t area_w = res->area.w ? res->area.w : widget->clip.w;
    scui_coord_t area_h = res->area.h ? res->area.h : widget->clip.h;
    
    scui_object_sub_t sub = {.part = res->part, .form = res->form};
    sub.rect.alpha.alpha        = res->alpha;
    sub.rect.color.color32      = res->color.color_s;
    sub.rect.align.align        = res->align;
    sub.rect.width.number       = area_w;
    sub.rect.height.number      = area_h;
    sub.rect.radius.number      = res->radius;
    sub.rect.stroke.number      = res->width;
    sub.rect.multi.multi.grad_w = res->gradw ? res->gradw : obj_bar->way;
    sub.rect.multi.multi.grad   = res->grad;
    sub.rect.multi.multi.shadow = res->shadow;
    sub.rect.grad_c.color32     = res->color.color_e;
    
    if (res->part == scui_object_part_rect_fg) {
        /* 反向: 前景对齐翻转(从另一端生长) */
        if (obj_bar->rev) {
            scui_opt_pos_t align = scui_opt_pos_none;
            align |= (res->align & scui_opt_pos_l) ? scui_opt_pos_r : 0;
            align |= (res->align & scui_opt_pos_r) ? scui_opt_pos_l : 0;
            align |= (res->align & scui_opt_pos_u) ? scui_opt_pos_d : 0;
            align |= (res->align & scui_opt_pos_d) ? scui_opt_pos_u : 0;
            sub.rect.align.align = align;
        }
        sub.rect.width.number  = obj_bar->way ? area_w : 0;
        sub.rect.height.number = obj_bar->way ? 0 : area_h;
    }
    
    if (res->part == scui_object_part_rect_knob) {
        /* knob尺寸/端点由update_value统一管理(fg更新时双轴写入), 此处仅同步当前值 */
        scui_object_prop_t prop_knob = {0};
        prop_knob.part  = scui_object_part_rect_knob;
        prop_knob.form  = res->form;
        prop_knob.state = scui_object_state_def;
        prop_knob.style = scui_object_style_rect_width;
        scui_object_prop_sync(handle, &prop_knob);
        sub.rect.width.number = prop_knob.data.number;
        prop_knob.style = scui_object_style_rect_height;
        scui_object_prop_sync(handle, &prop_knob);
        sub.rect.height.number = prop_knob.data.number;
        prop_knob.style = scui_object_style_rect_point;
        scui_object_prop_sync(handle, &prop_knob);
        sub.rect.point.point = prop_knob.data.point;
    }
    
    sub.state = scui_object_state_def;
    scui_object_prop_rect(handle, &sub);
    
    /* 同步全局time属性(默认值/可覆盖) */
    scui_coord_t time = res->time;
    if (time == 0) time = SCUI_WIDGET_OBJ_BAR_TIME;
    
    scui_object_prop_add_s(handle, scui_object_part_main, 0,
        scui_object_style_main_time, scui_object_state_def,
        scui_object_data_number(time));
    
    /* 前景进度复位到默认值(其他层级无进度概念) */
    if (res->part == scui_object_part_rect_fg &&
        res->form == scui_object_form_rect_base)
        scui_obj_bar_update_value(handle, 0.0f, false);
}

/*@brief 控件当前值
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_obj_bar_current_value(scui_handle_t handle, scui_coord3_t *value)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    *value = obj_bar->value_cur;
}

/*@brief 控件更新值
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_obj_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 这可以实现丝滑到分段效果 */
    value = scui_clamp(value, 0.0f, obj_bar->value_lim);
    if (obj_bar->value_int) value = (scui_coord_t)value;
    obj_bar->value_cur = value;
    scui_event_define(event, widget->myself, true, scui_event_update_value, NULL);
    scui_event_notify(&event);
    
    bool way = obj_bar->way;
    scui_object_prop_t prop_def = {0};
    scui_object_tran_t tran_def = {0};
    prop_def.part  = scui_object_part_rect_fg;
    prop_def.form  = scui_object_form_rect_base;
    prop_def.state = scui_object_state_def;
    if (way) prop_def.style = scui_object_style_rect_height;
    else prop_def.style = scui_object_style_rect_width;
    scui_object_prop_sync(handle, &prop_def);
    
    #if 1
    /* 计算宽高值 */
    scui_area_t  dst_part = widget->clip;
    scui_coord3_t value_d = obj_bar->value_lim;
    scui_coord3_t value_c = obj_bar->value_cur;
    scui_object_data_t value_m = {0};
    scui_object_prop_sync_s(handle, scui_object_part_rect_bg, scui_object_form_rect_base,
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
    tran_def.form    = prop_def.form;
    tran_def.state_p = prop_def.state;
    tran_def.state_n = prop_def.state;
    tran_def.style   = prop_def.style;
    tran_def.data_p.number = prop_def.data.number;
    tran_def.data_n.number = scui_map(value, 0.0f, value_d, size_min, size_max);
    SCUI_LOG_INFO("tran(%d->%d)", tran_def.data_p.number, tran_def.data_n.number);
    
    /* 端点(knob)坐标: 平行于fg更新(当前端点->目标端点)
       knob 包围宽/高 = min(fg当前尺寸, 直径); 左/上边缘贴fg增长端;
       放得下整圆时为直径, 否则与fg一致(圆角钳制自动退化) */
    scui_object_tran_t tran_knob = {0};
    scui_object_tran_t tran_size = {0};
    scui_object_prop_t prop_knob = {0};
    prop_knob.part  = scui_object_part_rect_knob;
    prop_knob.form  = scui_object_form_rect_base;
    prop_knob.state = scui_object_state_def;
    prop_knob.style = scui_object_style_rect_point;
    
    scui_coord_t size_knob = way ? dst_part.w : dst_part.h;   /* 直径=fg固定厚度 */
    if (size_knob > 0) {
        scui_coord_t fg_p = prop_def.data.number;
        scui_coord_t fg_n = tran_def.data_n.number;
        
        /* knob 动态尺寸: 放得下整圆才为直径, 否则跟fg一致 */
        scui_coord_t size_p = scui_min(fg_p, size_knob);
        scui_coord_t size_n = scui_min(fg_n, size_knob);
        
        /* 左/上边缘贴fg增长端(反向时贴另一端) */
        scui_point_t point_p = {0};
        scui_point_t point_n = {0};
        if (obj_bar->rev) {
            if (way) {
                point_p.x = 0; point_p.y = dst_part.h - fg_p;
                point_n.x = 0; point_n.y = dst_part.h - fg_n;
            } else {
                point_p.x = dst_part.w - fg_p; point_p.y = 0;
                point_n.x = dst_part.w - fg_n; point_n.y = 0;
            }
        } else {
            if (way) {
                point_p.x = 0; point_p.y = fg_p - size_p;
                point_n.x = 0; point_n.y = fg_n - size_n;
            } else {
                point_p.x = fg_p - size_p; point_p.y = 0;
                point_n.x = fg_n - size_n; point_n.y = 0;
            }
        }
        
        tran_knob.part    = scui_object_part_rect_knob;
        tran_knob.form    = scui_object_form_rect_base;
        tran_knob.state_p = scui_object_state_def;
        tran_knob.state_n = scui_object_state_def;
        tran_knob.style   = scui_object_style_rect_point;
        tran_knob.data_p.point = point_p;
        tran_knob.data_n.point = point_n;
        
        /* knob 动态尺寸: 平行tran */
        tran_size.part    = scui_object_part_rect_knob;
        tran_size.form    = scui_object_form_rect_base;
        tran_size.state_p = scui_object_state_def;
        tran_size.state_n = scui_object_state_def;
        if (way) tran_size.style = scui_object_style_rect_height;
        else tran_size.style = scui_object_style_rect_width;
        tran_size.data_p.number = size_p;
        tran_size.data_n.number = size_n;
    }
    
    if (anim) {
        /* 同步time属性 */
        scui_object_data_t main_time = {0};
        scui_object_prop_sync_s(handle, scui_object_part_main, 0,
            scui_object_style_main_time, scui_object_state_def, main_time);
        
        scui_coord_t  val_dif = scui_dist(tran_def.data_p.number, tran_def.data_n.number);
        scui_coord3_t value_d = obj_bar->value_lim;
        tran_def.time = scui_map(val_dif, 0, size_max, 0,
            main_time.number * value_d / 100.0f);
        
        /* 过渡动画更新(fg) */
        scui_object_tran_add(handle, &tran_def);
        scui_object_tran_work(handle, &tran_def);
        
        /* 过渡动画更新(knob point/size): 平行于fg推进 */
        if (size_knob > 0) {
            tran_knob.time = tran_def.time;
            scui_object_tran_add(handle, &tran_knob);
            scui_object_tran_work(handle, &tran_knob);
            tran_size.time = tran_def.time;
            scui_object_tran_add(handle, &tran_size);
            scui_object_tran_work(handle, &tran_size);
        }
    } else {
        /* 直接更新(过渡动画移除) */
        scui_object_tran_del(handle, &tran_def);
        if (size_knob > 0) {
            scui_object_tran_del(handle, &tran_knob);
            scui_object_tran_del(handle, &tran_size);
        }
        
        prop_def.data.number = tran_def.data_n.number;
        scui_object_prop_add(handle, &prop_def);
        
        /* 端点(knob)坐标/尺寸: 直接更新到样式 */
        if (size_knob > 0) {
            prop_knob.data = tran_knob.data_n;
            scui_object_prop_add(handle, &prop_knob);
            
            if (way) prop_knob.style = scui_object_style_rect_height;
            else prop_knob.style = scui_object_style_rect_width;
            
            prop_knob.data = tran_size.data_n;
            scui_object_prop_add(handle, &prop_knob);
        }
    }
    
    /* knob固定轴(直径=轨道厚度): 恒等, 直接写样式 */
    if (size_knob > 0) {
        if (way) prop_knob.style = scui_object_style_rect_width;
        else prop_knob.style = scui_object_style_rect_height;
        
        prop_knob.data.number = size_knob;
        scui_object_prop_add(handle, &prop_knob);
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_bar_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_draw_graph: {
        
        /* 默认绘制全部部件 */
        static const scui_object_type_t part_table[] = {
            scui_object_part_rect_bg,
            scui_object_part_rect_fg,
            scui_object_part_rect_knob,
        };
        /* 默认绘制全部层级: 阴影->基础->边界->盒子 */
        static const scui_object_type_t form_table[] = {
            scui_object_form_rect_sha,
            scui_object_form_rect_base,
            scui_object_form_rect_edge,
            scui_object_form_rect_box,
        };
        
        for (uint8_t idx_i = 0; idx_i < scui_arr_len(part_table); idx_i++)
        for (uint8_t idx_j = 0; idx_j < scui_arr_len(form_table); idx_j++) {
            scui_object_prop_t prop = {0};
            prop.part = part_table[idx_i];
            prop.form = form_table[idx_j];
            scui_object_state_get(event->object, &prop.state);
            scui_object_draw_rect(event->object, &prop);
        }
        break;
    }
    default:
        break;
    }
}
