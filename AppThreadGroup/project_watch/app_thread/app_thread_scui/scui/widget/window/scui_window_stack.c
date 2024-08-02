/*实现目标:
 *    窗口栈管理插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_stack_t scui_window_stack = {0};

/*@brief 窗口栈深度
 *@param top 栈深
 */
void scui_window_stack_nest(scui_handle_t *top)
{
    SCUI_ASSERT(scui_window_stack.top > 0);
    SCUI_ASSERT(scui_window_stack.top < SCUI_WINDOW_STACK_NEST);
    *top = scui_window_stack.top;
}

/*@brief 获得次级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_last(scui_handle_t *handle)
{
    SCUI_ASSERT(handle != NULL);
    SCUI_ASSERT(scui_window_stack.top > 1);
    SCUI_ASSERT(scui_window_stack.top < SCUI_WINDOW_STACK_NEST);
    *handle = scui_window_stack.stack[scui_window_stack.top - 2];
}

/*@brief 获得顶级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_top(scui_handle_t *handle)
{
    SCUI_ASSERT(handle != NULL);
    SCUI_ASSERT(scui_window_stack.top > 0);
    SCUI_ASSERT(scui_window_stack.top < SCUI_WINDOW_STACK_NEST);
    *handle = scui_window_stack.stack[scui_window_stack.top - 1];
}

/*@brief 窗口栈复位
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_reset(scui_handle_t handle, bool reserve)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    
    if (reserve) {
        /* 当前场景保留则只插入新场景 */
        scui_window_stack_top(&handle_top);
        scui_window_stack.top = 0;
        scui_window_stack.stack[scui_window_stack.top++] = handle;
        scui_window_stack.stack[scui_window_stack.top++] = handle_top;
    } else {
        scui_window_stack.top = 0;
        scui_window_stack.stack[scui_window_stack.top++] = handle;
    }
    
    scui_window_stack_top(&handle_top);
    scui_window_jump(handle_top, scui_window_switch_auto, scui_opt_dir_none);
}

/*@brief 窗口栈覆盖
 *@param handle 窗口句柄
 */
void scui_window_stack_cover(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    
    scui_window_stack_top(&handle_top);
    if (handle_top == handle)
        return;
    
    SCUI_ASSERT(scui_window_stack.top > 0);
    scui_window_stack.stack[scui_window_stack.top - 1] = handle;
    
    scui_window_stack_top(&handle_top);
    scui_window_jump(handle_top, scui_window_switch_auto, scui_opt_dir_none);
}

/*@brief 窗口栈添加
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_add(scui_handle_t handle, bool reserve)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    
    if (reserve) {
        /* 当前场景保留则只插入新场景 */
        scui_window_stack_top(&handle_top);
        scui_window_stack.top++;
        scui_window_stack.stack[scui_window_stack.top - 2] = handle;
        scui_window_stack.stack[scui_window_stack.top - 1] = handle_top;
    } else {
        scui_window_stack.top++;
        scui_window_stack.stack[scui_window_stack.top - 1] = handle;
    }
    
    scui_window_stack_top(&handle_top);
    scui_window_jump(handle_top, scui_window_switch_auto, scui_opt_dir_none);
}

/*@brief 窗口栈移除
 */
void scui_window_stack_del(void)
{
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    
    SCUI_ASSERT(scui_window_stack.top > 0);
    scui_window_stack.top--;
    
    scui_window_stack_top(&handle_top);
    scui_window_jump(handle_top, scui_window_switch_auto, scui_opt_dir_none);
}
