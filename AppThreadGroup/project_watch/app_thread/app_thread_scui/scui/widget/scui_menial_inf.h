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
/* 属性过渡参考配置模板: */

/*@brief 绘制经典按钮
 *@param handle 对象控件句柄
 *@retval 成功失败
 */
bool scui_menial_btn_draw(scui_handle_t handle);

/*@brief 属性过渡配置
 *@param handle 控件句柄
 */
void scui_menial_btn_crect(scui_handle_t handle);

/*@brief 属性过渡配置
 *@param handle 控件句柄
 */
void scui_menial_btn_prop_tran(scui_handle_t handle);

#endif
