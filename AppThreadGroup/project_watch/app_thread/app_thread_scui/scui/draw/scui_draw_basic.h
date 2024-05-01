#ifndef SCUI_DRAW_BASIC_H
#define SCUI_DRAW_BASIC_H

/*@brief 行拷贝(可以使用DMA-copy加速优化)
 *@param dst_addr 目标地址
 *@param src_addr 源地址
 *@param len 数据字节长度
 */
void scui_draw_line_copy(void *dst_addr, void *src_addr, uint32_t len);

/*@brief 区域填充像素点(可以使用DMA-fill加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_pixel   像素点
 *@param alpha       像素点透明度
 */
void scui_draw_area_fill(scui_surface_t  *dst_surface, scui_area_t *dst_clip,
                         SCUI_PIXEL_TYPE *src_pixel,   scui_alpha_t alpha);

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 */
void scui_draw_area_copy(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                         scui_surface_t *src_surface, scui_area_t *src_clip);

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 */
void scui_draw_area_blend(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                          scui_surface_t *src_surface, scui_area_t *src_clip);

/*@brief 图形变换迁移(可以使用DMA2D-blend加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 *@param matrix      变换矩阵的逆矩阵
 */
void scui_draw_area_blit_by_matrix(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                                   scui_surface_t *src_surface, scui_area_t *src_clip,
                                   scui_matrix_t  *inv_matrix);

#endif
