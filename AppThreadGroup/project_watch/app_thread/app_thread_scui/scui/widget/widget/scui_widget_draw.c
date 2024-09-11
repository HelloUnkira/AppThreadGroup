/*实现目标:
 *    控件(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制目标重定向
 */
static bool scui_widget_draw_target(scui_widget_t *widget, scui_area_t **target)
{
    // 这里挂个内部的全局量,用于给外部提供使用
    static scui_area_t widget_clip = {0};
    
    if (scui_area_empty(&widget->clip_set.clip))
        return false;
    widget_clip = widget->clip;
    if (*target != NULL)
    if (!scui_area_inter2(&widget_clip, *target))
         return false;
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget_clip.x -= widget->clip.x;
        widget_clip.y -= widget->clip.y;
    }
    *target = &widget_clip;
    return true;
}

/*@brief 绘制滴答检查
 */
static void scui_widget_draw_tick(bool check)
{
    #if SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK
    
    if (check)
        scui_tick_elapse_us(true);
    else {
        uint64_t tick_us = scui_tick_elapse_us(false);
        if (tick_us > SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER)
            SCUI_LOG_WARN("expend:%u.%u", tick_us / 1000, tick_us % 1000);
    }
    
    #endif
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
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    /* 当前本接口作为控件专用绘制接口: */
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域要相对同步偏移 */
        scui_area_t string_clip = {
            .w = widget->clip.w,
            .h = widget->clip.h,
        };
        scui_area_t  src_clip   = string_clip;
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        if (!scui_area_limit_offset(&src_clip, &src_offset))
             continue;
        // src_clip使用原始偏移坐标,非调整后的偏移坐标
        src_clip.x = -(string_clip.w - src_clip.w);
        src_clip.y = -(string_clip.h - src_clip.h);
        src_clip.w =  (string_clip.w);
        src_clip.h =  (string_clip.h);
        
        scui_widget_draw_tick(true);
        scui_draw_string(widget->surface, &dst_clip, args, &src_clip, widget->alpha);
        scui_widget_draw_tick(false);
    }
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
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &clip))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, clip))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_area_fill(widget->surface, &dst_clip, color, widget->alpha);
        scui_widget_draw_tick(false);
    }
}

/*@brief 控件在画布绘制模糊
 *@param handle 控件句柄
 *@param clip   绘制区域
 */
void scui_widget_draw_blur(scui_handle_t handle, scui_area_t *clip)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &clip))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, clip))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_area_blur(widget->surface, &dst_clip);
        scui_widget_draw_tick(false);
    }
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
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(  image_inst != NULL);
    scui_area_t   image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    if (clip != NULL)
    if (!scui_area_inter2(&image_clip, clip))
         return;
    clip = &image_clip;
    /* step:image<e> */
    
    #if SCUI_WIDGET_ROOT_IMAGE_DIRECT
    /* 这里有一个优化点(主要用于根控件背景绘制) */
    /* 条件达到时,将资源直接加载到绘制画布上即可 */
    if (widget->myself == SCUI_HANDLE_INVALID &&
        scui_area_equal(&widget->clip_set.clip, clip)) {
        scui_area_t clip_widget = scui_surface_area(widget->surface);
    if (scui_area_equal(&widget->clip_set.clip, &clip_widget)) {
        #if 1
        scui_widget_draw_tick(true);
        scui_image_src_read(image_inst, widget->surface->pixel);
        scui_widget_draw_tick(false);
        /* 上面默认使用的全局剪切域 */
        /* 所以可能存在覆盖,为所有控件补充剪切域 */
        scui_widget_clip_reset(widget, &widget->clip_set.clip, true);
        scui_widget_clip_clear(widget, false);
        scui_widget_clip_update(widget);
        return;
        #endif
    }
    }
    #endif
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域相对同步偏移 */
        scui_area_t src_clip = *clip;
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        if (!scui_area_limit_offset(&src_clip, &src_offset))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_image(widget->surface, &dst_clip, image_inst, &src_clip,
                        widget->alpha, color);
        scui_widget_draw_tick(false);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param scale  图形缩放比例(1024为放大系数)
 *@param pos    缩放锚点
 */
void scui_widget_draw_image_scale(scui_handle_t handle, scui_area_t   *target,
                                  scui_handle_t image,  scui_area_t   *clip,
                                  scui_point_t  scale,  scui_opt_pos_t pos)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(  image_inst != NULL);
    scui_area_t   image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    if (clip != NULL)
    if (!scui_area_inter2(&image_clip, clip))
         return;
    clip = &image_clip;
    /* step:image<e> */
    
    /* step:align<s> */
    scui_point_t src_offset = {0};
    scui_point_t dst_offset = {0};
    
    if ((pos & scui_opt_pos_hor) != 0) {
        src_offset.x = clip->w / 2;
        dst_offset.x = target->x + target->w / 2;
    } else if ((pos & scui_opt_pos_l) != 0) {
        dst_offset.x = target->x;
    } else if ((pos & scui_opt_pos_r) != 0) {
        src_offset.x = clip->w;
        dst_offset.x = target->x + target->w;
    }
    
    if ((pos & scui_opt_pos_ver) != 0) {
        src_offset.y = clip->h / 2;
        dst_offset.y = target->y + target->h / 2;
    } else if ((pos & scui_opt_pos_u) != 0) {
        dst_offset.y = target->y;
    } else if ((pos & scui_opt_pos_d) != 0) {
        src_offset.y = clip->h;
        dst_offset.y = target->y + target->h;
    }
    /* step:align<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_widget_draw_tick(false);
        scui_draw_image_scale(widget->surface, &dst_clip, image_inst, clip,
                              widget->alpha, scale, dst_offset, src_offset);
        scui_widget_draw_tick(true);
    }
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
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(  image_inst != NULL);
    scui_area_t   image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    if (clip != NULL)
    if (!scui_area_inter2(&image_clip, clip))
         return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_image_rotate(widget->surface, &dst_clip, image_inst, clip,
                               widget->alpha, angle, anchor, center);
        scui_widget_draw_tick(false);
    }
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
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(  image_inst != NULL);
    scui_area_t   image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    if (clip != NULL)
    if (!scui_area_inter2(&image_clip, clip))
         return;
    clip = &image_clip;
    /* step:image<e> */
    
    // 给进来的就是逆矩阵, 这个接口现在不完善, 待定中
    scui_matrix_t reb_matrix = *matrix;
    scui_matrix_inverse(&reb_matrix);
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_image_blit_by_matrix(widget->surface, &dst_clip, image_inst, clip,
                                       widget->alpha, matrix, &reb_matrix);
        scui_widget_draw_tick(false);
    }
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
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
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
    
    scui_image_t *image_e_inst = scui_handle_get(image_e);
    SCUI_ASSERT(  image_e_inst != NULL);
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(  image_inst != NULL);
    scui_area_t   image_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    if (clip != NULL)
    if (!scui_area_inter2(&image_clip, clip))
         return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_point_t dst_center = {
        // .x = target->x - widget->clip.x + image_inst->pixel.width  / 2,
        // .y = target->y - widget->clip.y + image_inst->pixel.height / 2,
        .x = target->x + image_inst->pixel.width  / 2,
        .y = target->y + image_inst->pixel.height / 2,
    };
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域相对同步偏移 */
        scui_area_t src_clip = *clip;
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        if (!scui_area_limit_offset(&src_clip, &src_offset))
             continue;
        
        scui_widget_draw_tick(true);
        scui_draw_ring(widget->surface, &dst_clip, &dst_center,
                       image_e_inst, image_inst, &src_clip,
                       angle_as, widget->alpha, angle_ae, color);
        scui_widget_draw_tick(false);
    }
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
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_widget_draw_tick(true);
        scui_draw_line(widget->surface, &unit->clip, color,
                       width, pos_1, pos_2, widget->alpha);
        scui_widget_draw_tick(false);
    }
}
