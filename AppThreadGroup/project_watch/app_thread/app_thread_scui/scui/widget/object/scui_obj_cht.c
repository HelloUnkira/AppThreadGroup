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
void scui_obj_cht_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t  *widget  = inst;
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t  *widget_maker  = inst_maker;
    scui_object_maker_t  *object_maker  = (void *)widget_maker;
    scui_obj_cht_maker_t *obj_cht_maker = (void *)widget_maker;
    
    /* 必须标记ptr,widget事件 */
    widget_maker->style.indev_ptr    = true;
    widget_maker->style.sched_widget = true;
    /* 循环: 开放anima事件(外部按帧推送, 驱动刷新) */
    if (obj_cht_maker->loop)
        widget_maker->style.sched_anima = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_cht));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_cht->type       = obj_cht_maker->type;
    obj_cht->value_min  = obj_cht_maker->value_min;
    obj_cht->value_max  = obj_cht_maker->value_max;
    obj_cht->number     = obj_cht_maker->number;
    obj_cht->step       = obj_cht_maker->step;
    obj_cht->loop       = obj_cht_maker->loop;
    obj_cht->form       = scui_object_form_rect_base;
    obj_cht->vlist_min  = NULL;
    obj_cht->vlist_max  = NULL;
    obj_cht->vlist_dot  = NULL;
    obj_cht->vlist_pos  = NULL;
    obj_cht->ring       = 0;
    obj_cht->ofs_max    = 0;
    obj_cht->ofs_cur    = 0;
    obj_cht->ofs_base   = 0;
    obj_cht->point_base = 0;
    
    /* 运行初值断言 */
    SCUI_ASSERT(obj_cht->value_min < obj_cht->value_max);
    SCUI_ASSERT(obj_cht->number != 0);
    
    /* 步进限制: 非正值退化为1(逐像素铺开) */
    if (obj_cht->step <= 0) obj_cht->step = 1;
    
    /* 写头留白宽度(像素): 未给定时以步进为基准(一格) */
    obj_cht->gap = obj_cht_maker->gap > 0 ? obj_cht_maker->gap : obj_cht->step;
    
    /* 运行数据缓冲分配(按运行初值) */
    switch (obj_cht->type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        scui_multi_t data_size = obj_cht->number * sizeof(scui_coord_t);
        obj_cht->vlist_min = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        obj_cht->vlist_max = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < obj_cht->number; idx++) {
            obj_cht->vlist_min[idx] = obj_cht->value_min;
            obj_cht->vlist_max[idx] = obj_cht->value_min;
        }
        break;
    }
    case 1: {
        scui_multi_t data_size = obj_cht->number * sizeof(scui_coord_t);
        obj_cht->vlist_dot = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        scui_multi_t vpos_size = obj_cht->number * sizeof(scui_point_t);
        obj_cht->vlist_pos = SCUI_MEM_ALLOC(scui_mem_type_mix, vpos_size);
        for (scui_coord_t idx = 0; idx < obj_cht->number; idx++) {
            obj_cht->vlist_dot[idx] = obj_cht->value_min;
        }
        break;
    }
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_cht_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_cht));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    /* 资源析构 */
    switch (obj_cht->type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        SCUI_MEM_FREE(obj_cht->vlist_min);
        SCUI_MEM_FREE(obj_cht->vlist_max);
        break;
    }
    case 1: {
        SCUI_MEM_FREE(obj_cht->vlist_dot);
        SCUI_MEM_FREE(obj_cht->vlist_pos);
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
void scui_obj_cht_type(scui_handle_t handle, scui_coord_t *type)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_cht));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    *type = obj_cht->type;
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_cht_style(scui_handle_t handle, scui_obj_cht_res_t *res)
{
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    switch (res->part) {
    case scui_object_part_line_item: {
        /* 折线: 单层图元(form恒0), 绘制区域由绘制期同步(随控件尺寸) */
        scui_object_sub_t sub = {0};
        sub.line.alpha.alpha       = res->alpha;
        sub.line.color.color32     = res->color.color;
        sub.line.stroke.number     = scui_max(res->width, 1);
        sub.line.multi.multi.round = res->round;
        sub.line.multi.multi.grad  = res->grad;
        
        sub.part  = res->part;
        sub.state = scui_object_state_def;
        scui_object_prop_line(handle, &sub);
        break;
    }
    case scui_object_part_rect_fg: {
        /* 柱状: 单层图元(条目几何绘制期改写), 绘制区域即控件所在区域 */
        scui_object_sub_t sub = {.part = res->part, .form = res->form};
        sub.rect.alpha.alpha   = res->alpha;
        sub.rect.color.color32 = res->color.color;
        sub.rect.width.number  = scui_max(res->width, 1);
        sub.rect.radius.number = res->round ? -1 : 0;
        sub.state = scui_object_state_def;
        scui_object_prop_rect(handle, &sub);
        
        /* 层级录入: 绘制期回读(与样式同层) */
        obj_cht->form = res->form;
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
void scui_obj_cht_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_cht));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    SCUI_ASSERT(obj_cht->type == 0);
    
    scui_coord_t value_min = obj_cht->value_min;
    scui_coord_t value_max = obj_cht->value_max;
    for (scui_coord_t idx = 0; idx < obj_cht->number; idx++) {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, vlist_min[idx]));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, vlist_max[idx]));
        if (min > max) {scui_coord_t tmp = min; min = max; max = tmp;}
        
        obj_cht->vlist_min[idx] = min;
        obj_cht->vlist_max[idx] = max;
    }
    
    /* 数据更新: 重绘 */
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 控件数据列表更新(line)
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_obj_cht_line_data(scui_handle_t handle, scui_coord_t *vlist_dot)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_cht));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    SCUI_ASSERT(obj_cht->type == 1);
    
    scui_coord_t value_min = obj_cht->value_min;
    scui_coord_t value_max = obj_cht->value_max;
    for (scui_coord_t idx = 0; idx < obj_cht->number; idx++) {
        scui_coord_t val = scui_min(value_max, scui_max(value_min, vlist_dot[idx]));
        obj_cht->vlist_dot[idx] = val;
    }
    
    /* 数据更新: 重绘 */
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 控件循环推送(环上写入一个样本)
 *@param handle  控件句柄
 *@param value_1 数据值(hist:最小值; line:数据值)
 *@param value_2 数据值(hist:最大值; line:忽略)
 */
void scui_obj_cht_loop_push(scui_handle_t handle, scui_coord_t value_1, scui_coord_t value_2)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_cht));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    SCUI_ASSERT(obj_cht->loop != 0);
    
    scui_coord_t value_min = obj_cht->value_min;
    scui_coord_t value_max = obj_cht->value_max;
    scui_coord_t slot      = obj_cht->ring;
    
    switch (obj_cht->type) {
    default:SCUI_ASSERT(false);break;
    case 0: {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, value_1));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, value_2));
        if (min > max) {scui_coord_t tmp = min; min = max; max = tmp;}
        
        obj_cht->vlist_min[slot] = min;
        obj_cht->vlist_max[slot] = max;
        break;
    }
    case 1: {
        obj_cht->vlist_dot[slot] = scui_min(value_max, scui_max(value_min, value_1));
        break;
    }
    }
    
    /* 环首推进: 最旧槽位让位给下一个样本 */
    obj_cht->ring = slot + 1 < obj_cht->number ? slot + 1 : 0;
    
    /* 数据更新: 重绘 */
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_cht_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t  *widget  = scui_handle_source_check(event->object);
    scui_object_t  *object  = (void *)widget;
    scui_obj_cht_t *obj_cht = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_layout: {
        
        /* 条目自身宽度(柱状取条宽; 折线恒1) */
        scui_coord_t item_w = 1;
        if (obj_cht->type == 0) {
            scui_object_data_t width = {0};
            scui_object_prop_sync_s(event->object, scui_object_part_rect_fg,
                obj_cht->form, scui_object_style_rect_width,
                scui_object_state_def, width);
            item_w = scui_max(width.number, 1);
        }
        /* 内容总宽: 条目步进铺开 + 末条目自身宽度 */
        scui_coord_t content_w = (obj_cht->number - 1) * obj_cht->step + item_w;
        
        /* 自动宽度: 撑满父级可视区(可视宽 = 父右边 - 自身左边) */
        /* 宽度不取内容总宽: 内容超出部转为跟手行程(见ofs_max), 二者不可兼得 */
        if (widget->state.layout_w) {
            scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
            scui_widget_adjust_size(event->object, scui_max(widget_p->clip.x +
                widget_p->clip.w - widget->clip.x, 1), widget->clip.h);
        }
        
        /* 循环: 数据项+gap=整个宽度(环宽即控件宽), 故无超出无跟手 */
        if (obj_cht->loop)
            content_w = widget->clip.w;
        
        /* 可视偏移: 内容超出部即跟手行程 */
        obj_cht->ofs_max = scui_max(content_w - widget->clip.w, 0);
        obj_cht->ofs_cur = scui_min(obj_cht->ofs_cur, obj_cht->ofs_max);
        break;
    }
    case scui_event_ptr_down: {
        
        /* 内容不超出: 让位父级容器 */
        if (obj_cht->ofs_max <= 0)
            break;
        
        /* 记录按下基准(偏移/点) */
        obj_cht->ofs_base   = obj_cht->ofs_cur;
        obj_cht->point_base = event->ptr_c.x;
        break;
    }
    case scui_event_ptr_move: {
        
        /* 内容不超出: 让位父级容器 */
        if (obj_cht->ofs_max <= 0)
            break;
        /* 仅水平方向接管(水平拖动查看超界数据) */
        if (!scui_opt_bits_check(event->ptr_dir, scui_opt_dir_hor))
            break;
        scui_event_mask_over(event);
        
        /* 增量跟手: 从按下基准起始, 按位移推进(无跳变) */
        scui_coord_t delta = event->ptr_e.x - obj_cht->point_base;
        scui_coord_t ofs_c = scui_clamp(obj_cht->ofs_base - delta, 0, obj_cht->ofs_max);
        if (ofs_c == obj_cht->ofs_cur)
            break;
        
        obj_cht->ofs_cur = ofs_c;
        scui_widget_draw(widget->myself, NULL, false, 0);
        break;
    }
    case scui_event_draw_graph: {
        
        /* 绘制区域即控件所在区域(无外部区域参数) */
        scui_object_type_t state = scui_object_state_def;
        scui_object_state_get(event->object, &state);
        
        switch (obj_cht->type) {
        default:SCUI_ASSERT(false);break;
        case 0: {
            scui_object_data_t width = {0};
            scui_object_prop_sync_s(event->object, scui_object_part_rect_fg,
                obj_cht->form, scui_object_style_rect_width,
                scui_object_state_def, width);
            
            scui_object_prop_t prop = {0};
            prop.part  = scui_object_part_rect_fg;
            prop.form  = obj_cht->form;
            prop.state = state;
            
            /* 逐条目: 值域映射为柱高, 步进铺开为柱位 */
            for (scui_coord_t idx = 0; idx < obj_cht->number; idx++) {
                /* 循环: 按时间序重排(环首=最旧), 槽位固定不随数据移动 */
                scui_coord_t slot = idx;
                if (obj_cht->loop) {
                    slot = obj_cht->ring + idx;
                    if (slot >= obj_cht->number) slot -= obj_cht->number;
                }
                /* 循环: 写头留白(数据项+gap=整个宽度; gap不绘制, 被数据逐格挤走) */
                if (obj_cht->loop && idx * widget->clip.w /
                    scui_max(obj_cht->number - 1, 1) < obj_cht->gap)
                    continue;
                scui_coord_t offset_1y = scui_map(obj_cht->vlist_min[slot],
                    obj_cht->value_min, obj_cht->value_max, widget->clip.h, 0);
                scui_coord_t offset_2y = scui_map(obj_cht->vlist_max[slot],
                    obj_cht->value_min, obj_cht->value_max, widget->clip.h, 0);
                
                /* 柱高不足条宽: 不绘制 */
                if (offset_1y - offset_2y < width.number)
                    continue;
                
                /* 循环: 槽位铺满整宽(数据项+gap=整个宽度; 末槽贴右边界) */
                scui_coord_t slot_x = obj_cht->loop ?
                    slot * (widget->clip.w - width.number) /
                    scui_max(obj_cht->number - 1, 1) : slot * obj_cht->step;
                scui_point_t point = {
                    .x = slot_x - obj_cht->ofs_cur,
                    .y = offset_2y,
                };
                prop.data.point = point;
                prop.style = scui_object_style_rect_point;
                scui_object_prop_add(event->object, &prop);
                prop.data.number = offset_1y - offset_2y;
                prop.style = scui_object_style_rect_height;
                scui_object_prop_add(event->object, &prop);
                /* 样式不全 → 回退def(副本隔离) */
                scui_object_prop_t prop_draw = prop;
                if (!scui_object_draw_rect(event->object, &prop_draw) &&
                    prop_draw.state != scui_object_state_def) {
                    prop_draw.state  = scui_object_state_def;
                    scui_object_draw_rect(event->object, &prop_draw);
                }
            }
            break;
        }
        case 1: {
            
            /* 绘制区域: 控件所在区域(尺寸随AUTO_W/布局变化, 绘制期同步) */
            scui_object_prop_t prop = {
                .part  = scui_object_part_line_item,
                .state = state,
                .style = scui_object_style_line_area,
                .data.area = (scui_area_t){.w = widget->clip.w, .h = widget->clip.h},
            };
            scui_object_prop_add(event->object, &prop);
            
            /* 循环: 写头留白(数据项+gap=整个宽度; gap不绘制, 被数据逐格挤走) */
            scui_coord_t dot_beg = obj_cht->loop ?
                (obj_cht->gap * scui_max(obj_cht->number - 1, 1) +
                widget->clip.w - 1) / widget->clip.w : 0;
            /* 逐条目: 值域映射为点高, 步进铺开为点位 */
            for (scui_coord_t idx = dot_beg; idx < obj_cht->number; idx++) {
                /* 循环: 按时间序重排(环首=最旧), 槽位固定不随数据移动 */
                scui_coord_t slot = idx;
                if (obj_cht->loop) {
                    slot = obj_cht->ring + idx;
                    if (slot >= obj_cht->number) slot -= obj_cht->number;
                }
                scui_coord_t offset_y = scui_map(obj_cht->vlist_dot[slot],
                    obj_cht->value_min, obj_cht->value_max, widget->clip.h, 0);
                
                /* 循环: 点位铺满整宽(数据项+gap=整个宽度; 末点贴右边界) */
                obj_cht->vlist_pos[idx - dot_beg].x = (obj_cht->loop ?
                    slot * widget->clip.w / scui_max(obj_cht->number - 1, 1) :
                    slot * obj_cht->step) - obj_cht->ofs_cur;
                obj_cht->vlist_pos[idx - dot_beg].y = offset_y;
            }
            
            /* 循环: 接缝(尾槽→首槽)在x上不相邻, 断开为俩段 */
            scui_coord_t seg_dot[2] = {obj_cht->number - dot_beg, 0};
            if (obj_cht->loop) {
                scui_coord_t dot_all  = obj_cht->number - dot_beg;
                scui_coord_t dot_slot = obj_cht->ring + dot_beg;
                if (dot_slot >= obj_cht->number) dot_slot -= obj_cht->number;
                if (dot_slot + dot_all > obj_cht->number) {
                    seg_dot[0] = obj_cht->number - dot_slot;
                    seg_dot[1] = dot_slot + dot_all - obj_cht->number;
                }
            }
            
            scui_coord_t dot_ofs = 0;
            for (scui_coord_t seg = 0; seg < 2; seg++) {
                scui_coord_t dot_num = seg_dot[seg];
                if (dot_num < 2) {
                    dot_ofs += dot_num;
                    continue;
                }
                prop.style = scui_object_style_line_vpos;
                prop.data.pointer = &obj_cht->vlist_pos[dot_ofs];
                scui_object_prop_add(event->object, &prop);
                prop.style = scui_object_style_line_vpos_num;
                prop.data.number = dot_num;
                scui_object_prop_add(event->object, &prop);
                scui_object_draw_line(event->object, &prop);
                dot_ofs += dot_num;
            }
            break;
        }
        }
        break;
    }
    default:
        break;
    }
}
