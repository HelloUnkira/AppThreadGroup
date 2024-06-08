#ifndef SCUI_CUSTOM_UTIL_H
#define SCUI_CUSTOM_UTIL_H

/*@brief 自定义控件:插件:导航点
 *@param event       自定义控件事件
 *@param clip        剪切域(绘制区域)
 *@param wait        图像句柄(未选中)
 *@param color_wait  图像源色调(调色板使用)
 *@param focus       图像句柄(选中)
 *@param color_focus 图像源色调(调色板使用)
 *@param count       导航点数量
 *@param index       目标索引(选中)
 *@param span        间隙
 *@param dir_hor     水平方向
 */
void scui_custom_draw_image_indicator(scui_event_t *event, scui_area_t  *clip,
                                      scui_handle_t wait,  scui_color_t  color_wait,
                                      scui_handle_t focus, scui_color_t  color_focus,
                                      scui_handle_t count, scui_handle_t index,
                                      scui_handle_t span,  bool          dir_hor);

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param event  自定义控件事件
 *@param center 旋转中心
 *@param handle 图像句柄
 *@param color  图像源色调(调色板使用)
 *@param radius 旋转半径
 *@param angle  旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_custom_draw_image_circle_rotate(scui_event_t *event,  scui_point_t *center,
                                          scui_handle_t handle, scui_color_t  color,
                                          scui_coord_t  radius, scui_coord_t  angle);

/*@brief 自定义控件:插件:区域绘制
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_s,.color_e,)
 *@param mix   绘制模式(0:常规绘制;1:渐变绘制;)
 *@param way   绘制方向(渐变绘制)(0:水平渐变;1:垂直渐变;)
 */
void scui_custom_draw_area(scui_event_t *event, scui_area_t *clip,
                           scui_color_t  color, uint8_t mix, bool way);

#endif
