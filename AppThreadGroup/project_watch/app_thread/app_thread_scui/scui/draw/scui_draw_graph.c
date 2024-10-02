/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 线条绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
static void scui_draw_line(scui_draw_graph_dsc_t *draw_graph)
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
        if (!scui_area_point(&dst_area, &point))
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
    
    #if 1
    // EmbeddedGUI移植: egui_canvas_draw_line
    scui_coord_t x1 = src_pos_1.x;
    scui_coord_t y1 = src_pos_1.y;
    scui_coord_t x2 = src_pos_2.x;
    scui_coord_t y2 = src_pos_2.y;
    scui_coord_t yinc = 1;
    scui_coord_t deltax = scui_abs(x2 - x1);
    scui_coord_t deltay = scui_abs(y2 - y1);
    bool steep = deltay >= deltax;
    
    if (steep) {
        scui_coord_t t1 = 0, t2 = 0;
        t1 = x1; x1 = y1; y1 = t1;
        t2 = x2; x2 = y2; y2 = t2;
    }
    if (x1 > x2) {
        scui_coord_t tx = 0, ty = 0;
        tx = x1; x1 = x2; x2 = tx;
        ty = y1; y1 = y2; y2 = ty;
    }
    
    scui_coord_t dx = x2 - x1;
    scui_coord_t dy = y2 - y1;
    if (dy < 0) {
        dy = -dy;
        yinc = -1;
    }
    
    scui_multi_t grad = dy * SCUI_SCALE_COF / dx;
    scui_multi_t itor = 0;
    scui_coord_t x_s  = draw_area.x;
    scui_coord_t x_e  = draw_area.x + draw_area.w;
    scui_coord_t y_s  = draw_area.y;
    scui_coord_t y_e  = draw_area.y + draw_area.h;
    
    scui_coord_t y = y1;
    for (scui_coord_t x = x1; x <= x2; x++) {
        // 为了简化代码, 我和并了流程, 为了性能是可以拆分出的
        if (( steep && (x >= y_s && x < y_e && y < x_e && y + src_width >= x_s)) ||
            (!steep && (x >= x_s && x < x_e && y < y_e && y + src_width >= y_s))) {
            
            scui_alpha_t alpha_0 = (yinc > 0 ? itor : SCUI_SCALE_COF - itor) >> (SCUI_SCALE_OFS - 8);
            scui_alpha_t alpha_1 = scui_alpha_cover - alpha_0;
            if (src_alpha != scui_alpha_cover) {
                alpha_0 = scui_alpha_mix(src_alpha, alpha_0);
                alpha_1 = scui_alpha_mix(src_alpha, alpha_1);
            }
            
            uint8_t *dst_ofs = NULL;
            scui_multi_t p0_x = x, p0_y = y;
            scui_multi_t pw_x = x, pw_y = y + src_width;
            if (steep) {
                p0_y = x; p0_x = y;
                pw_y = x; pw_x = y + src_width;
            }
            
            for (scui_multi_t idx = y + 1; idx < y + src_width; idx++) {
                dst_ofs = dst_addr + (steep ? x * dst_line + idx * dst_byte : idx * dst_line + x * dst_byte);
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                    dst_surface->format, &src_pixel, src_alpha);
            }
            dst_ofs = dst_addr + p0_y * dst_line + p0_x * dst_byte;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha_1,
                                dst_surface->format, &src_pixel, alpha_1);
            dst_ofs = dst_addr + pw_y * dst_line + pw_x * dst_byte;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha_0,
                                dst_surface->format, &src_pixel, alpha_0);
        }
        itor += grad;
        if (itor >= SCUI_SCALE_COF) {
            itor -= SCUI_SCALE_COF;
            y = yinc > 0 ? y + 1 : y - 1;
        }
    }
    #endif
}

/*@brief 水平线绘制
 *@param draw_graph    绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
static inline void scui_draw_hline(scui_draw_graph_dsc_t *draw_graph, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_graph->type = scui_draw_graph_type_line;
    draw_graph->line.src_width   = width;
    draw_graph->line.src_pos_1.x = x;
    draw_graph->line.src_pos_1.y = y;
    draw_graph->line.src_pos_2.x = x + len - 1;
    draw_graph->line.src_pos_2.y = y;
    scui_draw_graph(draw_graph);
}

/*@brief 垂直线绘制
 *@param draw_graph 绘制描述符实例
 *@param x,y,h      绘制参数
 */
static inline void scui_draw_vline(scui_draw_graph_dsc_t *draw_graph, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width)
{
    draw_graph->line.src_width   = width;
    draw_graph->line.src_pos_1.x = x;
    draw_graph->line.src_pos_1.y = y;
    draw_graph->line.src_pos_2.x = x;
    draw_graph->line.src_pos_2.y = y + len - 1;
    scui_draw_graph(draw_graph);
}

/* EmbeddedGUI移植 */
static scui_alpha_t scui_draw_circle_corner_val(scui_coord_t pos_row, scui_coord_t pos_col, const scui_draw_circle_info_t *info)
{
    scui_coord_t min_pos = pos_col;
    scui_coord_t max_pos = pos_row;
    if (pos_row < pos_col) {
        min_pos = pos_row;
        max_pos = pos_col;
    }
    if (min_pos >= info->count)
        return scui_alpha_cover;
    
    if (max_pos <  info->item[min_pos].offset)
        return scui_alpha_trans;
    
    if (max_pos >= info->item[min_pos].offset + info->item[min_pos].count)
        return scui_alpha_cover;
    
    return info->data[info->item[min_pos].pixel + max_pos - info->item[min_pos].offset];
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 *@param type       绘制角落类型
 */
static void scui_draw_circle_corner(scui_draw_graph_dsc_t *draw_graph, scui_draw_circle_type_t type)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->circle.src_width;
    scui_coord_t    src_radius  = draw_graph->circle.src_radius;
    scui_point_t    src_center  = draw_graph->circle.src_center;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_radius <= 0)
        src_radius  = 1;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    // 获得corner剪切域
    scui_area_t src_clip = {0};
    src_clip.w = src_radius;
    src_clip.h = src_radius;
    switch (type) {
    case scui_draw_circle_type_lt:
        src_clip.x = src_center.x - src_radius;
        src_clip.y = src_center.y - src_radius;
        break;
    case scui_draw_circle_type_lb:
        src_clip.x = src_center.x - src_radius;
        src_clip.y = src_center.y + 1;
        break;
    case scui_draw_circle_type_rt:
        src_clip.x = src_center.x + 1;
        src_clip.y = src_center.y - src_radius;
        break;
    case scui_draw_circle_type_rb:
        src_clip.x = src_center.x + 1;
        src_clip.y = src_center.y + 1;
        break;
    default:
        SCUI_LOG_ERROR("unknown type:%d", type);
        SCUI_ASSERT(false);
        return;
    }
    if (!scui_area_inter2(&draw_area, &src_clip))
         return;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel;
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    
    scui_draw_graph_dsc_t draw_graph_line = {
        .type = scui_draw_graph_type_line,
        .dst_surface = dst_surface,
        .dst_clip    = dst_clip,
        .src_color   = src_color,
        .src_alpha   = src_alpha,
    };
    
    #if 1
    // EmbeddedGUI移植: egui_canvas_draw_circle_corner_fill, egui_canvas_draw_circle_corner
    extern scui_draw_circle_info_t scui_draw_circle2c_EGUI_array[];
    scui_draw_circle_info_t *info = &scui_draw_circle2c_EGUI_array[src_radius - 1];
    SCUI_ASSERT(info->radius == src_radius);
    
    // 完全填充
    if (src_width == 0 || src_width >= src_radius) {
        
        for (scui_coord_t idx_info = 0; idx_info < info->count; idx_info++) {
            
            const scui_draw_circle_item_t *item = &info->item[idx_info];
            scui_coord_t sel_y = src_radius - idx_info;
            uint8_t *dst_ofs = NULL;
            
            for (scui_coord_t idx_item = 0; idx_item < item->count; idx_item++) {
                
                scui_point_t point = {0};
                scui_coord_t sel_x = src_radius - (item->offset + idx_item);
                scui_alpha_t mix_a = info->data[item->pixel + idx_item];
                mix_a = scui_alpha_mix(src_alpha, mix_a);
                
                point = src_center;
                switch (type) {
                case scui_draw_circle_type_lt: point.x -= sel_x; point.y -= sel_y; break;
                case scui_draw_circle_type_lb: point.x -= sel_x; point.y += sel_y; break;
                case scui_draw_circle_type_rt: point.x += sel_x; point.y -= sel_y; break;
                case scui_draw_circle_type_rb: point.x += sel_x; point.y += sel_y; break;
                }
                if (!scui_area_point(&dst_area, &point))
                     continue;
                
                dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                    dst_surface->format, &src_pixel, mix_a);
                
                // 跳过对角线的两次绘制
                if (sel_x == sel_y)
                    continue;
                
                point = src_center;
                switch (type) {
                case scui_draw_circle_type_lt: point.x -= sel_y; point.y -= sel_x; break;
                case scui_draw_circle_type_lb: point.x -= sel_y; point.y += sel_x; break;
                case scui_draw_circle_type_rt: point.x += sel_y; point.y -= sel_x; break;
                case scui_draw_circle_type_rb: point.x += sel_y; point.y += sel_x; break;
                }
                if (!scui_area_point(&dst_area, &point))
                     continue;
                
                dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                    dst_surface->format, &src_pixel, mix_a);
            }
            
            scui_coord_t length = src_radius - (item->offset + item->count);
            scui_point_t point1 = src_center;
            scui_point_t point2 = src_center;
            switch (type) {
            case scui_draw_circle_type_lt:
                point1.x -= length; point1.y -= sel_y;
                point2.x -= sel_y;  point2.y -= length;
                break;
            case scui_draw_circle_type_lb:
                point1.x -= length; point1.y += sel_y;
                point2.x -= sel_y;  point2.y += 1;
                break;
            case scui_draw_circle_type_rt:
                point1.x += 1;     point1.y -= sel_y;
                point2.x += sel_y; point2.y -= length;
                break;
            case scui_draw_circle_type_rb:
                point1.x += 1;     point1.y += sel_y;
                point2.x += sel_y; point2.y += 1;
                break;
            }
            if (length > 0) {
                scui_draw_hline(&draw_graph_line, point1.x, point1.y, length, 1);
                scui_draw_vline(&draw_graph_line, point2.x, point2.y, length, 1);
            }
        }
        
        scui_coord_t length = src_radius - info->count;
        scui_point_t point  = src_center;
        if (length != 0) {
            switch (type) {
            case scui_draw_circle_type_lt: point.x -= length; point.y -= length; break;
            case scui_draw_circle_type_lb: point.x -= length; point.y += 1;      break;
            case scui_draw_circle_type_rt: point.x += 1;      point.y -= length; break;
            case scui_draw_circle_type_rb: point.x += 1;      point.y += 1;      break;
            }
            scui_draw_hline(&draw_graph_line, point.x, point.y, length, length);
        }
    } else {
        scui_coord_t radius_in = src_radius - src_width;
        scui_draw_circle_info_t *info_in = &scui_draw_circle2c_EGUI_array[radius_in - 1];
        SCUI_ASSERT(info_in->radius == radius_in);
        
        scui_coord_t idx_row_s = 0;
        scui_coord_t idx_row_e = src_radius;
        scui_coord_t idx_col_s = 0;
        scui_coord_t idx_col_e = src_radius;
        scui_coord_t diff_x = draw_area.x - src_clip.x;
        scui_coord_t diff_y = draw_area.y - src_clip.y;
        
        switch (type) {
        case scui_draw_circle_type_lt:
            idx_row_s = diff_y;
            idx_row_e = idx_row_s + draw_area.h;
            idx_col_s = diff_x;
            idx_col_e = idx_col_s + draw_area.w;
            break;
        case scui_draw_circle_type_lb:
            idx_row_e = src_radius - diff_y;
            idx_row_s = idx_row_e - draw_area.h;
            idx_col_s = diff_x;
            idx_col_e = idx_col_s + draw_area.w;
            break;
        case scui_draw_circle_type_rt:
            idx_row_s = diff_y;
            idx_row_e = idx_row_s + draw_area.h;
            idx_col_e = src_radius - diff_x;
            idx_col_s = idx_col_e - draw_area.w;
            break;
        case scui_draw_circle_type_rb:
            idx_row_e = src_radius - diff_y;
            idx_row_s = idx_row_e - draw_area.h;
            idx_col_e = src_radius - diff_x;
            idx_col_s = idx_col_e - draw_area.w;
            break;
        }
        
        for (scui_coord_t idx_row = idx_row_s; idx_row < idx_row_e; idx_row++) {
            
            scui_coord_t sel_y = src_radius - idx_row;
            scui_alpha_t cir_a = scui_alpha_trans;
            scui_alpha_t mix_a = scui_alpha_trans;
            scui_alpha_t inn_a = scui_alpha_trans;
            
            scui_coord_t idx_col = 0;
            if (idx_row < info->count)
                idx_col = info->item[idx_row].offset;
            
            for (idx_col = scui_max(idx_col, idx_col_s); idx_col < idx_col_e; idx_col++) {
                
                scui_coord_t sel_x = src_radius - idx_col;
                if (cir_a != scui_alpha_cover)
                    cir_a  = scui_draw_circle_corner_val(idx_row, idx_col, info);
                if (cir_a == scui_alpha_trans)
                    continue;
                
                mix_a = scui_alpha_mix(cir_a, src_alpha);
                if (idx_row >= src_width && idx_col >= src_width) {
                    inn_a = scui_draw_circle_corner_val(idx_row - src_width, idx_col - src_width, info_in);
                    if (inn_a == scui_alpha_cover)
                        break;
                    mix_a = scui_alpha_mix(mix_a, scui_alpha_cover - inn_a);
                }
                
                
                scui_point_t point = src_center;
                switch (type) {
                case scui_draw_circle_type_lt: point.x -= sel_x; point.y -= sel_y; break;
                case scui_draw_circle_type_lb: point.x -= sel_x; point.y += sel_y; break;
                case scui_draw_circle_type_rt: point.x += sel_x; point.y -= sel_y; break;
                case scui_draw_circle_type_rb: point.x += sel_x; point.y += sel_y; break;
                }
                if (!scui_area_point(&dst_area, &point))
                     continue;
                
                uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                    dst_surface->format, &src_pixel, mix_a);
            }
        }
    }
    #endif
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
static void scui_draw_circle(scui_draw_graph_dsc_t *draw_graph)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->circle.src_width;
    scui_coord_t    src_radius  = draw_graph->circle.src_radius;
    scui_point_t    src_center  = draw_graph->circle.src_center;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_radius <= 0)
        src_radius  = 1;
    
    scui_draw_circle_corner(draw_graph, scui_draw_circle_type_lt);
    scui_draw_circle_corner(draw_graph, scui_draw_circle_type_lb);
    scui_draw_circle_corner(draw_graph, scui_draw_circle_type_rt);
    scui_draw_circle_corner(draw_graph, scui_draw_circle_type_rb);
    
    scui_draw_graph_dsc_t draw_graph_line = {
        .type = scui_draw_graph_type_line,
        .dst_surface = dst_surface,
        .dst_clip    = dst_clip,
        .src_color   = src_color,
        .src_alpha   = src_alpha,
    };
    scui_coord_t x = 0;
    scui_coord_t y = 0;
    scui_coord_t l = 0;
    
    // 完全填充
    if (src_width == 0 || src_width >= src_radius) {
        x = src_center.x - src_radius;
        y = src_center.y;
        l = src_radius;
        scui_draw_hline(&draw_graph_line, x, y, l, 1);
        x = src_center.x + 1;
        y = src_center.y;
        l = src_radius;
        scui_draw_hline(&draw_graph_line, x, y, l, 1);
        x = src_center.x;
        y = src_center.y - src_radius;
        l = (src_radius << 1) + 1;
        scui_draw_vline(&draw_graph_line, x, y, l, 1);
    } else {
        x = src_center.x - src_radius;
        y = src_center.y;
        l = src_width;
        scui_draw_hline(&draw_graph_line, x, y, l, 1);
        x = src_center.x + src_radius - src_width + 1;
        y = src_center.y;
        l = src_width;
        scui_draw_hline(&draw_graph_line, x, y, l, 1);
        
        x = src_center.x;
        y = src_center.y - src_radius;
        l = src_width;
        scui_draw_vline(&draw_graph_line, x, y, l, 1);
        x = src_center.x;
        y = src_center.y + src_radius - src_width + 1;
        l = src_width;
        scui_draw_vline(&draw_graph_line, x, y, l, 1);
    }
}

/*@brief 线条绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph(scui_draw_graph_dsc_t *draw_graph)
{
    switch (draw_graph->type) {
    case scui_draw_graph_type_line:
        scui_draw_line(draw_graph);
        break;
    case scui_draw_graph_type_circle:
        scui_draw_circle(draw_graph);
        break;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_graph->type);
        break;
    }
}
