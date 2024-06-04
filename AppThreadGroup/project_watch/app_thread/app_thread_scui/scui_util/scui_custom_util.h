#ifndef SCUI_CUSTOM_UTIL_H
#define SCUI_CUSTOM_UTIL_H

/*@brief 自定义控件:插件:区域绘制
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_s,.color_e,)
 *@param mix   绘制模式(0:常规绘制;1:渐变绘制;)
 *@param way   绘制方向(渐变绘制)(0:水平渐变;1:垂直渐变;)
 */
void scui_custom_draw_area(scui_event_t *event, scui_area_t *clip,
                           scui_color_t  color, uint8_t mix, bool way);

/*@brief 自定义控件:插件:画线
 *@param event 自定义控件事件
 *@param color 颜色(.color,)
 *@param width 线条宽
 *@param pos_1 位置1
 *@param pos_2 位置2
 */
void scui_custom_draw_line(scui_event_t *event, scui_coord_t width,
                           scui_point_t  pos_1, scui_point_t pos_2,
                           scui_color_t  color);

#endif
