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
void scui_obj_bmat_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t   *widget   = inst;
    scui_object_t   *object   = (void *)widget;
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t   *widget_maker   = inst_maker;
    scui_object_maker_t   *object_maker   = (void *)widget_maker;
    scui_obj_bmat_maker_t *obj_bmat_maker = (void *)widget_maker;
    
    /* 必须标记ptr,widget事件 */
    widget_maker->style.indev_ptr    = true;
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_bmat));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_bmat->item_num   = obj_bmat_maker->item_num;
    obj_bmat->row_num    = obj_bmat_maker->row_num;
    obj_bmat->gap        = obj_bmat_maker->gap;
    
    obj_bmat->item_max   = obj_bmat_maker->item_num;
    obj_bmat->item_press = -1;
    obj_bmat->item_click = -1;
    
    /* 运行初值断言 */
    SCUI_ASSERT(obj_bmat->item_num != 0);
    SCUI_ASSERT(obj_bmat->row_num  != 0);
    
    /* 条目序列缓冲分配(按条目上限) */
    scui_multi_t unit_size = obj_bmat->item_max * sizeof(scui_coord_t);
    scui_multi_t area_size = obj_bmat->item_max * sizeof(scui_area_t);
    obj_bmat->item_row  = SCUI_MEM_ZALLOC(scui_mem_type_mix, unit_size);
    obj_bmat->item_unit = SCUI_MEM_ZALLOC(scui_mem_type_mix, unit_size);
    obj_bmat->item_area = SCUI_MEM_ZALLOC(scui_mem_type_mix, area_size);
    for (scui_coord_t idx = 0; idx < obj_bmat->item_max; idx++) {
        obj_bmat->item_unit[idx] = 1;
        obj_bmat->item_row[idx]  = 0;
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_bmat_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bmat));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    /* 条目序列缓冲释放 */
    SCUI_MEM_FREE(obj_bmat->item_row);
    SCUI_MEM_FREE(obj_bmat->item_unit);
    SCUI_MEM_FREE(obj_bmat->item_area);
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_bmat_style(scui_handle_t handle, scui_obj_bmat_res_t *res)
{
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_object_t   *object   = (void *)widget;
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    /* 统一基准(form_rect_all): 几何(条目左上角; 绘制期逐条目改写) */
    if (res->form == scui_object_form_rect_all) {
        scui_object_sub_t sub = {.part = res->part, .form = scui_object_form_rect_all};
        sub.rect.alpha.alpha   = res->alpha;
        sub.rect.align.align   = res->align;
        sub.rect.width.number  = widget->clip.w;
        sub.rect.height.number = widget->clip.h;
        sub.rect.radius.number = res->radius;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
        sub.state = scui_object_state_pre;
        scui_object_prop_rect(handle, &sub);
    } else {
        /* 该层样式: alpha/stroke/color/grad+阴影(仅sha) */
        bool shadow = (res->form == scui_object_form_rect_sha);
        scui_object_sub_t sub = {.part = res->part, .form = res->form};
        sub.rect.alpha.alpha        = res->alpha;
        sub.rect.stroke.number      = res->width;
        sub.rect.multi.multi.grad   = res->grad;
        sub.rect.multi.multi.grad_w = res->gradw;
        sub.rect.multi.multi.shadow = shadow;
        sub.rect.color.color32  = res->color[0].color_s;
        sub.rect.grad_c.color32 = res->color[0].color_e;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
        sub.state = scui_object_state_pre;
        sub.rect.color.color32  = res->color[1].color_s;
        sub.rect.grad_c.color32 = res->color[1].color_e;
        scui_object_prop_rect(handle, &sub);
    }
}

/*@brief 条目序列设置
 *@param handle    控件句柄
 *@param item_num  条目数量(<=构造上限)
 *@param row_num   轨道数量
 *@param item_unit 条目宽度(单位数; 空=等分)
 *@param item_row  条目轨道(升序; 空=单轨)
 */
void scui_obj_bmat_item_set(scui_handle_t handle, scui_coord_t item_num, scui_coord_t row_num,
    scui_coord_t *item_unit, scui_coord_t *item_row)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bmat));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    SCUI_ASSERT(item_num <= obj_bmat->item_max);
    obj_bmat->item_num = item_num;
    obj_bmat->row_num  = row_num;
    if (obj_bmat->item_press >= item_num)
        obj_bmat->item_press = -1;
    if (obj_bmat->item_click >= item_num)
        obj_bmat->item_click = -1;
    
    /* 条目序列拷贝(空则等分单轨) */
    for (scui_coord_t idx = 0; idx < item_num; idx++) {
        obj_bmat->item_unit[idx] = item_unit ? item_unit[idx] : 1;
        obj_bmat->item_row[idx]  = item_row  ? item_row[idx]  : 0;
    }
    
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 条目区域
 *@param handle    控件句柄
 *@param idx       条目号
 *@param item_area 条目区域(控件相对)
 */
void scui_obj_bmat_item_area(scui_handle_t handle, scui_coord_t idx, scui_area_t *item_area)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bmat));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    SCUI_ASSERT(idx < obj_bmat->item_num);
    *item_area = obj_bmat->item_area[idx];
}

/*@brief 最近点击条目
 *@param handle 控件句柄
 *@retval 条目号(-1:无)
 */
scui_coord_t scui_obj_bmat_click_item(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bmat));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    return obj_bmat->item_click;
}

/*@brief 按下条目
 *@param handle 控件句柄
 *@retval 条目号(-1:无)
 */
scui_coord_t scui_obj_bmat_press_item(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_bmat));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    return obj_bmat->item_press;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_bmat_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t   *widget   = scui_handle_source_check(event->object);
    scui_object_t   *object   = (void *)widget;
    scui_obj_bmat_t *obj_bmat = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_ptr_down: {
        scui_point_t point = event->ptr_c;
        scui_widget_switch_point(event->object, &point);
        
        /* 命中条目(缝隙亦算命中: 区域外扩半个间距) */
        scui_coord_t item_press = -1;
        for (scui_coord_t idx = 0; idx < obj_bmat->item_num; idx++) {
            scui_area_t *src = &obj_bmat->item_area[idx];
            scui_area_t area = {.x = src->x - obj_bmat->gap.x / 2, .y = src->y - obj_bmat->gap.y / 2,
                                .w = src->w + obj_bmat->gap.x,     .h = src->h + obj_bmat->gap.y};
            if (scui_area_point(&area, &point)) {
                item_press = idx;
                break;
            }
        }
        
        /* 局部重绘: 仅按下条目 */
        obj_bmat->item_press = item_press;
        if (item_press >= 0)
            scui_widget_draw(widget->myself, &obj_bmat->item_area[item_press], false, 0);
        break;
    }
    case scui_event_ptr_move: {
        scui_point_t point = event->ptr_e;
        scui_widget_switch_point(event->object, &point);
        
        /* 命中条目(缝隙亦算命中: 区域外扩半个间距) */
        scui_coord_t item_press = -1;
        for (scui_coord_t idx = 0; idx < obj_bmat->item_num; idx++) {
            scui_area_t *src = &obj_bmat->item_area[idx];
            scui_area_t area = {.x = src->x - obj_bmat->gap.x / 2, .y = src->y - obj_bmat->gap.y / 2,
                                .w = src->w + obj_bmat->gap.x,     .h = src->h + obj_bmat->gap.y};
            if (scui_area_point(&area, &point)) {
                item_press = idx;
                break;
            }
        }
        
        /* 局部重绘: 仅旧新两个按下条目 */
        if (item_press != obj_bmat->item_press) {
            if (obj_bmat->item_press >= 0)
                scui_widget_draw(widget->myself, &obj_bmat->item_area[obj_bmat->item_press], false, 0);
            if (item_press >= 0)
                scui_widget_draw(widget->myself, &obj_bmat->item_area[item_press], false, 0);
            obj_bmat->item_press = item_press;
        }
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_up: {
        scui_point_t point = event->ptr_c;
        scui_widget_switch_point(event->object, &point);
        
        /* 命中条目(缝隙亦算命中: 区域外扩半个间距) */
        scui_coord_t item_press = -1;
        for (scui_coord_t idx = 0; idx < obj_bmat->item_num; idx++) {
            scui_area_t *src = &obj_bmat->item_area[idx];
            scui_area_t area = {.x = src->x - obj_bmat->gap.x / 2, .y = src->y - obj_bmat->gap.y / 2,
                                .w = src->w + obj_bmat->gap.x,     .h = src->h + obj_bmat->gap.y};
            if (scui_area_point(&area, &point)) {
                item_press = idx;
                break;
            }
        }
        
        if (item_press >= 0 && item_press == obj_bmat->item_press) {
            obj_bmat->item_click = item_press;
            scui_event_define(event, widget->myself, true, scui_event_button_click, NULL);
            scui_event_notify(&event);
        }
        /* 局部重绘: 仅抬起条目 */
        if (obj_bmat->item_press >= 0)
            scui_widget_draw(widget->myself, &obj_bmat->item_area[obj_bmat->item_press], false, 0);
        obj_bmat->item_press = -1;
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        break;
    }
    case scui_event_draw_graph: {
        if (obj_bmat->item_num <= 0)
            break;
        
        /* 条目布局(控件相对): 轨道高等分, 轨道内按单位比例分配 */
        scui_coord_t row_h = widget->clip.h - obj_bmat->gap.y * (obj_bmat->row_num - 1);
        if (row_h < 0) row_h = 0;
        scui_coord_t item_i = 0;
        
        for (scui_coord_t row = 0; row < obj_bmat->row_num; row++) {
            /* 统计轨道: 条目数与单位总数 */
            scui_coord_t row_beg  = item_i;
            scui_coord_t unit_cnt = 0;
            while (item_i < obj_bmat->item_num && obj_bmat->item_row[item_i] == row) {
                unit_cnt += obj_bmat->item_unit[item_i] > 0 ? obj_bmat->item_unit[item_i] : 1;
                item_i++;
            }
            if (item_i == row_beg)
                continue;
            
            scui_coord_t item_cnt  = item_i - row_beg;
            scui_coord_t row_y     = row_h * (row + 0) / obj_bmat->row_num + row * obj_bmat->gap.y;
            scui_coord_t row_y1    = row_h * (row + 1) / obj_bmat->row_num + row * obj_bmat->gap.y;
            scui_coord_t row_w     = widget->clip.w - obj_bmat->gap.x * (item_cnt - 1);
            scui_coord_t unit_pos  = 0;
            
            for (scui_coord_t idx = row_beg; idx < item_i; idx++) {
                scui_coord_t unit_i  = obj_bmat->item_unit[idx] > 0 ? obj_bmat->item_unit[idx] : 1;
                scui_coord_t item_x  = row_w * (unit_pos + 0) / unit_cnt + (idx - row_beg) * obj_bmat->gap.x;
                scui_coord_t item_x1 = row_w * (unit_pos + unit_i) / unit_cnt + (idx - row_beg) * obj_bmat->gap.x;
                
                /* 相邻刻度之差作尺寸: 吸收整除截断, 条目间零缝隙 */
                obj_bmat->item_area[idx].x = item_x;
                obj_bmat->item_area[idx].y = row_y;
                obj_bmat->item_area[idx].w = item_x1 - item_x;
                obj_bmat->item_area[idx].h = row_y1 - row_y;
                unit_pos += unit_i;
            }
        }
        
        /* 条目绘制: 基准几何逐条目改写, 绘制完还原 */
        static const scui_object_type_t form_table[] = {
            scui_object_form_rect_base,
            scui_object_form_rect_edge,
            scui_object_form_rect_box,
            scui_object_form_rect_sha,
        };
        
        for (scui_coord_t idx = 0; idx < obj_bmat->item_num; idx++) {
            scui_area_t *area = &obj_bmat->item_area[idx];
            scui_object_type_t state = idx == obj_bmat->item_press ?
                scui_object_state_pre : scui_object_state_def;
            
            /* 基准几何: 当前条目 */
            scui_object_prop_t prop = {0};
            prop.part  = scui_object_part_rect_item;
            prop.form  = scui_object_form_rect_all;
            prop.state = state;
            prop.style = scui_object_style_rect_point;
            prop.data.point = (scui_point_t){.x = area->x, .y = area->y};
            scui_object_prop_add(event->object, &prop);
            prop.style = scui_object_style_rect_width;
            prop.data.number = area->w;
            scui_object_prop_add(event->object, &prop);
            prop.style = scui_object_style_rect_height;
            prop.data.number = area->h;
            scui_object_prop_add(event->object, &prop);
            
            /* 统一基准(form_rect_all): 逐层推演 */
            scui_object_sub_t sub = {.part = scui_object_part_rect_item};
            sub.state = state;
            /* 无基准几何 → 回退def */
            if (!scui_object_form_rect(event->object, &sub) && sub.state != scui_object_state_def) {
                sub.state  = scui_object_state_def;
                scui_object_form_rect(event->object, &sub);
            }
            
            for (uint8_t form_i = 0; form_i < scui_arr_len(form_table); form_i++) {
                scui_object_prop_t prop_draw = {.form = form_table[form_i]};
                prop_draw.part  = scui_object_part_rect_item;
                prop_draw.state = sub.state;
                /* 样式不全 → 回退def */
                if (!scui_object_draw_rect(event->object, &prop_draw) &&
                    prop_draw.state != scui_object_state_def) {
                    prop_draw.state  = scui_object_state_def;
                    scui_object_draw_rect(event->object, &prop_draw);
                }
            }
        }
        
        /* 还原基准几何: 铺满控件 */
        scui_object_prop_t prop = {0};
        prop.part  = scui_object_part_rect_item;
        prop.form  = scui_object_form_rect_all;
        prop.state = scui_object_state_def;
        prop.style = scui_object_style_rect_point;
        prop.data.point = (scui_point_t){0};
        scui_object_prop_add(event->object, &prop);
        prop.style = scui_object_style_rect_width;
        prop.data.number = widget->clip.w;
        scui_object_prop_add(event->object, &prop);
        prop.style = scui_object_style_rect_height;
        prop.data.number = widget->clip.h;
        scui_object_prop_add(event->object, &prop);
        break;
    }
    default:
        break;
    }
}
