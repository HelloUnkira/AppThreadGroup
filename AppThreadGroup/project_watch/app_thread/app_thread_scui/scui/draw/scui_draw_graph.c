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
void scui_draw_ctx_graph(scui_draw_dsc_t *draw_dsc);

// 使用slab分配器加速快速获取绘制描述符实例
static void * scui_draw_dsc_slab_mem = NULL;

/*@brief 绘制资源就绪
 */
void scui_draw_ready(void)
{
    // 绘制描述符资源就绪
    uintptr_t mem_slab_size = sizeof(scui_draw_dsc_t) * SCUI_CACHE_DRAW_DSC_NUM + sizeof(app_sys_mem_slab_t) + sizeof(uintptr_t) * 4;
    scui_draw_dsc_slab_mem  = SCUI_MEM_ALLOC(scui_mem_type_graph, mem_slab_size);
    uintptr_t mem_slab_src  = (uintptr_t)scui_draw_dsc_slab_mem + sizeof(app_sys_mem_slab_t) + sizeof(uintptr_t);
    app_sys_mem_slab_ready(scui_draw_dsc_slab_mem, mem_slab_src, mem_slab_size, sizeof(scui_draw_dsc_t));
}

/*@brief 绘制描述符实例申请
 *@param draw_dsc 绘制描述符实例地址
 */
void scui_draw_dsc_ready(scui_draw_dsc_t **draw_dsc)
{
    SCUI_ASSERT(draw_dsc != NULL);
    *draw_dsc = app_sys_mem_slab_alloc(scui_draw_dsc_slab_mem);
    SCUI_ASSERT(*draw_dsc != NULL);
    
    // 此处不使用memset, 使用者需要给定完整参数集, 未给定参数为未知值
    // memset(*draw_dsc, 0, sizeof(scui_draw_dsc_t));
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_sched(scui_draw_dsc_t *draw_dsc)
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
        
        [scui_draw_type_pixel_line] =               scui_draw_ctx_graph,
        [scui_draw_type_pixel_circle] =             scui_draw_ctx_graph,
        [scui_draw_type_pixel_crect] =              scui_draw_ctx_graph,
        [scui_draw_type_pixel_shadow] =             scui_draw_ctx_graph,
        [scui_draw_type_pixel_arc] =                scui_draw_ctx_graph,
    };
    
    SCUI_ASSERT(draw_dsc->type > scui_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_draw_type_num);
    scui_draw_ctx_cb[draw_dsc->type](draw_dsc);
    
    app_sys_mem_slab_free(scui_draw_dsc_slab_mem, draw_dsc);
}

/*@brief 线条绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_aline(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    if (draw_dsc->src_width <= 0)
        draw_dsc->src_width  = 1;
    
    // 基础线不使用此接口绘制
    if (draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x ||
        draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(draw_dsc->dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, &draw_dsc->dst_clip))
         return;
    
    scui_coord_t dst_byte = scui_pixel_bits(draw_dsc->dst_surface->format) / 8;
    scui_multi_t dst_line = draw_dsc->dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr  = draw_dsc->dst_surface->pixel;
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(draw_dsc->dst_surface->format, &src_pixel, draw_dsc->src_color.color);
    
    #if 1   /* 下面的内容是从其他地方抄录整理,效果有待商榷 */
    scui_point_t pos_s = draw_dsc->src_pos_1.y < draw_dsc->src_pos_2.y ? draw_dsc->src_pos_1 : draw_dsc->src_pos_2;
    scui_point_t pos_e = draw_dsc->src_pos_1.y < draw_dsc->src_pos_2.y ? draw_dsc->src_pos_2 : draw_dsc->src_pos_1;
    
    scui_area_t  clip_d = {0};
    scui_multi_t dx = pos_e.x - pos_s.x;
    scui_multi_t dy = pos_e.y - pos_s.y;
    
    if (dy / dx == 0) {
        /* 斜率在-45到+45之间 */
        
        /* 斜率在-45到0之间, 从左上往右下画 */
        /* 斜率在0到+45之间, 从右上往左下画 */
        
        scui_multi_t dt = pos_s.x;
        clip_d.x = pos_s.x;
        clip_d.h = draw_dsc->src_width - 1;
        
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
            clip_d.y = pos_s.y + idx_j - draw_dsc->src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_i = 0; idx_i < clip_d.w; idx_i++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_i + 1) / (2 * clip_d.w);
                alpha =  dx < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * draw_dsc->src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x + idx_i;
                point.y = clip_d.y - 1;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(&draw_dsc->dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        draw_dsc->dst_surface->format, &src_pixel, alpha);
                }
                
                point.y = clip_d.y + clip_d.h;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(&draw_dsc->dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        draw_dsc->dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, &draw_dsc->dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - draw_dsc->src_alpha,
                                            draw_dsc->dst_surface->format, &src_pixel, draw_dsc->src_alpha);
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
        clip_d.w = draw_dsc->src_width - 1;
        
        if (dx < 0) {
            dx = -dx;
            df = -1;
        }
        
        for (scui_multi_t idx_i = 1; idx_i <= dx; idx_i++) {
            
            clip_d.y = clip_d.y + clip_d.h;
            clip_d.h = pos_s.y + idx_i * dy / dx - clip_d.y;
            clip_d.x = pos_s.x + idx_i * df - draw_dsc->src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_j = 0; idx_j < clip_d.h; idx_j++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_j + 1) / (2 * clip_d.h);
                alpha =  df < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * draw_dsc->src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x - 1;
                point.y = clip_d.y + idx_j;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(&draw_dsc->dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        draw_dsc->dst_surface->format, &src_pixel, alpha);
                }
                
                point.x = clip_d.x + clip_d.w;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(&draw_dsc->dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                        draw_dsc->dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, &draw_dsc->dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - draw_dsc->src_alpha,
                                            draw_dsc->dst_surface->format, &src_pixel, draw_dsc->src_alpha);
                    }
                }
            }
        }
    }
    #endif
}

/*@brief 线条绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_sline(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    if (draw_dsc->src_width <= 0)
        draw_dsc->src_width  = 1;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(draw_dsc->dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, &draw_dsc->dst_clip))
         return;
    
    scui_coord_t dst_byte = scui_pixel_bits(draw_dsc->dst_surface->format) / 8;
    scui_multi_t dst_line = draw_dsc->dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = draw_dsc->dst_surface->pixel;
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(draw_dsc->dst_surface->format, &src_pixel, draw_dsc->src_color.color);
    
    /* 这里变成了一个点, 直接填色 */
    if (draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x && draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y) {
        scui_point_t point = {.x = draw_dsc->src_pos_1.x, .y = draw_dsc->src_pos_1.y};
        if (!scui_area_point(&draw_area, &point))
             return;
        
        uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
        scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - draw_dsc->src_alpha,
                            draw_dsc->dst_surface->format, &src_pixel, draw_dsc->src_alpha);
    }
    /* 这里变成了一个区域, 直接填色 */
    if (draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x || draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y) {
        scui_area_t src_clip = {
            .x1 = scui_min(draw_dsc->src_pos_1.x, draw_dsc->src_pos_2.x),
            .y1 = scui_min(draw_dsc->src_pos_1.y, draw_dsc->src_pos_2.y),
            .x2 = scui_max(draw_dsc->src_pos_1.x, draw_dsc->src_pos_2.x),
            .y2 = scui_max(draw_dsc->src_pos_1.y, draw_dsc->src_pos_2.y),
        };
        scui_area_m_by_s(&src_clip, &src_clip);
        
        if (draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x)
            src_clip.w  += draw_dsc->src_width - 1;
        if (draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y)
            src_clip.h  += draw_dsc->src_width - 1;
        
        scui_area_t dst_area = {0};
        if (!scui_area_inter(&dst_area, &draw_area, &src_clip))
             return;
        
        scui_draw_area_fill(draw_dsc->dst_surface, dst_area, draw_dsc->src_alpha, draw_dsc->src_color);
        return;
    }
}

/*@brief 水平线绘制
 *@param draw_dsc      绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_hline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_dsc->type = scui_draw_type_pixel_line;
    draw_dsc->src_width   = width;
    draw_dsc->src_pos_1.x = x;
    draw_dsc->src_pos_1.y = y;
    draw_dsc->src_pos_2.x = x + len - 1;
    draw_dsc->src_pos_2.y = y;
    scui_draw_sline(draw_dsc);
}

/*@brief 垂直线绘制
 *@param draw_dsc      绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_vline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_dsc->type = scui_draw_type_pixel_line;
    draw_dsc->src_width   = width;
    draw_dsc->src_pos_1.x = x;
    draw_dsc->src_pos_1.y = y;
    draw_dsc->src_pos_2.x = x;
    draw_dsc->src_pos_2.y = y + len - 1;
    scui_draw_sline(draw_dsc);
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_graph(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_GRAPH_USE_LVGL
    bool scui_draw_graph_LVGL(scui_draw_dsc_t *draw_dsc);
    if (scui_draw_graph_LVGL(draw_dsc))
        return;
    #endif
    
    #if SCUI_DRAW_GRAPH_USE_EGUI
    bool scui_draw_graph_EGUI(scui_draw_dsc_t *draw_dsc);
    if (scui_draw_graph_EGUI(draw_dsc))
        return;
    #endif
    
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line:
        scui_draw_sline(draw_dsc);
        scui_draw_aline(draw_dsc);
        break;
    default:
        break;
    }
}
