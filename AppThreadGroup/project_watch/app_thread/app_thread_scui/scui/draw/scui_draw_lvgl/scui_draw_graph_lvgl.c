/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if defined(LVGL_VERSION_MAJOR) || defined(LVGL_VERSION_MINOR) || defined(LVGL_VERSION_PATCH)
#error "do not import any lvgl header files"
#endif

// 这里只写一个思路
// 需要从lvgl迁移以下的文件的内容
// lv_draw_mask.c.h
// 以及lv_draw_系列的基础图形绘制函数

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
    
    #if 1
    (void)src_center;(void)src_color;
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
    (void)src_color;
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
    
    #if 1
    (void)src_center;(void)src_color;
    #endif
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
bool scui_draw_graph_LVGL(scui_draw_graph_dsc_t *draw_graph)
{
    #if SCUI_DRAW_GRAPH_USE_EGUI
    switch (draw_graph->type) {
    case scui_draw_graph_type_line:
        scui_draw_line(draw_graph);
        return false;
    case scui_draw_graph_type_circle:
        scui_draw_circle(draw_graph);
        return false;
    case scui_draw_graph_type_crect:
        scui_draw_crect(draw_graph);
        return false;
    case scui_draw_graph_type_arc:
        scui_draw_arc(draw_graph);
        return false;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_graph->type);
        return false;
    }
    #endif
    
    return false;
}
