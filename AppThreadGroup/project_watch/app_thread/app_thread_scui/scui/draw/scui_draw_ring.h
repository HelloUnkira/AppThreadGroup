#ifndef SCUI_DRAW_RING_H
#define SCUI_DRAW_RING_H

/*@brief 区域图像绘制
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param dst_center  图像旋转点
 *@param src_image_e 图像源
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_angle_s 起始角度
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param src_angle_e 结束角度
 *@param src_color   图像源色调(调色板使用)
 */
void scui_draw_ring(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                    scui_point_t   *dst_center,  scui_image_t *src_image_e,
                    scui_image_t   *src_image,   scui_area_t  *src_clip,
                    scui_coord_t    src_angle_s, scui_alpha_t  src_alpha,
                    scui_coord_t    src_angle_e, scui_color_t  src_color);

#endif
