#ifndef SCUI_WINDOW_INF_H
#define SCUI_WINDOW_INF_H

/*@brief 窗口配置参数获取
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_get(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数设置
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_set(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数获取
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_get(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

/*@brief 窗口配置参数设置
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_set(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

/*@brief 窗口资源获取
 *@param handle    窗口控件句柄
 *@param local_res 窗口资源
 */
void scui_window_local_res_get(scui_handle_t handle, void **local_res);

/*@brief 窗口资源设置
 *@param handle 窗口控件句柄
 *@param size   窗口资源大小
 */
void scui_window_local_res_set(scui_handle_t handle, scui_handle_t size);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口切换风格实例
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_cfg_type(scui_window_switch_type_t **cfg_type);

/*@brief 窗口切换参数实例
 *@param cfg_args 实例
 */
void scui_window_switch_cfg_args(scui_window_switch_args_t **cfg_args);

/*@brief 窗口切换方向实例
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_cfg_dir(scui_opt_dir_t **cfg_dir);

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle);

/*@brief 获得活跃窗口句柄
 *@param index 0:当前活跃;1:上一活跃;2~n以此类推
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(scui_handle_t index);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口栈深度
 *@param top 栈深
 */
void scui_window_stack_nest(scui_handle_t *top);

/*@brief 获得次级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_last(scui_handle_t *handle);

/*@brief 获得顶级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_top(scui_handle_t *handle);

/*@brief 窗口栈复位
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_reset(scui_handle_t handle, bool reserve);

/*@brief 窗口栈覆盖
 *@param handle 窗口句柄
 */
void scui_window_stack_cover(scui_handle_t handle);

/*@brief 窗口栈添加
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_add(scui_handle_t handle, bool reserve);

/*@brief 窗口栈移除
 *@param handle 目标窗口
 */
void scui_window_stack_del(scui_handle_t handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 *@retval 成功失败
 */
bool scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
