#ifndef SCUI_DRAW_GRAPH_H
#define SCUI_DRAW_GRAPH_H

/*绘制语义约定:
 *    dst_surface + dst_clip
 *    src_surface + src_clip
 *    将src_surface中的src_clip范围内的区域
 *    到dst_surface中的dst_clip范围内的区域
 *    且变换要求取最小交集区域
 *    即为原点对齐变换
 */

typedef enum {
    scui_draw_type_none = 0,
    scui_draw_type_byte_copy,
    scui_draw_type_area_blur,
    scui_draw_type_area_fill,
    scui_draw_type_area_fill_grad,
    scui_draw_type_area_fill_grads,
    scui_draw_type_area_copy,
    scui_draw_type_area_blend,
    scui_draw_type_area_alpha_filter,
    scui_draw_type_area_matrix_fill,
    scui_draw_type_area_matrix_blend,
    scui_draw_type_image,
    scui_draw_type_image_scale,
    scui_draw_type_image_rotate,
    scui_draw_type_image_matrix_blend,
    scui_draw_type_letter,
    scui_draw_type_string,
    scui_draw_type_qrcode,
    scui_draw_type_barcode,
    scui_draw_type_ring,
    
    // 基础图元放到最后
    scui_draw_type_pixel_line,
    scui_draw_type_pixel_circle,
    scui_draw_type_pixel_crect,
    scui_draw_type_pixel_shadow,
    scui_draw_type_pixel_arc,
    
    scui_draw_type_num,
} scui_draw_type_t;

typedef struct {
    scui_draw_type_t type;
    
    /* 绘制上文所有信息 */
    /* 目标:收集到一个栈帧 */
    
    void *dst_addr; // 目标地址
    void *src_addr; // 源初地址
    uint32_t   len; // 数据字节长度
    
    // source(part surface)
    scui_surface_t *dst_surface;    // 画布实例
    scui_area_t    *dst_clip;       // 画布绘制域
    
    // source(part surface)
    scui_surface_t *src_surface;    // 画布实例
    scui_area_t    *src_clip;       // 画布剪切域
    
    // source(part pixel)
    scui_alpha_t    src_alpha;      // 像素点,字符透明度
    scui_color_t    src_color;      // 画布,像素点,字符协议色
    
    // source(part image)
    scui_image_t   *src_image;      // 图像源
    scui_point_t    src_scale;      // 图像缩放比例
    scui_point_t    dst_offset;     // 缩放锚点
    scui_point_t    src_offset;     // 缩放锚点
    scui_multi_t    src_angle;      // 图形旋转角度(顺时针:+,逆时针:-)
    scui_point_t    src_anchor;     // 图像旋转轴心
    scui_point_t    src_center;     // 图像旋转中心
    
    // source(part image ring)
    scui_point_t   *dst_center;     // 图像旋转点
    scui_image_t   *src_image_e;    // 图像(边界点)
    scui_coord_t    src_angle_s;    // 起始角度
    scui_coord_t    src_angle_e;    // 结束角度
    
    // source(part transform)
    scui_matrix_t  *src_matrix;     // 源变换矩阵
    scui_matrix_t  *inv_matrix;     // 逆变换矩阵
    
    // source(part letter & string)
    void           *src_glyph;      // 字符信息(scui_font_glyph_t)
    void           *src_args;       // 字符串绘制参数(scui_string_args_t)
    
    // source(part qrcode & barcode)
    scui_multi_t    src_size;       // 字符串长度
    uint8_t        *src_data;       // url网址链接字符串
    
    // source(part grad)
    scui_color_t   *src_grad_s;     // 源渐变列表
    scui_coord_t    src_grad_n;     // 源渐变列表数量
    scui_color_t    src_filter;     // 源渐变滤色
    scui_coord_t    src_way;        // 源渐变方向(0:hor;1:ver;)
    
    // 基础图元绘制单独提出(Antialias)
    // 因为这一部分可以使用任何库的绘制层
    
    scui_coord_t    src_width;      // 线宽, 环宽, 边界宽
    scui_point_t    src_pos_1;      // 坐标点
    scui_point_t    src_pos_2;      // 坐标点
    scui_coord_t    src_radius;     // 弧, 圆半径
    
} scui_draw_dsc_t;

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx(scui_draw_dsc_t *draw_dsc);

/*@brief 线条绘制(简易绘制)
 *@param draw_dsc 绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_sline(scui_draw_dsc_t *draw_dsc);
void scui_draw_hline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width);
void scui_draw_vline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width);

/*****************************************************************************/
/*基础图元绘制:
 *    可以自己写(反正就是堆工作量)
 *    可以移植第三方的gui中的内容
 */
#define   SCUI_DRAW_GRAPH_USE_LVGL      0
#define   SCUI_DRAW_GRAPH_USE_EGUI      1
#include "scui_draw_graph_LVGL.h"
#include "scui_draw_graph_EGUI.h"
/*****************************************************************************/

#endif
