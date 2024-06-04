#ifndef SCUI_DRAW_LETTER_H
#define SCUI_DRAW_LETTER_H

/*@brief 绘制字符
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_glyph   字符信息
 *@param src_clip    字符源绘制区域
 *@param src_alpha   字符透明度
 *@param src_color   字符色调
 *@param src_trans   字符滤色使能
 *@param src_filter  字符滤色
 */
void scui_draw_letter(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                      scui_font_glyph_t *src_glyph,   scui_area_t *src_clip,
                      scui_alpha_t       src_alpha,   scui_color_t src_color,
                      bool               src_trans,   scui_color_t src_filter);

#endif
