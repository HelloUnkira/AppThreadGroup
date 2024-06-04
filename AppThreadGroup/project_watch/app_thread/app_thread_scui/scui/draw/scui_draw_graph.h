#ifndef SCUI_DRAW_GRAPH_H
#define SCUI_DRAW_GRAPH_H

/*绘制语义约定:
 *    dst_surface + dst_clip
 *    src_surface + src_clip
 *    将src_surface中的src_clip范围内的区域
 *    到dst_surface中的dst_clip范围内的区域
 *    且变换要求取最小交集区域
 *    即为原点对齐变换
 */

/*@brief 线条绘制(抗锯齿)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_color   源色调
 *@param src_width   线宽
 *@param src_pos_1   坐标点
 *@param src_pos_2   坐标点
 *@param src_alpha   透明度
 */
void scui_draw_line(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                    scui_color_t    src_color,   scui_coord_t src_width,
                    scui_point_t    src_pos_1,   scui_point_t src_pos_2,
                    scui_alpha_t    src_alpha);

#endif
