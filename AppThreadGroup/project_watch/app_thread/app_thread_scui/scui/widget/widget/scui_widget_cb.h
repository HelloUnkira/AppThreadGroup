#ifndef SCUI_WIDGET_CB_H
#define SCUI_WIDGET_CB_H

/*@brief 控件类型检查
 *@param handle 控件句柄
 *@param type   控件类型
 *@retval 控件类型检查
 */
bool scui_widget_type_check(scui_handle_t handle, scui_widget_type_t type);

/*@brief 查找控件映射表
 *@param type 控件类型
 *@param widget_cb 控件映射表
 */
void scui_widget_cb_find(scui_widget_type_t type, scui_widget_cb_t **widget_cb);

/*@brief 通过映射表调用创建一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 控件句柄
 */
void scui_widget_create_layout_tree(scui_handle_t handle);

#endif