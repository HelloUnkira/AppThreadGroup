#ifndef SCUI_WINDOW_STACK_H
#define SCUI_WINDOW_STACK_H

typedef struct {
    scui_handle_t stack[SCUI_WINDOW_STACK_NEST];
    scui_handle_t top;
} scui_window_stack_t;

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
 */
void scui_window_stack_del(void);

#endif
