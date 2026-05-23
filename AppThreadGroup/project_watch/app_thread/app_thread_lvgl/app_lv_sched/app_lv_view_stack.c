#include "app_lv_view_sched.h"

extern app_lv_view_sched_t app_lv_view_sched;

static void app_lv_view_stack_update_rcd(uint32_t page)
{
    for (int32_t idx = APP_LV_VIEW_STACK_NEST - 1; idx - 1 >= 0; idx--)
        app_lv_view_sched.stack_rcd[idx] = app_lv_view_sched.stack_rcd[idx - 1];
    
    app_lv_view_sched.stack_rcd[0] = page;
}

static void app_lv_view_stack_check(void)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    LV_ASSERT(app_lv_view_sched.stack_top < APP_LV_VIEW_STACK_NEST);
    
    uint8_t stack_top = 0;
    uint8_t stack_str[10 * APP_LV_VIEW_STACK_NEST] = {0};
    for (uint32_t top = 0; top < app_lv_view_sched.stack_top; top++)
        stack_top += lv_snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "%u ", app_lv_view_sched.stack[top]);
    
    LV_LOG_WARN("window stack: %s", stack_str);
    
    stack_top = 0;
    for (uint32_t top = 0; top < APP_LV_VIEW_STACK_NEST; top++)
        stack_top += lv_snprintf(stack_str + stack_top, sizeof(stack_str) - stack_top,
        "%u ", app_lv_view_sched.stack_rcd[top]);
    
    LV_LOG_WARN("window stack_rcd: %s", stack_str);
}

void app_lv_view_stack_update(uint32_t page, uint8_t type)
{
    LV_ASSERT(page != 0);
    uint32_t handle_top = 0;
    handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    
    switch (type) {
    case 0:
        app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1] = page;
        app_lv_view_stack_update_rcd(page);
        break;
    case 1:
        app_lv_view_sched.stack_top++;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1] = page;
        app_lv_view_stack_update_rcd(page);
        break;
    case 2:
        LV_ASSERT(handle_top == page);
        app_lv_view_sched.stack_top--;
        break;
    default:
        LV_LOG_ERROR("unknown type");
        LV_ASSERT(false);
        break;
    }
    
    app_lv_view_stack_check();
}

uint32_t app_lv_view_stack_nest(void)
{
    return app_lv_view_sched.stack_top;
}

uint32_t app_lv_view_stack_node(uint32_t top)
{
    LV_ASSERT(top > 0 && top <= app_lv_view_sched.stack_top);
    return app_lv_view_sched.stack[top - 1];
}

uint32_t app_lv_view_stack_last(void)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 1);
    LV_ASSERT(app_lv_view_sched.stack_top < APP_LV_VIEW_STACK_NEST);
    return app_lv_view_sched.stack[app_lv_view_sched.stack_top - 2];
}

uint32_t app_lv_view_stack_top(void)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    LV_ASSERT(app_lv_view_sched.stack_top < APP_LV_VIEW_STACK_NEST);
    return app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
}

uint32_t app_lv_view_stack_rcd_last(uint32_t idx)
{
    LV_ASSERT(idx < APP_LV_VIEW_STACK_NEST);
    return app_lv_view_sched.stack_rcd[idx];
}

/*@brief 下面的接口都是由stack的push/pop语义分化得出
 *         理论上是可以合并到一起去的, reserve是特殊语义(栈顶DIY)
 *
 *         例:reset语义 pop all + push one + (if(reserve)push stack_top old)
 *         例:goback语义 pop until...连续弹出多个节点
 *         例:cover语义 pop one + push one
 *         例:add语义 (if(reserve):pop stack_top) push one + (if(reserve):push stack_top)
 *         例:add_prio pop high nodes + push one + push high nodes back ---> adjust stack, insert one like
 *         例:del语义 pop one
 */


bool app_lv_view_stack_reset_by(uint32_t page, bool reserve, uint32_t transform_type, lv_dir_t dir)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0 || !reserve);
    LV_ASSERT(page != 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    uint32_t target_top = reserve ? handle_top : page;
    if (!app_lv_view_sched_jump(target_top, transform_type, dir)) {
         LV_LOG_INFO("jump false");
         return false;
    }
    
    if (reserve) {
        app_lv_view_sched.stack_top = 0;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top++] = page;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top++] = handle_top;
    } else {
        app_lv_view_sched.stack_top = 0;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top++] = page;
    }
    
    app_lv_view_stack_update_rcd(target_top);
    app_lv_view_stack_check();
    return true;
}

bool app_lv_view_stack_goback_by(uint32_t page, uint32_t transform_type, lv_dir_t dir)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    uint32_t target_top = 0;
    
    /* 追溯到最先到达的位置, 并且停留在此处 */
    for (uint32_t idx = 0; idx < app_lv_view_sched.stack_top; idx++) {
        if (app_lv_view_sched.stack[idx] != page)
            continue;
        
        app_lv_view_sched.stack_top = idx + 1;
        target_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
        if (!app_lv_view_sched_jump(target_top, transform_type, dir)) {
             LV_LOG_INFO("jump false");
             return false;
        }
        
        app_lv_view_stack_update_rcd(target_top);
        app_lv_view_stack_check();
        return true;
    }
    
    return false;
}

bool app_lv_view_stack_cover_by(uint32_t page, uint32_t transform_type, lv_dir_t dir)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    LV_ASSERT(page != 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    uint32_t target_top = page;
    if (!app_lv_view_sched_jump(target_top, transform_type, dir)) {
         LV_LOG_INFO("jump false");
         return false;
    }
    
    app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1] = page;
    app_lv_view_stack_update_rcd(target_top);
    app_lv_view_stack_check();
    return true;
}

bool app_lv_view_stack_add_by(uint32_t page, bool reserve, uint32_t transform_type, lv_dir_t dir)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    LV_ASSERT(page != 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    uint32_t target_top = reserve ? handle_top : page;
    if (!app_lv_view_sched_jump(target_top, transform_type, dir)) {
         LV_LOG_INFO("jump false");
         return false;
    }
    
    if (reserve) {
        app_lv_view_sched.stack_top++;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top - 2] = page;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1] = handle_top;
    } else {
        app_lv_view_sched.stack_top++;
        app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1] = page;
    }
    
    app_lv_view_stack_update_rcd(target_top);
    app_lv_view_stack_check();
    return true;
}

// prio_compare: return page > page_new
bool app_lv_view_stack_add_prio_by(uint32_t page, bool reserve, uint32_t transform_type, lv_dir_t dir,
                                 bool (*prio_compare)(uint32_t page, uint32_t page_new))
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    if (!prio_compare(handle_top, page))
        return app_lv_view_stack_add_by(page, reserve, transform_type, dir);
    
    // 栈顶特殊检查一次, 因为只有更变它才会产生真正的界面切换, 下面的优先级比较
    // 最多涉及到栈内部的层级信息更变
    
    // 从栈顶开始往下查
    for (int32_t top = app_lv_view_sched.stack_top; top - 1 >= 0; top--) {
        handle_top = app_lv_view_sched.stack[top - 1];
        if (prio_compare(handle_top, page)) continue;
        
        // 找到插入位置为top - 1 + 1
        // 倒着搬一下节点
        app_lv_view_sched.stack_top++;
        for (int32_t top_i = app_lv_view_sched.stack_top - 1; top_i - 1 > top - 1; top_i--)
            app_lv_view_sched.stack[top_i] = app_lv_view_sched.stack[top_i - 1];
        
        // 插入目标
        app_lv_view_sched.stack[top - 1 + 1] = page;
        app_lv_view_stack_check();
        return true;
    }
    
    app_lv_view_stack_check();
    return false;
}

bool app_lv_view_stack_del_by(uint32_t page, uint32_t transform_type, lv_dir_t dir)
{
    LV_ASSERT(app_lv_view_sched.stack_top > 0);
    uint32_t handle_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 1];
    
    /* 仅仅移除记录, 不涉及窗口调度 */
    if (page != 0 && handle_top != page) {
        
        bool handle_unhit = true;
        for (uint32_t idx = 0; idx < app_lv_view_sched.stack_top; idx++) {
            if (app_lv_view_sched.stack[idx] != page)
                continue;
            /* 后面的窗口覆盖前面的窗口 */
            for (idx = idx; idx + 1 < app_lv_view_sched.stack_top; idx++)
                app_lv_view_sched.stack[idx] = app_lv_view_sched.stack[idx + 1];
            
            handle_unhit = false;
            break;
        }
        if (handle_unhit)
            return true;
        
        /* 如果不是移除最顶层窗口, 这里直接退出 */
        app_lv_view_sched.stack_top--;
        app_lv_view_stack_check();
        return true;
    }
    
    /* 断言下放, 兼容用户可能的空释放 */
    LV_ASSERT(app_lv_view_sched.stack_top > 1);
    uint32_t target_top = app_lv_view_sched.stack[app_lv_view_sched.stack_top - 2];
    
    if (!app_lv_view_sched_jump(target_top, transform_type, dir)) {
         LV_LOG_INFO("jump false");
         return false;
    }
    app_lv_view_sched.stack_top--;
    app_lv_view_stack_update_rcd(target_top);
    app_lv_view_stack_check();
    return true;
}
