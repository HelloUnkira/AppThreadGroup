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

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

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

/*@brief 控件画布在画布绘制图像
 *@param widget      控件实例
 *@param dst_clip    控件剪切域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 *@param src_color   图像源色调(调色板使用)
 */
void scui_widget_surface_draw_pattern(scui_widget_t   *widget,      scui_area_t *dst_clip,
                                      scui_surface_t  *src_surface, scui_area_t *src_clip,
                                      scui_color_mix_t src_color)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t pattern_clip = scui_surface_area(src_surface);
    
    if (src_clip == NULL)
        src_clip  = &pattern_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        /* 子剪切域要相对同步偏移 */
        scui_point_t dst_offset = {
            .x = dst_clip->x - widget->clip_set.clip.x,
            .y = dst_clip->y - widget->clip_set.clip.y,
        };
        scui_area_t dst_clip = unit->clip;
        if (!scui_area_limit_offset(&dst_clip, &dst_offset))
             continue;
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
        scui_draw_area_blend(widget->surface, &dst_clip, src_surface, &src_area, src_color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制字符串
 *@param widget   控件实例
 *@param src_args 字符串绘制参数(scui_string_args_t)
 */
void scui_widget_surface_draw_string(scui_widget_t *widget, void *src_args)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    /* 当前本接口作为控件专用绘制接口: */
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t string_clip = {
        .w = widget->clip_set.clip.w,
        .h = widget->clip_set.clip.h,
    };
    scui_area_t *src_clip = &string_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
    scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
        scui_draw_string(widget->surface, &unit->clip, src_args, &src_area, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制纯色区域
 *@param widget   控件实例
 *@param src_clip 绘制区域
 *@param color    源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget,
                                    scui_area_t *src_clip, scui_color_mix_t color)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_area_t color_clip = widget->clip_set.clip;
    
    if (src_clip == NULL)
        src_clip  = &color_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
        scui_draw_area_fill(widget->surface, &src_area, &pixel, widget->alpha);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_surface_draw_image(scui_widget_t *widget, scui_handle_t handle,
                                    scui_area_t *src_clip, scui_color_mix_t color)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
    
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
        scui_draw_image(widget->surface, &unit->clip, image, &src_area,
                        widget->alpha, color);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param angle    图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 *@param anchor   图像旋转轴心
 *@param center   图像旋转中心
 */
void scui_widget_surface_draw_image_rotate(scui_widget_t *widget, scui_handle_t handle,
                                           scui_area_t *src_clip, scui_coord_t  angle,
                                           scui_point_t  *anchor, scui_point_t *center)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
    
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
        scui_draw_image_rotate(widget->surface, &unit->clip, image, &src_area,
                               widget->alpha, angle, anchor, center);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
        SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param matrix   变换矩阵
 */
void scui_widget_surface_draw_image_by_matrix(scui_widget_t *widget, scui_handle_t  handle,
                                              scui_area_t *src_clip, scui_matrix_t *matrix)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
    
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    scui_tick_elapse_us(true);
    #endif
    
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
       scui_clip_unit_t *unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        /* 子剪切域要相对同步偏移 */
        scui_point_t offset = {
            .x = unit->clip.x - widget->clip_set.clip.x,
            .y = unit->clip.y - widget->clip_set.clip.y,
        };
        scui_area_t src_area = *src_clip;
        if (!scui_area_limit_offset(&src_area, &offset))
             continue;
       scui_draw_image_blit_by_matrix(widget->surface, &unit->clip, image, &src_area,
                                      widget->alpha, matrix);
    }
    
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    uint64_t tick_us = scui_tick_elapse_us(false);
    if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
       SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    #endif
}
