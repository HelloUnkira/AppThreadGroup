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

/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (scui_area_empty(&widget->clip_set.clip))
        return true;
    if (scui_clip_empty(&widget->clip_set))
        return true;
    
    return false;
}

/*@brief 控件绘制上下文
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx(scui_widget_draw_dsc_t *draw_dsc)
{
    void scui_widget_draw_ctx_string(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_color(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_color_grad(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_blur(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_image(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_image_scale(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_image_rotate(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_image_matrix(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_ring(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_graph(scui_widget_draw_dsc_t *draw_dsc);
    
    switch (draw_dsc->type) {
    case scui_widget_draw_type_string:
        scui_widget_draw_ctx_string(draw_dsc);
        break;
    case scui_widget_draw_type_color:
        scui_widget_draw_ctx_color(draw_dsc);
        break;
    case scui_widget_draw_type_color_grad:
        scui_widget_draw_ctx_color_grad(draw_dsc);
        break;
    case scui_widget_draw_type_blur:
        scui_widget_draw_ctx_blur(draw_dsc);
        break;
    case scui_widget_draw_type_image:
        scui_widget_draw_ctx_image(draw_dsc);
        break;
    case scui_widget_draw_type_image_scale:
        scui_widget_draw_ctx_image_scale(draw_dsc);
        break;
    case scui_widget_draw_type_image_rotate:
        scui_widget_draw_ctx_image_rotate(draw_dsc);
        break;
    case scui_widget_draw_type_image_matrix:
        scui_widget_draw_ctx_image_matrix(draw_dsc);
        break;
    case scui_widget_draw_type_ring:
        scui_widget_draw_ctx_ring(draw_dsc);
        break;
    case scui_widget_draw_type_graph:
        scui_widget_draw_ctx_graph(draw_dsc);
        break;
    default:
        SCUI_LOG_ERROR("unknown type :%d", draw_dsc->type);
        SCUI_ASSERT(false);
        break;
    }
}

/*@brief 控件在画布绘制字符串
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_string(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_string_args_t *args = draw_dsc->string.args;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 当前本接口作为控件专用绘制接口: */
    SCUI_ASSERT(widget->type == scui_widget_type_string ||
                widget->type == scui_widget_type_custom);
    
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
        scui_area_t  src_clip   = args->clip;
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        if (!scui_area_limit_offset(&src_clip, &src_offset))
             continue;
        // src_clip使用原始偏移坐标,非调整后的偏移坐标
        src_clip.x = -(args->clip.w - src_clip.w);
        src_clip.y = -(args->clip.h - src_clip.h);
        src_clip.w =  (args->clip.w);
        src_clip.h =  (args->clip.h);
        
        scui_draw_string(widget->surface, &dst_clip,
            args, &src_clip, widget->alpha);
    }
}

/*@brief 控件在画布绘制纯色区域
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_color(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_color_t  color  = draw_dsc->color.color;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_draw_area_fill(widget->surface, &dst_clip, widget->alpha, color);
    }
}

/*@brief 控件在画布绘制渐变纯色区域
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_color_grad(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_color_t  color  = draw_dsc->color_grad.color;
    scui_coord_t  way    = draw_dsc->color_grad.way;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_draw_area_fill_grad(widget->surface, &dst_clip,
            color, target, widget->alpha, way);
    }
}

/*@brief 控件在画布绘制模糊
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_blur(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_area_t  *clip   = draw_dsc->blur.clip;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &clip))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, clip))
             continue;
        
        scui_draw_area_blur(widget->surface, &dst_clip);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_handle_t image  = draw_dsc->image.image;
    scui_area_t  *clip   = draw_dsc->image.clip;
    scui_color_t  color  = draw_dsc->image.color;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_source_check(image);
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
        scui_image_src_read(image_inst, widget->surface->pixel);
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
        
        scui_draw_image(widget->surface, &dst_clip,
            image_inst, &src_clip, widget->alpha, color);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_scale(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_handle_t image  = draw_dsc->image_scale.image;
    scui_area_t   *clip  = draw_dsc->image_scale.clip;
    scui_point_t  scale  = draw_dsc->image_scale.scale;
    scui_opt_pos_t pos   = draw_dsc->image_scale.pos;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_source_check(image);
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
        
        scui_draw_image_scale(widget->surface, &dst_clip,
            image_inst, clip, widget->alpha, scale,
            dst_offset, src_offset);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_rotate(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_handle_t image  = draw_dsc->image_rotate.image;
    scui_area_t  *clip   = draw_dsc->image_rotate.clip;
    scui_point_t *anchor = draw_dsc->image_rotate.anchor;
    scui_point_t *center = draw_dsc->image_rotate.center;
    scui_coord_t  angle  = draw_dsc->image_rotate.angle;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_source_check(image);
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
        
        scui_draw_image_rotate(widget->surface, &dst_clip,
            image_inst, clip, widget->alpha, angle, anchor, center);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_matrix(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle  = draw_dsc->handle;
    scui_area_t  *target  = draw_dsc->target;
    scui_handle_t  image  = draw_dsc->image_matrix.image;
    scui_area_t   *clip   = draw_dsc->image_matrix.clip;
    scui_matrix_t *matrix = draw_dsc->image_matrix.matrix;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_source_check(image);
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
        
        scui_draw_image_matrix_blend(widget->surface, &dst_clip,
            image_inst, clip, widget->alpha, SCUI_COLOR_UNUSED,
            matrix, &reb_matrix);
    }
}

/*@brief 控件在画布绘制圆环
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_ring(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle  = draw_dsc->handle;
    scui_area_t  *target  = draw_dsc->target;
    scui_handle_t image   = draw_dsc->ring.image;
    scui_area_t  *clip    = draw_dsc->ring.clip;
    scui_color_t  color   = draw_dsc->ring.color;
    scui_coord_t  angle_s = draw_dsc->ring.angle_s;
    scui_coord_t  angle_e = draw_dsc->ring.angle_e;
    scui_coord_t  percent = draw_dsc->ring.percent;
    scui_handle_t image_e = draw_dsc->ring.image_e;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
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
    
    scui_image_t *image_e_inst = NULL;
    if (image_e != SCUI_HANDLE_INVALID)
        image_e_inst = scui_handle_source_check(image_e);
    
    /* step:image<s> */
    scui_image_t *image_inst = scui_handle_source_check(image);
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
        
        scui_draw_ring(widget->surface, &dst_clip,
            &dst_center, image_e_inst, image_inst, &src_clip,
            angle_as, widget->alpha, angle_ae, color);
    }
}

/*@brief 控件在画布绘制图形
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_graph(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_draw_graph_dsc_t *graph_dsc = draw_dsc->graph.graph_dsc;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    // 绘制目标重定向
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        graph_dsc->dst_surface = widget->surface;
        graph_dsc->dst_clip    = &dst_clip;
        graph_dsc->src_alpha   = widget->alpha;
        scui_draw_graph_ctx(graph_dsc);
    }
}
