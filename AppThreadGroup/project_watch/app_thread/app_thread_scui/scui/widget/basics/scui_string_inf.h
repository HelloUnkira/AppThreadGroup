#ifndef SCUI_STRING_INF_H
#define SCUI_STRING_INF_H

/*@brief 字符串控件文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_text(scui_handle_t handle, scui_handle_t *text);

/*@brief 字符串控件字符串
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_str_utf8(scui_handle_t handle, uint8_t **str_utf8);

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_text(scui_handle_t handle, scui_handle_t text);

/*@brief 字符串控件更新字符串
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_update_str(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 字符串控件更新字符串(重上色)
 *@param 使用#- -#包裹的内容为重上色区域
 *@param handle    字符串控件句柄
 *@param str_utf8  字符串(utf8)
 *@param color_num 重上色颜色数量
 *@param color_ll  重上色颜色列表
 */
void scui_string_update_str_rec(scui_handle_t handle, uint8_t *str_utf8, scui_coord_t color_num, scui_color_t *color_ll);

/*@brief 字符串控件渐变序列更新
 *@param handle 字符串控件句柄
 *@param grad_s 渐变序列
 *@param grad_n 渐变序列数量
 *@param grad_w 渐变序列方向(0:hor;1:ver;)
 */
void scui_string_upgrade_grads(scui_handle_t handle, scui_color_t *grad_s, scui_coord_t grad_n, bool grad_w);

/*@brief 字符串控件滚动中止
 *@param handle 字符串控件句柄
 *@param abort  中止标记
 */
void scui_string_scroll_abort(scui_handle_t handle, bool abort);

/*@brief 字符串控件滚动结束(单次滚动)
 *@param handle 字符串控件句柄
 *@retval 是否结束
 */
bool scui_string_scroll_over(scui_handle_t handle);

/*@brief 字符串控件修改文字尺寸(仅矢量字体有效)
 *@param handle 字符串控件句柄
 *@param size   新尺寸
 */
void scui_string_adjust_size(scui_handle_t handle, scui_coord_t size);

#endif
