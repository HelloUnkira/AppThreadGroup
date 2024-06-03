#ifndef SCUI_WIDGET_SURFACE_H
#define SCUI_WIDGET_SURFACE_H

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget);

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse);

/*@brief 控件画布为独立画布
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_widget_t *widget, scui_surface_t *surface);

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件画布在画布绘制图像
 *@param widget      控件实例
 *@param dst_clip    控件剪切域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 *@param src_color   图像源色调(调色板使用)
 */
void scui_widget_surface_draw_pattern(scui_widget_t   *widget,      scui_area_t *dst_clip,
                                      scui_surface_t  *src_surface, scui_area_t *src_clip,
                                      scui_color_mix_t src_color);

/*@brief 控件画布在画布绘制字符串
 *@param widget   控件实例
 *@param src_args 字符串绘制参数(scui_string_args_t)
 */
void scui_widget_surface_draw_string(scui_widget_t *widget, void *src_args);

/*@brief 控件画布在画布绘制纯色区域
 *@param widget   控件实例
 *@param src_clip 绘制区域
 *@param color    源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget,
                                    scui_area_t *src_clip, scui_color_mix_t color);

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_surface_draw_image(scui_widget_t *widget, scui_handle_t handle,
                                    scui_area_t *src_clip, scui_color_mix_t color);

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param angle    图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 *@param anchor   图像旋转轴心
 *@param center   图像旋转中心
 */
void scui_widget_surface_draw_image_rotate(scui_widget_t *widget, scui_handle_t handle,
                                           scui_area_t *src_clip, scui_coord_t  angle,
                                           scui_point_t  *anchor, scui_point_t *center);

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param matrix   变换矩阵
 */
void scui_widget_surface_draw_image_by_matrix(scui_widget_t *widget, scui_handle_t  handle,
                                              scui_area_t *src_clip, scui_matrix_t *matrix);

#endif
