#ifndef SCUI_WIDGET_OPT_H
#define SCUI_WIDGET_OPT_H

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle);

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_style_is_hide(scui_handle_t handle);

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件移动到目标坐标
 *@param handle 控件句柄
 */
void scui_widget_move(scui_handle_t handle, scui_point_t *point);

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle);

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle);

#endif
