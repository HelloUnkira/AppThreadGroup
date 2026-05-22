#ifndef SCUI_MENIAL_INF_H
#define SCUI_MENIAL_INF_H

/*@brief 仆从控件子类型
 *@param handle 控件句柄
 *@param type   子类型
 */
void scui_menial_type_sub(scui_handle_t handle, scui_menial_type_t *type);

/*@brief 仆从控件数据实例
 *@param handle 控件句柄
 *@param data   数据实例
 */
void scui_menial_data_inst(scui_handle_t handle, scui_menial_data_t **data);

/******************************************************************************/

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 *@param anim   动画更新
 */
void scui_menial_arc_update_angle(scui_handle_t handle, scui_coord3_t angle, bool anim);

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 */
void scui_menial_arc_current_angle(scui_handle_t handle, scui_coord3_t *angle);

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, 100.0f]
 *@param anim   动画更新
 */
void scui_menial_arc_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_menial_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_menial_bar_current_value(scui_handle_t handle, scui_coord3_t *value);

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_menial_cht_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max);

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_menial_cht_line_data(scui_handle_t handle, scui_coord_t *vlist_dot);

#endif
