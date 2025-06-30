#ifndef SCUI_SCROLL_INF_H
#define SCUI_SCROLL_INF_H

/*@brief 滚动控件获取偏移量百分比(自动布局)
 *@param handle 滚动控件句柄
 *@param percent 偏移量
 */
void scui_scroll_percent_get(scui_handle_t handle, scui_coord_t *percent);

/*@brief 滚动控件获取偏移量(自动布局)
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 */
void scui_scroll_offset_get(scui_handle_t handle, scui_coord_t *offset);

/*@brief 滚动控件设置偏移量
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 *@param anima  动画模式
 */
void scui_scroll_offset(scui_handle_t handle, scui_point_t *offset, bool anima);

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 */
void scui_scroll_center_get(scui_handle_t handle, scui_handle_t *target);

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 *@param anima  动画模式
 */
void scui_scroll_center(scui_handle_t handle, scui_handle_t target, bool anima);

/*@brief 滚动控件边距(自由布局)
 *@param handle 滚动控件句柄
 *@param edge   滚动边距
 */
void scui_scroll_edge(scui_handle_t handle, scui_point_t *edge);

#endif
