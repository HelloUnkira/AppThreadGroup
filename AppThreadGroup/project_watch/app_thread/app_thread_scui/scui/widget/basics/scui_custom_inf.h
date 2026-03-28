#ifndef SCUI_CUSTOM_INF_H
#define SCUI_CUSTOM_INF_H

/*@brief 自定义控件文本绘制(不建议使用)
 *       备注:不建议使用,string控件更合适
 *@param handle 控件句柄
 *@param args   绘制参数(scui_string_args_t)
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, void *args, scui_handle_t text);

#endif
