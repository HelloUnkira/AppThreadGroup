/*实现目标:
 *    事件管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 引擎有一个全局默认的先响应和后响应回调 */
/* 事件会依照类别分为系统事件与自定义事件 */
static scui_event_cb_t scui_event_cb_access = NULL;
static scui_event_cb_t scui_event_cb_custom = NULL;
static scui_event_cb_t scui_event_cb_finish = NULL;

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_access(scui_event_cb_t event_cb)
{
    scui_event_cb_access = event_cb;
}

/*@brief 事件响应注册(custom)
 *@param event_cb 事件回调
 */
void scui_event_register_custom(scui_event_cb_t event_cb)
{
    scui_event_cb_custom = event_cb;
}

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_finish(scui_event_cb_t event_cb)
{
    scui_event_cb_finish = event_cb;
}

/*@brief 事件回调全局响应权限检查
 *       before和after的响应权限检查
 *@param event 事件包
 *@retval 允许该事件
 */
static bool scui_event_cb_check(scui_event_t *event)
{
    /* 自定义事件一律允许响应before和after */
    if (scui_event_type_custom(event->type))
        return true;
    
    /* 大部分系统事件不需要响应before和after */
    /* 系统事件中必须该表中事件才响应before和after */
    static const uint32_t event_table[] = {
        scui_event_ptr_hold,
        scui_event_ptr_move,
        scui_event_ptr_click,
        scui_event_ptr_fling,
        scui_event_key_hold,
        scui_event_key_click,
        scui_event_enc_fdir,
        scui_event_enc_bdir,
        
        scui_event_focus_get,
        scui_event_focus_lost,
        
        scui_event_scroll_start,
        scui_event_scroll_over,
        scui_event_scroll_keep,
    };
    
    for (scui_multi_t idx = 0; idx < scui_arr_len(event_table); idx++)
        if (event->type == event_table[idx])
            return true;
    
    return false;
}

/*@brief 事件回调全局响应权限检查
 *       prepare和finish的响应权限检查
 *@param event 事件包
 *@retval 允许该事件
 */
static bool scui_event_order_check(scui_event_t *event)
{
    /* 自定义事件一律允许响应prepare和finish */
    if (scui_event_type_custom(event->type))
        return true;
    
    /* 绝大部分系统事件不需要响应prepare和finish */
    /* 系统事件中必须该表中事件才响应prepare和finish */
    static const uint32_t event_table[] = {
        scui_event_draw,
    };
    
    for (scui_multi_t idx = 0; idx < scui_arr_len(event_table); idx++)
        if (event->type == event_table[idx])
            return true;
    
    return false;
}

/*@brief 事件优先级更新
 *@param event 事件包
 */
static void scui_event_adjust_prior(scui_event_t *event)
{
    scui_event_prior_t event_sys_prior[scui_event_sys_num] = {
        
        [scui_event_sched_async]    = scui_event_prior_real,
        [scui_event_focus_lost]     = scui_event_prior_high,
        [scui_event_focus_get]      = scui_event_prior_high,
        [scui_event_show]           = scui_event_prior_high,
        [scui_event_hide]           = scui_event_prior_high,
        [scui_event_draw]           = scui_event_prior_low,
        [scui_event_draw_empty]     = scui_event_prior_above,
        [scui_event_refr]           = scui_event_prior_low,
        
        [scui_event_create]         = scui_event_prior_high,
        [scui_event_layout]         = scui_event_prior_above,
        [scui_event_destroy]        = scui_event_prior_high,
        
        [scui_event_child_nums]     = scui_event_prior_above,
        [scui_event_child_size]     = scui_event_prior_above,
        [scui_event_child_pos]      = scui_event_prior_above,
        
        [scui_event_size_auto]      = scui_event_prior_above,
        [scui_event_size_adjust]    = scui_event_prior_above,
        [scui_event_lang_change]    = scui_event_prior_above,
    };
    
    /* 已经指定的优先级不必额外配置 */
    if (event->style.prior != scui_event_prior_none)
        return;
    
    /* indev: ptr,enc,key */
    if (scui_event_type_ptr(event->type) ||
        scui_event_type_enc(event->type) ||
        scui_event_type_key(event->type)) {
        event->style.prior = scui_event_prior_normal;
        return;
    }
    
    /* widget: */
    if (scui_event_type_widget(event->type)) {
        event->style.prior = scui_event_prior_above;
        return;
    }
    
    /* sys: */
    if (scui_event_type_sched(event->type)) {
        event->style.prior = event_sys_prior[event->type];
        if (event->style.prior != scui_event_prior_none)
            return;
    }
    
    /* default: */
    event->style.prior = scui_event_prior_normal;
}

/*@brief 事件冒泡过滤
 *@param event 事件包
 *@retval 吸收与否
 */
static bool scui_event_bubble_filter(scui_event_t *event)
{
    /* 默认事件无条件传递给窗口管理器 */
    /* 仅特殊事件按需传递给窗口管理器 */
    switch (event->type) {
    case scui_event_ptr_hold:
    case scui_event_ptr_move:
    case scui_event_ptr_fling:
        return true;
    case scui_event_enc_fdir:
    case scui_event_enc_bdir:
        return true;
    case scui_event_key_hold:
    case scui_event_key_click:
        return true;
    }
    
    return false;
}

/*@brief 事件冒泡终止
 *@param event 事件包
 *@retval 吸收与否
 */
static bool scui_event_bubble_over(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        return true;
    }
    
    /* 控件事件默认不传递给窗口管理器 */
    if (scui_event_type_sched(event->type))
        return false;
    /* 控件事件默认不传递给窗口管理器 */
    if (scui_event_type_widget(event->type))
        return false;
    
    return true;
}

/*@brief 事件响应
 *@param event 事件包
 */
static void scui_event_respond(scui_event_t *event)
{
    SCUI_ASSERT(scui_event_cb_access != NULL);
    SCUI_ASSERT(scui_event_cb_finish != NULL);
    SCUI_ASSERT(scui_event_cb_custom != NULL);
    SCUI_ASSERT(event->object != SCUI_HANDLE_INVALID);
    
    /* 内存监控 */
    #if SCUI_MEM_RECORD_CHECK
    if (event->type == scui_event_draw) {
        #if SCUI_MEM_RECORD_STATISTIC
        /* scui_mem_record_statistic(false); */
        #endif
        #if SCUI_MEM_RECORD_ANALYSIS
        /* scui_mem_record_analysis(false); */
        #endif
        
        #if SCUI_MEM_SENTRY_CHECK
        scui_mem_sentry_check();
        #endif
    }
    #endif
    
    /* 特殊系统事件处理 */
    /* 部分内部事件不允许正常控件监督流程 */
    /* 优先则走系统调度管理流程 */
    switch (event->type) {
    case scui_event_sched_none:
        scui_event_mask_over(event);
        return;
    case scui_event_sched_async:
        event->sched(event->handle);
        scui_event_mask_over(event);
        return;
    case scui_event_anima_elapse:
        if (event->object == SCUI_HANDLE_SYSTEM) {
            scui_anima_update();
            
            scui_window_event_dispatch(event);
            scui_event_mask_over(event);
            return;
        }
        break;
    case scui_event_refr:
        scui_window_event_dispatch(event);
        scui_event_mask_over(event);
        return;
    case scui_event_lang_change:
        if (event->object == SCUI_HANDLE_SYSTEM) {
            scui_window_event_dispatch(event);
            scui_event_mask_over(event);
            return;
        }
        break;
    }
    
    /* 系统事件发给活跃窗口 */
    if (scui_event_type_sys(event->type)) {
        if (event->object == SCUI_HANDLE_SYSTEM)
            event->object  = scui_window_active_curr();
        
        /* 本事件无活跃窗口接收 */
        if (scui_handle_unmap(event->object)) {
            /* 存在控件树已经被回收的情况 */
            /* 但是事件调度队列还存在控件树的事件未响应 */
            /* 直接丢弃这个事件即可, 因为它还未来得及生效已经失效了, 无额外影响 */
            SCUI_LOG_ERROR("event %u widget %u error", event->type, event->object);
            return;
        }
        
        /* 事件响应对象无效(未知情况?) */
        if (scui_handle_source(event->object) == NULL) {
            SCUI_LOG_ERROR("event %u widget %u error", event->type, event->object);
            return;
        }
    }
    
    /* 标记事件为未处理 */
    scui_event_mask_quit(event);
    
    /* 事件响应回调 */
    if (scui_event_cb_check(event))
        scui_event_cb_access(event);
    if (scui_event_check_over(event))
        return;
    
    /* 系统事件响应<sys> */
    if (scui_event_type_sys(event->type)) {
        
        /* 系统事件调度工步:prepare */
        if (scui_event_order_check(event)) {
            scui_event_mask_prepare(event);
            scui_widget_event_dispatch(event);
        }
        /* 系统事件调度工步:execute */
        if (true/* must execute */) {
            scui_event_mask_execute(event);
            scui_widget_event_dispatch(event);
        }
        /* 系统事件调度工步:finish */
        if (scui_event_order_check(event)) {
            scui_event_mask_finish(event);
            scui_widget_event_dispatch(event);
        }
        
        /* 控件树调度结束, 检查事件是否处理完毕 */
        if (scui_event_check_over(event) &&
            scui_event_bubble_filter(event))
            return;
        
        if (scui_event_bubble_over(event)) {
            
            /* 系统事件调度工步:prepare */
            if (scui_event_order_check(event)) {
                scui_event_mask_prepare(event);
                scui_window_event_dispatch(event);
            }
            /* 系统事件调度工步:execute */
            if (true/* must execute */) {
                scui_event_mask_execute(event);
                scui_window_event_dispatch(event);
            }
            /* 系统事件调度工步:finish */
            if (scui_event_order_check(event)) {
                scui_event_mask_finish(event);
                scui_window_event_dispatch(event);
            }
        }
        
        if (scui_event_check_over(event))
            return;
    }
    
    /* 自定义事件响应<custom> */
    if (scui_event_type_custom(event->type)) {
        
        /* 系统事件调度工步:prepare */
        if (scui_event_order_check(event)) {
            scui_event_mask_prepare(event);
            scui_event_cb_custom(event);
        }
        /* 系统事件调度工步:execute */
        if (scui_event_order_check(event)) {
            scui_event_mask_execute(event);
            scui_event_cb_custom(event);
        }
        /* 系统事件调度工步:finish */
        if (scui_event_order_check(event)) {
            scui_event_mask_finish(event);
            scui_event_cb_custom(event);
        }
        
        if (scui_event_check_over(event))
            return;
    }
    
    /* 事件响应回调 */
    if (scui_event_cb_check(event))
        scui_event_cb_finish(event);
    if (scui_event_check_over(event))
        return;
    
    /* 不是未知事件, 流程结束 */
    if (!scui_event_check_quit(event))
         return;
    
    /* 参考事件区间 */
    #if 0
    SCUI_LOG_ERROR("scui_event_sched_s:%u", scui_event_sched_s);
    SCUI_LOG_ERROR("scui_event_sched_e:%u", scui_event_sched_e);
    SCUI_LOG_ERROR("scui_event_widget_s:%u", scui_event_widget_s);
    SCUI_LOG_ERROR("scui_event_widget_e:%u", scui_event_widget_e);
    SCUI_LOG_ERROR("scui_event_ptr_s:%u", scui_event_ptr_s);
    SCUI_LOG_ERROR("scui_event_ptr_e:%u", scui_event_ptr_e);
    SCUI_LOG_ERROR("scui_event_enc_s:%u", scui_event_enc_s);
    SCUI_LOG_ERROR("scui_event_enc_e:%u", scui_event_enc_e);
    SCUI_LOG_ERROR("scui_event_key_s:%u", scui_event_key_s);
    SCUI_LOG_ERROR("scui_event_key_e:%u", scui_event_key_e);
    SCUI_LOG_ERROR("scui_event_custom_s:%u", scui_event_custom_s);
    SCUI_LOG_ERROR("scui_event_custom_e:%u", scui_event_custom_e);
    #endif
    
    /* 未定义事件响应 */
    SCUI_LOG_ERROR("catch unknown event:");
    SCUI_LOG_ERROR("event->type:%u",   event->type);
    SCUI_LOG_ERROR("event->object:%u", event->object);
}

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event)
{
    /* 同步事件就地响应 */
    if (event->style.sync) {
        scui_tick_stat(scui_tick_stat_sched_rcd);
        scui_event_respond(event);
        scui_tick_stat(scui_tick_stat_sched_sum);
        return;
    }
    
    /* 为所有事件调整优先级 */
    scui_event_adjust_prior(event);
    /* 异步事件入调度队列 */
    scui_event_enqueue(event);
}

/*@brief 事件派发
 *@param event 事件包
 */
void scui_event_dispatch(void)
{
    uint64_t tick_us = scui_tick_us();
    
    scui_event_t event = {0};
    while (scui_event_num() != 0) {
        bool retval = scui_event_dequeue(&event, false, false);
        SCUI_ASSERT(retval);
        
        scui_tick_stat(scui_tick_stat_sched_rcd);
        scui_event_respond(&event);
        scui_tick_stat(scui_tick_stat_sched_sum);
        
        /* 不要让此调度, 长期的占用CPU资源 */
        uint64_t tick_us_now = scui_tick_us();
        if (tick_us_now - tick_us > SCUI_EVENT_OCCUPY_LIMIT) {
            /* 如果还有事件未处理 */
            /* 通过一个空的事件去补充通知 */
            if (scui_event_num() != 0) {
                scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false, scui_event_sched_none, NULL);
                scui_event_notify(&event_ui);
            }
            SCUI_LOG_WARN("occupy cpu too long");
            break;
        }
    }
}
