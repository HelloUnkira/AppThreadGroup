#ifndef SCUI_DRAW_IMAGE_H
#define SCUI_DRAW_IMAGE_H

/*@brief 区域图像绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image(scui_draw_dsc_t *draw_dsc);

/*@brief 区域图像缩放
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_scale(scui_draw_dsc_t *draw_dsc);

/*@brief 区域图像旋转
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_rotate(scui_draw_dsc_t *draw_dsc);

/*@brief 区域图像变换
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_matrix_blend(scui_draw_dsc_t *draw_dsc);

#endif
