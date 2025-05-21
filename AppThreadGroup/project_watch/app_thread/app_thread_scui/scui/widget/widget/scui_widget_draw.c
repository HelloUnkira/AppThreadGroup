/*实现目标:
 *    控件(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

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

/*@breaf 计算绘制片段的最小区域
 *@param clip 最小绘制区域
 *@param frag 绘制片段区域
 *@param face 图形形状
 */
bool scui_widget_draw_frag(scui_area_t *clip, scui_area_t *frag, scui_face2_t *face)
{
    bool area_frag = false;
    scui_area_t frag_t = *frag;
    
    scui_coord_t y_min = scui_coord_max;
    scui_coord_t y_max = scui_coord_min;
    scui_coord_t x_min_a = scui_coord_max;
    scui_coord_t x_max_a = scui_coord_min;
    scui_coord_t y_min_idx = -1;
    scui_coord_t y_max_idx = -1;
    /* 上下边界,共计8次线相交 */
    scui_coord_t y_edge[2] = {frag_t.y, frag_t.y + frag_t.h - 1,};
    
    const uint8_t idx_line[4][2] = {{0,1},{1,2},{2,3},{3,1}};
    /* 如果线段与frag上边界相交,最多可能有四个焦点,求最小值和最大值 */
    /* 如果线段与frag下边界相交,最多可能有四个焦点,求最小值和最大值 */
    for (scui_handle_t idx_i = 0; idx_i < 4; idx_i++) {
        
        /* 先计算上下线 */
        for (scui_handle_t idx_c = 0; idx_c < 2; idx_c++) {
            
            scui_coord_t x_min = 0;
            scui_coord_t x_max = 0;
            float x1 = face->point2[idx_line[idx_i][0]].x;
            float y1 = face->point2[idx_line[idx_i][0]].y;
            float x2 = face->point2[idx_line[idx_i][1]].x;
            float y2 = face->point2[idx_line[idx_i][1]].y;
            /* 三角形相似性原理(浮点计算,所以有俩个值): */
            bool inter = false;
            if ((y1 <= y_edge[idx_c] && y_edge[idx_c] <= y2) ||
                (y2 <= y_edge[idx_c] && y_edge[idx_c] <= y1)) {
                float dst_x = scui_dist(x1, x2);
                float dst_y = scui_dist(y1, y2);
                float dst_l = scui_dist(scui_min(y1, y2), y_edge[idx_c]);
                float x = dst_x * dst_l / dst_y;
                x_min = x - 0.5;
                x_max = x + 0.5;
                inter = true;
            }
            if (inter) {
                x_min_a = scui_min(x_min_a, x_min);
                x_max_a = scui_max(x_max_a, x_max);
                area_frag = true;
            }
        }
        /* 任意点 */
        scui_point_t vertex = {
            .x = face->point2[idx_i].x,
            .y = face->point2[idx_i].y,
        };
        if (scui_area_point(&frag_t, &vertex)) {
            x_min_a = scui_min(x_min_a, vertex.x);
            x_max_a = scui_max(x_max_a, vertex.x);
            area_frag = true;
        }
        /* y上下边界 */
        if (y_min > face->point2[idx_i].y) {
            y_min = face->point2[idx_i].y;
            y_min_idx = idx_i;
        }
        if (y_max < face->point2[idx_i].y) {
            y_max = face->point2[idx_i].y;
            y_max_idx = idx_i;
        }
    }
    /* y上下边界点 */
    scui_point_t vertex_max = {
        .x = face->point2[y_max_idx].x,
        .y = face->point2[y_max_idx].y,
    };
    if (scui_area_point(&frag_t, &vertex_max)) {
        x_min_a = scui_min(x_min_a, face->point2[y_max_idx].x);
        x_max_a = scui_max(x_max_a, face->point2[y_max_idx].x);
        frag_t.h = face->point2[y_max_idx].y - frag_t.y + 1;
        area_frag = true;
    }
    /* y上下边界点 */
    scui_point_t vertex_min = {
        .x = face->point2[y_min_idx].x,
        .y = face->point2[y_min_idx].y,
    };
    if (scui_area_point(&frag_t, &vertex_min)) {
        x_min_a = scui_min(x_min_a, face->point2[y_min_idx].x);
        x_max_a = scui_max(x_max_a, face->point2[y_min_idx].x);
        frag_t.y = face->point2[y_min_idx].y;
        area_frag = true;
    }
    /* 区域交集检查 */
    if (frag->x < x_max_a && x_max_a < frag->x + frag->w - 1)
        frag_t.w = x_max_a - frag_t.x + 1;
    if (frag->x < x_min_a && x_min_a < frag->x + frag->w - 1)
        frag_t.x = x_min_a;
    
    *clip = frag_t;
    
    bool area_inside = scui_area_inside(frag, clip);
    bool area_empty = scui_area_empty(clip);
    return area_frag && area_inside && !area_empty;
}

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

/*@brief 绘制目标重定向
 */
#if SCUI_MEM_FEAT_MINI
static bool scui_widget_draw_target_seg(scui_area_t *target)
{
    /* 将段剪切域偏移追加到该目标上 */
    scui_area_t clip_seg = {0};
    scui_frame_buffer_seg(&clip_seg);
    if (!scui_area_inter2(target, &clip_seg))
         return false;
    /* 在结果去除段偏移以映射到surface上 */
    target->x -= clip_seg.x;
    target->y -= clip_seg.y;
}
#endif

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
    void scui_widget_draw_ctx_qrcode(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_barcode(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_ring(scui_widget_draw_dsc_t *draw_dsc);
    void scui_widget_draw_ctx_graph(scui_widget_draw_dsc_t *draw_dsc);
    
    void (*ctx_cb)(scui_widget_draw_dsc_t *draw_dsc) = NULL;
    static const struct {uint32_t type; void *exec_ctx;} ctx_table[] = {
        {scui_widget_draw_type_string,          (void *)scui_widget_draw_ctx_string,},
        {scui_widget_draw_type_color,           (void *)scui_widget_draw_ctx_color,},
        {scui_widget_draw_type_color_grad,      (void *)scui_widget_draw_ctx_color_grad,},
        {scui_widget_draw_type_blur,            (void *)scui_widget_draw_ctx_blur,},
        {scui_widget_draw_type_image,           (void *)scui_widget_draw_ctx_image,},
        {scui_widget_draw_type_image_scale,     (void *)scui_widget_draw_ctx_image_scale,},
        {scui_widget_draw_type_image_rotate,    (void *)scui_widget_draw_ctx_image_rotate,},
        {scui_widget_draw_type_image_matrix,    (void *)scui_widget_draw_ctx_image_matrix,},
        {scui_widget_draw_type_ring,            (void *)scui_widget_draw_ctx_ring,},
        {scui_widget_draw_type_qrcode,          (void *)scui_widget_draw_ctx_qrcode,},
        {scui_widget_draw_type_barcode,         (void *)scui_widget_draw_ctx_barcode,},
        {scui_widget_draw_type_graph,           (void *)scui_widget_draw_ctx_graph,},
    };
    
    for (scui_handle_t idx = 0; idx < scui_arr_len(ctx_table); idx++)
        if (ctx_table[idx].type == draw_dsc->type) {
            ctx_cb = (void (*)(void *))ctx_table[idx].exec_ctx;
            ctx_cb(draw_dsc);
            return;
        }
    
    SCUI_LOG_ERROR("unknown type :%d", draw_dsc->type);
    SCUI_ASSERT(false);
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
    scui_area_t   image_clip = scui_image_area(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip))
        return;
    clip = &image_clip;
    /* step:image<e> */
    
    /* 这里有一个优化点(主要用于覆盖的背景绘制) */
    /* 条件达到时,将资源直接加载到绘制画布上即可 */
    #if SCUI_WIDGET_IMAGE_DIRECT
    /* 图片的大小有个阈值,且类型要匹配 */
    if (image_inst->type != scui_image_type_mem &&
       !scui_pixel_have_alpha(image_inst->format) &&
        image_inst->format == widget->surface->format)
    if (SCUI_WIDGET_IMAGE_DIRECT_LIMIT <= scui_image_size(image_inst)) {
        scui_area_t clip_surface = scui_surface_area(widget->surface);
        scui_area_t clip_image   = scui_image_area(image);
        /* 宽度要保持一致,因为需要连续的线性内存空间 */
        if (target->w == clip_surface.w && scui_area_equal(&clip_image, clip) &&
            target->w == clip_image.w && scui_area_inside(target, &clip_image)) {
            /* 折算y偏移量, 到达图片的对齐点 */
            uint32_t byte = scui_pixel_bits(widget->surface->format) / 8;
            uint32_t line = target->y * target->w * byte;
            uint8_t *data = widget->surface->pixel + line;
            #if 1
            SCUI_LOG_INFO("image direct");
            scui_image_src_read(image_inst, data);
            /* 注意:这里必须要补充的是整个控件树的完整剪切域 */
            scui_handle_t  handle_r = scui_widget_root(widget->myself);
            scui_widget_t *widget_r = scui_handle_source_check(handle_r);
            
            scui_multi_t size_old = 0, size_new = 0;
            // scui_widget_clip_sizes(widget_r->myself, &size_old);
            /* 上面默认使用的全局剪切域 */
            /* 所以可能存在覆盖,为所有控件补充剪切域 */
            scui_widget_clip_reset(widget_r, target, true);
            
            // scui_widget_clip_check(widget_r->myself, true);
            scui_widget_clip_update(widget_r);
            // scui_widget_clip_check(widget_r->myself, true);
            // scui_widget_clip_sizes(widget_r->myself, &size_new);
            SCUI_LOG_DEBUG("size_ofs:%d", size_new - size_old);
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
    scui_area_t   image_clip = scui_image_area(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip))
        return;
    clip = &image_clip;
    /* step:image<e> */
    
    /* step:align<s> */
    scui_point_t src_offset = {0};
    scui_point_t dst_offset = {0};
    
    if (scui_opt_bits_equal(pos, scui_opt_dir_hor)) {
        src_offset.x = clip->w / 2;
        dst_offset.x = target->x + target->w / 2;
    } else if (scui_opt_bits_equal(pos, scui_opt_pos_l)) {
        dst_offset.x = target->x;
    } else if (scui_opt_bits_equal(pos, scui_opt_pos_r)) {
        src_offset.x = clip->w;
        dst_offset.x = target->x + target->w;
    }
    
    if (scui_opt_bits_equal(pos, scui_opt_pos_ver)) {
        src_offset.y = clip->h / 2;
        dst_offset.y = target->y + target->h / 2;
    } else if (scui_opt_bits_equal(pos, scui_opt_pos_u)) {
        dst_offset.y = target->y;
    } else if (scui_opt_bits_equal(pos, scui_opt_pos_d)) {
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
    scui_area_t   image_clip = scui_image_area(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip))
        return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
    scui_area_t   image_clip = scui_image_area(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip))
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
        scui_draw_image_matrix_blend(widget->surface, &dst_clip,
            image_inst, clip, widget->alpha, SCUI_COLOR_UNUSED,
            matrix, &reb_matrix);
    }
}

/*@brief 控件在画布绘制二维码
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_qrcode(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_color_t  color  = draw_dsc->qrcode.color;
    scui_multi_t  size   = draw_dsc->qrcode.size;
    uint8_t      *data   = draw_dsc->qrcode.data;
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
        scui_draw_qrcode(widget->surface, &dst_clip,
            widget->alpha, color, size, data);
    }
}

/*@brief 控件在画布绘制条形码
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_barcode(scui_widget_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t handle = draw_dsc->handle;
    scui_area_t  *target = draw_dsc->target;
    scui_color_t  color  = draw_dsc->qrcode.color;
    scui_multi_t  size   = draw_dsc->qrcode.size;
    uint8_t      *data   = draw_dsc->qrcode.data;
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
        scui_draw_barcode(widget->surface, &dst_clip,
            widget->alpha, color, size, data);
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
    scui_area_t   image_clip = scui_image_area(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip))
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
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
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_target_seg(&dst_clip))
             continue;
        #endif
        
        graph_dsc->dst_surface = widget->surface;
        graph_dsc->dst_clip    = &dst_clip;
        graph_dsc->src_alpha   = widget->alpha;
        scui_draw_graph_ctx(graph_dsc);
    }
}
