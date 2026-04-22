#ifndef SCUI_OBJECT_INF_H
#define SCUI_OBJECT_INF_H

/*@brief 对象控件移除属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_del(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_add(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件同步属性
 *@param handle 控件句柄
 *@param prop   控件属性
 *@retval 成功失败
 */
bool scui_object_prop_sync(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件移除过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_del(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件添加过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_add(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件添加过渡(取属性)
 *@param handle 控件句柄
 *@param tran   控件过渡
 *@retval 缺属性
 */
bool scui_object_tran_add_by(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件状态获取
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_get(scui_handle_t handle, scui_object_type_t *state);

/*@brief 对象控件状态设置
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_set(scui_handle_t handle, scui_object_type_t state);

#endif
