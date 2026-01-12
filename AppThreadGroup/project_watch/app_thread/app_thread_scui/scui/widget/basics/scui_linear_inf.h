#ifndef SCUI_LINEAR_INF_H
#define SCUI_LINEAR_INF_H

/*@brief 列表子控件事件响应回调(主)
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_get(scui_handle_t handle, scui_handle_t *handle_s);

/*@brief 列表子控件事件响应回调(主)
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_set(scui_handle_t handle, scui_handle_t *handle_s);

/*@brief 列表子控件事件响应回调(从)
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_get(scui_handle_t handle, scui_handle_t *handle_m);

/*@brief 列表子控件事件响应回调(从)
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_set(scui_handle_t handle, scui_handle_t *handle_m);

#endif
