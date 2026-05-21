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
void scui_menial_cht_make(bool maker, void *inst)
{
    scui_menial_t *menial = inst;
    scui_menial_maker_t *menial_maker = inst;
    
    if (maker) {
    } else {
        /* 这里加点断言判断参数的有效性 */
        SCUI_ASSERT(menial->data.cht.value_min < menial->data.cht.value_max);
        SCUI_ASSERT(menial->data.cht.number != 0);
        SCUI_ASSERT(menial->data.cht.area.w != 0);
        SCUI_ASSERT(menial->data.cht.area.h != 0);
        
        /* 限制(冗余限制) */
        if (menial->data.cht.space <= 0) menial->data.cht.space = 1;
        if (menial->data.cht.width <= 0) menial->data.cht.width = 1;
        
        switch (menial->data.cht.type) {
        default:SCUI_ASSERT(false);break;
        case 0: {
            scui_multi_t data_size = menial->data.cht.number * sizeof(scui_coord_t);
            menial->data.cht.vlist_min = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
            menial->data.cht.vlist_max = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
            for (scui_coord_t idx = 0; idx < menial->data.cht.number; idx++) {
                menial->data.cht.vlist_min[idx] = menial->data.cht.value_min;
                menial->data.cht.vlist_max[idx] = menial->data.cht.value_min;
            }
            break;
        }
        case 1: { 
            scui_multi_t data_size = menial->data.cht.number * sizeof(scui_coord_t);
            menial->data.cht.vlist_dot = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
            scui_multi_t vpos_size = menial->data.cht.number * sizeof(scui_point_t);
            menial->data.cht.vlist_pos = SCUI_MEM_ALLOC(scui_mem_type_mix, vpos_size);
            for (scui_coord_t idx = 0; idx < menial->data.cht.number; idx++) {
                menial->data.cht.vlist_dot[idx] = menial->data.cht.value_min;
            }
            break;
        }
        }
    }
}

/*@brief 控件析构(子类型)
 *@param menial 控件实例
 */
void scui_menial_cht_burn(scui_menial_t *menial)
{
    switch (menial->data.cht.type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        SCUI_MEM_FREE(menial->data.cht.vlist_min);
        SCUI_MEM_FREE(menial->data.cht.vlist_max);
        break;
    }
    case 1: {
        SCUI_MEM_FREE(menial->data.cht.vlist_dot);
        SCUI_MEM_FREE(menial->data.cht.vlist_pos);
        break;
    }
    }
}

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_menial_cht_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_cht);
    SCUI_ASSERT(menial->data.cht.type == 0);
    
    scui_coord_t value_min = menial->data.cht.value_min;
    scui_coord_t value_max = menial->data.cht.value_max;
    for (scui_coord_t idx = 0; idx < menial->data.cht.number; idx++) {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, vlist_min[idx]));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, vlist_max[idx]));
        if (min > max) {scui_coord_t tmp = min; min = max; max = tmp;}
        
        menial->data.cht.vlist_min[idx] = min;
        menial->data.cht.vlist_max[idx] = max;
    }
}

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_menial_cht_line_data(scui_handle_t handle, scui_coord_t *vlist_dot)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    SCUI_ASSERT(menial->type == scui_menial_type_cht);
    SCUI_ASSERT(menial->data.cht.type == 1);
    
    scui_coord_t value_min = menial->data.cht.value_min;
    scui_coord_t value_max = menial->data.cht.value_max;
    for (scui_coord_t idx = 0; idx < menial->data.cht.number; idx++) {
        scui_coord_t val = scui_min(value_max, scui_max(value_min, vlist_dot[idx]));
        menial->data.cht.vlist_dot[idx] = val;
    }
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_cht_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        switch (menial->data.cht.type) {
        default:SCUI_ASSERT(false);break;
        case 0: {
            scui_object_rect_t rect = {
                .state    = scui_object_state_def,
                .area.w   = menial->data.cht.width,
                .alpha[0] = scui_alpha_cover,
                .color[0] = menial->data.cht.color,
                .radius   = menial->data.cht.round ? -1 : 0,
            };
            
            scui_object_prop_rect(event->object, &rect);
            break;
        }
        case 1: {
            scui_object_line_t line = {
                .state      = scui_object_state_def,
                .alpha      = scui_alpha_cover,
                .color      = menial->data.cht.color,
                .area       = menial->data.cht.area,
                .vpos_num   = menial->data.cht.number,
                .width      = menial->data.cht.width,
                .round      = menial->data.cht.round,
            };
            
            scui_object_prop_line(event->object, &line);
            break;
        }
        }
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        switch (menial->data.cht.type) {
        default:SCUI_ASSERT(false);break;
        case 0: {
            scui_object_prop_t prop = {0};
            prop.part = scui_object_part_rect_bg;
            scui_object_state_get(event->object, &prop.state);
            
            scui_point_t offset = menial->data.cht.area.pos;
            for (scui_coord_t idx = 0; idx < menial->data.cht.number; idx++) {
                scui_coord_t offset_1y = scui_map(menial->data.cht.vlist_min[idx],
                    menial->data.cht.value_min, menial->data.cht.value_max, menial->data.cht.area.h, 0);
                scui_coord_t offset_2y = scui_map(menial->data.cht.vlist_max[idx],
                    menial->data.cht.value_min, menial->data.cht.value_max, menial->data.cht.area.h, 0);
                
                scui_point_t point = offset;
                offset.x += menial->data.cht.width + menial->data.cht.space;
                
                /* 值为0, 不进行绘制 */
                if (offset_1y - offset_2y < menial->data.cht.width)
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
            
            scui_point_t offset = menial->data.cht.area.pos;
            for (scui_coord_t idx = 0; idx + 1 < menial->data.cht.number; idx++) {
                scui_coord_t offset_1y = scui_map(menial->data.cht.vlist_dot[idx + 0],
                    menial->data.cht.value_min, menial->data.cht.value_max, menial->data.cht.area.h, 0);
                scui_coord_t offset_2y = scui_map(menial->data.cht.vlist_dot[idx + 1],
                    menial->data.cht.value_min, menial->data.cht.value_max, menial->data.cht.area.h, 0);
                
                scui_point_t offset_1 = {.x = offset.x, .y = offset.y + offset_1y};
                scui_point_t offset_2 = {.x = offset.x + menial->data.cht.space, .y = offset.y + offset_2y};
                
                menial->data.cht.vlist_pos[idx + 0] = offset_1;
                menial->data.cht.vlist_pos[idx + 1] = offset_2;
                
                offset.x += menial->data.cht.width + menial->data.cht.space;
            }
            
            scui_object_prop_t prop = {
                .part  = scui_object_part_line,
                .state = scui_object_state_def,
                .style = scui_object_style_line_vpos,
                .data.pointer = menial->data.cht.vlist_pos,
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
