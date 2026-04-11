/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 线条绘制(抗锯齿,效果差)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_aline(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_point_t    src_pos_1   =  draw_dsc->graph.src_pos_1;
    scui_point_t    src_pos_2   =  draw_dsc->graph.src_pos_2;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    /* 基础线不使用此接口绘制 */
    if (src_pos_1.x == src_pos_2.x ||
        src_pos_1.y == src_pos_2.y)
        return;
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel;
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
            
            /* 上下两边渐变补偿颜色打点 */
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
                    scui_pixel_mix_with(dst_surface->format, dst_ofs,
                        dst_surface->format, &src_pixel, alpha);
                }
                
                point.y = clip_d.y + clip_d.h;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs,
                        dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(dst_surface->format, dst_ofs,
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
            
            /* 上下两边渐变补偿颜色打点 */
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
                    scui_pixel_mix_with(dst_surface->format, dst_ofs,
                        dst_surface->format, &src_pixel, alpha);
                }
                
                point.x = clip_d.x + clip_d.w;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs,
                        dst_surface->format, &src_pixel, alpha);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
                        scui_pixel_mix_with(dst_surface->format, dst_ofs,
                            dst_surface->format, &src_pixel, src_alpha);
                    }
                }
            }
        }
    }
    #endif
}

/*@brief 线条绘制(水平垂直线)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_sline(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_point_t    src_pos_1   =  draw_dsc->graph.src_pos_1;
    scui_point_t    src_pos_2   =  draw_dsc->graph.src_pos_2;
    /* draw dsc args<e> */
    /* */
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
    
    /* 这里变成了一个点, 直接填色 */
    if (src_pos_1.x == src_pos_2.x && src_pos_1.y == src_pos_2.y) {
        scui_point_t point = {.x = src_pos_1.x, .y = src_pos_1.y};
        if (!scui_area_point(&draw_area, &point))
             return;
        
        scui_color_wt_t src_pixel = 0;
        scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
        uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
        scui_pixel_mix_with(dst_surface->format, dst_ofs,
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
        
        if (src_pos_1.x == src_pos_2.x) src_clip.w += src_width - 1;
        if (src_pos_1.y == src_pos_2.y) src_clip.h += src_width - 1;
        
        scui_area_t dst_area = {0};
        if (!scui_area_inter(&dst_area, &draw_area, &src_clip))
             return;
        
        scui_draw_area_fill(true, dst_surface, dst_area, src_alpha, src_color);
        return;
    }
}

/*@brief 水平线绘制
 *@param draw_dsc 绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_hline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_dsc->type = scui_draw_type_pixel_line;
    draw_dsc->graph.src_width   = width;
    draw_dsc->graph.src_pos_1.x = x;
    draw_dsc->graph.src_pos_1.y = y;
    draw_dsc->graph.src_pos_2.x = x + len - 1;
    draw_dsc->graph.src_pos_2.y = y;
    scui_draw_sline(draw_dsc);
}

/*@brief 垂直线绘制
 *@param draw_dsc 绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_vline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_dsc->type = scui_draw_type_pixel_line;
    draw_dsc->graph.src_width   = width;
    draw_dsc->graph.src_pos_1.x = x;
    draw_dsc->graph.src_pos_1.y = y;
    draw_dsc->graph.src_pos_2.x = x;
    draw_dsc->graph.src_pos_2.y = y + len - 1;
    scui_draw_sline(draw_dsc);
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_graph(scui_draw_dsc_t *draw_dsc)
{
    /* 基础填色 */
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line: {
        scui_point_t src_pos_1 = draw_dsc->graph.src_pos_1;
        scui_point_t src_pos_2 = draw_dsc->graph.src_pos_2;
        /* 变成了一个点, 变成了一个区域, 直接填色 */
        if ((src_pos_1.x == src_pos_2.x && src_pos_1.y == src_pos_2.y) ||
            (src_pos_1.x == src_pos_2.x || src_pos_1.y == src_pos_2.y)) {
            /* 外部入口简单调整一下居中对齐 */
            if (src_pos_1.x == src_pos_2.x) {
                draw_dsc->graph.src_pos_1.x -= draw_dsc->graph.src_width / 2;
                draw_dsc->graph.src_pos_2.x -= draw_dsc->graph.src_width / 2;
            }
            if (src_pos_1.y == src_pos_2.y) {
                draw_dsc->graph.src_pos_1.y -= draw_dsc->graph.src_width / 2;
                draw_dsc->graph.src_pos_2.y -= draw_dsc->graph.src_width / 2;
            }
            scui_draw_sline(draw_dsc);
            return;
        }
        break;
    }
    default:
        break;
    }
    
    
    
    /* 矢量绘图引擎 */
    #if SCUI_DRAW_USE_THORVG
    bool scui_draw_ctx_graph_TVG(scui_draw_dsc_t *draw_dsc);
    if (scui_draw_ctx_graph_TVG(draw_dsc))
        return;
    #endif
    
    /* 外源图形组件 */
    #if SCUI_DRAW_GRAPH_USE_EGUI
    bool scui_draw_ctx_graph_EGUI(scui_draw_dsc_t *draw_dsc);
    if (scui_draw_ctx_graph_EGUI(draw_dsc))
        return;
    #endif
    
    
    
    /* 基础处理(备注:) */
    /* 原生绘制最多需求斜线绘制 */
    /* 其余绘制内容都调整成贴图 */
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line:
        scui_draw_aline(draw_dsc);
        break;
    default:
        break;
    }
}
