/*实现目标:
 *    事件管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 引擎有一个全局默认的先响应和后响应回调 */
static scui_event_cb_t scui_event_cb_prepare = NULL;
static scui_event_cb_t scui_event_cb_finish  = NULL;
/* 自定义事件使用回调注册 */
static scui_event_cb_t scui_event_cb_custom  = NULL;

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_prepare(scui_event_cb_t event_cb)
{
    scui_event_cb_prepare = event_cb;
}

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_finish(scui_event_cb_t event_cb)
{
    scui_event_cb_finish = event_cb;
}

/*@brief 事件响应注册(custom)
 *@param event_cb 事件回调
 */
void scui_event_register_custom(scui_event_cb_t event_cb)
{
    scui_event_cb_custom = event_cb;
}

/*@brief 事件类型转为字符串
 *@param 事件
 *@retval 字符串
 */
const char * scui_event_to_str(scui_event_type_t type)
{
    if (type >= scui_event_sys_s && type <= scui_event_sys_e)
        return scui_event_sys_to_str(type);
    
    if (type >= scui_event_custom_s && type <= scui_event_custom_e)
        return scui_event_custom_to_str(type);
    
    return "unknown event";
}

/*@brief 事件回调全局响应权限检查
 *       before和after的响应权限检查
 *@param event 事件包
 *@retval 允许该事件
 */
bool scui_event_cb_check(scui_event_t *event)
{
    /* 自定义事件一律允许响应before和after */
    if (event->type >= scui_event_custom_s &&
        event->type <= scui_event_custom_e)
        return true;
    
    /* 大部分系统事件不需要响应before和after */
    /* 系统事件中必须该表中事件才响应before和after */
    static const uint32_t event_table[] = {
        scui_event_ptr_hold,
        scui_event_key_hold,
        scui_event_ptr_move,
        scui_event_ptr_click,
        scui_event_ptr_fling,
        scui_event_key_click,
        scui_event_enc_clockwise,
        scui_event_enc_clockwise_anti,
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
    /* 异步事件入调度队列 */
    
    if (event->style.sync) {
        
        #if SCUI_EVENT_MGR_TICK_CHECK
        scui_tick_elapse_us(true);
        #endif
        
        scui_event_respond(event);
        
        #if SCUI_EVENT_MGR_TICK_CHECK
        uint64_t tick_us = scui_tick_elapse_us(false);
        if (tick_us > SCUI_EVENT_MGR_TICK_FILTER)
            SCUI_LOG_WARN("event %u expend:%u.%u", event->type, tick_us / 1000, tick_us % 1000);
        #endif
        
        return;
    }
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
        retval = scui_event_dequeue(&event, false);
        SCUI_ASSERT(retval);
        
        #if SCUI_EVENT_MGR_TICK_CHECK
        scui_tick_elapse_us(true);
        #endif
        
        scui_event_respond(&event);
        
        #if SCUI_EVENT_MGR_TICK_CHECK
        uint64_t tick_us = scui_tick_elapse_us(false);
        if (tick_us > SCUI_EVENT_MGR_TICK_FILTER)
            SCUI_LOG_WARN("event %u expend:%u.%u", event.type, tick_us / 1000, tick_us % 1000);
        #endif
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_respond(scui_event_t *event)
{
    SCUI_ASSERT(event->object != SCUI_HANDLE_INVALID);
    
    SCUI_ASSERT(scui_event_cb_prepare != NULL);
    SCUI_ASSERT(scui_event_cb_finish  != NULL);
    SCUI_ASSERT(scui_event_cb_custom != NULL);
    
    /* 内存监控 */
    #if SCUI_MEM_RECORD_CHECK
    scui_mem_check();
    #if SCUI_MEM_RECORD_STATISTIC
    scui_mem_record_statistic(false);
    #endif
    #if SCUI_MEM_RECORD_ANALYSIS
    scui_mem_record_analysis(false);
    #endif
    #endif
    
    /* 特殊系统事件处理 */
    /* 部分内部事件不允许正常控件监督流程 */
    /* 优先则走系统调度管理流程 */
    switch (event->type) {
    case scui_event_sched_delay:
        event->sched(event->handle);
        scui_widget_event_mask_over(event);
        return;
    case scui_event_anima_elapse:
        scui_anima_update(SCUI_HANDLE_INVALID);
        break;
    case scui_event_refr:
        scui_window_surface_blend();
        scui_widget_event_mask_over(event);
        /* 混合绘制刷新流程结束 */
        /* 使用假绘制启动正式的刷新流程 */
        scui_surface_draw_routine(NULL);
        /* 开始refr的switch模式同步 */
        scui_widget_t *widget = NULL;
        if (scui_window_surface_switch(0x02, &widget)) {
            SCUI_ASSERT(widget != NULL);
            SCUI_LOG_INFO("switch mode sync");
            scui_surface_t *surface_fb = scui_surface_fb_refr();
            scui_widget_surface_sync(widget, surface_fb);
        }
        return;
    default:
        break;
    }
    
    /* 系统事件只发给活跃场景 */
    if (event->object == SCUI_HANDLE_SYSTEM)
        event->object  = scui_window_active_curr();
    
    /* 本事件无活跃场景接收 */
    if (scui_handle_unmap(event->object)) {
        SCUI_LOG_WARN("unknown widget %u %u", event->object, event->type);
        return;
    }
    
    event->style.result = 0x00;
    
    /* 事件响应回调 */
    if (scui_event_cb_check(event)) {
        scui_event_cb_prepare(event);
        if (scui_widget_event_check_over(event))
            return;
    }
    
    /* 系统事件响应 */
    if (event->type >= scui_event_sys_s &&
        event->type <= scui_event_sys_e) {
        
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
        
        scui_widget_event_mask_prepare(event);
        scui_widget_event_dispatch(event);
        scui_widget_event_mask_execute(event);
        scui_widget_event_dispatch(event);
        scui_widget_event_mask_finish(event);
        scui_widget_event_dispatch(event);
        
        if (scui_widget_event_check_over(event) && event_filter)
            return;
        
        scui_widget_event_mask_prepare(event);
        scui_window_event_dispatch(event);
        scui_widget_event_mask_execute(event);
        scui_window_event_dispatch(event);
        scui_widget_event_mask_finish(event);
        scui_window_event_dispatch(event);
        
        if (scui_widget_event_check_over(event))
            return;
    }
    
    /* 自定义事件响应<custom> */
    if (event->type >= scui_event_custom_s &&
        event->type <= scui_event_custom_e) {
        
        scui_widget_event_mask_prepare(event);
        scui_event_cb_custom(event);
        scui_widget_event_mask_execute(event);
        scui_event_cb_custom(event);
        scui_widget_event_mask_finish(event);
        scui_event_cb_custom(event);
        
        if (scui_widget_event_check_over(event))
            return;
    }
    
    /* 事件响应回调 */
    if (scui_event_cb_check(event)) {
        scui_event_cb_finish(event);
        if (scui_widget_event_check_over(event))
            return;
    }
    
    if (event->style.result != 0)
        return;
    
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
    SCUI_LOG_ERROR("event->type:%u",            event->type);
    SCUI_LOG_ERROR("event->style:%u",           event->style);
    SCUI_LOG_ERROR("event->object:%u",          event->object);
    SCUI_LOG_ERROR("event->style.priority:%u",  event->style.priority);
}
