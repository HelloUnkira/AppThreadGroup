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

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 为剪切域集合计算以调整源到目标的剪切域(内部接口)
 *@param 形如scui_widget_surface_draw_xxx的接口使用
 */
bool scui_widget_surface_clip_adjust(scui_widget_t *widget,   scui_area_t *unit_clip,
                                     scui_area_t   *target,   scui_area_t *clip,
                                     scui_area_t   *dst_clip, scui_area_t *src_clip);

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

/*@brief 控件剪切域(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_surface_clip(scui_handle_t handle);

/*@brief 控件画布在画布绘制图像
 *@param handle  控件句柄
 *@param target  控件绘制区域
 *@param surface 画布实例
 *@param clip    画布绘制区域
 *@param color   图像源色调(调色板)
 */
void scui_widget_surface_draw_pattern(scui_handle_t   handle,  scui_area_t *target,
                                      scui_surface_t *surface, scui_area_t *clip,
                                      scui_color_t    color);

/*@brief 控件画布在画布绘制字符串
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数(scui_string_args_t)
 */
void scui_widget_surface_draw_string(scui_handle_t handle, scui_area_t *target, void *args);

/*@brief 控件画布在画布绘制纯色区域
 *@param handle 控件句柄
 *@param clip   绘制区域
 *@param color  源色调
 */
void scui_widget_surface_draw_color(scui_handle_t handle, scui_area_t *clip,
                                    scui_color_t  color);

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param color  图像源色调(调色板)
 */
void scui_widget_surface_draw_image(scui_handle_t handle, scui_area_t *target,
                                    scui_handle_t image,  scui_area_t *clip,
                                    scui_color_t  color);

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param scale  图形缩放比例(1024为放大系数)
 */
void scui_widget_surface_draw_image_scale(scui_handle_t handle, scui_area_t *target,
                                          scui_handle_t image,  scui_area_t *clip,
                                          scui_point_t  scale);

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param anchor 图像旋转轴心
 *@param center 图像旋转中心
 *@param angle  图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_widget_surface_draw_image_rotate(scui_handle_t handle, scui_area_t  *target,
                                           scui_handle_t image,  scui_area_t  *clip,
                                           scui_point_t *anchor, scui_point_t *center,
                                           scui_coord_t  angle);

/*@brief 控件画布在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param matrix 变换矩阵
 */
void scui_widget_surface_draw_image_by_matrix(scui_handle_t  handle, scui_area_t *target,
                                              scui_handle_t  image,  scui_area_t *clip,
                                              scui_matrix_t *matrix);

/*@brief 控件画布在画布绘制圆环
 *@param handle  控件句柄
 *@param target  控件绘制区域
 *@param image   图像句柄
 *@param clip    图像源绘制区域
 *@param angle_s 起始角度
 *@param color   源色调
 *@param angle_s 起始角度
 *@param percent 圆环进度(百分比)
 *@param image_e 控件句柄
 */
void scui_widget_surface_draw_ring(scui_handle_t handle,  scui_area_t *target,
                                   scui_handle_t image,   scui_area_t *clip,
                                   scui_coord_t  angle_s, scui_color_t color,
                                   scui_coord_t  angle_e, scui_coord_t percent,
                                   scui_handle_t image_e);

/*@brief 控件画布在画布绘制线条
 *@param handle 控件句柄
 *@param width  线条宽
 *@param pos_1  位置1
 *@param pos_2  位置2
 *@param color  源色调
 */
void scui_widget_surface_draw_line(scui_handle_t handle, scui_coord_t width,
                                   scui_point_t  pos_1,  scui_point_t pos_2,
                                   scui_color_t  color);

#endif
