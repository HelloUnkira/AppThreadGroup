/*实现目标:
 *    事件管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 事件类型转标号字符串回调(可选支持,可能浪费空间占用) */
static scui_event_cb_type_stringify_t scui_event_cb_type_stringify = NULL;

/* 引擎有一个全局默认的先响应和后响应回调 */
/* 事件会依照类别分为系统事件与自定义事件 */
static scui_event_cb_t scui_event_cb_access = NULL;
static scui_event_cb_t scui_event_cb_custom = NULL;
static scui_event_cb_t scui_event_cb_finish = NULL;

/*@brief 事件类型转标记字符串
 *@param type 事件类型
 *@retval 标记字符串
 */
const char * scui_event_type_stringify(scui_event_type_t type)
{
    const char * type_stringify = NULL;
    if (scui_event_cb_type_stringify != NULL)
        type_stringify = scui_event_cb_type_stringify(type);
    
    // 不支持的和未识别的直接返回本身的数字字符串
    if (type_stringify == NULL) {
        static uint16_t stringify[10] = {0};
        snprintf(stringify, sizeof(stringify), "%d", type);
        return stringify;
    }
    return type_stringify;
}

/*@brief 事件引擎注册转标号字符串回调
 *@param type_stringify 回调
 */
void scui_event_register_type_stringify(scui_event_cb_type_stringify_t type_stringify)
{
    scui_event_cb_type_stringify = type_stringify;
}

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
        
        scui_event_widget_scroll_start,
        scui_event_widget_scroll_over,
        scui_event_widget_scroll_keep,
    };
    
    for (scui_handle_t idx = 0; idx < scui_arr_len(event_table); idx++)
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
    if (event->type >= scui_event_custom_s &&
        event->type <= scui_event_custom_e)
        return true;
    
    /* 绝大部分系统事件不需要响应prepare和finish */
    /* 系统事件中必须该表中事件才响应prepare和finish */
    static const uint32_t event_table[] = {
        scui_event_draw,
    };
    
    for (scui_handle_t idx = 0; idx < scui_arr_len(event_table); idx++)
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
        
        [scui_event_sched_delay]    = scui_event_prior_real,
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
    if ((event->type > scui_event_ptr_s && event->type < scui_event_ptr_e) ||
        (event->type > scui_event_enc_s && event->type < scui_event_enc_e) ||
        (event->type > scui_event_key_s && event->type < scui_event_key_e)) {
        event->style.prior = scui_event_prior_normal;
        return;
    }
    
    /* widget: */
    if (event->type > scui_event_widget_s &&
        event->type < scui_event_widget_e) {
        event->style.prior = scui_event_prior_above;
        return;
    }
    
    /* sys: */
    if (event->type > scui_event_sched_s &&
        event->type < scui_event_sched_e) {
        event->style.prior = event_sys_prior[event->type];
        if (event->style.prior != scui_event_prior_none)
            return;
    }
    
    /* default: */
    event->style.prior = scui_event_prior_normal;
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
        // scui_mem_record_statistic(false);
        #endif
        #if SCUI_MEM_RECORD_ANALYSIS
        // scui_mem_record_analysis(false);
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
    case scui_event_sched_delay:
        event->sched(event->handle);
        scui_event_mask_over(event);
        return;
    case scui_event_anima_elapse:
        if (event->object == SCUI_HANDLE_SYSTEM) {
            scui_anima_update();
            
            /* 全局滚动检查 */
            #if SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL
            if (scui_widget_global_scroll_flag(0x02, NULL))
                return;
            #endif
            
            /* 系统事件发给所有场景(同步) */
            event->style.sync = true;
            scui_window_list_event_notify(event);
            return;
        }
        break;
    case scui_event_refr:
        scui_window_refresh();
        scui_event_mask_over(event);
        return;
    case scui_event_lang_change: {
        if (event->object == SCUI_HANDLE_SYSTEM) {
            
            /* 系统事件发给所有场景(同步) */
            event->style.sync = true;
            scui_window_list_event_notify(event);
            return;
        }
        break;
    }
    default:
        break;
    }
    
    /* 系统事件发给活跃场景 */
    if (event->object == SCUI_HANDLE_SYSTEM)
        event->object  = scui_window_active_curr();
    
    /* 本事件无活跃场景接收 */
    if (scui_handle_unmap(event->object)) {
        // 存在控件树已经被回收的情况
        // 但是事件调度队列还存在控件树的事件未响应
        // 直接丢弃这个事件即可, 因为它还未来得及生效已经失效了, 无额外影响
        const char *type_stringify = scui_event_type_stringify(event->type);
        SCUI_LOG_ERROR("error widget %u %s", event->object, type_stringify);
        return;
    }
    
    /* 事件响应对象无效(未知情况?) */
    if (scui_handle_source(event->object) == NULL) {
        const char *type_stringify = scui_event_type_stringify(event->type);
        SCUI_LOG_ERROR("error widget %u %s", event->object, type_stringify);
        return;
    }
    
    /* 标记事件为未处理 */
    scui_event_mask_quit(event);
    
    /* 事件响应回调 */
    if (scui_event_cb_check(event))
        scui_event_cb_access(event);
    if (scui_event_check_over(event))
        return;
    
    /* 系统事件响应 */
    if (event->type >= scui_event_sys_s &&
        event->type <= scui_event_sys_e) {
        
        bool event_filter = false;
        /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器(ptr) */
        event_filter = event_filter || event->type == scui_event_ptr_hold;
        event_filter = event_filter || event->type == scui_event_ptr_move;
        event_filter = event_filter || event->type == scui_event_ptr_fling;
        /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器(key) */
        event_filter = event_filter || event->type == scui_event_key_hold;
        event_filter = event_filter || event->type == scui_event_key_click;
        
        // 系统事件调度工步:prepare
        if (scui_event_order_check(event)) {
            scui_event_mask_prepare(event);
            scui_widget_event_dispatch(event);
        }
        // 系统事件调度工步:execute
        if (true/* must execute */) {
            scui_event_mask_execute(event);
            scui_widget_event_dispatch(event);
        }
        // 系统事件调度工步:finish
        if (scui_event_order_check(event)) {
            scui_event_mask_finish(event);
            scui_widget_event_dispatch(event);
        }
        
        // 控件树调度结束, 检查事件是否处理完毕
        if (scui_event_check_over(event) && event_filter)
            return;
        
        event_filter = true;
        /* 有些事件仅仅为控件事件,默认不传递给场景管理器 */
        if (event->type >= scui_event_sched_s &&
            event->type <= scui_event_sched_e)
            event_filter = false;
        if (event->type >= scui_event_widget_s &&
            event->type <= scui_event_widget_e)
            event_filter = false;
        /* 有些事件仅仅为控件事件,默认不传递给场景管理器(sched) */
        event_filter = event_filter || event->type == scui_event_anima_elapse;
        
        if (event_filter) {
            
            // 系统事件调度工步:prepare
            if (scui_event_order_check(event)) {
                scui_event_mask_prepare(event);
                scui_window_event_dispatch(event);
            }
            // 系统事件调度工步:execute
            if (true/* must execute */) {
                scui_event_mask_execute(event);
                scui_window_event_dispatch(event);
            }
            // 系统事件调度工步:finish
            if (scui_event_order_check(event)) {
                scui_event_mask_finish(event);
                scui_window_event_dispatch(event);
            }
        }
        
        if (scui_event_check_over(event))
            return;
    }
    
    /* 自定义事件响应<custom> */
    if (event->type >= scui_event_custom_s &&
        event->type <= scui_event_custom_e) {
        
        // 系统事件调度工步:prepare
        if (scui_event_order_check(event)) {
            scui_event_mask_prepare(event);
            scui_event_cb_custom(event);
        }
        // 系统事件调度工步:execute
        if (scui_event_order_check(event)) {
            scui_event_mask_execute(event);
            scui_event_cb_custom(event);
        }
        // 系统事件调度工步:finish
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
    SCUI_LOG_ERROR("scui_event_ptr_s:%u", scui_event_ptr_s);
    SCUI_LOG_ERROR("scui_event_ptr_e:%u", scui_event_ptr_e);
    SCUI_LOG_ERROR("scui_event_enc_s:%u", scui_event_enc_s);
    SCUI_LOG_ERROR("scui_event_enc_e:%u", scui_event_enc_e);
    SCUI_LOG_ERROR("scui_event_key_s:%u", scui_event_key_s);
    SCUI_LOG_ERROR("scui_event_key_e:%u", scui_event_key_e);
    SCUI_LOG_ERROR("scui_event_custom_s:%u", scui_event_custom_s);
    SCUI_LOG_ERROR("scui_event_custom_e:%u", scui_event_custom_e);
    #endif
    
    const char *type_stringify = scui_event_type_stringify(event->type);
    /* 未定义事件响应 */
    SCUI_LOG_ERROR("catch unknown event:");
    SCUI_LOG_ERROR("event->type:%s",            type_stringify);
    SCUI_LOG_ERROR("event->style:%u",           event->style);
    SCUI_LOG_ERROR("event->object:%u",          event->object);
    SCUI_LOG_ERROR("event->style.priority:%u",  event->style.prior);
}

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event)
{
    /* 同步事件就地响应 */
    if (event->style.sync) {
        
        scui_tick_calc(0x10, NULL, NULL, NULL);
        scui_event_respond(event);
        scui_tick_calc(0x11, NULL, NULL, NULL);
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
        
        scui_tick_calc(0x10, NULL, NULL, NULL);
        scui_event_respond(&event);
        scui_tick_calc(0x11, NULL, NULL, NULL);
        
        // 不要让此调度, 长期的占用CPU资源
        uint64_t tick_us_now = scui_tick_us();
        if (tick_us_now - tick_us > SCUI_EVENT_OCCUPY_LIMIT) {
            // 如果还有事件未处理
            // 通过一个空的事件去补充通知
            if (scui_event_num() != 0) {
                scui_event_t event_ui = {
                    .object = SCUI_HANDLE_SYSTEM,
                    .type   = scui_event_sched_none,
                    .absorb = scui_event_absorb_none,
                };
                scui_event_notify(&event_ui);
            }
            SCUI_LOG_WARN("occupy cpu too long");
            break;
        }
    }
}
