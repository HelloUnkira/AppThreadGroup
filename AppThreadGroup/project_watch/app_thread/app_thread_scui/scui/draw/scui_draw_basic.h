#ifndef SCUI_DRAW_BASIC_H
#define SCUI_DRAW_BASIC_H

/*@brief 绘制字节拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_byte_copy(scui_draw_dsc_t *draw_dsc);

/*@brief 区域模糊(可以使用GPU-blur加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_blur(scui_draw_dsc_t *draw_dsc);

/*@brief 区域填充像素点(可以使用DMA-fill加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill(scui_draw_dsc_t *draw_dsc);

/*@brief 区域填充渐变像素点(可以使用DMA-fill-grad加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill_grad(scui_draw_dsc_t *draw_dsc);

/*@brief 区域序列渐变像素点(暂未知优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill_grads(scui_draw_dsc_t *draw_dsc);

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_copy(scui_draw_dsc_t *draw_dsc);

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_blend(scui_draw_dsc_t *draw_dsc);

/*@brief 区域透明过滤像素点(暂未知优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_alpha_filter(scui_draw_dsc_t *draw_dsc);

/*@brief 图形变换填色(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_matrix_fill(scui_draw_dsc_t *draw_dsc);

/*@brief 图形变换迁移(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_matrix_blend(scui_draw_dsc_t *draw_dsc);

#endif
