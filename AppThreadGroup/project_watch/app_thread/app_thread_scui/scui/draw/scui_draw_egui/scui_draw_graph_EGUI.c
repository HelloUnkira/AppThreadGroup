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
    
    /* 变成了一个点, 变成了一个区域, 直接填色 */
    if ((src_pos_1.x == src_pos_2.x && src_pos_1.y == src_pos_2.y) ||
        (src_pos_1.x == src_pos_2.x || src_pos_1.y == src_pos_2.y)) {
         scui_draw_sline(draw_graph);
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

/* EmbeddedGUI移植 */
#if 1
/* EmbeddedGUI移植 */
#if     SCUI_DRAW_CIRCLE_RES_RANGE < SCUI_DRV_HOR_RES
#error "circle res is not enough"
#endif
#if     SCUI_DRAW_CIRCLE_RES_RANGE < SCUI_DRV_VER_RES
#error "circle res is not enough"
#endif
/* EmbeddedGUI移植 */
static const float scui_draw_graph_tan_table[91] = {
     0.000000f,  0.017455f,  0.034921f,  0.052408f,  0.069927f,
     0.087489f,  0.105104f,  0.122785f,  0.140541f,  0.158384f,
     0.176327f,  0.194380f,  0.212557f,  0.230868f,  0.249328f,
     0.267949f,  0.286745f,  0.305731f,  0.324920f,  0.344328f,
     0.363970f,  0.383864f,  0.404026f,  0.424475f,  0.445229f,
     0.466308f,  0.487733f,  0.509525f,  0.531709f,  0.554309f,
     0.577350f,  0.600861f,  0.624869f,  0.649408f,  0.674509f,
     0.700208f,  0.726543f,  0.753554f,  0.781286f,  0.809784f,
     0.839100f,  0.869287f,  0.900404f,  0.932515f,  0.965689f,
     1.000000f,  1.035530f,  1.072369f,  1.110613f,  1.150368f,
     1.191754f,  1.234897f,  1.279942f,  1.327045f,  1.376382f,
     1.428148f,  1.482561f,  1.539865f,  1.600335f,  1.664279f,
     1.732051f,  1.804048f,  1.880726f,  1.962611f,  2.050304f,
     2.144507f,  2.246037f,  2.355852f,  2.475087f,  2.605089f,
     2.747477f,  2.904211f,  3.077684f,  3.270853f,  3.487414f,
     3.732051f,  4.010781f,  4.331476f,  4.704630f,  5.144554f,
     5.671282f,  6.313752f,  7.115370f,  8.144346f,  9.514364f,
    11.430052f, 14.300666f, 19.081137f, 28.636253f, 57.289962f,
    9999.0f,
};
/* EmbeddedGUI移植 */
typedef enum {
    scui_draw_circle_type_lt,       // left  top
    scui_draw_circle_type_lb,       // left  bottom
    scui_draw_circle_type_rt,       // right top
    scui_draw_circle_type_rb,       // right bottm
} scui_draw_circle_type_t;

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
                if (scui_area_point(&draw_area, &point)) {
                    dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                        dst_surface->format, &src_pixel, mix_a);
                }
                
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
                if (scui_area_point(&draw_area, &point)) {
                    dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                        dst_surface->format, &src_pixel, mix_a);
                }
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
            idx_row_e = idx_row_s  + draw_area.h;
            idx_col_s = diff_x;
            idx_col_e = idx_col_s  + draw_area.w;
            break;
        case scui_draw_circle_type_lb:
            idx_row_e = src_radius - diff_y;
            idx_row_s = idx_row_e  - draw_area.h;
            idx_col_s = diff_x;
            idx_col_e = idx_col_s  + draw_area.w;
            break;
        case scui_draw_circle_type_rt:
            idx_row_s = diff_y;
            idx_row_e = idx_row_s  + draw_area.h;
            idx_col_e = src_radius - diff_x;
            idx_col_s = idx_col_e  - draw_area.w;
            break;
        case scui_draw_circle_type_rb:
            idx_row_e = src_radius - diff_y;
            idx_row_s = idx_row_e  - draw_area.h;
            idx_col_e = src_radius - diff_x;
            idx_col_s = idx_col_e  - draw_area.w;
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
                if (!scui_area_point(&draw_area, &point))
                     continue;
                
                uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                    dst_surface->format, &src_pixel, mix_a);
            }
        }
    }
    #endif
}

/* EmbeddedGUI移植 */
static bool scui_draw_arc_corner_in(float x, float y, float tan_k_s, float tan_k_e)
{
    if (x <= 0 || y <= 0)
        return true;
    
    if (scui_dist(tan_k_s, scui_draw_graph_tan_table[0]) > 1E-6)
    if (y <= x * tan_k_s)
        return false;
    if (scui_dist(tan_k_e, scui_draw_graph_tan_table[90]) > 1E-6)
    if (y >= x * tan_k_e)
        return false;
    
    return true;
}

/* EmbeddedGUI移植 */
static scui_alpha_t scui_draw_arc_corner_val(scui_coord_t x, scui_coord_t y, float tan_k_s, float tan_k_e)
{
    uint16_t sum = 0;
    
    if (scui_draw_arc_corner_in(x - 0.5f, y - 0.5f, tan_k_s, tan_k_e)) sum++;
    if (scui_draw_arc_corner_in(x - 0.5f, y + 0.5f, tan_k_s, tan_k_e)) sum++;
    if (scui_draw_arc_corner_in(x + 0.5f, y - 0.5f, tan_k_s, tan_k_e)) sum++;
    if (scui_draw_arc_corner_in(x + 0.5f, y + 0.5f, tan_k_s, tan_k_e)) sum++;
    if (scui_draw_arc_corner_in(x + 0.0f, y + 0.0f, tan_k_s, tan_k_e)) sum++;
    
    return scui_alpha_pct100 * sum / 5;
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 *@param type       绘制角落类型
 */
static void scui_draw_arc_corner(scui_draw_graph_dsc_t *draw_graph, scui_draw_circle_type_t type)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->arc.src_width;
    scui_coord_t    src_radius  = draw_graph->arc.src_radius;
    scui_point_t    src_center  = draw_graph->arc.src_center;
    scui_coord_t    src_angle_s = draw_graph->arc.src_angle_s;
    scui_coord_t    src_angle_e = draw_graph->arc.src_angle_e;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_radius <= 0)
        src_radius  = 1;
    
    if (src_angle_e > 90)
        src_angle_e = 90;
    if (src_angle_s < 0)
        src_angle_s = 0;
    if (src_angle_s >= src_angle_e)
        return;
    
    if (src_angle_s == 0 && src_angle_e == 90) {
        scui_draw_graph_dsc_t draw_graph_circle = {
            .type = scui_draw_graph_type_circle,
            .dst_surface = dst_surface,
            .dst_clip    = dst_clip,
            .src_alpha   = src_alpha,
            .src_color   = src_color,
            .circle.src_width  = src_width,
            .circle.src_radius = src_radius,
            .circle.src_center = src_center,
        };
        scui_draw_circle_corner(&draw_graph_circle, type);
        return;
    }
    
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
    
    #if 1
    // EmbeddedGUI移植: egui_canvas_draw_circle_corner_fill, egui_canvas_draw_circle_corner
    extern scui_draw_circle_info_t scui_draw_circle2c_EGUI_array[];
    scui_draw_circle_info_t *info = &scui_draw_circle2c_EGUI_array[src_radius - 1];
    SCUI_ASSERT(info->radius == src_radius);
    scui_coord_t radius_in = src_radius - src_width;
    scui_draw_circle_info_t *info_in = NULL;
    if (radius_in != 0) {
        info_in = &scui_draw_circle2c_EGUI_array[radius_in - 1];
        SCUI_ASSERT(info_in->radius == radius_in);
    }
    
    scui_coord_t idx_row_s = 0;
    scui_coord_t idx_row_e = src_radius;
    scui_coord_t idx_col_s = 0;
    scui_coord_t idx_col_e = src_radius;
    scui_coord_t diff_x = draw_area.x - src_clip.x;
    scui_coord_t diff_y = draw_area.y - src_clip.y;
    
    switch (type) {
    case scui_draw_circle_type_lt:
        idx_row_s = diff_y;
        idx_row_e = idx_row_s  + draw_area.h;
        idx_col_s = diff_x;
        idx_col_e = idx_col_s  + draw_area.w;
        break;
    case scui_draw_circle_type_lb:
        idx_row_e = src_radius - diff_y;
        idx_row_s = idx_row_e  - draw_area.h;
        idx_col_s = diff_x;
        idx_col_e = idx_col_s  + draw_area.w;
        break;
    case scui_draw_circle_type_rt:
        idx_row_s = diff_y;
        idx_row_e = idx_row_s  + draw_area.h;
        idx_col_e = src_radius - diff_x;
        idx_col_s = idx_col_e  - draw_area.w;
        break;
    case scui_draw_circle_type_rb:
        idx_row_e = src_radius - diff_y;
        idx_row_s = idx_row_e  - draw_area.h;
        idx_col_e = src_radius - diff_x;
        idx_col_s = idx_col_e  - draw_area.w;
        break;
    }
    
    float tan_k_s  = scui_draw_graph_tan_table[src_angle_s];
    float tan_k_e  = scui_draw_graph_tan_table[src_angle_e];
    float tan_ck_s = scui_draw_graph_tan_table[90 - src_angle_s];
    float tan_ck_e = scui_draw_graph_tan_table[90 - src_angle_e];
    
    scui_coord_t sel_y    = src_radius;
    scui_coord_t cur_s_x  = scui_coord_max - 0xF;
    scui_coord_t cur_e_x  = scui_coord_max - 0xF;
    scui_coord_t next_s_x = scui_coord_max - 0xF;
    scui_coord_t next_e_x = scui_coord_max - 0xF;
    if (src_angle_s != 0) {
        cur_s_x  = tan_ck_s * sel_y;
        next_s_x = cur_s_x;
    }
    if (src_angle_e != 0) {
        cur_e_x  = tan_ck_e * sel_y;
        next_e_x = cur_e_x;
    }
    scui_coord_t last_s_x = cur_s_x;
    scui_coord_t last_e_x = cur_e_x;
    
    for (scui_multi_t idx_row = idx_row_s; idx_row < idx_row_e; idx_row++) {
        
        sel_y = src_radius - idx_row;
        if (src_angle_s != 0) {
            cur_s_x  = next_s_x;
            next_s_x = tan_ck_s * (sel_y - 1);
        }
        if (src_angle_e != 0) {
            cur_e_x  = next_e_x;
            next_e_x = tan_ck_e * (sel_y - 1);
        }
        
        scui_coord_t x_allow_min = next_e_x - 1;
        scui_coord_t x_allow_max = last_s_x + 1;
        scui_coord_t x_arc_allow_min = next_s_x - 1;
        scui_coord_t x_arc_allow_max = last_e_x + 1;
        scui_alpha_t cir_a = scui_alpha_trans;
        scui_alpha_t mix_a = scui_alpha_trans;
        scui_alpha_t inn_a = scui_alpha_trans;
        scui_alpha_t pos_a = scui_alpha_trans;
        scui_coord_t idx_col = 0;
        
        if (idx_row < info->count) {
            idx_col = info->item[idx_row].offset;
        }
        
        for (idx_col = scui_max(idx_col, idx_col_s); idx_col < idx_col_e; idx_col++) {
            
            scui_coord_t sel_x = src_radius - idx_col;
            if (sel_x < x_allow_min || sel_x > x_allow_max)
                continue;
            
            if (cir_a != scui_alpha_cover)
                cir_a  = scui_draw_circle_corner_val(idx_row, idx_col, info);
            if (cir_a == scui_alpha_trans)
                continue;
            
            mix_a = scui_alpha_mix(cir_a, src_alpha);
            
            // 局部填充
            if (!(src_width == 0 || src_width >= src_radius)) {
                if (idx_row >= src_width && idx_col >= src_width) {
                    inn_a = scui_draw_circle_corner_val(idx_row - src_width, idx_col - src_width, info_in);
                    if (inn_a == scui_alpha_cover)
                        break;
                    mix_a = scui_alpha_mix(mix_a, scui_alpha_cover - inn_a);
                }
            }
            
            if (!((sel_x > x_arc_allow_max) && (sel_x < x_arc_allow_min || next_s_x > src_radius))) {
                pos_a = scui_draw_arc_corner_val(sel_x, sel_y, tan_k_s, tan_k_e);
                if (pos_a == 0)
                    continue;
                mix_a = scui_alpha_mix(mix_a, pos_a);
            }
            
            scui_point_t point = src_center;
            switch (type) {
            case scui_draw_circle_type_lt: point.x -= sel_x; point.y -= sel_y; break;
            case scui_draw_circle_type_lb: point.x -= sel_x; point.y += sel_y; break;
            case scui_draw_circle_type_rt: point.x += sel_x; point.y -= sel_y; break;
            case scui_draw_circle_type_rb: point.x += sel_x; point.y += sel_y; break;
            }
            if (!scui_area_point(&draw_area, &point))
                 continue;
            
            uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * dst_byte;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - mix_a,
                                dst_surface->format, &src_pixel, mix_a);
        }
        
        last_s_x = cur_s_x;
        last_e_x = cur_e_x;
    }
    #endif
}

#endif

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
    
    /* EmbeddedGUI移植 */
    #if 1
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
    #endif
}

/*@brief 圆角矩形绘制
 *@param draw_graph 绘制描述符实例
 */
static void scui_draw_crect(scui_draw_graph_dsc_t *draw_graph)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->crect.src_width;
    scui_coord_t    src_radius  = draw_graph->crect.src_radius;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_radius <= 0)
        src_radius  = 1;
    
    #if 1
    /* 绘制四个象限的圆或圆环 */
    scui_area_t  dst_area   = {0};
    scui_point_t src_center = {0};
    dst_area.w = src_radius * 2;
    dst_area.h = src_radius * 2;
    scui_draw_graph_dsc_t draw_graph_circle = {
        .type = scui_draw_graph_type_circle,
        .dst_surface = dst_surface,
        .dst_clip    = &dst_area,
        .src_color   = src_color,
        .src_alpha   = src_alpha,
        .circle.src_width  = src_width,
        .circle.src_radius = src_radius,
    };
    //
    dst_area.x = dst_clip->x + 0;
    dst_area.y = dst_clip->y + 0;
    draw_graph_circle.circle.src_center.x = dst_area.x + src_radius;
    draw_graph_circle.circle.src_center.y = dst_area.y + src_radius;
    scui_draw_circle_corner(&draw_graph_circle, scui_draw_circle_type_lt);
    dst_area.x = dst_clip->x + dst_clip->w - src_radius * 2 - 1;
    dst_area.y = dst_clip->y + 0;
    draw_graph_circle.circle.src_center.x = dst_area.x + src_radius;
    draw_graph_circle.circle.src_center.y = dst_area.y + src_radius;
    scui_draw_circle_corner(&draw_graph_circle, scui_draw_circle_type_rt);
    dst_area.x = dst_clip->x + 0;
    dst_area.y = dst_clip->y + dst_clip->h - src_radius * 2 - 1;
    draw_graph_circle.circle.src_center.x = dst_area.x + src_radius;
    draw_graph_circle.circle.src_center.y = dst_area.y + src_radius;
    scui_draw_circle_corner(&draw_graph_circle, scui_draw_circle_type_lb);
    dst_area.x = dst_clip->x + dst_clip->w - src_radius * 2 - 1;
    dst_area.y = dst_clip->y + dst_clip->h - src_radius * 2 - 1;
    draw_graph_circle.circle.src_center.x = dst_area.x + src_radius;
    draw_graph_circle.circle.src_center.y = dst_area.y + src_radius;
    scui_draw_circle_corner(&draw_graph_circle, scui_draw_circle_type_rb);
    
    // 色块填充
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
    scui_coord_t w = 0;
    
    // 完全填充
    if (src_width == 0 || src_width >= src_radius) {
        /* 绘制中间水平矩形 */
        x = dst_clip->x + src_radius;
        l = dst_clip->w - src_radius * 2;
        y = dst_clip->y;
        w = dst_clip->h;
        scui_draw_hline(&draw_graph_line, x, y, l, w);
        /* 绘制左右水平矩形 */
        y = dst_clip->y + src_radius;
        l = src_radius;
        w = dst_clip->h - src_radius * 2;
        /*  */
        x = dst_clip->x;
        scui_draw_hline(&draw_graph_line, x, y, l, w);
        /*  */
        x = dst_clip->x + dst_clip->w - src_radius;
        scui_draw_hline(&draw_graph_line, x, y, l, w);
    } else {
        /* 绘制上下两条边线 */
        x = dst_clip->x + src_radius;
        l = dst_clip->w - src_radius * 2;
        w = src_width;
        /*  */
        y = dst_clip->y;
        scui_draw_hline(&draw_graph_line, x, y, l, w);
        y = dst_clip->y + dst_clip->h - src_width;
        scui_draw_hline(&draw_graph_line, x, y, l, w);
        /* 绘制左右两条边线 */
        y = dst_clip->y + src_radius;
        l = dst_clip->h - src_radius * 2;
        w = src_width;
        /*  */
        x = dst_clip->x;
        scui_draw_vline(&draw_graph_line, x, y, l, w);
        x = dst_clip->x + dst_clip->w - src_width;
        scui_draw_vline(&draw_graph_line, x, y, l, w);
    }
    #endif
}

/*@brief 弧绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
static void scui_draw_arc(scui_draw_graph_dsc_t *draw_graph)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_graph->dst_surface;
    scui_area_t    *dst_clip    = draw_graph->dst_clip;
    scui_alpha_t    src_alpha   = draw_graph->src_alpha;
    scui_color_t    src_color   = draw_graph->src_color;
    scui_coord_t    src_width   = draw_graph->arc.src_width;
    scui_coord_t    src_radius  = draw_graph->arc.src_radius;
    scui_point_t    src_center  = draw_graph->arc.src_center;
    scui_coord_t    src_angle_s = draw_graph->arc.src_angle_s;
    scui_coord_t    src_angle_e = draw_graph->arc.src_angle_e;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_radius <= 0)
        src_radius  = 1;
    
    /* 角度交换(保证s < e) */
    /* 这将限制角度以顺时针旋转绘制为目标 */
    if (src_angle_s > src_angle_e) {
        /*  */
        scui_coord_t  src_angle_t = 0;
        src_angle_t = src_angle_e;
        src_angle_e = src_angle_s;
        src_angle_s = src_angle_t;
    }
    /* 保证两个角度之间的间隙不超过360度 */
    while (src_angle_e - src_angle_s >= 360)
           src_angle_s += 360;
    
    /* 角度限制(-360, +360): */
    while (src_angle_e >  360) {
           src_angle_e -= 360;
           src_angle_s -= 360;
    }
    while (src_angle_s <    0) {
           src_angle_s += 360;
           src_angle_e += 360;
    }
    
    /* 不存在0度弧度 */
    if (src_angle_s == src_angle_e)
        return;
    
    /* EmbeddedGUI移植 */
    #if 1
    scui_draw_graph_dsc_t draw_graph_arc = *draw_graph;
    draw_graph_arc.arc.src_angle_s = src_angle_s;
    draw_graph_arc.arc.src_angle_e = src_angle_e;
    
    bool center_point = false;
    scui_draw_graph_dsc_t draw_graph_line = {
        .type = scui_draw_graph_type_line,
        .dst_surface = dst_surface,
        .dst_clip    = dst_clip,
        .src_color   = src_color,
        .src_alpha   = src_alpha,
    };
    
    do {
        if (src_angle_s < 90) {
            draw_graph_arc.arc.src_angle_s = src_angle_s;
            draw_graph_arc.arc.src_angle_e = src_angle_e;
            scui_draw_arc_corner(&draw_graph_arc, scui_draw_circle_type_rb);
        }
        if (src_angle_s < 180) {
            draw_graph_arc.arc.src_angle_e = 90 - (src_angle_s - 90);
            draw_graph_arc.arc.src_angle_s = 90 - (src_angle_e - 90);
            scui_draw_arc_corner(&draw_graph_arc, scui_draw_circle_type_lb);
        }
        if (src_angle_s < 270) {
            draw_graph_arc.arc.src_angle_s = src_angle_s - 180;
            draw_graph_arc.arc.src_angle_e = src_angle_e - 180;
            scui_draw_arc_corner(&draw_graph_arc, scui_draw_circle_type_lt);
        }
        if (src_angle_s < 360) {
            draw_graph_arc.arc.src_angle_e = 90 - (src_angle_s - 270);
            draw_graph_arc.arc.src_angle_s = 90 - (src_angle_e - 270);
            scui_draw_arc_corner(&draw_graph_arc, scui_draw_circle_type_rt);
        }
        // 补线
        if (src_angle_s <= 0 && src_angle_e >= 0) {
            scui_coord_t x = src_center.x + src_radius - src_width + 1;
            scui_coord_t y = src_center.y;
            scui_coord_t l = src_width;
            scui_draw_hline(&draw_graph_line, x, y, l, 1);
            center_point = true;
        }
        if (src_angle_s <= 90 && src_angle_e >= 90) {
            scui_coord_t x = src_center.x;
            scui_coord_t y = src_center.y + src_radius - src_width + 1;
            scui_coord_t l = src_width;
            scui_draw_vline(&draw_graph_line, x, y, l, 1);
            center_point = true;
        }
        if (src_angle_s <= 180 && src_angle_e >= 180) {
            scui_coord_t x = src_center.x - src_radius;
            scui_coord_t y = src_center.y;
            scui_coord_t l = src_width;
            scui_draw_hline(&draw_graph_line, x, y, l, 1);
            center_point = true;
        }
        if (src_angle_s <= 270 && src_angle_e >= 270) {
            scui_coord_t x = src_center.x;
            scui_coord_t y = src_center.y - src_radius;
            scui_coord_t l = src_width;
            scui_draw_vline(&draw_graph_line, x, y, l, 1);
            center_point = true;
        }
        src_angle_s -= 360;
        src_angle_e -= 360;
        if (src_angle_s < 0)
            src_angle_s = 0;
        
    } while (src_angle_e > 0);

    // 完全填充
    if (src_width == 0 || src_width >= src_radius) {
        if (center_point) {
            scui_coord_t x = src_center.x;
            scui_coord_t y = src_center.y;
            scui_draw_hline(&draw_graph_line, x, y, 1, 1);
        }
    }
    #endif
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph_EGUI(scui_draw_graph_dsc_t *draw_graph)
{
    #if SCUI_DRAW_GRAPH_USE_EGUI
    switch (draw_graph->type) {
    case scui_draw_graph_type_line:
        scui_draw_line(draw_graph);
        break;
    case scui_draw_graph_type_circle:
        scui_draw_circle(draw_graph);
        break;
    case scui_draw_graph_type_crect:
        scui_draw_crect(draw_graph);
        break;
    case scui_draw_graph_type_arc:
        scui_draw_arc(draw_graph);
        break;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_graph->type);
        break;
    }
    #endif
}
