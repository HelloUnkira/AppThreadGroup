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
    
    return false;
}

/*@brief 计算绘制片段的最小区域
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

/*@brief 重定向画布及其剪切域
 *@param handle 控件句柄
 *@param redirect 重定向
 *@retval 成功失败
 */
bool scui_widget_draw_buffer(scui_handle_t handle, bool redirect)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->parent == SCUI_HANDLE_INVALID) return false;
    SCUI_ASSERT(widget->surface_s != NULL);
    
    static scui_surface_t *surface = NULL;
    static scui_clip_set_t  clip_set  = {0};
    
    if (redirect) {
        SCUI_ASSERT(surface == NULL);
        surface = widget->surface;
        clip_set  = widget->clip_set;
        
        /* 映射到父控件画布 */
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        widget->surface  =  widget_p->surface;
        widget->clip_set = *widget->clip_set_p;
        return true;
    } else {
        SCUI_ASSERT(surface == widget->surface_s);
        scui_clip_clear(widget->clip_set_p);
        
        widget->surface  = surface;
        widget->clip_set = clip_set;
        surface = NULL;
        return true;
    }
    
    return false;
}

/*@brief 绘制目标重定向
 *@param widget   控件实例
 *@param target   控件剪切域
 *@retval 绘制目标有效
 */
static bool scui_widget_draw_target(scui_widget_t *widget, scui_area_t **target)
{
    /* 这里挂个内部的全局量,用于给外部提供使用 */
    static scui_area_t widget_clip = {0};
    widget_clip = widget->clip;
    
    if (*target != NULL) {
        widget_clip = **target;
        /* 相对剪切域先要检查, 再进行转换剪切域 */
        scui_area_t clip_s = scui_widget_clip_self(widget->myself);
        if (!scui_area_inter2(&widget_clip, &clip_s))
             return false;
        
        widget_clip.x += widget->clip.x;
        widget_clip.y += widget->clip.y;
        if (!scui_area_inter2(&widget_clip, &widget->clip))
             return false;
    }
    
    /* 画布的坐标区域是相对独立画布控件 */
    if (widget->surface_s != NULL &&
        widget->surface == widget->surface_s) {
        widget_clip.x -= widget->clip.x;
        widget_clip.y -= widget->clip.y;
    }
    
    *target = &widget_clip;
    return true;
}

/*@brief 控件在画布绘制字符串
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_string(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    /* 当前本接口作为控件专用绘制接口: */
    scui_string_args_t *str_args = draw_dsc->str_args;
    SCUI_ASSERT(widget->type == scui_widget_type_string ||
                widget->type == scui_widget_type_custom);
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
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
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t dst_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, &dst_offset, NULL, NULL)) continue;
        
        src_clip.x -= dst_offset.x;
        src_clip.y -= dst_offset.y;
        #endif
        
        scui_draw_string(false, widget->surface, dst_clip,
            src_clip, widget->alpha, str_args);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_symbol(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_area_t  *clip = draw_dsc->clip;
    scui_handle_t name = draw_dsc->font_name;
    uint32_t    symbol = draw_dsc->symbol;
    
    /* step:symbol<s> */
    scui_area_t symbol_clip = scui_symbol_area(name, symbol);
    if (clip != NULL && !scui_area_inter2(&symbol_clip, clip))
        return;
    clip = &symbol_clip;
    /* step:symbol<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
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
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t dst_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, &dst_offset, &src_clip, NULL)) continue;
        #endif
        
        scui_draw_symbol(false, widget->surface, dst_clip,
            name, symbol, src_clip, widget->alpha, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制纯色区域
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_color(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, NULL)) continue;
        #endif
        
        scui_draw_area_fill(false, widget->surface, dst_clip, widget->alpha, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制渐变纯色区域
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_color_grad(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, NULL)) continue;
        #endif
        
        scui_draw_area_grad(false, widget->surface, dst_clip,
            *target, draw_dsc->color, widget->alpha, draw_dsc->way);
    }
}

/*@brief 控件在画布绘制抖动
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_dither(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, NULL)) continue;
        #endif
        
        scui_draw_area_dither(false, widget->surface, dst_clip);
    }
}

/*@brief 控件在画布绘制模糊
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_blur(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, NULL)) continue;
        #endif
        
        scui_draw_area_blur(false, widget->surface, dst_clip);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_image(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    /* step:image<s> */
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    /* 这里有一个优化点(主要用于覆盖的背景绘制) */
    /* 条件达到时,将资源直接加载到绘制画布上即可 */
    #if SCUI_WIDGET_IMAGE_DIRECT
    /* 图片的大小有个阈值,且类型要匹配 */
    if (image_inst->type != scui_image_type_mem &&
       !scui_pixel_alpha_in(image_inst->format) &&
        image_inst->format == widget->surface->format)
    if (SCUI_WIDGET_IMAGE_DIRECT_LIMIT <= scui_image_size(image_inst)) {
        scui_area_t clip_surface = scui_surface_area(widget->surface);
        scui_area_t clip_image   = scui_image_area(image);
        /* 宽度要保持一致,因为需要连续的线性内存空间 */
        if (target->w == clip_surface.w && scui_area_equal(&clip_image, clip) &&
            target->w == clip_image.w && scui_area_inside(target, &clip_image)) {
            /* 折算y偏移量, 到达图片的对齐点 */
            uint32_t byte = scui_pixel_byte(widget->surface->format);
            uint32_t line = target->y * target->w * byte;
            uint8_t *data = widget->surface->pixel + line;
            #if 1
            SCUI_LOG_INFO("image direct");
            scui_image_src_read(image_inst, data);
            /* 注意:这里必须要补充的是整个控件树的完整剪切域 */
            scui_handle_t  handle_t = scui_widget_tree(widget->myself);
            scui_widget_t *widget_t = scui_handle_source_check(handle_t);
            
            scui_multi_t size_old = 0, size_new = 0;
            /* scui_widget_clip_sizes(widget_t->myself, &size_old); */
            /* 上面默认使用的全局剪切域 */
            /* 所以可能存在覆盖,为所有控件补充剪切域 */
            scui_widget_clip_reset(widget_t, NULL, true);
            
            /* scui_widget_clip_check(widget_r->myself, true); */
            scui_widget_clip_update(widget_t);
            /* scui_widget_clip_check(widget_t->myself, true); */
            /* scui_widget_clip_sizes(widget_t->myself, &size_new); */
            SCUI_LOG_DEBUG("size_ofs:%d", size_new - size_old);
            return;
            #endif
        }
    }
    #endif
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
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
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t dst_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, &src_clip, &dst_offset, NULL)) continue;
        #endif
        
        scui_draw_image(false, widget->surface, dst_clip,
            image_inst, src_clip, widget->alpha, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_image_scale(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    /* step:image<s> */
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    /* step:align<s> */
    scui_point_t src_center = {0};
    scui_point_t dst_anchor = {0};
    
    if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_dir_hor)) {
        src_center.x = clip->w / 2;
        dst_anchor.x = target->x + target->w / 2;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_l)) {
        dst_anchor.x = target->x;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_r)) {
        src_center.x = clip->w;
        dst_anchor.x = target->x + target->w;
    }
    
    if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_ver)) {
        src_center.y = clip->h / 2;
        dst_anchor.y = target->y + target->h / 2;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_u)) {
        dst_anchor.y = target->y;
    } else if (scui_opt_bits_equal(draw_dsc->pos, scui_opt_pos_d)) {
        src_center.y = clip->h;
        dst_anchor.y = target->y + target->h;
    }
    /* step:align<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_anchor.x -= seg_offset.x;
        dst_anchor.y -= seg_offset.y;
        #endif
        
        scui_multi_t angle = 0;
        scui_draw_image_2d(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha,
            draw_dsc->scale, angle,
            dst_anchor, src_center);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_image_rotate(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image  = draw_dsc->image;
    scui_area_t  *clip   = draw_dsc->clip;
    scui_point_t  anchor = draw_dsc->anchor;
    scui_point_t  center = draw_dsc->center;
    
    /* step:image<s> */
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域相对同步偏移 */
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        anchor.x -= seg_offset.x;
        anchor.y -= seg_offset.y;
        #endif
        
        scui_point_t scale = {
            .x = SCUI_SCALE_COF,
            .y = SCUI_SCALE_COF,
        };
        scui_draw_image_2d(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha,
            scale, draw_dsc->angle,
            anchor, center);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_image_2d(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image  = draw_dsc->image;
    scui_area_t  *clip   = draw_dsc->clip;
    scui_point_t  anchor = draw_dsc->anchor;
    scui_point_t  center = draw_dsc->center;
    
    /* step:image<s> */
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        /* 子剪切域相对同步偏移 */
        scui_point_t src_offset = {
            .x = dst_clip.x - target->x,
            .y = dst_clip.y - target->y,
        };
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        anchor.x -= seg_offset.x;
        anchor.y -= seg_offset.y;
        #endif
        
        scui_draw_image_2d(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha,
            draw_dsc->scale, draw_dsc->angle,
            anchor, center);
    }
}

/*@brief 控件在画布绘制图像
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_image_3d(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image = draw_dsc->image;
    scui_area_t  *clip  = draw_dsc->clip;
    
    #if SCUI_FRAME_BUFFER_SEG
    /* 该接口无独立画布的段绘制下还未调试 */
    if (!scui_widget_surface_only(widget))
         return;
    #endif
    
    /* step:image<s> */
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        scui_matrix_t inv_matrix = *draw_dsc->inv_matrix;
        scui_matrix_t src_matrix = *draw_dsc->matrix;
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        scui_point2_t matrix_ofs_i = {0};
        matrix_ofs_i.x = seg_offset.x;
        matrix_ofs_i.y = seg_offset.y;
        scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
        
        src_matrix = inv_matrix;
        scui_matrix_inverse(&src_matrix);
        #endif
        
        scui_draw_image_3d(false, widget->surface, dst_clip,
            image_inst, *clip, widget->alpha, SCUI_COLOR_UNUSED,
            inv_matrix, src_matrix);
    }
}

/*@brief 控件在画布绘制圆环
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_ring(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    scui_handle_t image   = draw_dsc->image;
    scui_area_t  *clip    = draw_dsc->clip;
    scui_coord_t  angle_s = draw_dsc->angle_s;
    scui_coord_t  angle_e = draw_dsc->angle_e;
    scui_coord_t  percent = draw_dsc->percent;
    scui_handle_t image_e = draw_dsc->image_e;
    
    #if SCUI_FRAME_BUFFER_SEG
    /* 该接口无独立画布的段绘制下还未调试 */
    if (!scui_widget_surface_only(widget))
         return;
    #endif
    
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
    scui_area_t   image_clip = scui_image_area(image);
    scui_image_t *image_inst = scui_handle_source_check(image);
    if (clip != NULL && !scui_area_inter2(&image_clip, clip)) return;
    clip = &image_clip;
    /* step:image<e> */
    
    scui_point_t dst_center = {
        .x = target->x + image_inst->pixel.width  / 2,
        .y = target->y + image_inst->pixel.height / 2,
    };
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
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
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_center.x -= seg_offset.x;
        dst_center.y -= seg_offset.y;
        #endif
        
        scui_draw_ring(false, widget->surface, dst_clip,
            dst_center, image_e_inst, image_inst, src_clip,
            angle_as, widget->alpha, angle_ae, draw_dsc->color);
    }
}

/*@brief 控件在画布绘制图形
 *@param handle   控件句柄
 *@param target   控件剪切域
 *@param draw_dsc 绘制参数实例
 */
void scui_widget_draw_ctx_graph(scui_handle_t handle, scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (!scui_widget_draw_target(widget, &target)) return;
    
    #if SCUI_FRAME_BUFFER_SEG
    /* 该接口无独立画布的段绘制下还未调试 */
    if (!scui_widget_surface_only(widget))
         return;
    #endif
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        
        scui_area_t dst_clip = {0};
        if (!scui_area_inter(&dst_clip, &unit->clip, target))
             continue;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(widget->surface,
             &dst_clip, NULL, NULL, NULL)) continue;
        #endif
        
        scui_draw_dsc_t *draw_dsc_graph = draw_dsc->graph;
        scui_draw_graph(false, widget->surface, dst_clip,
            draw_dsc->alpha, draw_dsc->color, draw_dsc_graph);
    }
}
