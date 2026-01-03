/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*备注:
 *    其实,在面向效果的Gui框架中
 *    基础图形的绘制显得不是那么重要
 *    因为随着实验效果表示,再好显示的抗锯齿基础图形
 *    它的显示效果是不如图像进行图形变换得来的要好
 *    此外,基础图象绘制的效果没有想象中的优秀
 *    它在使用过程中,限制较大
 */
    
/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_byte_copy(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_blur(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_fill(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_fill_grad(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_fill_grads(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_copy(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_blend(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_alpha_filter(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_matrix_fill(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_area_matrix_blend(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_image(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_image_scale(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_image_rotate(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_image_matrix_blend(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_letter(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_string(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_qrcode(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_barcode(scui_draw_dsc_t *draw_dsc);
void scui_draw_ctx_ring(scui_draw_dsc_t *draw_dsc);

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx(scui_draw_dsc_t *draw_dsc)
{
    typedef void (*scui_draw_ctx_cb_t)(scui_draw_dsc_t *draw_dsc);
    static const scui_draw_ctx_cb_t scui_draw_ctx_cb[scui_draw_type_num] = {
        [scui_draw_type_byte_copy] =                scui_draw_ctx_byte_copy,
        [scui_draw_type_area_blur] =                scui_draw_ctx_area_blur,
        [scui_draw_type_area_fill] =                scui_draw_ctx_area_fill,
        [scui_draw_type_area_fill_grad] =           scui_draw_ctx_area_fill_grad,
        [scui_draw_type_area_fill_grads] =          scui_draw_ctx_area_fill_grads,
        [scui_draw_type_area_copy] =                scui_draw_ctx_area_copy,
        [scui_draw_type_area_blend] =               scui_draw_ctx_area_blend,
        [scui_draw_type_area_alpha_filter] =        scui_draw_ctx_area_alpha_filter,
        [scui_draw_type_area_matrix_fill] =         scui_draw_ctx_area_matrix_fill,
        [scui_draw_type_area_matrix_blend] =        scui_draw_ctx_area_matrix_blend,
        [scui_draw_type_image] =                    scui_draw_ctx_image,
        [scui_draw_type_image_scale] =              scui_draw_ctx_image_scale,
        [scui_draw_type_image_rotate] =             scui_draw_ctx_image_rotate,
        [scui_draw_type_image_matrix_blend] =       scui_draw_ctx_image_matrix_blend,
        [scui_draw_type_letter] =                   scui_draw_ctx_letter,
        [scui_draw_type_string] =                   scui_draw_ctx_string,
        [scui_draw_type_qrcode] =                   scui_draw_ctx_qrcode,
        [scui_draw_type_barcode] =                  scui_draw_ctx_barcode,
        [scui_draw_type_ring] =                     scui_draw_ctx_ring,
    };
    
    SCUI_ASSERT(draw_dsc->type > scui_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_draw_type_num);
    scui_draw_ctx_cb[draw_dsc->type](draw_dsc);
}

/*@brief 线条绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
static void scui_draw_aline(scui_draw_graph_dsc_t *draw_graph)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->line.src_width;
    scui_point_t    src_pos_1   = draw_graph->line.src_pos_1;
    scui_point_t    src_pos_2   = draw_graph->line.src_pos_2;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    // 基础线不使用此接口绘制
    if (src_pos_1.x == src_pos_2.x ||
        src_pos_1.y == src_pos_2.y)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr  = dst_surface->pixel;
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    
    #if 1   /* 下面的内容是从其他地方抄录整理,效果有待商榷 */
    scui_point_t pos_s = src_pos_1.y < src_pos_2.y ? src_pos_1 : src_pos_2;
    scui_point_t pos_e = src_pos_1.y < src_pos_2.y ? src_pos_2 : src_pos_1;
    
    scui_area_t  clip_d = {0};
    scui_multi_t dx = pos_e.x - pos_s.x;
    scui_multi_t dy = pos_e.y - pos_s.y;
    
    if (dy / dx == 0) {
        /* 斜率在-45到+45之间 */
        
        /* 斜率在-45到0之间, 从左上往右下画 */
        /* 斜率在0到+45之间, 从右上往左下画 */
        
        scui_multi_t dt = pos_s.x;
        clip_d.x = pos_s.x;
        clip_d.h = src_width - 1;
        
        if (dx > 0)
            dt = pos_s.x - 1;
        else
            clip_d.x = pos_s.x + 1;
        
        for (scui_multi_t idx_j = 1; idx_j <= dy; idx_j++) {
            
            if (dx > 0) {
                clip_d.x = dt;
                dt = pos_s.x + idx_j * dx / dy;
            } else {
                dt = clip_d.x;
                clip_d.x = pos_s.x + idx_j * dx / dy;
            }
            
            clip_d.w = dt - clip_d.x;
            clip_d.y = pos_s.y + idx_j - src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_i = 0; idx_i < clip_d.w; idx_i++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_i + 1) / (2 * clip_d.w);
                alpha =  dx < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x + idx_i;
                point.y = clip_d.y - 1;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        dst_surface->format, &src_pixel, alpha);
                }
                
                point.y = clip_d.y + clip_d.h;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                            dst_surface->format, &src_pixel, src_alpha);
                    }
                }
            }
        }
    } else {
        /* 斜率在+45到+135之间 */
        scui_multi_t df = +1;
        
        clip_d.x = pos_s.x;
        clip_d.y = pos_s.y;
        clip_d.h = 0;
        clip_d.w = src_width - 1;
        
        if (dx < 0) {
            dx = -dx;
            df = -1;
        }
        
        for (scui_multi_t idx_i = 1; idx_i <= dx; idx_i++) {
            
            clip_d.y = clip_d.y + clip_d.h;
            clip_d.h = pos_s.y + idx_i * dy / dx - clip_d.y;
            clip_d.x = pos_s.x + idx_i * df - src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_j = 0; idx_j < clip_d.h; idx_j++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_j + 1) / (2 * clip_d.h);
                alpha =  df < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x - 1;
                point.y = clip_d.y + idx_j;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        dst_surface->format, &src_pixel, alpha);
                }
                
                point.x = clip_d.x + clip_d.w;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                            dst_surface->format, &src_pixel, src_alpha);
                    }
                }
            }
        }
    }
    #endif
}

/*@brief 线条绘制
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_sline(scui_draw_graph_dsc_t *draw_graph)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->line.src_width;
    scui_point_t    src_pos_1   = draw_graph->line.src_pos_1;
    scui_point_t    src_pos_2   = draw_graph->line.src_pos_2;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel;
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    
    /* 这里变成了一个点, 直接填色 */
    if (src_pos_1.x == src_pos_2.x && src_pos_1.y == src_pos_2.y) {
        scui_point_t point = {.x = src_pos_1.x, .y = src_pos_1.y};
        if (!scui_area_point(&draw_area, &point))
             return;
        
        uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                            dst_surface->format, &src_pixel, src_alpha);
    }
    /* 这里变成了一个区域, 直接填色 */
    if (src_pos_1.x == src_pos_2.x || src_pos_1.y == src_pos_2.y) {
        scui_area_t src_clip = {
            .x1 = scui_min(src_pos_1.x, src_pos_2.x),
            .y1 = scui_min(src_pos_1.y, src_pos_2.y),
            .x2 = scui_max(src_pos_1.x, src_pos_2.x),
            .y2 = scui_max(src_pos_1.y, src_pos_2.y),
        };
        scui_area_m_by_s(&src_clip, &src_clip);
        
        if (src_pos_1.x == src_pos_2.x)
            src_clip.w  += src_width - 1;
        if (src_pos_1.y == src_pos_2.y)
            src_clip.h  += src_width - 1;
        
        scui_area_t dst_area = {0};
        if (!scui_area_inter(&dst_area, &draw_area, &src_clip))
             return;
        
        scui_draw_area_fill(dst_surface, &dst_area, src_alpha, src_color);
        return;
    }
}

/*@brief 水平线绘制
 *@param draw_graph    绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_hline(scui_draw_graph_dsc_t *draw_graph, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_graph->type = scui_draw_graph_type_line;
    draw_graph->line.src_width   = width;
    draw_graph->line.src_pos_1.x = x;
    draw_graph->line.src_pos_1.y = y;
    draw_graph->line.src_pos_2.x = x + len - 1;
    draw_graph->line.src_pos_2.y = y;
    scui_draw_sline(draw_graph);
}

/*@brief 垂直线绘制
 *@param draw_graph    绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_vline(scui_draw_graph_dsc_t *draw_graph, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_graph->line.src_width   = width;
    draw_graph->line.src_pos_1.x = x;
    draw_graph->line.src_pos_1.y = y;
    draw_graph->line.src_pos_2.x = x;
    draw_graph->line.src_pos_2.y = y + len - 1;
    scui_draw_sline(draw_graph);
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph_ctx(scui_draw_graph_dsc_t *draw_graph)
{
    #if SCUI_DRAW_GRAPH_USE_LVGL
    if (scui_draw_graph_LVGL(draw_graph))
        return;
    #endif
    
    #if SCUI_DRAW_GRAPH_USE_EGUI
    if (scui_draw_graph_EGUI(draw_graph))
        return;
    #endif
    
    switch (draw_graph->type) {
    case scui_draw_graph_type_line:
        scui_draw_sline(draw_graph);
        scui_draw_aline(draw_graph);
        break;
    default:
        break;
    }
}
