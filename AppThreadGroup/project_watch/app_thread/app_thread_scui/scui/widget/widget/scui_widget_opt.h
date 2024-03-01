#ifndef SCUI_WIDGET_OPT_H
#define SCUI_WIDGET_OPT_H

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

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_repos(scui_handle_t handle, scui_point_t *point);

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_resize(scui_handle_t handle, scui_coord_t width, scui_coord_t height);

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle);

/*@brief 控件隐藏
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle);

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_hide_without(scui_handle_t handle, scui_handle_t child);

/*@brief 控件树镜像(相对父控件)
 *@param handle 控件句柄
 */
void scui_widget_mirror(scui_handle_t handle);

#endif
