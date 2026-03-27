#ifndef SCUI_LINEAR_INF_H
#define SCUI_LINEAR_INF_H

/*@brief 列表主子控件获取从子控件树句柄
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_get(scui_handle_t handle, scui_handle_t *handle_s);

/*@brief 列表主子控件设置从子控件树句柄
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_set(scui_handle_t handle, scui_handle_t *handle_s);

/*@brief 列表从子控件树获取主子控件句柄
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_get(scui_handle_t handle, scui_handle_t *handle_m);

/*@brief 列表从子控件树设置主子控件句柄
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_set(scui_handle_t handle, scui_handle_t *handle_m);

/*@brief 列表从子控件树获得内存画布图
 *@param handle 控件
 *@param image  内存画布图
 */
void scui_linear_s_image(scui_handle_t handle, scui_handle_t *image);

#endif
