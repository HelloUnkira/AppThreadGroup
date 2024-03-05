/*实现目标:
 *    事件管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 引擎有一个全局默认的先响应和后响应回调 */
static scui_event_cb_t scui_event_cb_before = NULL;
static scui_event_cb_t scui_event_cb_after = NULL;
/* 自定义事件使用回调注册 */
static scui_event_cb_t scui_event_cb_custom = NULL;

/*@brief 事件引擎注册先响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_before(scui_event_cb_t event_cb)
{
    scui_event_cb_before = event_cb;
}

/*@brief 事件引擎注册后响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_after(scui_event_cb_t event_cb)
{
    scui_event_cb_after = event_cb;
}

/*@brief 事件响应注册(custom)
 *@param event_cb 事件回调
 */
void scui_event_register_custom(scui_event_cb_t event_cb)
{
    scui_event_cb_custom = event_cb;
}

/*@brief 事件回调全局响应权限检查
 *       before和after的响应权限检查
 *@param event 事件包
 *@retval 允许该事件
 */
bool scui_event_cb_check(scui_event_t *event)
{
    /* 自定义事件一律允许响应before和after */
    if (event->type > scui_event_custom_s &&
        event->type < scui_event_custom_e)
        return true;
    
    /* 大部分系统事件不需要响应before和after */
    /* 系统事件中必须该表中事件才响应before和after */
    static const uint32_t event_table[] = {
        scui_event_ptr_hold,
        scui_event_ptr_click,
        scui_event_ptr_fling,
        scui_event_ptr_move,
        scui_event_enc_clockwise,
        scui_event_enc_clockwise_anti,
        scui_event_key_hold,
        scui_event_key_click,
    };
    
    for (uint32_t idx = 0; idx < scui_arr_len(event_table); idx++)
        if (event->type == event_table[idx])
            return true;
    
    return false;
}

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event)
{
    /* 同步事件就地响应 */
    if (event->style == scui_event_style_sync) { 
        scui_event_respond(event);
        return;
    }
    /* 异步事件入调度队列 */
    scui_event_enqueue(event);
}

/*@brief 事件派发
 *@param event 事件包
 */
void scui_event_dispatch(void)
{
    bool retval = true;
    scui_event_t event = {0};
    while (scui_event_num() != 0) {
        /* 按顺序提取出一个事件 */
        retval = scui_event_dequeue(&event, false);
        SCUI_ASSERT(retval);
        scui_event_respond(&event);
    }
}

/*@brief 事件响应
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond(scui_event_t *event)
{
    scui_event_retval_t ret = scui_event_retval_quit;
    SCUI_ASSERT(event->object != SCUI_HANDLE_INVALID);
    
    SCUI_ASSERT(scui_event_cb_before != NULL);
    SCUI_ASSERT(scui_event_cb_after  != NULL);
    SCUI_ASSERT(scui_event_cb_custom != NULL);
    
    /* 特殊系统事件处理 */
    /* 部分内部事件不允许正常控件监督流程 */
    /* 优先则走系统调度管理流程 */
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_anima_update();
        break;
    case scui_event_show_delay:
        scui_widget_show(event->handle);
        ret |= scui_event_retval_over;
        return ret;
    case scui_event_hide_delay:
        scui_widget_hide(event->handle);
        ret |= scui_event_retval_over;
        return ret;
    default:
        break;
    }
    
    /* 系统事件只发给活跃场景 */
    if (event->object == SCUI_HANDLE_SYSTEM)
        event->object  = scui_window_active_curr();
    
    /* 本事件无活跃场景接收 */
    if (scui_handle_unmap(event->object)) {
        SCUI_LOG_WARN("unknown widget %u %u", event->object, event->type);
        return ret;
    }
    
    /* 事件前响应回调 */
    if (scui_event_cb_check(event))
        ret |= scui_event_cb_before(event);
    if ((ret & scui_event_retval_over) != 0)
        return ret;
    
    /* 系统事件响应 */
    if (event->type > scui_event_sys_s && event->type < scui_event_sys_e) {
        
        bool event_filter = false;
        /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器(sched) */
        event_filter = event_filter || event->type == scui_event_anima_elapse;
        /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器(ptr) */
        event_filter = event_filter || event->type == scui_event_ptr_hold;
        event_filter = event_filter || event->type == scui_event_ptr_move;
        event_filter = event_filter || event->type == scui_event_ptr_fling;
        /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器(key) */
        event_filter = event_filter || event->type == scui_event_key_hold;
        event_filter = event_filter || event->type == scui_event_key_click;
        
        ret |= scui_widget_event_dispatch(event);
        if ((ret & scui_event_retval_over) != 0 && event_filter)
            return ret;
        ret |= scui_window_event_dispatch(event);
        if ((ret & scui_event_retval_over) != 0)
            return ret;
    }
    
    /* 自定义事件响应<custom> */
    if (event->type > scui_event_custom_s &&
        event->type < scui_event_custom_e)
        ret |= scui_event_cb_custom(event);
    if ((ret & scui_event_retval_over) != 0)
        return ret;
    
    /* 事件后响应回调 */
    if (scui_event_cb_check(event))
        ret |= scui_event_cb_after(event);
    if ((ret & scui_event_retval_over) != 0)
        return ret;
    
    if (ret != scui_event_retval_quit)
        return ret;
    
    /* 参考事件区间 */
    SCUI_LOG_ERROR("scui_event_sched_s:%u", scui_event_sched_s);
    SCUI_LOG_ERROR("scui_event_sched_e:%u", scui_event_sched_e);
    SCUI_LOG_ERROR("scui_event_ptr_s:%u", scui_event_ptr_s);
    SCUI_LOG_ERROR("scui_event_ptr_e:%u", scui_event_ptr_e);
    SCUI_LOG_ERROR("scui_event_enc_s:%u", scui_event_enc_s);
    SCUI_LOG_ERROR("scui_event_enc_e:%u", scui_event_enc_e);
    SCUI_LOG_ERROR("scui_event_key_s:%u", scui_event_key_s);
    SCUI_LOG_ERROR("scui_event_key_e:%u", scui_event_key_e);
    SCUI_LOG_ERROR("scui_event_custom_s:%u", scui_event_custom_s);
    SCUI_LOG_ERROR("scui_event_custom_e:%u", scui_event_custom_e);
    /* 未定义事件响应 */
    SCUI_LOG_ERROR("catch unknown event:");
    SCUI_LOG_ERROR("event->type:%u",     event->type);
    SCUI_LOG_ERROR("event->style:%u",    event->style);
    SCUI_LOG_ERROR("event->object:%u",   event->object);
    SCUI_LOG_ERROR("event->priority:%u", event->priority);
    return scui_event_retval_quit;
}
