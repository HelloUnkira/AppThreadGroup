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
void scui_obj_led_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t  *widget  = inst;
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t  *widget_maker  = inst_maker;
    scui_object_maker_t  *object_maker  = (void *)widget_maker;
    scui_obj_led_maker_t *obj_led_maker = (void *)widget_maker;
    
    /* 必须标记widget事件 */
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_led));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_led->on          = obj_led_maker->on;
    obj_led->color_on    = obj_led_maker->color_on;
    obj_led->color_off   = obj_led_maker->color_off;
    obj_led->brightness  = obj_led_maker->brightness;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_led_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_led));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_led_style(scui_handle_t handle, scui_obj_led_res_t *res)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    /* 统一基准(form_rect_all): 几何+亮灭色 */
    if (res->form == scui_object_form_rect_all) {
        obj_led->color_on   = res->color_on.color;
        obj_led->color_off  = res->color_off.color;
        obj_led->brightness = res->brightness;
        if (obj_led->brightness == 0)
            obj_led->brightness  = 100;
        
        scui_object_sub_t sub = {.part = res->part, .form = scui_object_form_rect_all};
        sub.rect.alpha.alpha   = res->alpha;
        sub.rect.align.align   = res->align;
        sub.rect.width.number  = widget->clip.w;
        sub.rect.height.number = widget->clip.h;
        sub.rect.radius.number = res->radius;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
    } else {
        /* 该层样式: alpha/stroke/grad+阴影(仅sha) */
        scui_object_sub_t sub = {.part = res->part, .form = res->form};
        sub.rect.alpha.alpha        = res->alpha;
        sub.rect.stroke.number      = res->width;
        sub.rect.multi.multi.grad   = res->grad;
        sub.rect.multi.multi.grad_w = res->gradw;
        sub.rect.multi.multi.shadow = (res->form == scui_object_form_rect_sha);
        sub.rect.color.color32      = res->color_off.color;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
    }
    
    /* 亮灭颜色异步刷新 */
    scui_event_define_absorb_none(event, widget->myself, false, scui_event_update_value);
    scui_event_notify(&event);
}

/*@brief 控件点亮颜色设置
 *@param handle    控件句柄
 *@param color_on  点亮颜色
 *@param color_off 熄灭颜色
 */
void scui_obj_led_color(scui_handle_t handle, scui_color32_t color_on, scui_color32_t color_off)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    obj_led->color_on  = color_on;
    obj_led->color_off = color_off;
    
    /* 亮灭颜色异步刷新 */
    scui_event_define_absorb_none(event, widget->myself, false, scui_event_update_value);
    scui_event_notify(&event);
}

/*@brief 控件亮度设置
 *@param handle 控件句柄
 *@param level  亮度(0-100)
 */
void scui_obj_led_level(scui_handle_t handle, scui_coord_t level)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    obj_led->brightness = scui_clamp(level, 0, 100);
    
    /* 亮灭颜色异步刷新 */
    scui_event_define_absorb_none(event, widget->myself, false, scui_event_update_value);
    scui_event_notify(&event);
}

/*@brief 控件亮灭设置
 *@param handle 控件句柄
 *@param toggle 切换(亮<->灭)
 *@param onoff  亮灭(非切换时生效)
 */
void scui_obj_led_onoff(scui_handle_t handle, bool toggle, bool onoff)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    if (toggle) obj_led->on = !obj_led->on;
    else obj_led->on = onoff;
    
    /* 亮灭颜色异步刷新 */
    scui_event_define_absorb_none(event, widget->myself, false, scui_event_update_value);
    scui_event_notify(&event);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_led_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t  *widget  = scui_handle_source_check(event->object);
    scui_object_t  *object  = (void *)widget;
    scui_obj_led_t *obj_led = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_update_value: {
        /* 亮灭颜色刷新 */
        if (obj_led->on) {
            /* 亮: base亮色, sha同色淡出 */
            scui_color32_t color    = obj_led->color_on;
            scui_color32_t color_ed = SCUI_COLOR32_MAKE32(0xFF000000);
            scui_color32_mix_with(&color, &color, &color_ed, obj_led->brightness);
            
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_base,
                scui_object_style_rect_color, scui_object_state_def,
                scui_object_data_color32(color));
            
            /* 阴影: 径向渐变淡出 */
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_sha,
                scui_object_style_rect_color, scui_object_state_def,
                scui_object_data_color32(obj_led->color_on));
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_sha,
                scui_object_style_rect_grad_c, scui_object_state_def,
                scui_object_data_color32(SCUI_COLOR32_MAKE32(0x00000000)));
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_sha,
                scui_object_style_rect_alpha, scui_object_state_def,
                scui_object_data_alpha(scui_alpha_pct(obj_led->brightness)));
        } else {
            /* 灭: base淡白, sha隐藏 */
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_base,
                scui_object_style_rect_color, scui_object_state_def,
                scui_object_data_color32(obj_led->color_off));
            scui_object_prop_add_s(event->object, scui_object_part_rect_bg, scui_object_form_rect_sha,
                scui_object_style_rect_alpha, scui_object_state_def,
                scui_object_data_alpha(scui_alpha_trans));
        }
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
        
        /* 绘制全部层级: 阴影->基础(part_bg) */
        static const scui_object_type_t form_table[] = {
            scui_object_form_rect_sha,
            scui_object_form_rect_base,
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
