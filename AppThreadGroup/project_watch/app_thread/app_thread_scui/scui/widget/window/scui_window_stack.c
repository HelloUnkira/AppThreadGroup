/*实现目标:
 *    窗口栈管理插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;

/*@brief 窗口栈检查
 */
static void scui_window_stack_check(void)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(scui_window_mgr.stack_args.top < SCUI_WINDOW_STACK_NEST);
    
    uint8_t stack_top = 0;
    uint8_t stack_str[10 * SCUI_WINDOW_STACK_NEST] = {0};
    for (scui_handle_t top = 0; top < scui_window_mgr.stack_args.top; top++)
        stack_top += snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "0x%x ", scui_window_mgr.stack_args.stack[top]);
    
    SCUI_LOG_INFO("window stack: %s", stack_str);
}

/*@brief 窗口栈更新栈顶窗口(内部接口)
 *@param handle 窗口句柄
 *@param type   更新类型(0:cover;1:add;2:del;)
 */
void scui_window_stack_update(scui_handle_t handle, uint8_t type)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    
    switch (type) {
    case 0:
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
        break;
    case 1:
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
        break;
    case 2:
        SCUI_ASSERT(handle_top == handle);
        scui_window_mgr.stack_args.top--;
        break;
    default:
        SCUI_LOG_ERROR("unknown type");
        SCUI_ASSERT(false);
        break;
    }
    
    scui_window_stack_check();
}

/*@brief 窗口栈深度
 *@param top 栈深
 */
void scui_window_stack_nest(scui_handle_t *top)
{
    *top = scui_window_mgr.stack_args.top;
}

/*@brief 获得次级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_last(scui_handle_t *handle)
{
    SCUI_ASSERT(handle != NULL);
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 1);
    SCUI_ASSERT(scui_window_mgr.stack_args.top < SCUI_WINDOW_STACK_NEST);
    *handle = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2];
}

/*@brief 获得顶级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_top(scui_handle_t *handle)
{
    SCUI_ASSERT(handle != NULL);
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(scui_window_mgr.stack_args.top < SCUI_WINDOW_STACK_NEST);
    *handle = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
}

/*@brief 窗口栈复位
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_reset(scui_handle_t handle, bool reserve)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0 || !reserve);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    if (reserve) {
        scui_window_mgr.stack_args.top = 0;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle_top;
    } else {
        scui_window_mgr.stack_args.top = 0;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
    }
    
    scui_window_stack_check();
}

/*@brief 窗口栈覆盖
 *@param handle 窗口句柄
 */
void scui_window_stack_cover(scui_handle_t handle)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
    scui_window_stack_check();
}

/*@brief 窗口栈添加
 *@param handle  窗口句柄
 *@param reserve 保留当前显示窗口
 */
void scui_window_stack_add(scui_handle_t handle, bool reserve)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    
    if (reserve) {
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2] = handle;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle_top;
    } else {
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
    }
    
    scui_window_stack_check();
}

/*@brief 窗口栈移除
 *@param handle 目标窗口
 */
void scui_window_stack_del(scui_handle_t handle)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 1);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2];
    
    // 仅仅移除记录, 不涉及窗口调度
    if (handle != SCUI_HANDLE_INVALID && handle_top != handle) {
        
        bool handle_unhit = true;
        for (scui_handle_t idx = 0; idx < scui_window_mgr.stack_args.top; idx++) {
            if (scui_window_mgr.stack_args.stack[idx] != handle)
                continue;
            // 后面的窗口覆盖前面的窗口
            for (idx = idx; idx + 1 < scui_window_mgr.stack_args.top; idx++)
                scui_window_mgr.stack_args.stack[idx] = scui_window_mgr.stack_args.stack[idx + 1];
            
            handle_unhit = false;
            break;
        }
        if (handle_unhit)
            return;
        
        target_top = handle_top;
    }
    
    if (!scui_window_jump(target_top, scui_window_switch_auto, scui_opt_dir_none)) {
         SCUI_LOG_INFO("jump false");
         return;
    }
    scui_window_mgr.stack_args.top--;
    scui_window_stack_check();
}
