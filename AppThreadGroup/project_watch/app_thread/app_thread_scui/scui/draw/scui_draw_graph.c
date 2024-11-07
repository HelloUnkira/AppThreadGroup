/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

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
        scui_area_m_by_s(&src_clip);
        
        if (src_pos_1.x == src_pos_2.x)
            src_clip.w  += src_width - 1;
        if (src_pos_1.y == src_pos_2.y)
            src_clip.h  += src_width - 1;
        
        scui_area_t dst_area = {0};
        if (!scui_area_inter(&dst_area, &draw_area, &src_clip))
             return;
        
        scui_draw_dsc_t draw_dsc = {
            .area_fill.dst_surface = dst_surface,
            .area_fill.dst_clip    = &dst_area,
            .area_fill.src_alpha   = src_alpha,
            .area_fill.src_color   = src_color,
        };
        scui_draw_area_fill(&draw_dsc);
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
 *@param draw_graph 绘制描述符实例
 *@param x,y,h      绘制参数
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
void scui_draw_graph(scui_draw_graph_dsc_t *draw_graph)
{
    #if SCUI_DRAW_GRAPH_USE_LVGL
    scui_draw_graph_LVGL(draw_graph);
    return;
    #endif
    
    #if SCUI_DRAW_GRAPH_USE_EGUI
    scui_draw_graph_EGUI(draw_graph);
    return;
    #endif
}
