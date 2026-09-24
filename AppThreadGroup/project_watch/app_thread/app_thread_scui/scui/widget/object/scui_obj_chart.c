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
void scui_obj_chart_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_object_t *object = widget;
    scui_object_maker_t *object_maker = widget_maker;
    /* 本类对象 */
    scui_obj_chart_t *obj_chart = widget;
    scui_obj_chart_maker_t *obj_chart_maker = widget_maker;
    
    /* 必须标记anima,widget事件 */
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_chart));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_chart->type      = obj_chart_maker->type;
    obj_chart->area      = obj_chart_maker->area;
    obj_chart->value_min = obj_chart_maker->value_min;
    obj_chart->value_max = obj_chart_maker->value_max;
    obj_chart->number    = obj_chart_maker->number;
    obj_chart->space     = obj_chart_maker->space;
    obj_chart->vlist_min = NULL;
    obj_chart->vlist_max = NULL;
    obj_chart->vlist_dot = NULL;
    obj_chart->vlist_pos = NULL;
    
    /* 运行初值断言 */
    SCUI_ASSERT(obj_chart->value_min < obj_chart->value_max);
    SCUI_ASSERT(obj_chart->number != 0);
    SCUI_ASSERT(obj_chart->area.w != 0);
    SCUI_ASSERT(obj_chart->area.h != 0);
    
    /* 限制(冗余限制) */
    if (obj_chart->space <= 0) obj_chart->space = 1;
    
    /* 运行数据缓冲分配(按运行初值) */
    switch (obj_chart->type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        scui_multi_t data_size = obj_chart->number * sizeof(scui_coord_t);
        obj_chart->vlist_min = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        obj_chart->vlist_max = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < obj_chart->number; idx++) {
            obj_chart->vlist_min[idx] = obj_chart->value_min;
            obj_chart->vlist_max[idx] = obj_chart->value_min;
        }
        break;
    }
    case 1: {
        scui_multi_t data_size = obj_chart->number * sizeof(scui_coord_t);
        obj_chart->vlist_dot = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        scui_multi_t vpos_size = obj_chart->number * sizeof(scui_point_t);
        obj_chart->vlist_pos = SCUI_MEM_ALLOC(scui_mem_type_mix, vpos_size);
        for (scui_coord_t idx = 0; idx < obj_chart->number; idx++) {
            obj_chart->vlist_dot[idx] = obj_chart->value_min;
        }
        break;
    }
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_chart_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    /* 资源析构 */
    switch (obj_chart->type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        SCUI_MEM_FREE(obj_chart->vlist_min);
        SCUI_MEM_FREE(obj_chart->vlist_max);
        break;
    }
    case 1: {
        SCUI_MEM_FREE(obj_chart->vlist_dot);
        SCUI_MEM_FREE(obj_chart->vlist_pos);
        break;
    }
    }
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件类型
 *@param handle 控件句柄
 *@param type   子类型
 */
void scui_obj_chart_type(scui_handle_t handle, scui_coord_t *type)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    *type = obj_chart->type;
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_chart_style(scui_handle_t handle, scui_obj_chart_res_t *res)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    switch (res->part) {
    case scui_object_part_line_item: {
        /* 线型图表(折线) */
        scui_object_sub_t sub = {0};
        sub.line.alpha.alpha       = scui_alpha_cover;
        sub.line.color.color32     = res->color.color;
        sub.line.area.area         = obj_chart->area;
        sub.line.vpos_num.number   = obj_chart->number;
        sub.line.stroke.number     = scui_max(res->width, 1);
        sub.line.multi.multi.round = res->round;
        sub.line.multi.multi.grad  = res->grad;
        
        sub.part  = scui_object_part_line_item;
        sub.state = scui_object_state_def;
        scui_object_prop_line(handle, &sub);
        break;
    }
    case scui_object_part_rect_fg: {
        /* 直方图表(柱状) */
        scui_object_sub_t sub = {0};
        sub.rect.alpha.alpha   = scui_alpha_cover;
        sub.rect.color.color32 = res->color.color;
        sub.rect.width.number  = scui_max(res->width, 1);
        sub.rect.radius.number = res->round ? -1 : 0;
        
        sub.part  = scui_object_part_rect_fg;
        sub.form  = scui_object_form_rect_base;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件数据列表更新(hist)
 *@param handle    控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_obj_chart_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    SCUI_ASSERT(obj_chart->type == 0);
    
    scui_coord_t value_min = obj_chart->value_min;
    scui_coord_t value_max = obj_chart->value_max;
    for (scui_coord_t idx = 0; idx < obj_chart->number; idx++) {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, vlist_min[idx]));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, vlist_max[idx]));
        if (min > max) {scui_coord_t tmp = min; min = max; max = tmp;}
        
        obj_chart->vlist_min[idx] = min;
        obj_chart->vlist_max[idx] = max;
    }
}

/*@brief 控件数据列表更新(line)
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_obj_chart_line_data(scui_handle_t handle, scui_coord_t *vlist_dot)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    SCUI_ASSERT(obj_chart->type == 1);
    
    scui_coord_t value_min = obj_chart->value_min;
    scui_coord_t value_max = obj_chart->value_max;
    for (scui_coord_t idx = 0; idx < obj_chart->number; idx++) {
        scui_coord_t val = scui_min(value_max, scui_max(value_min, vlist_dot[idx]));
        obj_chart->vlist_dot[idx] = val;
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_chart_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_obj_chart_t *obj_chart = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_create: {
        
        /* 运行时数据已就绪(样式由 apply 应用默认) */
        break;
    }
    case scui_event_draw_graph: {
        
        switch (obj_chart->type) {
        default:SCUI_ASSERT(false);break;
        case 0: {
            scui_object_data_t width = {0};
            scui_object_prop_sync_s(event->object, scui_object_part_rect_fg,
                scui_object_form_rect_base,
                scui_object_style_rect_width, scui_object_state_def, width);
            
            scui_object_prop_t prop = {0};
            prop.part = scui_object_part_rect_fg;
            prop.form = scui_object_form_rect_base;
            scui_object_state_get(event->object, &prop.state);
            
            scui_point_t offset = obj_chart->area.pos;
            for (scui_coord_t idx = 0; idx < obj_chart->number; idx++) {
                scui_coord_t offset_1y = scui_map(obj_chart->vlist_min[idx],
                    obj_chart->value_min, obj_chart->value_max, obj_chart->area.h, 0);
                scui_coord_t offset_2y = scui_map(obj_chart->vlist_max[idx],
                    obj_chart->value_min, obj_chart->value_max, obj_chart->area.h, 0);
                
                scui_point_t point = offset;
                offset.x += width.number + obj_chart->space;
                
                /* 值为0, 不进行绘制 */
                if (offset_1y - offset_2y < width.number)
                    continue;
                
                prop.data.point = point;
                prop.data.point.y += offset_2y;
                prop.style = scui_object_style_rect_point;
                scui_object_prop_add(event->object, &prop);
                prop.data.number = offset_1y - offset_2y;
                prop.style = scui_object_style_rect_height;
                scui_object_prop_add(event->object, &prop);
                scui_object_draw_rect(event->object, &prop);
            }
            break;
        }
        case 1: {
            
            scui_object_data_t width = {0};
            scui_object_prop_sync_s(event->object, scui_object_part_line_item, 0,
                scui_object_style_line_stroke, scui_object_state_def, width);
            
            scui_point_t offset = obj_chart->area.pos;
            for (scui_coord_t idx = 0; idx + 1 < obj_chart->number; idx++) {
                scui_coord_t offset_1y = scui_map(obj_chart->vlist_dot[idx + 0],
                    obj_chart->value_min, obj_chart->value_max, obj_chart->area.h, 0);
                scui_coord_t offset_2y = scui_map(obj_chart->vlist_dot[idx + 1],
                    obj_chart->value_min, obj_chart->value_max, obj_chart->area.h, 0);
                
                scui_point_t offset_1 = {.x = offset.x, .y = offset.y + offset_1y};
                scui_point_t offset_2 = {.x = offset.x + obj_chart->space, .y = offset.y + offset_2y};
                
                obj_chart->vlist_pos[idx + 0] = offset_1;
                obj_chart->vlist_pos[idx + 1] = offset_2;
                
                offset.x += width.number + obj_chart->space;
            }
            
            scui_object_prop_t prop = {
                .part  = scui_object_part_line_item,
                .state = scui_object_state_def,
                .style = scui_object_style_line_vpos,
                .data.pointer = obj_chart->vlist_pos,
            };
            scui_object_prop_add(event->object, &prop);
            scui_object_draw_line(event->object, &prop);
            break;
        }
        }
    }
    default:
        break;
    }
}