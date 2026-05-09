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
    /* 不打印历史窗口记录(有需要的时候才使用) */
    return;
    
    stack_top = 0;
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_STACK_NEST; idx++)
        stack_top += snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "%u ", scui_window_mgr.stack_args.stack_rcd[idx]);
    
    SCUI_LOG_WARN("window stack rcd: %s", stack_str);
}

/*@brief 窗口栈更新栈顶窗口(内部接口)
 *@param handle 窗口句柄
 */
void scui_window_stack_switch(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
    scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
    scui_window_stack_record(handle);
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

/*@brief 窗口栈入栈出栈语义
 *@param handle       窗口句柄
 *@param switch_type  切换类型
 *@param switch_dir   切换方向
 *@param add          常规入栈
 *@param del          常规出栈
 *@param prio         优先级入栈
 *@param clean        清空入栈
 *@param cover        出栈入栈
 *@param goback       回退出栈
 *@param prio_compare 优先级比较(返回:arg1 > arg2)
 *@param reserve      保留栈顶
 *@retval 成功失败
 */
bool scui_window_stack_jump(scui_handle_t handle,
    scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir,
    bool add, bool del, bool prio, bool clean, bool cover, bool goback,
    bool (*prio_compare)(scui_handle_t, scui_handle_t), bool reserve)
{
    bool result = false;
    scui_handle_t handle_top_b = scui_window_mgr.stack_args.top == 0 ? SCUI_HANDLE_INVALID :
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    
    /* 保留记录, 如果执行失败则撤回记录 */
    scui_handle_t stack_bak[SCUI_WINDOW_STACK_NEST] = {0};
    scui_handle_t stack_top_bak = scui_window_mgr.stack_args.top;
    for (scui_multi_t top = 0; top < scui_window_mgr.stack_args.top; top++)
        stack_bak[top] = scui_window_mgr.stack_args.stack[top];
    
    
    
    #if 1
    /* push语义 */
    
    /* cond 1: 清空栈后入栈 */
    if (clean) {
        result = true;
        scui_window_mgr.stack_args.top = 0;
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        if (reserve && handle_top_b != SCUI_HANDLE_INVALID) {
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle_top_b;
        } else {
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top++] = handle;
        }
        goto over;
    }
    /* cond 2: 弹出栈顶后入栈 */
    if (cover) {
        result = true;
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
        goto over;
    }
    /* cond 3: 优先级入栈 */
    if (prio) {
        result = true;
        scui_handle_t top_prio = 0;
        SCUI_ASSERT(prio_compare != NULL);
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
        /* 逆向查找搜索点位, 找到插入点, 默认插入到栈底 */
        for (scui_multi_t idx = scui_window_mgr.stack_args.top - 1; idx >= 0; idx--) {
            /* 如需保留栈顶元素, 则默认它的优先级无条件是最高的即可 */
            if (reserve && idx == scui_window_mgr.stack_args.top - 1) continue;
            /* 比较元素的优先级, 如果优先级相等则视为小于 */
            scui_handle_t handle_top = scui_window_mgr.stack_args.stack[idx];
            if (prio_compare(handle_top, handle)) continue;
            top_prio = idx;
            break;
        }
        /* 逆向迁移出搜索点位 */
        scui_window_mgr.stack_args.top++;
        for (scui_multi_t idx = scui_window_mgr.stack_args.top - 1; idx - 1 >= top_prio; idx--)
            scui_window_mgr.stack_args.stack[idx] = scui_window_mgr.stack_args.stack[idx - 1];
        
        /* 记录插入到这里 */
        scui_window_mgr.stack_args.stack[top_prio] = handle;
        goto over;
    }
    /* cond 4: 常规入栈 */
    if (add) {
        result = true;
        scui_window_mgr.stack_args.top++;
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        if (reserve && handle_top_b != SCUI_HANDLE_INVALID) {
            SCUI_ASSERT(scui_window_mgr.stack_args.top > 1);
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 2] = handle;
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle_top_b;
        } else {
            SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
            scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1] = handle;
        }
        goto over;
    }
    #endif
    
    
    
    #if 1
    /* pop语义 */
    
    /* cond 1: 弹出多个节点 */
    if (goback) {
        /* 回溯到最先的一个记录点 */
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        for (scui_multi_t idx = 0; idx < scui_window_mgr.stack_args.top; idx++) {
            if (scui_window_mgr.stack_args.stack[idx] != handle) continue;
            
            /* 将栈顶调整到此处 */
            scui_window_mgr.stack_args.top = idx + 1;
            result = true;
            break;
        }
        goto over;
    }
    
    /* cond 1: 弹出一个节点 */
    if (del) {
        /* 默认移除栈顶元素 */
        if (handle == SCUI_HANDLE_INVALID)
            handle  = handle_top_b;
        
        scui_handle_t top_del = SCUI_WINDOW_STACK_NEST;
        SCUI_ASSERT(scui_window_mgr.stack_args.top > 0);
        /* 逆序找到目标点, 清除掉该记录(只允许弹一次) */
        for (scui_multi_t idx = scui_window_mgr.stack_args.top - 1; idx >= 0; idx--) {
            if (scui_window_mgr.stack_args.stack[idx] != handle) continue;
            
            top_del = idx;
            break;
        }
        
        /* 没找到目标(放弃对该记录的处理) */
        if (top_del == SCUI_WINDOW_STACK_NEST)
            goto over;
        
        /* 逆向覆盖这个目标 */
        for (scui_multi_t idx = top_del; idx + 1 < scui_window_mgr.stack_args.top; idx++)
            scui_window_mgr.stack_args.stack[idx] = scui_window_mgr.stack_args.stack[idx + 1];
        
        scui_window_mgr.stack_args.top--;
        result = true;
        goto over;
    }
    #endif
    
    
    
    over:
    scui_handle_t handle_top_a = scui_window_mgr.stack_args.top == 0 ? SCUI_HANDLE_INVALID :
        scui_window_mgr.stack_args.stack[scui_window_mgr.stack_args.top - 1];
    
    if (handle_top_b != handle_top_a) {
        /* 仅仅栈顶元素发生改变才出现窗口切换 */
        SCUI_ASSERT(handle_top_a != SCUI_HANDLE_INVALID);
        if (!scui_window_switch_jump(handle_top_a, switch_type, switch_dir)) {
            
            /* 窗口跳转无法执行, 撤销上述的记录更变 */
            scui_window_mgr.stack_args.top = stack_top_bak;
            for (scui_multi_t top = 0; top < scui_window_mgr.stack_args.top; top++)
                scui_window_mgr.stack_args.stack[top] = stack_bak[top];
            
            SCUI_LOG_INFO("jump false");
            return false;
        }
        
        scui_window_stack_record(handle_top_a);
    }
    
    /* 仅记录有效更新当作才输出(栈不一定变更) */
    if (result) scui_window_stack_check();
    
    return result;
}
