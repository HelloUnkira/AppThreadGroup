/*实现目标:
 *    控件(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 为剪切域集合计算以调整源到目标的剪切域(内部接口)
 *@param 形如scui_widget_surface_draw_xxx的接口使用
 */
static bool scui_widget_draw_clip_adjust(scui_widget_t *widget,   scui_area_t *unit_clip,
                                         scui_area_t   *target,   scui_area_t *clip,
                                         scui_area_t   *dst_clip, scui_area_t *src_clip)
{
    *dst_clip = (scui_area_t){0};
    *src_clip = *clip;
    
    /* 子剪切域要相对同步偏移 */
    if (!scui_area_inter(dst_clip, unit_clip, target))
         return false;
    /* 子剪切域要相对同步偏移 */
    scui_point_t offset = {
        .x = dst_clip->x - target->x,
        .y = dst_clip->y - target->y,
    };
    if (!scui_area_limit_offset(src_clip, &offset))
         return false;
    
    return true;
}

/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return true;
    if (scui_clip_empty(&widget->clip_set))
        return true;
    
    return false;
}

/*@brief 控件在画布绘制字符串
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数(scui_string_args_t)
 */
void scui_widget_draw_string(scui_handle_t handle, scui_area_t *target, void *args)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 当前本接口作为控件专用绘制接口: */
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t string_clip = {
        .w = widget->clip_set.clip.w,
        .h = widget->clip_set.clip.h,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, &string_clip, &dst_clip, &src_clip))
            scui_draw_string(widget->surface, &dst_clip, args, &src_clip, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制纯色区域
 *@param handle 控件句柄
 *@param clip   绘制区域
 *@param color  源色调
 */
void scui_widget_draw_color(scui_handle_t handle, scui_area_t *clip,
                            scui_color_t  color)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    if (clip == NULL)
        clip  = &widget->clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, clip, clip, &dst_clip, &src_clip))
            scui_draw_area_fill(widget->surface, &dst_clip, color, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param color  图像源色调(调色板)
 */
void scui_widget_draw_image(scui_handle_t handle, scui_area_t *target,
                            scui_handle_t image,  scui_area_t *clip,
                            scui_color_t  color)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    #if 1
    /* 这里有一个优化点(主要用于全局背景绘制) */
    /* 条件达到时,将资源直接加载到绘制画布上即可 */
    if ((widget->surface->format == scui_pixel_cf_bmp565 ||
         widget->surface->format == scui_pixel_cf_bmp888)   &&
         widget->clip_set.clip.w == scui_disp_get_hor_res() &&
         widget->clip_set.clip.h == scui_disp_get_ver_res() &&
         scui_area_equal(&widget->clip_set.clip, clip)) {
         scui_image_src_read(image_inst, widget->surface->pixel);
         #if 1
         /* 上面默认使用的全局截切域 */
         /* 所以可能存在覆盖,为所有子控件补充剪切域 */
         scui_widget_child_list_btra(widget, idx) {
             scui_handle_t handle = widget->child_list[idx];
             scui_widget_t *child = scui_handle_get(handle);
             scui_widget_clip_reset(child, &widget->clip_set.clip, true);
             scui_widget_clip_update(child);
         }
         #endif
         return;
    }
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, clip, &dst_clip, &src_clip))
            scui_draw_image(widget->surface, &dst_clip, image_inst, &src_clip,
                            widget->alpha, color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param scale  图形缩放比例(1024为放大系数)
 *@param pos    缩放锚点
 */
void scui_widget_draw_image_scale(scui_handle_t handle, scui_area_t     *target,
                                  scui_handle_t image,  scui_area_t     *clip,
                                  scui_point_t  scale,  scui_event_pos_t pos)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    if (clip == NULL)
        clip  = &image_clip;
    
#if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
#endif
    
    scui_point_t src_offset = {0};
    scui_point_t dst_offset = {0};
    
    if ((pos & scui_event_pos_hor) != 0) {
        src_offset.x = clip->w / 2;
        dst_offset.x = target->w / 2;
    } else if ((pos & scui_event_pos_l) != 0) {
    } else if ((pos & scui_event_pos_r) != 0) {
        src_offset.x = clip->w;
        dst_offset.x = target->w;
    }
    
    if ((pos & scui_event_pos_ver) != 0) {
        src_offset.y = clip->h / 2;
        dst_offset.y = target->h / 2;
    } else if ((pos & scui_event_pos_u) != 0) {
    } else if ((pos & scui_event_pos_d) != 0) {
        src_offset.y = clip->h;
        dst_offset.y = target->h;
    }
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, clip, &dst_clip, &src_clip)) {
            
            src_offset.x += +(dst_clip.x - target->x);
            src_offset.y += +(dst_clip.y - target->y);
            scui_draw_image_scale(widget->surface, &dst_clip, image_inst, &src_clip,
                                  widget->alpha, scale, dst_offset, src_offset);
        }
    }
    
#if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
#endif
}

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param anchor 图像旋转轴心
 *@param center 图像旋转中心
 *@param angle  图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_widget_draw_image_rotate(scui_handle_t handle, scui_area_t  *target,
                                   scui_handle_t image,  scui_area_t  *clip,
                                   scui_point_t *anchor, scui_point_t *center,
                                   scui_coord_t  angle)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, clip, &dst_clip, &src_clip))
            scui_draw_image_rotate(widget->surface, &dst_clip, image_inst, &src_clip,
                                   widget->alpha, angle, anchor, center);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param matrix 变换矩阵
 */
void scui_widget_draw_image_by_matrix(scui_handle_t  handle, scui_area_t *target,
                                      scui_handle_t  image,  scui_area_t *clip,
                                      scui_matrix_t *matrix)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, clip, &dst_clip, &src_clip))
            scui_draw_image_blit_by_matrix(widget->surface, &dst_clip, image_inst, &src_clip,
                                           widget->alpha, matrix);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
       SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制圆环
 *@param handle  控件句柄
 *@param target  控件绘制区域
 *@param image   图像句柄
 *@param clip    图像源绘制区域
 *@param angle_s 起始角度
 *@param color   源色调
 *@param angle_s 起始角度
 *@param percent 圆环进度(百分比)
 *@param image_e 控件句柄
 */
void scui_widget_draw_ring(scui_handle_t handle,  scui_area_t *target,
                           scui_handle_t image,   scui_area_t *clip,
                           scui_coord_t  angle_s, scui_color_t color,
                           scui_coord_t  angle_e, scui_coord_t percent,
                           scui_handle_t image_e)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    /* 参数检查: */
    SCUI_ASSERT(0 <= percent && percent <= 100);
    /* 调整(adjust): */
    scui_coord_t angle_as = angle_s;
    scui_coord_t angle_ae = scui_map(percent, 0, 100, angle_s, angle_e);
    
    if (percent == 0)
        return;
    if (angle_s == angle_e) {
        SCUI_LOG_WARN("ring angle is zero");
        return;
    }
    
    scui_image_t *image_inst = scui_handle_get(image);
    scui_image_t *image_e_inst = scui_handle_get(image_e);
    SCUI_ASSERT(image_inst != NULL);
    SCUI_ASSERT(image_e_inst != NULL);
    
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    if (target == NULL)
        target  = &widget->clip;
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_point_t dst_center = {
        .x = target->x - widget->clip.x + image_inst->pixel.width  / 2,
        .y = target->y - widget->clip.y + image_inst->pixel.height / 2,
    };
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_draw_clip_adjust(widget,
            &unit->clip, target, clip, &dst_clip, &src_clip))
            scui_draw_ring(widget->surface, &dst_clip, &dst_center, image_e_inst, image_inst, &src_clip,
                           angle_as, widget->alpha, angle_ae, color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件在画布绘制线条
 *@param handle 控件句柄
 *@param width  线条宽
 *@param pos_1  位置1
 *@param pos_2  位置2
 *@param color  源色调
 */
void scui_widget_draw_line(scui_handle_t handle, scui_coord_t width,
                           scui_point_t  pos_1,  scui_point_t pos_2,
                           scui_color_t  color)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    if (width <= 0)
        width  = 1;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        scui_draw_line(widget->surface, &unit->clip, color, width, pos_1, pos_2, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}
