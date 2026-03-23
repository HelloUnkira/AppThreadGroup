/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if defined(LVGL_VERSION_MAJOR) || defined(LVGL_VERSION_MINOR) || defined(LVGL_VERSION_PATCH)
#error "do not import any lvgl header files"
#endif

/* 这里只写一个思路 */
/* 需要从lvgl迁移以下的文件的内容 */
/* lv_draw_mask.c.h */
/* 以及lv_draw_系列的基础图形绘制函数 */

/*@brief 线条绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_line(scui_draw_dsc_t *draw_dsc)
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
    
    /* 变成了一个点, 变成了一个区域, 直接填色 */
    if ((draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x && draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y) ||
        (draw_dsc->src_pos_1.x == draw_dsc->src_pos_2.x || draw_dsc->src_pos_1.y == draw_dsc->src_pos_2.y)) {
         scui_draw_sline(draw_dsc);
         return;
    }
    
    #if 1
    #endif
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_circle(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    if (draw_dsc->src_radius <= 0)
        draw_dsc->src_radius  = 1;
    
    #if 1
    (void)draw_dsc->src_center;(void)draw_dsc->src_color;
    #endif
}

/*@brief 圆角矩形绘制
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_crect(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    if (draw_dsc->src_radius <= 0)
        draw_dsc->src_radius  = 1;
    
    #if 1
    (void)draw_dsc->src_color;
    #endif
}

/*@brief 弧绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_arc(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    if (draw_dsc->src_radius <= 0)
        draw_dsc->src_radius  = 1;
    
    /* 角度交换(保证s < e) */
    /* 这将限制角度以顺时针旋转绘制为目标 */
    if (draw_dsc->src_angle_s > draw_dsc->src_angle_e) {
        /*  */
        scui_coord_t  src_angle_t = 0;
        src_angle_t = draw_dsc->src_angle_e;
        draw_dsc->src_angle_e = draw_dsc->src_angle_s;
        draw_dsc->src_angle_s = src_angle_t;
    }
    /* 保证两个角度之间的间隙不超过360度 */
    while (draw_dsc->src_angle_e - draw_dsc->src_angle_s >= 360)
           draw_dsc->src_angle_s += 360;
    
    /* 角度限制(-360, +360): */
    while (draw_dsc->src_angle_e >  360) {
           draw_dsc->src_angle_e -= 360;
           draw_dsc->src_angle_s -= 360;
    }
    while (draw_dsc->src_angle_s <    0) {
           draw_dsc->src_angle_s += 360;
           draw_dsc->src_angle_e += 360;
    }
    
    /* 不存在0度弧度 */
    if (draw_dsc->src_angle_s == draw_dsc->src_angle_e)
        return;
    
    #if 1
    (void)draw_dsc->src_center;(void)draw_dsc->src_color;
    #endif
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
bool scui_draw_graph_LVGL(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_GRAPH_USE_EGUI
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line:
        scui_draw_line(draw_dsc);
        return false;
    case scui_draw_type_pixel_circle:
        scui_draw_circle(draw_dsc);
        return false;
    case scui_draw_type_pixel_crect:
        scui_draw_crect(draw_dsc);
        return false;
    case scui_draw_type_pixel_arc:
        scui_draw_arc(draw_dsc);
        return false;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_dsc->type);
        return false;
    }
    #endif
    
    return false;
}
