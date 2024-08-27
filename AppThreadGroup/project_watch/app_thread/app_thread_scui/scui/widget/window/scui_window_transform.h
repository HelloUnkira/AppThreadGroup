#ifndef SCUI_WINDOW_TRANSFORM_H
#define SCUI_WINDOW_TRANSFORM_H

/*@brief 调整控件画布到绘制画布的剪切域
 *@param widget   控件实例
 *@param dst_clip 绘制画布剪切域
 *@param src_clip 控件画布剪切域
 *@retval 有效或无效
 */
bool scui_window_transform_clip(scui_widget_t *widget, scui_area_t *dst_clip, scui_area_t *src_clip);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_move(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cover(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_zoom(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_center_in_out(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_rotate(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_circle(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_grid(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip12(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip1(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip2(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip3(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cube(scui_widget_t **list, scui_handle_t num);

#endif
