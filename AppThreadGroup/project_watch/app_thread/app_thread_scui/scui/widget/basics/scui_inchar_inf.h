#ifndef SCUI_INCHAR_INF_H
#define SCUI_INCHAR_INF_H

/*@brief 输入字符控件方向标记
 *@param handle 输入字符控件句柄
 *@param prev   方向标记(前向:真;后向:假)
 */
void scui_inchar_char_ins_way(scui_handle_t handle, bool prev);

/*@brief 输入字符控件指定位置输入字符
 *@param handle   输入字符控件句柄
 *@param index    字符索引
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_ins(scui_handle_t handle, scui_coord_t index, uint8_t *str_utf8);

/*@brief 输入字符控件指定位置替换字符
 *@param handle   输入字符控件句柄
 *@param index    字符索引(定点替换本身)
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_rep(scui_handle_t handle, scui_coord_t index, uint8_t *str_utf8);

/*@brief 输入字符控件指定位置删除字符
 *@param handle 输入字符控件句柄
 *@param index  字符索引(定点删除本身)
 */
void scui_inchar_char_rem(scui_handle_t handle, scui_coord_t index);

/*@brief 输入字符控件输入字符
 *@param handle   输入字符控件句柄
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_add(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 输入字符控件删除字符
 *@param handle 输入字符控件句柄
 */
void scui_inchar_char_del(scui_handle_t handle);

/*@brief 输入字符控件字符数量
 *@param handle 输入字符控件句柄
 *@param num    字符数量
 */
void scui_inchar_char_num(scui_handle_t handle, scui_coord_t *num);

/*@brief 输入字符控件utf8字节数量
 *@param handle 输入字符控件句柄
 *@param num    utf8字节数量
 */
void scui_inchar_str_num(scui_handle_t handle, scui_coord_t *num);

/*@brief 输入字符控件取出utf8
 *@param handle   输入字符控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_inchar_str_utf8(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 输入字符控件光标显示
 *@param handle 输入字符控件句柄
 *@param hide   隐藏标记(隐藏时固定末尾编辑)
 */
void scui_inchar_cursor(scui_handle_t handle, bool hide);

/*@brief 输入字符控件设置光标
 *@param handle 输入字符控件句柄
 *@param index  光标字符索引
 */
void scui_inchar_cursor_set(scui_handle_t handle, scui_coord_t index);

/*@brief 输入字符控件获取光标
 *@param handle 输入字符控件句柄
 *@param index  光标字符索引
 */
void scui_inchar_cursor_get(scui_handle_t handle, scui_coord_t *index);

#endif
