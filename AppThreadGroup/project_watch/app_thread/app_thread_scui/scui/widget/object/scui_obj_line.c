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
void scui_obj_line_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t   *widget   = inst;
    scui_object_t   *object   = (void *)widget;
    scui_obj_line_t *obj_line = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t   *widget_maker   = inst_maker;
    scui_object_maker_t   *object_maker   = (void *)widget_maker;
    scui_obj_line_maker_t *obj_line_maker = (void *)widget_maker;
    
    /* 必须标记widget事件 */
    widget_maker->style.sched_widget = true;
    
    /* 构造派生控件实例 */
    scui_object_make(object, object_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_line));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_line->mode    = obj_line_maker->mode;
    obj_line->seg_num = obj_line_maker->seg_num;
    obj_line->dot_num = obj_line_maker->dot_num;
    
    obj_line->seg_max = obj_line_maker->seg_num;
    obj_line->dot_max = obj_line_maker->dot_num;
    
    /* 运行初值断言 */
    SCUI_ASSERT(obj_line->seg_num != 0);
    SCUI_ASSERT(obj_line->dot_num != 0);
    
    /* 线段序列缓冲分配(按线段上限) */
    scui_multi_t dot_size  = obj_line->seg_max * sizeof(scui_coord_t);
    scui_multi_t vpos_size = obj_line->dot_max * sizeof(scui_point_t);
    obj_line->seg_dot = SCUI_MEM_ZALLOC(scui_mem_type_mix, dot_size);
    obj_line->vpos    = SCUI_MEM_ZALLOC(scui_mem_type_mix, vpos_size);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_line_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_line));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_line_t *obj_line = (void *)widget;
    
    /* 线段序列缓冲释放 */
    SCUI_MEM_FREE(obj_line->seg_dot);
    SCUI_MEM_FREE(obj_line->vpos);
    
    /* 析构派生控件实例 */
    scui_object_burn(widget->myself);
}

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_line_style(scui_handle_t handle, scui_obj_line_res_t *res)
{
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_object_t   *object   = (void *)widget;
    scui_obj_line_t *obj_line = (void *)widget;
    
    scui_object_sub_t sub = {0};
    sub.line.alpha.alpha       = res->alpha;
    sub.line.color.color32     = res->color.color;
    sub.line.area.area         = (scui_area_t){.w = widget->clip.w, .h = widget->clip.h};
    sub.line.stroke.number     = scui_max(res->width, 1);
    sub.line.multi.multi.round = res->round;
    sub.line.multi.multi.grad  = res->grad;
    
    sub.part  = res->part;
    sub.state = scui_object_state_def;
    scui_object_prop_line(handle, &sub);
}

/*@brief 端点序列设置
 *@param handle  控件句柄
 *@param seg_num 线段数量(<=构造上限; 1:连续)
 *@param dot_num 端点数量(<=构造上限)
 *@param seg_dot 线段端点数(空=等分)
 *@param vpos    端点序列(坐标:控件相对; 极坐标:角度+半径)
 */
void scui_obj_line_data_set(scui_handle_t handle, scui_coord_t seg_num,
    scui_coord_t dot_num, scui_coord_t *seg_dot, scui_point_t *vpos)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_line));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_obj_line_t *obj_line = (void *)widget;
    
    SCUI_ASSERT(seg_num <= obj_line->seg_max);
    SCUI_ASSERT(dot_num <= obj_line->dot_max);
    obj_line->seg_num = seg_num;
    obj_line->dot_num = dot_num;
    
    /* 线段端点数(空则等分: 相邻刻度之差吸收整除截断) */
    for (scui_coord_t idx = 0; idx < seg_num; idx++) {
        scui_coord_t dot_beg = dot_num * (idx + 0) / seg_num;
        scui_coord_t dot_end = dot_num * (idx + 1) / seg_num;
        obj_line->seg_dot[idx] = seg_dot ? seg_dot[idx] : dot_end - dot_beg;
    }
    
    /* 端点序列拷贝 */
    for (scui_coord_t idx = 0; idx < dot_num; idx++)
        obj_line->vpos[idx] = vpos[idx];
    
    /* 极坐标换算: 角度(0:三点方向;顺时针)与半径 → 控件坐标 */
    if (obj_line->mode == 1) {
        scui_point_t center = {.x = widget->clip.w / 2, .y = widget->clip.h / 2};
        for (scui_coord_t idx = 0; idx < dot_num; idx++) {
            obj_line->vpos[idx].x = center.x + vpos[idx].y * scui_cos4096(vpos[idx].x) / 4096;
            obj_line->vpos[idx].y = center.y + vpos[idx].y * scui_sin4096(vpos[idx].x) / 4096;
        }
    }
    
    SCUI_LOG_INFO("line mode:%d seg:%d dot:%d",
        obj_line->mode, seg_num, dot_num);
    
    for (scui_coord_t idx = 0; idx < dot_num; idx++)
        SCUI_LOG_INFO("dot[%2d] x:%d y:%d", idx,
            obj_line->vpos[idx].x,
            obj_line->vpos[idx].y);
    
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_line_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t   *widget   = scui_handle_source_check(event->object);
    scui_object_t   *object   = (void *)widget;
    scui_obj_line_t *obj_line = (void *)widget;
    
    /* 基类处理(过渡动画推进) */
    scui_object_invoke(event);
    
    switch (event->type) {
    case scui_event_draw_graph: {
        if (obj_line->seg_num <= 0)
            break;
        
        scui_object_prop_t prop = {
            .part  = scui_object_part_line_item,
            .state = scui_object_state_def,
        };
        
        /* 线段绘制: 逐段改写端点序列与端点数 */
        scui_coord_t dot_i = 0;
        for (scui_coord_t seg = 0; seg < obj_line->seg_num; seg++) {
            scui_coord_t seg_dot = obj_line->seg_dot[seg];
            if (seg_dot <= 1)
                continue;
            
            prop.style = scui_object_style_line_vpos;
            prop.data.pointer = &obj_line->vpos[dot_i];
            scui_object_prop_add(event->object, &prop);
            prop.style = scui_object_style_line_vpos_num;
            prop.data.number = seg_dot;
            scui_object_prop_add(event->object, &prop);
            scui_object_draw_line(event->object, &prop);
            dot_i += seg_dot;
        }
        break;
    }
    default:
        break;
    }
}
