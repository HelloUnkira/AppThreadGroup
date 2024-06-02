#ifndef SCUI_STRING_ARGS_H
#define SCUI_STRING_ARGS_H

/*@brief 字符串预处理(变形,连字符等等)
 *@param args    字符串绘制参数
 *@param name    字库名字句柄
 *@param utf8    字符串(utf8)
 *@param unicode 字符串(unicode)
 */
void scui_string_args_prepare(scui_string_args_t *args, scui_handle_t name, uint8_t *utf8);

/*@brief 字符串排版布局清除
 *@param layout 排版布局
 */
void scui_string_args_layout_clear(void *layout);

/*@brief 字符串绘制信息清除
 *@param args 绘制信息清除
 */
void scui_string_args_clear(scui_string_args_t *args);

#endif
