#ifndef SCUI_DRAW_IMAGE_H
#define SCUI_DRAW_IMAGE_H

/*@brief 区域图像绘制(可以使用DMA2D-blend加速优化)
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param image_alpha  图像透明度(非图像自带透明度)
 *@param color        图像源色调(调色板使用)
 */
void scui_draw_image(scui_surface_t    *dst_surface, scui_area_t     *dst_clip,
                     scui_image_unit_t *image_unit,  scui_area_t     *src_clip,
                     scui_alpha_t       image_alpha, scui_color_mix_t color);

/*@brief 区域图像缩放(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param image_alpha  图像透明度(非图像自带透明度)
 *@param zoom         图形缩放比例(放大:+,缩小:-)
 */
void scui_draw_image_zoom(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                          scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                          scui_alpha_t       image_alpha, scui_coord_t zoom);

/*@brief 区域图像旋转(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以指定方向旋转指定角度
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param image_alpha  图像透明度(非图像自带透明度)
 *@param angle        图形旋转角度(顺时针旋转:+,逆时针旋转:-)
 *@param anchor       图形旋转轴心
 *@param center       图形旋转中心
 */
void scui_draw_image_rotate(scui_surface_t    *dst_surface, scui_area_t  *dst_clip,
                            scui_image_unit_t *image_unit,  scui_area_t  *src_clip,
                            scui_alpha_t       image_alpha, scui_coord_t  angle,
                            scui_point_t      *anchor,      scui_point_t *center);

/*@brief 区域图像变换(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param image_alpha  图像透明度(非图像自带透明度)
 *@param matrix       图像变换矩阵
 */
void scui_draw_image_blit_by_matrix(scui_surface_t    *dst_surface, scui_area_t   *dst_clip,
                                    scui_image_unit_t *image_unit,  scui_area_t   *src_clip,
                                    scui_alpha_t       image_alpha, scui_matrix_t *matrix);

#endif
