#ifndef SCUI_WINDOW_SWITCH_H
#define SCUI_WINDOW_SWITCH_H

/*@brief 窗口切换风格自动更新
 *@param type 窗口切换风格
 *@param dir  窗口切换方向
 */
void scui_window_switch_type_update(scui_window_switch_type_t type, scui_event_dir_t dir);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_start(void *instance);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_ready(void *instance);

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_expired(void *instance);

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_start(void *instance);

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_ready(void *instance);

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_expired(void *instance);

/*@brief 窗口移动动画自动化
 */
void scui_window_move_anima_auto(int32_t value_s, int32_t value_e, uint32_t peroid);

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
void scui_window_event_switch(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event);

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 */
void scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_event_dir_t dir);

#endif
