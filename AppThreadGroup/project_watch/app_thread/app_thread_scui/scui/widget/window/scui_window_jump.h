#ifndef SCUI_WINDOW_JUMP_H
#define SCUI_WINDOW_JUMP_H

/*@brief 窗口切换风格自动更新
 *@param switch_type 窗口切换风格
 *@param switch_dir  窗口切换方向
 */
void scui_window_jump_auto_update(scui_window_switch_type_t type, scui_event_dir_t dir);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_start(void *instance);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_ready(void *instance);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_expired(void *instance);

#endif
