/*实现目标:
 *    窗口栈管理插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_stack_t scui_window_stack = {0};

/*@brief 窗口栈检查
 */
static void scui_window_stack_check(void)
{
    SCUI_ASSERT(scui_window_stack.top > 0);
    SCUI_ASSERT(scui_window_stack.top < SCUI_WINDOW_STACK_NEST);
    
    SCUI_LOG_INFO_RAW("window stack: ");
    for (uint8_t top = 0; top < scui_window_stack.top; top++) {
        SCUI_LOG_INFO_RAW("%u ", scui_window_stack.stack[top]);
    }
    SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
}

/*@brief 窗口栈更新栈顶窗口(内部接口)
 *@param handle 窗口句柄
 */
void scui_window_stack_update(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_window_stack.stack[scui_window_stack.top - 1] = handle;
    scui_window_stack_check();
}

/*@brief 窗口栈深度
 *@param top 栈深
 */
void scui_window_stack_nest(scui_handle_t *top)
{
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
    SCUI_ASSERT(scui_window_stack.top > 0 || !reserve);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_stack.stack[scui_window_stack.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    if (reserve) {
        scui_window_stack.top = 0;
        scui_window_stack.stack[scui_window_stack.top++] = handle;
        scui_window_stack.stack[scui_window_stack.top++] = handle_top;
    } else {
        scui_window_stack.top = 0;
        scui_window_stack.stack[scui_window_stack.top++] = handle;
    }
    
    scui_window_stack_check();
}

/*@brief 窗口栈覆盖
 *@param handle 窗口句柄
 */
void scui_window_stack_cover(scui_handle_t handle)
{
    SCUI_ASSERT(scui_window_stack.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_stack.stack[scui_window_stack.top - 1];
    scui_handle_t target_top = handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    scui_window_stack.stack[scui_window_stack.top - 1] = handle;
    scui_window_stack_check();
}

/*@brief 窗口栈添加
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_add(scui_handle_t handle, bool reserve)
{
    SCUI_ASSERT(scui_window_stack.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_stack.stack[scui_window_stack.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    if (reserve) {
        scui_window_stack.top++;
        scui_window_stack.stack[scui_window_stack.top - 2] = handle;
        scui_window_stack.stack[scui_window_stack.top - 1] = handle_top;
    } else {
        scui_window_stack.top++;
        scui_window_stack.stack[scui_window_stack.top - 1] = handle;
    }
    
    scui_window_stack_check();
}

/*@brief 窗口栈移除
 */
void scui_window_stack_del(void)
{
    SCUI_ASSERT(scui_window_stack.top > 1);
    scui_handle_t handle_top = scui_window_stack.stack[scui_window_stack.top - 2];
    scui_handle_t target_top = handle_top;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    scui_window_stack.top--;
    scui_window_stack_check();
}
