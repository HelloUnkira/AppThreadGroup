/*实现目标:
 *    窗口栈管理插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口栈记录
 *@param handle 窗口句柄
 */
static void scui_window_stack_record(scui_handle_t handle)
{
    /* 仅有效切换记录才被使用 */
    if (scui_window_mgr.stack_args.stack_rcd[0] == handle)
        return;
    
    for (scui_multi_t idx = SCUI_WINDOW_STACK_NEST - 1; idx - 1 >= 0; idx--)
        scui_window_mgr.stack_args.stack_rcd[idx] = 
        scui_window_mgr.stack_args.stack_rcd[idx - 1];
    
    scui_window_mgr.stack_args.stack_rcd[0] = handle;
}

/*@brief 窗口栈检查
 */
static void scui_window_stack_check(void)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(scui_window_mgr.stack_args.top < SCUI_WINDOW_STACK_NEST);
    
    uint8_t stack_top = 0;
    uint8_t stack_str[10 * SCUI_WINDOW_STACK_NEST] = {0};
    for (scui_multi_t top = 0; top < scui_window_mgr.stack_args.top; top++)
        stack_top += snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "%u ", scui_window_mgr.stack_args.stack[top]);
    
    SCUI_LOG_WARN("window stack: %s", stack_str);
    
    stack_top = 0;
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_STACK_NEST; idx++)
        stack_top += snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "%u ", scui_window_mgr.stack_args.stack_rcd[idx]);
    
    SCUI_LOG_WARN("window stack rcd: %s", stack_str);
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
        scui_window_stack_record(handle);
        break;
    case 1:
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
        scui_window_stack_record(handle);
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

/*@brief 获得窗口记录
 *@param handle 窗口句柄
 *@param index  索引(0:当前;1:最近;以此类推)
 */
void scui_window_stack_rcd(scui_handle_t *handle, scui_handle_t index)
{
    SCUI_ASSERT(index < SCUI_WINDOW_STACK_NEST);
    *handle = scui_window_mgr.stack_args.stack_rcd[index];
}

/*@brief 窗口栈复位
 *@param handle      窗口句柄
 *@param reserve     保留当前显示窗口
 *@param switch_type 切换类型
 *@param switch_dir  切换方向
 *@retval 成功失败
 */
bool scui_window_stack_reset_by(scui_handle_t handle, bool reserve, scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0 || !reserve);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, switch_type, switch_dir)) {
         SCUI_LOG_INFO("jump false");
         return false;
    }
    
    if (reserve) {
        scui_window_mgr.stack_args.top = 0;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle_top;
    } else {
        scui_window_mgr.stack_args.top = 0;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
    }
    
    scui_window_stack_record(target_top);
    scui_window_stack_check();
    return true;
}

/*@brief 窗口栈回退
 *@param handle      目标窗口
 *@param switch_type 切换类型
 *@param switch_dir  切换方向
 *@retval 成功失败
 */
bool scui_window_stack_goback_by(scui_handle_t handle, scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 1);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2];
    
    /* 回溯到最先的一个记录点 */
    for (scui_multi_t idx = 0; idx < scui_window_mgr.stack_args.top; idx++) {
        if (scui_window_mgr.stack_args.stack[idx] != handle)
            continue;
        
        scui_window_mgr.stack_args.top = idx + 1;
        target_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
        if (handle_top == target_top)
            return false;
        
        if (!scui_window_jump(target_top, switch_type, switch_dir)) {
             SCUI_LOG_INFO("jump false");
             return false;
        }
        
        scui_window_stack_record(target_top);
        scui_window_stack_check();
        return true;
    }
    
    return false;
}

/*@brief 窗口栈覆盖
 *@param handle      窗口句柄
 *@param switch_type 切换类型
 *@param switch_dir  切换方向
 *@retval 成功失败
 */
bool scui_window_stack_cover_by(scui_handle_t handle, scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = handle;
    if (!scui_window_jump(target_top, switch_type, switch_dir)) {
         SCUI_LOG_INFO("jump false");
         return false;
    }
    
    scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
    scui_window_stack_record(target_top);
    scui_window_stack_check();
    return true;
}

/*@brief 窗口栈添加
 *@param handle      窗口句柄
 *@param reserve     保留当前显示窗口
 *@param switch_type 切换类型
 *@param switch_dir  切换方向
 *@retval 成功失败
 */
bool scui_window_stack_add_by(scui_handle_t handle, bool reserve, scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = reserve ? handle_top : handle;
    if (!scui_window_jump(target_top, switch_type, switch_dir)) {
         SCUI_LOG_INFO("jump false");
         return false;
    }
    
    if (reserve) {
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2] = handle;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle_top;
    } else {
        scui_window_mgr.stack_args.top++;
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
    }
    
    scui_window_stack_record(target_top);
    scui_window_stack_check();
    return true;
}

/*@brief 窗口栈移除
 *@param handle      目标窗口
 *@param switch_type 切换类型
 *@param switch_dir  切换方向
 *@retval 成功失败
 */
bool scui_window_stack_del_by(scui_handle_t handle, scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir)
{
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 1);
    scui_handle_t handle_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    scui_handle_t target_top = scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2];
    
    /* 仅仅移除记录, 不涉及窗口调度 */
    if (handle != SCUI_HANDLE_INVALID && handle_top != handle) {
        
        bool handle_unhit = true;
        for (scui_multi_t idx = 0; idx < scui_window_mgr.stack_args.top; idx++) {
            if (scui_window_mgr.stack_args.stack[idx] != handle)
                continue;
            /* 后面的窗口覆盖前面的窗口 */
            for (idx = idx; idx + 1 < scui_window_mgr.stack_args.top; idx++)
                scui_window_mgr.stack_args.stack[idx] = scui_window_mgr.stack_args.stack[idx + 1];
            
            handle_unhit = false;
            break;
        }
        if (handle_unhit)
            return false;
        
        target_top = handle_top;
    }
    
    if (!scui_window_jump(target_top, switch_type, switch_dir)) {
         SCUI_LOG_INFO("jump false");
         return false;
    }
    scui_window_mgr.stack_args.top--;
    scui_window_stack_record(target_top);
    scui_window_stack_check();
    return true;
}
