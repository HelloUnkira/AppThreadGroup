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

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
typedef union {
    /**************************************************************************
     * draw basic(HW ACC Perhaps):
     */
    struct {
        void *dst_addr; // 目标地址
        void *src_addr; // 源初地址
        uint32_t   len; // 数据字节长度
    } byte_copy;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
    } area_blur;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_alpha_t    src_alpha;      // 像素点透明度
        scui_color_t    src_color;      // 像素点协议色
    } area_fill;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_alpha_t    src_alpha;      // 像素点透明度
        scui_area_t    *src_clip;       // 源剪切域
        scui_color_t    src_color;      // 像素点协议色
        scui_coord_t    src_way;        // 渐变方向(0:hor;1:ver;)
    } area_fill_grad;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_alpha_t    src_alpha;      // 像素点透明度
        scui_color_t   *src_grad_s;     // 源渐变列表
        scui_coord_t    src_grad_n;     // 源渐变列表数量
        scui_color_t    src_filter;     // 源渐变滤色
        scui_coord_t    src_way;        // 源渐变方向(0:hor;1:ver;)
    } area_fill_grads;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_surface_t *src_surface;    // 画布实例
        scui_area_t    *src_clip;       // 画布绘制区域
    } area_copy;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_surface_t *src_surface;    // 画布实例
        scui_area_t    *src_clip;       // 画布绘制区域
        scui_color_t    src_color;      // 画布协议色
    } area_blend;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_area_t    *src_clip;       // 画布绘制区域
        scui_alpha_t    src_alpha;      // 像素点透明度
        scui_color_t    src_color;      // 画布协议色
        scui_matrix_t  *inv_matrix;     // 逆变换矩阵
        scui_matrix_t  *src_matrix;     // 源变换矩阵
    } area_fill_by_matrix;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_surface_t *src_surface;    // 画布实例
        scui_area_t    *src_clip;       // 画布绘制区域
        scui_color_t    src_color;      // 画布协议色
        scui_matrix_t  *inv_matrix;     // 逆变换矩阵
        scui_matrix_t  *src_matrix;     // 源变换矩阵
    } area_blit_by_matrix;
    /**************************************************************************
     * draw image:
     */
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_image_t   *src_image;      // 图像源
        scui_area_t    *src_clip;       // 图像源绘制区域
        scui_alpha_t    src_alpha;      // 图像透明度
        scui_color_t    src_color;      // 图像源色调
    } image;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_image_t   *src_image;      // 图像源
        scui_area_t    *src_clip;       // 图像源绘制区域
        scui_alpha_t    src_alpha;      // 图像透明度
        scui_color_t    src_color;      // 图像源色调
        scui_point_t    src_scale;      // 图形缩放比例(SCUI_SCALE_COF为放大系数)
        scui_point_t    dst_offset;     // 缩放锚点
        scui_point_t    src_offset;     // 缩放锚点
    } image_scale;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_image_t   *src_image;      // 图像源
        scui_area_t    *src_clip;       // 图像源绘制区域
        scui_alpha_t    src_alpha;      // 图像透明度
        scui_color_t    src_color;      // 图像源色调
        scui_coord_t    src_angle;      // 图形旋转角度(顺时针:+,逆时针:-)
        scui_point_t   *src_anchor;     // 图像旋转轴心
        scui_point_t   *src_center;     // 图像旋转中心
    } image_rotate;
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_image_t   *src_image;      // 图像源
        scui_area_t    *src_clip;       // 图像源绘制区域
        scui_alpha_t    src_alpha;      // 图像透明度
        scui_color_t    src_color;      // 图像源色调
        scui_matrix_t  *inv_matrix;     // 逆变换矩阵
        scui_matrix_t  *src_matrix;     // 源变换矩阵
    } image_blit_by_matrix;
    /**************************************************************************
     * draw letter & string:
     */
    struct {
        scui_surface_t    *dst_surface;     // 画布实例
        scui_area_t       *dst_clip;        // 画布绘制区域
        scui_font_glyph_t *src_glyph;       // 字符信息
        scui_area_t       *src_clip;        // 字符绘制区域
        scui_alpha_t       src_alpha;       // 字符透明度
        scui_color_t       src_color;       // 字符色调
    } letter;
    struct {
        scui_surface_t     *dst_surface;    // 画布实例
        scui_area_t        *dst_clip;       // 画布绘制区域
        scui_string_args_t *src_args;       // 字符串绘制参数
        scui_area_t        *src_clip;       // 画布绘制区域
        scui_alpha_t        src_alpha;      // 字符透明度
    } string;
    /**************************************************************************
     * draw ring:
     */
    struct {
        scui_surface_t *dst_surface;    // 画布实例
        scui_area_t    *dst_clip;       // 画布绘制区域
        scui_point_t   *dst_center;     // 图像旋转点
        scui_image_t   *src_image_e;    // 图像源(边界点)
        scui_image_t   *src_image;      // 图像源
        scui_area_t    *src_clip;       // 图像源绘制区域
        scui_coord_t    src_angle_s;    // 起始角度
        scui_alpha_t    src_alpha;      // 图像透明度
        scui_coord_t    src_angle_e;    // 结束角度
        scui_color_t    src_color;      // 图像源色调
    } ring;
    /**************************************************************************
     * keep adding...
     */
} scui_draw_dsc_t;
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/



typedef enum {
    scui_draw_graph_type_none = 0,
    scui_draw_graph_type_line,
    scui_draw_graph_type_circle,
    scui_draw_graph_type_crect,
    scui_draw_graph_type_arc,
} scui_draw_graph_type_t;
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
typedef struct {
    scui_draw_graph_type_t type;
    // 基础图元绘制单独提出(Antialias)
    // 因为这一部分可以使用任何库的绘制层简要摘出原理使用
    /*************************************************************************/
    scui_surface_t *dst_surface;    // 画布实例
    scui_area_t    *dst_clip;       // 画布绘制区域
    scui_alpha_t    src_alpha;      // 透明度
    scui_color_t    src_color;      // 源色调
    union {
    /*************************************************************************/
    struct {
        scui_coord_t src_width;     // 线宽
        scui_point_t src_pos_1;     // 坐标点
        scui_point_t src_pos_2;     // 坐标点
    } line;
    struct {
        scui_coord_t src_width;     // 环宽(0或大于半径为填充圆)
        scui_coord_t src_radius;    // 圆半径
        scui_point_t src_center;    // 圆心点
    } circle;
    struct {
        scui_coord_t src_width;     // 边界宽
        scui_coord_t src_radius;    // 圆半径
    } crect;
    struct {
        scui_coord_t src_width;     // 弧环宽
        scui_coord_t src_radius;    // 弧圆半径
        scui_point_t src_center;    // 弧圆心点
        scui_coord_t src_angle_s;   // 起始角度
        scui_coord_t src_angle_e;   // 结束角度
    } arc;
    /*************************************************************************/
    };
} scui_draw_graph_dsc_t;
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph(scui_draw_graph_dsc_t *draw_graph);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* EmbeddedGUI: */
#define   SCUI_DRAW_GRAPH_USE_EGUI      0
#include "scui_draw_graph_EGUI.h"
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#endif
