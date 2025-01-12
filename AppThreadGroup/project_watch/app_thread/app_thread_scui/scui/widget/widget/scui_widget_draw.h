#ifndef SCUI_WIDGET_DRAW_H
#define SCUI_WIDGET_DRAW_H

/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle);

/*@brief 控件在画布绘制字符串
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数
 */
void scui_widget_draw_string(scui_handle_t handle, scui_area_t *target, scui_string_args_t *args);

/*@brief 控件在画布绘制纯色区域
 *@param handle 控件句柄
 *@param target 绘制区域
 *@param color  源色调
 */
void scui_widget_draw_color(scui_handle_t handle, scui_area_t *target, scui_color_t color);

/*@brief 控件在画布绘制渐变纯色区域
 *@param handle 控件句柄
 *@param target 绘制区域
 *@param color  源色调
 *@param way    渐变方向(0:hor;1:ver;)
 */
void scui_widget_draw_color_grad(scui_handle_t handle, scui_area_t *target, scui_color_t color, scui_coord_t way);

/*@brief 控件在画布绘制模糊
 *@param handle 控件句柄
 *@param clip   绘制区域
 */
void scui_widget_draw_blur(scui_handle_t handle, scui_area_t *clip);

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param color  图像源色调(调色板)
 */
void scui_widget_draw_image(scui_handle_t handle, scui_area_t *target,
                            scui_handle_t image,  scui_area_t *clip,
                            scui_color_t  color);

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param scale  图形缩放比例(1024为放大系数)
 *@param pos    缩放锚点
 */
void scui_widget_draw_image_scale(scui_handle_t handle, scui_area_t   *target,
                                  scui_handle_t image,  scui_area_t   *clip,
                                  scui_point_t  scale,  scui_opt_pos_t pos);

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param anchor 图像旋转轴心
 *@param center 图像旋转中心
 *@param angle  图像旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_widget_draw_image_rotate(scui_handle_t handle, scui_area_t  *target,
                                   scui_handle_t image,  scui_area_t  *clip,
                                   scui_point_t *anchor, scui_point_t *center,
                                   scui_coord_t  angle);

/*@brief 控件在画布绘制图像
 *@param handle 控件句柄
 *@param target 控件绘制区域
 *@param image  图像句柄
 *@param clip   图像源绘制区域
 *@param matrix 变换矩阵
 */
void scui_widget_draw_image_matrix(scui_handle_t  handle, scui_area_t *target,
                                   scui_handle_t  image,  scui_area_t *clip,
                                   scui_matrix_t *matrix);

/*@brief 控件在画布绘制圆环
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
void scui_widget_draw_ring(scui_handle_t handle,  scui_area_t *target,
                           scui_handle_t image,   scui_area_t *clip,
                           scui_coord_t  angle_s, scui_color_t color,
                           scui_coord_t  angle_e, scui_coord_t percent,
                           scui_handle_t image_e);

/*@brief 控件在画布绘制线条
 *@param handle     控件句柄
 *@param target     控件绘制区域
 *@param draw_graph 绘制描述符实例
 */
void scui_widget_draw_graph(scui_handle_t handle, scui_area_t *target,
                            scui_draw_graph_dsc_t *draw_graph);

#endif
