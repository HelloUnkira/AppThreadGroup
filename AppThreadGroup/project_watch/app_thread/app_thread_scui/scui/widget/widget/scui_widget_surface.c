/*实现目标:
 *    控件画布(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget)
{
    if (widget->surface->pixel == NULL ||
        widget->surface->pixel == scui_surface_fb_draw()->pixel ||
        widget->surface->pixel == scui_surface_fb_refr()->pixel)
        return false;
    
    return true;
}

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    SCUI_ASSERT(widget->surface->pixel != surface->pixel);
    
    /* 直接交换实例最简单 */
    uint8_t *pixel = widget->surface->pixel;
    widget->surface->pixel = surface->pixel;
    surface->pixel = pixel;
}

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->clip_set.clip = widget->clip;
    /* 有独立画布的根控件不记录原点偏移,控件树永远相对独立画布移动 */
    /* 没有独立画布的根控件保留原点偏移,控件树永远相对绘制画布移动 */
    if (widget->parent == SCUI_HANDLE_INVALID && scui_widget_surface_only(widget)) {
        widget->clip_set.clip.x = 0;
        widget->clip_set.clip.y = 0;
    }
    
    /* 画布的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget->clip_set.clip, &widget_parent->clip_set.clip))
            widget->clip_set.clip = clip_inter;
        else
            widget->clip_set.clip = (scui_area_t){0};
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_refr(child, recurse);
    }
}

/*@brief 控件画布重映射
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->surface = surface;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_remap(child, surface);
    }
}

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    SCUI_ASSERT(widget->surface->pixel != surface->pixel);
    
    scui_surface_t *dst_surface = widget->surface;
    scui_surface_t *src_surface = surface;
    scui_area_t dst_clip = {
        .w = dst_surface->hor_res,
        .h = dst_surface->ver_res,
    };
    scui_area_t src_clip = {
        .w = src_surface->hor_res,
        .h = src_surface->ver_res,
    };
    
    scui_draw_area_copy(dst_surface, &dst_clip, src_surface, &src_clip);
}

/*@brief 为剪切域集合计算以调整源到目标的剪切域(内部接口)
 *@param 形如scui_widget_surface_draw_xxx的接口使用
 */
bool scui_widget_surface_clip_unit_calc(scui_widget_t *widget,   scui_area_t *unit_clip,
                                        scui_area_t   *target,   scui_area_t *clip,
                                        scui_area_t   *dst_clip, scui_area_t *src_clip,
                                        bool           target_empty)
{
    *dst_clip = (scui_area_t){0};
    *src_clip = *clip;
    /* 是否有目标区域决定区域计算与偏移不一致 */
    if (target_empty) {
        /* 子剪切域要相对同步偏移 */
        scui_point_t dst_offset = {
            .x = target->x - widget->clip_set.clip.x,
            .y = target->y - widget->clip_set.clip.y,
        };
        scui_area_t dst_area = *unit_clip;
        if (!scui_area_limit_offset(&dst_area, &dst_offset))
             return false;
        if (!scui_area_inter(dst_clip, &dst_area, target))
             return false;
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit_clip->x - widget->clip_set.clip.x,
            .y = unit_clip->y - widget->clip_set.clip.y,
        };
        if (!scui_area_limit_offset(src_clip, &offset))
             return false;
    } else {
        /* 子剪切域要相对同步偏移 */
        scui_area_t  dst_area = *unit_clip;
        scui_point_t dst_offset = {
            .x = target->x - dst_area.x,
            .y = target->y - dst_area.y,
        };
        if (!scui_area_limit_offset(&dst_area, &dst_offset))
             return false;
        if (!scui_area_inter(dst_clip, &dst_area, target))
             return false;
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = dst_clip->x - target->x,
            .y = dst_clip->y - target->y,
        };
        if (!scui_area_limit_offset(src_clip, &offset))
             return false;
    }
    return true;
}

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

/*@brief 控件剪切域(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_surface_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->clip_set.clip;
}

/*@brief 控件画布在画布绘制图像
 *@param handle  控件句柄
 *@param target  控件绘制区域
 *@param surface 画布实例
 *@param clip    画布绘制区域
 *@param color   图像源色调(调色板)
 */
void scui_widget_surface_draw_pattern(scui_handle_t   handle,  scui_area_t *target,
                                      scui_surface_t *surface, scui_area_t *clip,
                                      scui_color_t    color)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t surface_clip = scui_surface_area(surface);
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    if (clip == NULL)
        clip  = &surface_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_area_blend(widget->surface, &dst_clip, surface, &src_clip, color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制字符串
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数(scui_string_args_t)
 */
void scui_widget_surface_draw_string(scui_handle_t handle, scui_area_t *target, void *args)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 当前本接口作为控件专用绘制接口: */
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t string_clip = {
        .w = widget->clip_set.clip.w,
        .h = widget->clip_set.clip.h,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, &string_clip, &dst_clip, &src_clip, target_empty))
            scui_draw_string(widget->surface, &dst_clip, args, &src_clip, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制纯色区域
 *@param handle 控件句柄
 *@param clip   绘制区域
 *@param color  源色调
 */
void scui_widget_surface_draw_color(scui_handle_t handle, scui_area_t *clip,
                                    scui_color_t  color)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t widget_clip = widget->clip_set.clip;
    
    if (clip == NULL)
        clip  = &widget_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
        /* 子剪切域要相对同步偏移 */
        scui_area_t dst_area = unit->clip;
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &dst_area, clip))
             continue;
        scui_draw_area_fill(widget->surface, &dst_clip, &pixel, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param color  图像源色调(调色板)
 */
void scui_widget_surface_draw_image(scui_handle_t handle, scui_area_t *target,
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
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_image(widget->surface, &dst_clip, image_inst, &src_clip,
                            widget->alpha, color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param scale  图形缩放比例(1024为放大系数)
 */
void scui_widget_surface_draw_image_scale(scui_handle_t handle, scui_area_t *target,
                                          scui_handle_t image,  scui_area_t *clip,
                                          scui_point_t  scale)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    if (clip == NULL)
        clip  = &image_clip;
    
#if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
#endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_image_scale(widget->surface, &dst_clip, image_inst, &src_clip,
                                  widget->alpha, scale);
    }
    
#if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
#endif
}

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param anchor 图像旋转轴心
 *@param center 图像旋转中心
 *@param angle  图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_widget_surface_draw_image_rotate(scui_handle_t handle, scui_area_t  *target,
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
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_image_rotate(widget->surface, &dst_clip, image_inst, &src_clip,
                                   widget->alpha, angle, anchor, center);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param matrix 变换矩阵
 */
void scui_widget_surface_draw_image_by_matrix(scui_handle_t  handle, scui_area_t *target,
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
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_image_blit_by_matrix(widget->surface, &dst_clip, image_inst, &src_clip,
                                           widget->alpha, matrix);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
       SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制圆环
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
void scui_widget_surface_draw_ring(scui_handle_t handle,  scui_area_t *target,
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
    
    scui_area_t widget_clip = widget->clip_set.clip;
    scui_area_t image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    
    bool target_empty = false;
    if (target == NULL) {
        target  = &widget_clip;
        target_empty = true;
    }
    
    if (clip == NULL)
        clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        
        scui_area_t dst_clip = {0};
        scui_area_t src_clip = {0};
        if (scui_widget_surface_clip_unit_calc(widget, &unit->clip,
            target, clip, &dst_clip, &src_clip, target_empty))
            scui_draw_ring(widget->surface, &dst_clip, image_inst, &src_clip,
                           angle_as, widget->alpha, angle_ae, color, image_e_inst);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制线条
 *@param handle 控件句柄
 *@param width  线条宽
 *@param pos_1  位置1
 *@param pos_2  位置2
 *@param color  源色调
 */
void scui_widget_surface_draw_line(scui_handle_t handle, scui_coord_t width,
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
