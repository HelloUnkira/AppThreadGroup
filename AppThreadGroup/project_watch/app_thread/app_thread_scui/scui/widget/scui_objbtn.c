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
 *@param layout     通过布局创建
 */
void scui_objbtn_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_objbtn_t *objbtn = widget;
    scui_objbtn_maker_t *objbtn_maker = widget_maker;
    
    /* 必须标记anima,widget,ptr事件 */
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    widget_maker->style.indev_ptr    = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_objbtn));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    for (scui_handle_t idx = 0; idx < 4; idx++) {
        objbtn->color[idx] = objbtn_maker->color[idx];
        objbtn->alpha[idx] = objbtn_maker->alpha[idx];
        objbtn->width[idx] = objbtn_maker->width[idx];
    }
    objbtn->radius = objbtn_maker->radius;
    
    objbtn->lim         = SCUI_WIDGET_OBJBTN_PCT;
    objbtn->pct         = SCUI_WIDGET_OBJBTN_PCT;
    objbtn->way         = -1;
    objbtn->hold        = false;
    objbtn->click       = false;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_objbtn_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_objbtn));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_objbtn_t *objbtn = (void *)widget;
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_objbtn_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_objbtn_t *objbtn = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        if (!(objbtn->pct <= objbtn->lim && objbtn->way == -1)) {
            if (!objbtn->hold && (objbtn->pct == 100 || objbtn->pct == objbtn->lim))
                 objbtn->way  = -objbtn->way;
            if ((objbtn->pct  < 100 && objbtn->pct > objbtn->lim) ||
                (objbtn->pct == 100 && objbtn->way == -1) ||
                (objbtn->pct == objbtn->lim && objbtn->way == +1)) {
                 objbtn->pct += objbtn->way * 2;
                 scui_widget_draw(widget->myself, NULL, false);
            }
            SCUI_LOG_INFO("<%d, %d>", objbtn->pct, objbtn->way);
        } else {
            if (objbtn->click) {
                objbtn->click = false;
                scui_event_t event = {
                    .object     = widget->myself,
                    .style.sync = true,
                    .type       = scui_event_widget_button_click,
                };
                scui_event_notify(&event);
            }
        }
        
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_area_t dst_clip[4] = {0};
        dst_clip[3] = objbtn->widget.clip;
        
        SCUI_ASSERT(objbtn->pct >= objbtn->lim && objbtn->pct <= 100);
        scui_multi_t scale_x = dst_clip[3].w * (100 - objbtn->pct) / 100 / 2;
        scui_multi_t scale_y = dst_clip[3].h * (100 - objbtn->pct) / 100 / 2;
        dst_clip[3].x += scale_x;
        dst_clip[3].y += scale_y;
        dst_clip[3].w -= scale_x * 2;
        dst_clip[3].h -= scale_y * 2;
        /* 生成各个部件剪切域 */
        for (scui_coord_t idx = 3; idx - 1 >= 0; idx--) {
            dst_clip[idx - 1] = dst_clip[idx];
            dst_clip[idx - 1].x += objbtn->width[idx];
            dst_clip[idx - 1].y += objbtn->width[idx];
            dst_clip[idx - 1].w -= objbtn->width[idx] * 2;
            dst_clip[idx - 1].h -= objbtn->width[idx] * 2;
        }
        
        scui_color_t src_color[4] = {0};
        scui_coord_t src_radius[4] = {0};
        src_radius[0] = objbtn->radius;
        for (scui_coord_t idx = 0; idx + 1 < 4; idx++) {
            src_radius[idx + 1] = src_radius[idx] + objbtn->width[idx + 1];
        }
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            
            scui_color32_t color_s = objbtn->color[idx].color_s;
            scui_color32_t color_e = objbtn->color[idx].color_e;
            src_color[idx].color.ch.b = scui_map(objbtn->pct, 100, objbtn->lim, color_e.ch.b, color_s.ch.b);
            src_color[idx].color.ch.g = scui_map(objbtn->pct, 100, objbtn->lim, color_e.ch.g, color_s.ch.g);
            src_color[idx].color.ch.r = scui_map(objbtn->pct, 100, objbtn->lim, color_e.ch.r, color_s.ch.r);
            src_color[idx].color.ch.a = scui_map(objbtn->pct, 100, objbtn->lim, color_e.ch.a, color_s.ch.a);
        }
        
        for (scui_coord_t idx = 3; idx >= 0; idx--) {
            
            scui_draw_graph_dsc_t draw_graph = {0};
            draw_graph.dst_surface = widget->surface;
            draw_graph.dst_clip  = &dst_clip[idx];
            draw_graph.src_alpha = objbtn->alpha[idx];
            draw_graph.src_color = src_color[idx];
            
            if (idx != 3) {
                draw_graph.type = scui_draw_graph_type_crect;
                draw_graph.crect.src_width  = objbtn->width[idx];
                draw_graph.crect.src_radius = src_radius[idx];
            } else {
                draw_graph.type = scui_draw_graph_type_crect_shadow;
                draw_graph.crect_shadow.src_width  = objbtn->width[idx];
                draw_graph.crect_shadow.src_radius = src_radius[idx];
            }
            scui_draw_graph_ctx(&draw_graph);
        }
        
        break;
    }
    case scui_event_ptr_down: {
        
        scui_handle_t handle = scui_widget_root(widget->myself);
        scui_widget_t  *root = scui_handle_source_check(handle);
        
        scui_area_t clip = widget->clip;
        if (widget != root) {
            clip.x += root->clip.x;
            clip.y += root->clip.y;
        }
        if (!scui_area_point(&clip, &event->ptr_c))
             break;
        
        SCUI_LOG_INFO("");
        objbtn->hold = true;
        objbtn->pct  = objbtn->lim;
        objbtn->way  = +1;
        break;
    }
    case scui_event_ptr_move:
    case scui_event_ptr_up: {
        
        SCUI_LOG_INFO("");
        objbtn->hold = false;
        break;
    }
    case scui_event_ptr_click: {
        
        scui_event_mask_over(event);
        SCUI_LOG_INFO("");
        objbtn->hold  = true;
        objbtn->click = true;
        break;
    }
    default:
        break;
    }
}
