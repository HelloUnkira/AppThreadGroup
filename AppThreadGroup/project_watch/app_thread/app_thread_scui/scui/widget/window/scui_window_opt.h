#ifndef SCUI_SCENE_OPT_H
#define SCUI_SCENE_OPT_H

/*@brief 窗口切换风格(配置)
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_type_cfg(scui_window_switch_type_t switch_type);

/*@brief 窗口切换方向(配置)
 *@param switch_dir 窗口切换方向
 */
void scui_window_switch_dir_cfg(scui_event_dir_t switch_dir);

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle);

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle);

/*@brief 窗口隐藏
 *@param handle 窗口句柄
 */
void scui_window_hide_without(scui_handle_t handle, bool any);

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(void);

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_curr(void);

/*@brief 获取滚动目标窗口
 *@retval 窗口句柄
 */
scui_handle_t scui_window_scroll_tar(void);

/*@brief 设置滚动目标窗口
 *@param handle 窗口句柄
 */
void scui_window_scroll_tar_update(scui_handle_t handle);

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle);

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 */
void scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_event_dir_t dir);

#endif
