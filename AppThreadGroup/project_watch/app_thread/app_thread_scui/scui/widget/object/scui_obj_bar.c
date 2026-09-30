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
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t  *widget  = inst;
    scui_object_t  *object  = (void *)widget;
    scui_obj_bar_t *obj_bar = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t  *widget_maker  = inst_maker;
    scui_object_maker_t  *object_maker  = (void *)widget_maker;
    scui_obj_bar_maker_t *obj_bar_maker = (void *)widget_maker;
    
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
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
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
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 部件宽高 */
    scui_coord_t area_w = widget->clip.w;
    scui_coord_t area_h = widget->clip.h;
    
    /* 统一基准(form_rect_all): 几何 */
    if (res->form == scui_object_form_rect_all) {
        scui_object_sub_t sub = {.part = res->part, .form = scui_object_form_rect_all};
        sub.rect.alpha.alpha   = res->alpha;
        sub.rect.align.align   = res->align;
        sub.rect.radius.number = res->radius;
        sub.rect.width.number  = area_w;
        sub.rect.height.number = area_h;
        
        /* 前景: 从0生长(反向翻转) */
        if (res->part == scui_object_part_rect_fg) {
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
        
        /* 端点(knob): 同步当前几何 */
        if (res->part == scui_object_part_rect_knob) {
            scui_object_prop_t prop_knob = {0};
            prop_knob.part  = scui_object_part_rect_knob;
            prop_knob.form  = scui_object_form_rect_all;
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
            
            if (obj_bar->knob_pct != 0 && obj_bar->knob_pct != 100) {
                scui_object_prop_add_s(handle, scui_object_part_rect_knob,
                    scui_object_form_rect_all, scui_object_style_rect_align,
                    scui_object_state_pre, scui_object_data_align(sub.rect.align.align));
                scui_object_prop_add_s(handle, scui_object_part_rect_knob,
                    scui_object_form_rect_all, scui_object_style_rect_radius,
                    scui_object_state_pre,
                    scui_object_data_number(sub.rect.radius.number));
            }
        }
        
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
        
        /* 同步time属性 */
        scui_coord_t time = res->time;
        if (time == 0) time = SCUI_WIDGET_OBJ_BAR_TIME;
        scui_object_prop_add_s(handle, scui_object_part_main, 0,
            scui_object_style_main_time, scui_object_state_def,
            scui_object_data_number(time));
        
        /* 前景进度复位 */
        if (res->part == scui_object_part_rect_fg)
            scui_obj_bar_update_value(handle, 0.0f, false);
        return;
    }
    
    /* 该层样式: alpha/color/stroke/grad(阴影仅sha) */
    scui_object_sub_t sub = {.part = res->part, .form = res->form};
    sub.rect.alpha.alpha        = res->alpha;
    sub.rect.stroke.number      = res->width;
    sub.rect.multi.multi.grad_w = res->gradw ? res->gradw : obj_bar->way;
    sub.rect.multi.multi.grad   = res->grad;
    sub.rect.multi.multi.shadow = (res->form == scui_object_form_rect_sha);
    sub.rect.color.color32      = res->color.color_s;
    sub.rect.grad_c.color32     = res->color.color_e;
    sub.state = scui_object_state_def;
    scui_object_prop_rect(handle, &sub);
    
    /* 端点按压态(pre): 风格镜像 */
    if (res->part == scui_object_part_rect_knob &&
        obj_bar->knob_pct != 0 && obj_bar->knob_pct != 100) {
        sub.state = scui_object_state_pre;
        scui_object_prop_rect(handle, &sub);
    }
}

/*@brief 控件当前值
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_obj_bar_current_value(scui_handle_t handle, scui_coord3_t *value)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bar));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
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
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
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
    prop_def.form  = scui_object_form_rect_all;
    prop_def.state = scui_object_state_def;
    if (way) prop_def.style = scui_object_style_rect_height;
    else prop_def.style = scui_object_style_rect_width;
    scui_object_prop_sync(handle, &prop_def);
    
    /* 进度长度: 圆角内缩(两端各容一个圆角) */
    scui_coord_t size_max = way ? widget->clip.h : widget->clip.w;
    scui_object_data_t value_m = {0};
    scui_object_prop_sync_s(handle, scui_object_part_rect_bg, scui_object_form_rect_all,
        scui_object_style_rect_radius, scui_object_state_def, value_m);
    
    value_m.number *= 2;
    scui_coord3_t value_l = scui_min(widget->clip.w, widget->clip.h);
    if (value_m.number < 0) value_m.number = value_l;
    value_m.number = scui_clamp(value_m.number, 0, value_l);
    
    scui_coord_t size_fg = scui_map(value, 0.0f, obj_bar->value_lim, 0, size_max - value_m.number);
    size_fg = scui_clamp(value_m.number + size_fg, value_m.number, size_max);
    
    tran_def.part    = prop_def.part;
    tran_def.form    = prop_def.form;
    tran_def.state_p = prop_def.state;
    tran_def.state_n = prop_def.state;
    tran_def.style   = prop_def.style;
    tran_def.data_p.number = prop_def.data.number;
    tran_def.data_n.number = size_fg;
    SCUI_LOG_INFO("tran(%d->%d)", tran_def.data_p.number, tran_def.data_n.number);
    
    /* 端点: 最大显示区域同心内缩 */
    scui_coord_t size_show = way ? widget->clip.w : widget->clip.h;
    scui_coord_t size_half = size_show / 2;
    scui_coord_t pct = obj_bar->knob_pct ? obj_bar->knob_pct : 100;
    scui_coord_t size_knob = (scui_multi_t)size_show * pct / 100;
    
    /* 端点中心: 尾边贴fg端, 夹紧于控件内 */
    scui_coord_t center = obj_bar->rev ? (size_max - size_fg + size_half) : (size_fg - size_half);
    scui_coord_t center_min = size_half;
    scui_coord_t center_max = size_max - size_half;
    if (center_max < center_min) center_max = center_min;
    center = scui_clamp(center, center_min, center_max);
    
    scui_area_t area_knob = {0};
    area_knob.w = size_knob;
    area_knob.h = size_knob;
    if (way) { area_knob.x = (size_show - size_knob) / 2; area_knob.y = center - size_knob / 2; }
    else     { area_knob.x = center - size_knob / 2;      area_knob.y = (size_show - size_knob) / 2; }
    
    /* 端点按压态: 与def同心, 边长=最大显示区域 */
    scui_area_t area_pre = {0};
    if (pct != 100) {
        area_pre.w = size_show;
        area_pre.h = size_show;
        if (way) { area_pre.x = 0;                      area_pre.y = center - size_show / 2; }
        else     { area_pre.x = center - size_show / 2; area_pre.y = 0; }
        
        scui_object_prop_t prop_pre = {0};
        prop_pre.part  = scui_object_part_rect_knob;
        prop_pre.form  = scui_object_form_rect_all;
        prop_pre.state = scui_object_state_pre;
        
        prop_pre.style = scui_object_style_rect_point;
        prop_pre.data.point.x = area_pre.x;
        prop_pre.data.point.y = area_pre.y;
        scui_object_prop_add(handle, &prop_pre);
        prop_pre.style = scui_object_style_rect_width;
        prop_pre.data.number = area_pre.w;
        scui_object_prop_add(handle, &prop_pre);
        prop_pre.style = scui_object_style_rect_height;
        prop_pre.data.number = area_pre.h;
        scui_object_prop_add(handle, &prop_pre);
        
        /* 按压过渡: 端点即两槽真值 */
        scui_object_type_t part = scui_object_part_rect_knob;
        scui_object_type_t form = scui_object_form_rect_all;
        scui_point_t point_d = {.x = area_knob.x, .y = area_knob.y};
        scui_point_t point_p = {.x = area_pre.x,  .y = area_pre.y};
        
        scui_object_tran_add_s2(handle, part, form, scui_object_style_rect_point,
            scui_object_state_def, scui_object_state_pre,
            scui_object_data_point(point_d), scui_object_data_point(point_p),
            NULL, SCUI_WIDGET_OBJ_BTN_TIME, 0);
        scui_object_tran_add_s2(handle, part, form, scui_object_style_rect_width,
            scui_object_state_def, scui_object_state_pre,
            scui_object_data_number(area_knob.w), scui_object_data_number(area_pre.w),
            NULL, SCUI_WIDGET_OBJ_BTN_TIME, 0);
        scui_object_tran_add_s2(handle, part, form, scui_object_style_rect_height,
            scui_object_state_def, scui_object_state_pre,
            scui_object_data_number(area_knob.h), scui_object_data_number(area_pre.h),
            NULL, SCUI_WIDGET_OBJ_BTN_TIME, 0);
    }
    
    scui_object_tran_t tran_knob = {0};
    scui_object_tran_t tran_w = {0};
    scui_object_tran_t tran_h = {0};
    scui_object_prop_t prop_knob = {0};
    prop_knob.part  = scui_object_part_rect_knob;
    prop_knob.form  = scui_object_form_rect_all;
    prop_knob.state = scui_object_state_def;
    prop_knob.style = scui_object_style_rect_point;
    
    if (area_knob.w > 0) {
        /* 起点: 端点当前几何 */
        scui_point_t point_p  = {0};
        scui_coord_t size_w_p = 0;
        scui_coord_t size_h_p = 0;
        if (scui_object_prop_sync(handle, &prop_knob)) point_p = prop_knob.data.point;
        prop_knob.style = scui_object_style_rect_width;
        if (scui_object_prop_sync(handle, &prop_knob)) size_w_p = prop_knob.data.number;
        prop_knob.style = scui_object_style_rect_height;
        if (scui_object_prop_sync(handle, &prop_knob)) size_h_p = prop_knob.data.number;
        
        tran_knob.part    = scui_object_part_rect_knob;
        tran_knob.form    = scui_object_form_rect_all;
        tran_knob.state_p = scui_object_state_def;
        tran_knob.state_n = scui_object_state_def;
        tran_knob.style   = scui_object_style_rect_point;
        tran_knob.data_p.point = point_p;
        tran_knob.data_n.point.x = area_knob.x;
        tran_knob.data_n.point.y = area_knob.y;
        
        tran_w = tran_knob;
        tran_w.style = scui_object_style_rect_width;
        tran_w.data_p.number = size_w_p;
        tran_w.data_n.number = area_knob.w;
        
        tran_h = tran_knob;
        tran_h.style = scui_object_style_rect_height;
        tran_h.data_p.number = size_h_p;
        tran_h.data_n.number = area_knob.h;
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
        /* 未映射出尺寸差: 给最短时长 */
        if (val_dif == 0) tran_def.time = SCUI_WIDGET_OBJ_BTN_TIME;
        
        /* 过渡动画更新(fg) */
        scui_object_tran_add(handle, &tran_def);
        scui_object_tran_work(handle, &tran_def);
        
        /* 过渡动画更新(knob): 与fg同步 */
        if (area_knob.w > 0) {
            tran_knob.time = tran_def.time;
            scui_object_tran_add(handle, &tran_knob);
            scui_object_tran_work(handle, &tran_knob);
            tran_w.time = tran_def.time;
            scui_object_tran_add(handle, &tran_w);
            scui_object_tran_work(handle, &tran_w);
            tran_h.time = tran_def.time;
            scui_object_tran_add(handle, &tran_h);
            scui_object_tran_work(handle, &tran_h);
        }
    } else {
        /* 直接更新(过渡动画移除) */
        scui_object_tran_del(handle, &tran_def);
        if (area_knob.w > 0) {
            scui_object_tran_del(handle, &tran_knob);
            scui_object_tran_del(handle, &tran_w);
            scui_object_tran_del(handle, &tran_h);
        }
        
        prop_def.data.number = tran_def.data_n.number;
        scui_object_prop_add(handle, &prop_def);
        
        /* 端点几何: 直接写样式 */
        if (area_knob.w > 0) {
            prop_knob.style = scui_object_style_rect_point;
            prop_knob.data.point.x = area_knob.x;
            prop_knob.data.point.y = area_knob.y;
            scui_object_prop_add(handle, &prop_knob);
            prop_knob.style = scui_object_style_rect_width;
            prop_knob.data.number = area_knob.w;
            scui_object_prop_add(handle, &prop_knob);
            prop_knob.style = scui_object_style_rect_height;
            prop_knob.data.number = area_knob.h;
            scui_object_prop_add(handle, &prop_knob);
        }
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_bar_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t  *widget  = scui_handle_source_check(event->object);
    scui_object_t  *object  = (void *)widget;
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
        
        /* 逐部件: 先刷基准几何, 再逐层绘制 */
        for (uint8_t idx_i = 0; idx_i < scui_arr_len(part_table); idx_i++) {
            scui_object_sub_t sub = {.part = part_table[idx_i]};
            scui_object_state_get(event->object, &sub.state);
            /* 无基准几何 → 回退def */
            if (!scui_object_form_rect(event->object, &sub) &&
                sub.state != scui_object_state_def) {
                sub.state  = scui_object_state_def;
                scui_object_form_rect(event->object, &sub);
            }
            
            for (uint8_t idx_j = 0; idx_j < scui_arr_len(form_table); idx_j++) {
                scui_object_prop_t prop = {0};
                prop.part = part_table[idx_i];
                prop.form = form_table[idx_j];
                prop.state = sub.state;
                /* 样式不全 → 回退def */
                if (!scui_object_draw_rect(event->object, &prop) &&
                    prop.state != scui_object_state_def) {
                    prop.state  = scui_object_state_def;
                    scui_object_draw_rect(event->object, &prop);
                }
            }
        }
        break;
    }
    default:
        break;
    }
}
