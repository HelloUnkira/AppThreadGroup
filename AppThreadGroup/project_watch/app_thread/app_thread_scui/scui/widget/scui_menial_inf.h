#ifndef SCUI_MENIAL_INF_H
#define SCUI_MENIAL_INF_H

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
 *@param anim   动画更新
 */
void scui_menial_bar_current_value(scui_handle_t handle, scui_coord3_t *value);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/* 自定义参考配置模板: */

/*@brief 事件处理回调(子类型)
 *       经典按钮样板
 *@param event 事件
 */
void scui_menial_btn_event_cb(scui_event_t *event);

#endif
