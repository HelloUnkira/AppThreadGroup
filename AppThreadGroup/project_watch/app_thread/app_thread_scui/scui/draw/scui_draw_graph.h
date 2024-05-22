#ifndef SCUI_DRAW_GRAPH_H
#define SCUI_DRAW_GRAPH_H

/*@brief 区域填充像素点
 *@param dst_surface 画布实例
 *@param dst_point   画布坐标
 *@param src_pixel   像素点
 *@param src_alpha   像素点透明度
 */
void scui_draw_pixel(scui_surface_t  *dst_surface, scui_point_t *dst_point,
                     SCUI_PIXEL_TYPE *src_pixel,   scui_alpha_t  src_alpha);

#endif
