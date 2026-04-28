/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_btn_maker(scui_menial_maker_t *menial_maker)
{
    /* 必须标记widget事件 */
    menial_maker->widget.style.sched_widget = true;
    
    /* 配置check状态激活 */
    menial_maker->object.check = menial_maker->data.btn.check;
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_config(scui_menial_t *menial)
{
    /* 未配置使用默认值 */
    if (menial->data.btn.time == 0)
        menial->data.btn.time  = SCUI_WIDGET_MENIAL_BTN_TIME;
    
    /* 未配置使用默认值 */
    if (menial->data.btn.lim == 0)
        menial->data.btn.lim  = SCUI_WIDGET_MENIAL_BTN_PCT;
    if (menial->data.btn.fixed)
        menial->data.btn.lim  = 100;
    
    scui_menial_btn_crect(menial->widget.myself);
    scui_menial_btn_prop_tran(menial->widget.myself);
}

/*@brief 控件反初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_recycle(scui_menial_t *menial)
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
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_menial_btn_draw(event->object);
        break;
    }
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        break;
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        
        /* 这里是直接响应的,要不要延迟到抬起? */
        scui_event_define(event, widget->myself, true, scui_event_widget_button_click, NULL);
        scui_event_notify(&event);
        break;
    }
    default:
        break;
    }
}

/*@brief 绘制经典按钮
 *@param handle 对象控件句柄
 *@retval 成功失败
 */
bool scui_menial_btn_draw(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_btn)
        return false;
    
    scui_object_prop_t prop = {0};
    prop.part  = scui_object_part_main;
    prop.style = scui_object_style_width;
    scui_object_prop_sync(widget->myself, &prop);
    scui_coord_t pct_w = prop.data.number;
    
    prop.part  = scui_object_part_main;
    prop.style = scui_object_style_height;
    scui_object_prop_sync(widget->myself, &prop);
    scui_coord_t pct_h = prop.data.number;
    SCUI_LOG_INFO("<%d,%d>", pct_w, pct_h);
    
    scui_area_t src_area = widget->clip;
    scui_coord_t scale_w = src_area.w * pct_w / 100;
    scui_coord_t scale_h = src_area.h * pct_h / 100;
    scale_w -= menial->data.btn.width * 2;
    scale_h -= menial->data.btn.width * 2;
    src_area.x += (src_area.w - scale_w) / 2;
    src_area.y += (src_area.h - scale_h) / 2;
    src_area.w  = scale_w;
    src_area.h  = scale_h;
    
    prop.part = scui_object_part_main;
    scui_object_state_get(widget->myself, &prop.state);
    scui_object_draw_crect(widget->myself, &src_area, &prop);
    
    return true;
}

/*@brief 属性过渡配置
 *@param handle 控件句柄
 */
void scui_menial_btn_crect(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    scui_object_crect_t crect = {
        .part     = scui_object_part_main,
        .index    = 0,
        .alpha[0] = scui_alpha_cover,
        .width[0] = menial->data.btn.width,
        .radius   = menial->data.btn.radius,
    };
    
    /* def<->pre */
    if (true) {
        crect.state = scui_object_state_def;
        scui_object_prop_crect(menial->widget.myself, &crect);
        crect.state = scui_object_state_pre;
        scui_object_prop_crect(menial->widget.myself, &crect);
    }
    /* chk */
    if (menial->data.btn.check) {
        crect.state = scui_object_state_chk;
        scui_object_prop_crect(menial->widget.myself, &crect);
    }
}

/*@brief 属性过渡配置
 *@param handle 控件句柄
 */
void scui_menial_btn_prop_tran(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    /* def<->pre */
    if (true) {
        scui_object_prop_t prop_def = {0};
        scui_object_prop_t prop_pre = {0};
        scui_object_tran_t tran_def = {0};
        scui_object_tran_t tran_pre = {0};
        prop_def.part  = scui_object_part_main;
        prop_pre.part  = scui_object_part_main;
        prop_def.state = scui_object_state_def;
        prop_pre.state = scui_object_state_pre;
        tran_def.state_p = scui_object_state_pre;
        tran_def.state_n = scui_object_state_def;
        tran_pre.state_p = scui_object_state_def;
        tran_pre.state_n = scui_object_state_pre;
        tran_def.part = scui_object_part_main;
        tran_pre.part = scui_object_part_main;
        tran_def.time = menial->data.btn.time;
        tran_pre.time = menial->data.btn.time;
        
        /* color bg prop */
        prop_def.style = scui_object_style_crect_color;
        prop_pre.style = scui_object_style_crect_color;
        prop_def.data.color32 = menial->data.btn.color[0].color_l;
        prop_pre.data.color32 = menial->data.btn.color[0].color_d;
        scui_object_prop_add(widget->myself, &prop_def);
        scui_object_prop_add(widget->myself, &prop_pre);
        /* color bg tran */
        tran_def.style = scui_object_style_crect_color;
        tran_pre.style = scui_object_style_crect_color;
        scui_object_tran_add_by(widget->myself, &tran_def);
        scui_object_tran_add_by(widget->myself, &tran_pre);
        
        /* width && height prop */
        prop_def.style = scui_object_style_width;
        prop_pre.style = scui_object_style_width;
        prop_def.data.number = menial->data.btn.lim;
        prop_pre.data.number = 100;
        scui_object_prop_add(widget->myself, &prop_def);
        scui_object_prop_add(widget->myself, &prop_pre);
        prop_def.style = scui_object_style_height;
        prop_pre.style = scui_object_style_height;
        prop_def.data.number = menial->data.btn.lim;
        prop_pre.data.number = 100;
        scui_object_prop_add(widget->myself, &prop_def);
        scui_object_prop_add(widget->myself, &prop_pre);
        
        if (!menial->data.btn.fixed) {
            /* width && height tran */
            tran_def.style = scui_object_style_width;
            tran_pre.style = scui_object_style_width;
            scui_object_tran_add_by(widget->myself, &tran_def);
            scui_object_tran_add_by(widget->myself, &tran_pre);
            tran_def.style = scui_object_style_height;
            tran_pre.style = scui_object_style_height;
            scui_object_tran_add_by(widget->myself, &tran_def);
            scui_object_tran_add_by(widget->myself, &tran_pre);
        }
    }
    
    /* chk<->pre */
    if (menial->data.btn.check) {
        scui_object_prop_t prop_chk = {0};
        scui_object_prop_t prop_pre = {0};
        scui_object_tran_t tran_chk = {0};
        scui_object_tran_t tran_pre = {0};
        prop_chk.part  = scui_object_part_main;
        prop_pre.part  = scui_object_part_main;
        prop_chk.state = scui_object_state_chk;
        prop_pre.state = scui_object_state_pre;
        tran_chk.state_p = scui_object_state_pre;
        tran_chk.state_n = scui_object_state_chk;
        tran_pre.state_p = scui_object_state_chk;
        tran_pre.state_n = scui_object_state_pre;
        tran_chk.part = scui_object_part_main;
        tran_pre.part = scui_object_part_main;
        tran_chk.time = menial->data.btn.time;
        tran_pre.time = menial->data.btn.time;
        
        /* color bg prop */
        prop_chk.style = scui_object_style_crect_color;
        prop_pre.style = scui_object_style_crect_color;
        prop_chk.data.color32 = menial->data.btn.color[1].color_l;
        prop_pre.data.color32 = menial->data.btn.color[1].color_d;
        scui_object_prop_add(widget->myself, &prop_chk);
        scui_object_prop_add(widget->myself, &prop_pre);
        /* color bg tran */
        tran_chk.style = scui_object_style_crect_color;
        tran_pre.style = scui_object_style_crect_color;
        scui_object_tran_add_by(widget->myself, &tran_chk);
        scui_object_tran_add_by(widget->myself, &tran_pre);
        
        /* width && height prop */
        prop_chk.style = scui_object_style_width;
        prop_pre.style = scui_object_style_width;
        prop_chk.data.number = menial->data.btn.lim;
        prop_pre.data.number = 100;
        scui_object_prop_add(widget->myself, &prop_chk);
        scui_object_prop_add(widget->myself, &prop_pre);
        prop_chk.style = scui_object_style_height;
        prop_pre.style = scui_object_style_height;
        prop_chk.data.number = menial->data.btn.lim;
        prop_pre.data.number = 100;
        scui_object_prop_add(widget->myself, &prop_chk);
        scui_object_prop_add(widget->myself, &prop_pre);
        
        if (!menial->data.btn.fixed) {
            /* width && height tran */
            tran_chk.style = scui_object_style_width;
            tran_pre.style = scui_object_style_width;
            scui_object_tran_add_by(widget->myself, &tran_chk);
            scui_object_tran_add_by(widget->myself, &tran_pre);
            tran_chk.style = scui_object_style_height;
            tran_pre.style = scui_object_style_height;
            scui_object_tran_add_by(widget->myself, &tran_chk);
            scui_object_tran_add_by(widget->myself, &tran_pre);
        }
    }
}
