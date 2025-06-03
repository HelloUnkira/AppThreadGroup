#ifndef SCUI_ROLLER_INF_H
#define SCUI_ROLLER_INF_H

/*@brief 滚轮控件中心对齐子控件
 *@param handle 滚轮控件句柄
 *@param target 中心对齐子控件
 */
void scui_roller_center_target_get(scui_handle_t handle, scui_handle_t *target);

/*@brief 滚轮控件设置字符串
 *@param handle   滚轮控件句柄
 *@param maker    字符控件构造器
 *@param str_utf8 字符串(utf8)
 */
void scui_roller_string_str(scui_handle_t handle, scui_string_maker_t *maker, uint8_t *str_utf8);

#endif
