#ifndef SCUI_DRAW_STRING_H
#define SCUI_DRAW_STRING_H

/*@brief 绘制字符串
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_args    字符串绘制参数
 *@param src_clip    画布绘制区域
 *@param src_alpha   字符透明度
 */
void scui_draw_string(scui_surface_t     *dst_surface, scui_area_t *dst_clip,
                      scui_string_args_t *src_args,    scui_area_t *src_clip,
                      scui_alpha_t        src_alpha);

#endif