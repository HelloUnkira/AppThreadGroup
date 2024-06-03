#ifndef SCUI_STRING_ARGS_H
#define SCUI_STRING_ARGS_H

/*@brief 绘制字符偏移计算
 *@param args   字符串绘制参数
 *@param glyph  字符
 *@param offset 偏移量
 */
void scui_string_args_offset(scui_string_args_t *args, scui_font_glyph_t *glyph, scui_point_t *offset);

/*@brief 字符串处理(变形,连字符等等)
 *@param args 字符串绘制参数
 */
void scui_string_args_process(scui_string_args_t *args);

#endif
