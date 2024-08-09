#ifndef SCUI_WINDOW_SWITCH_H
#define SCUI_WINDOW_SWITCH_H

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 */
void scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event);

#endif
