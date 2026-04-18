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
    /*随感:这部分逻辑我之前写了八分像,但始终未能解决所有缺陷
     *     后来这部分代码只有上帝看得懂了,我尝试性的用ai进行纠错
     *     豆包在看到我的错误日志信息后,帮我修订验证后,我再进行整理的结果
     */
    
    bool area_frag = false;
    scui_area_t frag_t = *frag;
    
    scui_coord_t y_min = scui_coord_max;
    scui_coord_t y_max = scui_coord_min;
    scui_coord_t x_min_a = scui_coord_max;
    scui_coord_t x_max_a = scui_coord_min;
    
    /* 上下边界,共计8次线相交 */
    scui_coord_t y_edge[2] = {frag_t.y, frag_t.y + frag_t.h - 1,};
    
    const uint8_t idx_line[4][2] = {{0,1},{1,2},{2,3},{3,0}};
    /* 如果线段与frag上边界相交,最多可能有四个焦点,求最小值和最大值 */
    /* 如果线段与frag下边界相交,最多可能有四个焦点,求最小值和最大值 */
    
    for (scui_multi_t idx_i = 0; idx_i < 4; idx_i++) {
        /* 先计算上下线: 每条边分别和frag的上下边界求交 */
        for (scui_multi_t idx_c = 0; idx_c < 2; idx_c++) {
            float x1 = face->point2[idx_line[idx_i][0]].x;
            float y1 = face->point2[idx_line[idx_i][0]].y;
            float x2 = face->point2[idx_line[idx_i][1]].x;
            float y2 = face->point2[idx_line[idx_i][1]].y;
            float yt = y_edge[idx_c];
            
            /* 判断线段是否跨越当前扫描线yt */
            if ((y1 <= yt && yt <= y2) || (y2 <= yt && yt <= y1)) {
                /* 正确的线段交点公式 */
                float x = x1 + (x2 - x1) * ((yt - y1) / (y2 - y1));
                /* 浮点转像素, 扩展半个像素防止丢失 */
                scui_coord_t px = (scui_coord_t)x;
                scui_coord_t x_min = px - 0.5f;
                scui_coord_t x_max = px + 0.5f;
                
                /* 更新最小最大 */
                x_min_a = scui_min(x_min_a, x_min);
                x_max_a = scui_max(x_max_a, x_max);
                area_frag = true;
            }
        }
        /* 记录当前顶点是否在frag内部并更新范围 */
        scui_point_t vertex = {
            .x = face->point2[idx_i].x,
            .y = face->point2[idx_i].y,
        };
        if (scui_area_point(&frag_t, &vertex)) {
            x_min_a = scui_min(x_min_a, vertex.x);
            x_max_a = scui_max(x_max_a, vertex.x);
            area_frag = true;
        }
        /* 遍历所有顶点求出整个四边形的 Y 范围 */
        if (y_min > face->point2[idx_i].y)
            y_min = face->point2[idx_i].y;
        if (y_max < face->point2[idx_i].y)
            y_max = face->point2[idx_i].y;
    }
    
    /* y方向裁剪:必须限制在frag内部 */
    scui_coord_t clip_y1 = scui_max(y_min, frag->y);
    scui_coord_t clip_y2 = scui_min(y_max, frag->y + frag->h - 1);
    
    /* 更新裁剪后的 Y 起始位置和高度 */
    frag_t.y = clip_y1;
    frag_t.h = clip_y2 - clip_y1 + 1;
    /* x方向裁剪 */
    if (x_min_a != scui_coord_max) frag_t.x = x_min_a;
    if (x_max_a != scui_coord_min) frag_t.w = x_max_a - frag_t.x + 1;
    /* 输出最终裁剪区域 */
    *clip = frag_t;
    
    /* 判断：有交集+区域在内部+区域非空 */
    bool area_inside = scui_area_inside(frag, clip);
    bool area_empty = scui_area_empty(clip);
    return area_frag && area_inside && !area_empty;
}

/*@brief 绘制目标重定向
 */
static bool scui_widget_draw_target(scui_widget_t *widget, scui_area_t **target)
{
    /* 这里挂个内部的全局量,用于给外部提供使用 */
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
static bool scui_widget_draw_clip_seg(scui_area_t *dst_clip, scui_point_t *dst_offset, scui_point_t *seg_offset)
{
    scui_area_t clip_seg = {0};
    scui_frame_buffer_seg(&clip_seg);
    /* 将段剪切域偏移追加到该目标上 */
    scui_area_t dst_clip_raw = *dst_clip;
    if (!scui_area_inter2(dst_clip, &clip_seg))
         return false;
    if (dst_offset != NULL) {
        dst_offset->x = +(dst_clip->x - dst_clip_raw.x);
        dst_offset->y = +(dst_clip->y - dst_clip_raw.y);
    }
    if (seg_offset != NULL) {
        seg_offset->x = +(clip_seg.x);
        seg_offset->y = +(clip_seg.y);
    }
    /* 在结果去除段偏移以映射到surface上 */
    dst_clip->x -= clip_seg.x;
    dst_clip->y -= clip_seg.y;
    
    SCUI_ASSERT(dst_clip->x + dst_clip->w <= 0 + clip_seg.w);
    SCUI_ASSERT(dst_clip->y + dst_clip->h <= 0 + clip_seg.h);
    return true;
}
#endif

/*@brief 控件在画布绘制字符串
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_string(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 当前本接口作为控件专用绘制接口: */
    scui_string_args_t *str_args = draw_dsc->str_args;
    SCUI_ASSERT(widget->type == scui_widget_type_string ||
                widget->type == scui_widget_type_custom);
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域要相对同步偏移 */
        scui_area_t  src_clip   = str_args->clip;
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        if (!scui_area_limit_offset(&src_clip, &src_offset))
             continue;
        /* src_clip特殊调整 */
        src_clip.x = -(str_args->clip.w - src_clip.w);
        src_clip.y = -(str_args->clip.h - src_clip.h);
        src_clip.w =  (str_args->clip.w);
        src_clip.h =  (str_args->clip.h);
        
        #if SCUI_MEM_FEAT_MINI
        scui_point_t dst_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, &dst_offset, NULL))
             continue;
        src_clip.x -= dst_offset.x;
        src_clip.y -= dst_offset.y;
        #endif
        
        /* 如果是本地布局 */
        if (str_args->local) str_args->nest++;
        scui_draw_string(false, widget->surface, dst_clip,
            src_clip, widget->alpha, str_args);
    }
}

/*@brief 控件在画布绘制纯色区域
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_color(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_area_fill(false, widget->surface, dst_clip, widget->alpha, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制渐变纯色区域
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_color_grad(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_area_grad(false, widget->surface, dst_clip,
            *target, draw_dsc->color, widget->alpha, draw_dsc->way);
    }
}

/*@brief 控件在画布绘制抖动
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_dither(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_area_dither(false, widget->surface, dst_clip);
    }
}

/*@brief 控件在画布绘制模糊
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_blur(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_area_blur(false, widget->surface, dst_clip);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    /* 绘制目标重定向 */
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
            /* scui_widget_clip_sizes(widget_r->myself, &size_old); */
            /* 上面默认使用的全局剪切域 */
            /* 所以可能存在覆盖,为所有控件补充剪切域 */
            scui_widget_clip_reset(widget_r, target, true);
            
            /* scui_widget_clip_check(widget_r->myself, true); */
            scui_widget_clip_update(widget_r);
            /* scui_widget_clip_check(widget_r->myself, true); */
            /* scui_widget_clip_sizes(widget_r->myself, &size_new); */
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
        scui_point_t dst_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, &dst_offset, NULL))
             continue;
        if (!scui_area_limit_offset(&src_clip, &dst_offset))
             continue;
        #endif
        
        scui_draw_image(false, widget->surface, dst_clip,
            image_inst, src_clip, widget->alpha, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_scale(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    /* 绘制目标重定向 */
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
    
    if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_dir_hor)) {
        src_offset.x = clip->w / 2;
        dst_offset.x = target->x + target->w / 2;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_l)) {
        dst_offset.x = target->x;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_r)) {
        src_offset.x = clip->w;
        dst_offset.x = target->x + target->w;
    }
    
    if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_ver)) {
        src_offset.y = clip->h / 2;
        dst_offset.y = target->y + target->h / 2;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_u)) {
        dst_offset.y = target->y;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_d)) {
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
        scui_point_t seg_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, &seg_offset))
             continue;
        dst_offset.x -= seg_offset.x;
        dst_offset.y -= seg_offset.y;
        #endif
        
        scui_draw_image_scale(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha, draw_dsc->scale,
            dst_offset, src_offset);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_rotate(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_handle_t image  = draw_dsc->image;
    scui_area_t  *clip   = draw_dsc->clip;
    scui_point_t  anchor = draw_dsc->anchor;
    scui_point_t  center = draw_dsc->center;
    
    /* 绘制目标重定向 */
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
        /* 子剪切域相对同步偏移 */
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        
        #if SCUI_MEM_FEAT_MINI
        scui_point_t seg_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, &seg_offset))
             continue;
        anchor.x -= seg_offset.x;
        anchor.y -= seg_offset.y;
        #endif
        
        scui_draw_image_rotate(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha, draw_dsc->angle, anchor, center);
    }
}

/*@brief 控件在画布绘制图像
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_image_matrix(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    #if SCUI_MEM_FEAT_MINI
    SCUI_ASSERT(false);
    return;
    #endif
    
    /* 绘制目标重定向 */
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
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_image_matrix_blend(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha, SCUI_COLOR_UNUSED,
            *draw_dsc->inv_matrix, *draw_dsc->matrix);
    }
}

/*@brief 控件在画布绘制二维码
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_qrcode(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    #if SCUI_MEM_FEAT_MINI
    return;
    #endif
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        scui_area_t src_clip = dst_clip;
        src_clip.x = 0;
        src_clip.y = 0;
        
        #if SCUI_MEM_FEAT_MINI
        scui_area_t dst_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, &dst_offset, NULL))
             continue;
        src_clip.x = dst_offset.x;
        src_clip.y = dst_offset.y;
        #endif
        
        scui_draw_qrcode(false, widget->surface, dst_clip,
            src_clip, widget->alpha, draw_dsc->color,
            draw_dsc->size, draw_dsc->data);
    }
}

/*@brief 控件在画布绘制条形码
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_barcode(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    #if SCUI_MEM_FEAT_MINI
    return;
    #endif
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        scui_area_t src_clip = dst_clip;
        
        #if SCUI_MEM_FEAT_MINI
        scui_area_t dst_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, &dst_offset, NULL))
             continue;
        src_clip.x = dst_offset.x; src_clip.w -= src_clip.x;
        src_clip.y = dst_offset.y; src_clip.h -= src_clip.y;
        #endif
        
        scui_draw_barcode(false, widget->surface, dst_clip,
            src_clip, widget->alpha, draw_dsc->color,
            draw_dsc->size, draw_dsc->data);
    }
}

/*@brief 控件在画布绘制圆环
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_ring(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_handle_t image   = draw_dsc->image;
    scui_area_t  *clip    = draw_dsc->clip;
    scui_coord_t  angle_s = draw_dsc->angle_s;
    scui_coord_t  angle_e = draw_dsc->angle_e;
    scui_coord_t  percent = draw_dsc->percent;
    scui_handle_t image_e = draw_dsc->image_e;
    
    #if SCUI_MEM_FEAT_MINI
    return;
    #endif
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    /* 调整(adjust): */
    scui_coord_t angle_as = angle_s;
    scui_coord_t angle_ae = scui_map(percent, 0, 100, angle_s, angle_e);
    SCUI_ASSERT(0 <= percent && percent <= 100);
    if (percent == 0 || angle_s == angle_e)
        return;
    
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
        /* .x = target->x - widget->clip.x + image_inst->pixel.width  / 2, */
        /* .y = target->y - widget->clip.y + image_inst->pixel.height / 2, */
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
        scui_point_t seg_offset = {0};
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, &seg_offset))
             continue;
        dst_center.x -= seg_offset.x;
        dst_center.y -= seg_offset.y;
        #endif
        
        scui_draw_ring(false, widget->surface, dst_clip,
            dst_center, image_e_inst, image_inst, src_clip,
            angle_as, widget->alpha, angle_ae, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制图形
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx_graph(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    #if SCUI_MEM_FEAT_MINI
    return;
    #endif
    
    /* 绘制目标重定向 */
    if (!scui_widget_draw_target(widget, &target))
         return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        /* 子剪切域相对同步偏移 */
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_MEM_FEAT_MINI
        if (!scui_widget_draw_clip_seg(&dst_clip, NULL, NULL))
             continue;
        #endif
        
        scui_draw_dsc_t *draw_dsc_graph = draw_dsc->graph;
        scui_draw_graph(false, widget->surface, dst_clip,
            draw_dsc->alpha, draw_dsc->color, draw_dsc_graph);
    }
}
