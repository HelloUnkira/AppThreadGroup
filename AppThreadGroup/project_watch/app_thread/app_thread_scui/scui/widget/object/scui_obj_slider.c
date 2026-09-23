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
void scui_obj_slider_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_object_t *object = widget;
    scui_object_maker_t *object_maker = widget_maker;
    /* 继承对象 */
    scui_obj_bar_t *obj_bar = widget;
    scui_obj_bar_maker_t *obj_bar_maker = widget_maker;
    /* 本类对象 */
    scui_obj_slider_t *obj_slider = widget;
    scui_obj_slider_maker_t *obj_slider_maker = widget_maker;
    
    /* 必须标记ptr,anima,widget事件 */
    widget_maker->style.indev_ptr    = true;
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_obj_bar_make(obj_bar, obj_bar_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_slider));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_slider_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_slider));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_slider_t *obj_slider = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_obj_bar_burn(widget->myself);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_slider_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_obj_bar_t *obj_bar = (void *)widget;
    
    /* 基类处理(绘制/过渡动画推进) */
    scui_obj_bar_invoke(event);
    
    switch (event->type) {
    case scui_event_ptr_move: {
        
        scui_point_t ptr_c = event->ptr_e;
        scui_area_t  dst_part = widget->clip;
        scui_area_m_to_s(&dst_part, &dst_part);
        
        scui_coord3_t value_c = 0.0f;
        scui_coord3_t value_d = obj_bar->value_lim;
        /* 取最后的落点计算百分比值就地更新 */
        if (obj_bar->way) {
            value_c = scui_map(ptr_c.y, dst_part.y1, dst_part.y2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        } else {
            value_c = scui_map(ptr_c.x, dst_part.x1, dst_part.x2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        }
        
        /* slider无动画跟手 */
        scui_obj_bar_update_value(widget->myself, value_c, false);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
