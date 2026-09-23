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
void scui_obj_spinner_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_object_t *object = widget;
    scui_object_maker_t *object_maker = widget_maker;
    /* 继承对象 */
    scui_obj_arc_t *obj_arc = widget;
    scui_obj_arc_maker_t *obj_arc_maker = widget_maker;
    /* 本类对象 */
    scui_obj_spinner_t *obj_spinner = widget;
    scui_obj_spinner_maker_t *obj_spinner_maker = widget_maker;
    
    /* 必须标记anima,widget事件 */
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    
    /* 旋转器不使用跟手 */
    obj_arc_maker->touch = false;
    
    /* 构造派生控件实例 */
    scui_obj_arc_make(obj_arc, obj_arc_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_spinner));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_spinner_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_spinner));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_spinner_t *obj_spinner = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_obj_arc_burn(widget->myself);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_spinner_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_obj_arc_t *obj_arc = (void *)widget;
    
    /* 基类处理(绘制/过渡动画推进) */
    scui_obj_arc_invoke(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 同步time属性(旋转速度) */
        scui_object_data_t main_time = {0};
        scui_object_prop_sync_s(event->object, scui_object_part_main,
            scui_object_style_main_time, scui_object_state_def, main_time);
        
        /* spinner端点基准固化 */
        scui_coord3_t angle_s = 0.0f;
        scui_coord3_t angle_e = 360.0f;
        scui_coord3_t angle_d = 360.0f;
        obj_arc->angle_c += scui_map(event->tick, 0, main_time.number, 0.0f, angle_d);
        if (obj_arc->angle_c > 360) obj_arc->angle_c -= 360;
        
        scui_coord_t  angle_w = obj_arc->anti ? -1 : +1;
        scui_coord_t  angle_c = obj_arc->angle_c;
        angle_c = scui_map(angle_c, 0, angle_d, 0, 360);
        /* angle_c映射到[0, 360]度中去(有精度损失但不影响) */
        scui_map_cb_t path_map = scui_map_ease_out;
        if (scui_mabs(angle_c / 180, 2) == 1)
            path_map = scui_map_ease_in;
        
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
        break;
    }
    default:
        break;
    }
}
