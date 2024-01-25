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
    scui_event_retval_t retval = scui_event_retval_keep;
    SCUI_ASSERT(event->object != SCUI_HANDLE_INVALID);
    
    /* 系统事件只发给活跃场景 */
    if (event->object == SCUI_HANDLE_SYSTEM)
        event->object  = scui_scene_mgr_active_get();
    
    /* 本事件无活跃场景接收 */
    if (!scui_handle_remap(event->object)) {
         SCUI_LOG_WARN("unknown widget %u %u", event->object, SCUI_HANDLE_SYSTEM);
         return scui_event_retval_quit;
    }
    
    /* 事件前响应回调 */
    if (scui_event_cb_before != NULL) {
        scui_event_retval_t retval_b = scui_event_cb_before(event);
        if (retval_b == scui_event_retval_over)
            return scui_event_retval_over;
    }
    
    /* 调度事件响应 */
    if (event->type > scui_event_sched_s &&
        event->type < scui_event_sched_e)
        retval = scui_event_respond_sched(event);
    /* 输入事件响应<ptr> */
    if (event->type > scui_event_ptr_s &&
        event->type < scui_event_ptr_e)
        retval = scui_event_respond_ptr(event);
    /* 输入事件响应<enc> */
    if (event->type > scui_event_enc_s &&
        event->type < scui_event_enc_e)
        retval = scui_event_respond_enc(event);
    /* 输入事件响应<key> */
    if (event->type > scui_event_key_s &&
        event->type < scui_event_key_e)
        retval = scui_event_respond_key(event);
    /* 自定义事件响应<custom> */
    if (event->type > scui_event_custom_s &&
        event->type < scui_event_custom_e)
        retval = scui_event_respond_custom(event);
    
    if (retval == scui_event_retval_over)
        return scui_event_retval_over;
    
    /* 事件后响应回调 */
    if (scui_event_cb_after != NULL) {
        scui_event_retval_t retval_r = scui_event_cb_after(event);
        if (retval_r == scui_event_retval_over)
            return scui_event_retval_over;
    }
    
    if (retval != scui_event_retval_quit)
        return retval;
    
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
    return scui_event_retval_over;
}

/*@brief 事件响应(sched)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond_sched(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_anima_update();
    default:
        break;
    }
    
    scui_event_retval_t ret_1 = scui_widget_event_dispatch(event);
    if (ret_1 == scui_event_retval_over)
        return scui_event_retval_over;
    scui_event_retval_t ret_2 = scui_scene_mgr_event_dispatch(event);
    return ret_2 != scui_event_retval_quit ? ret_2 : ret_1;
}

/*@brief 事件响应(ptr)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond_ptr(scui_event_t *event)
{
    SCUI_LOG_INFO("");
    SCUI_LOG_DEBUG("scui event ptr:");
    SCUI_LOG_DEBUG("event->object:%d",    event->object);
    SCUI_LOG_DEBUG("event->type:%d",      event->type);
    SCUI_LOG_DEBUG("event->style:%d",     event->style);
    SCUI_LOG_DEBUG("event->priority:%d",  event->priority);
    
    bool event_filter = false;
    /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器 */
    event_filter = event_filter || event->type == scui_event_ptr_hold;
    event_filter = event_filter || event->type == scui_event_ptr_move;
    event_filter = event_filter || event->type == scui_event_ptr_fling;
    
    scui_event_retval_t ret_1 = scui_widget_event_dispatch(event);
    if (ret_1 == scui_event_retval_over && event_filter)
        return scui_event_retval_over;
    scui_event_retval_t ret_2 = scui_scene_mgr_event_dispatch(event);
    return ret_2 != scui_event_retval_quit ? ret_2 : ret_1;
}

/*@brief 事件响应(enc)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond_enc(scui_event_t *event)
{
    SCUI_LOG_INFO("");
    SCUI_LOG_DEBUG("scui event enc:");
    SCUI_LOG_DEBUG("event->object:%d",    event->object);
    SCUI_LOG_DEBUG("event->type:%d",      event->type);
    SCUI_LOG_DEBUG("event->style:%d",     event->style);
    SCUI_LOG_DEBUG("event->priority:%d",  event->priority);
    SCUI_LOG_DEBUG("event->enc_diff:%d",  event->enc_diff);
    
    bool event_filter = false;
    /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器 */
    
    scui_event_retval_t ret_1 = scui_widget_event_dispatch(event);
    if (ret_1 == scui_event_retval_over && event_filter)
        return scui_event_retval_over;
    scui_event_retval_t ret_2 = scui_scene_mgr_event_dispatch(event);
    return ret_2 != scui_event_retval_quit ? ret_2 : ret_1;
}

/*@brief 事件响应(key)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond_key(scui_event_t *event)
{
    SCUI_LOG_INFO("");
    SCUI_LOG_DEBUG("scui event key:");
    SCUI_LOG_DEBUG("event->object:%d",    event->object);
    SCUI_LOG_DEBUG("event->type:%d",      event->type);
    SCUI_LOG_DEBUG("event->style:%d",     event->style);
    SCUI_LOG_DEBUG("event->priority:%d",  event->priority);
    SCUI_LOG_DEBUG("event->key_id:%d",    event->key_id);
    SCUI_LOG_DEBUG("event->key_val:%d",   event->key_val);
    SCUI_LOG_DEBUG("event->key_cnt:%d",   event->key_cnt);
    SCUI_LOG_DEBUG("event->key_tick:%d",  event->key_tick);
    
    bool event_filter = false;
    /* 仅在特殊事件中才按需传递给场景管理器,默认都传递给场景管理器 */
    event_filter = event_filter || event->type == scui_event_key_hold;
    event_filter = event_filter || event->type == scui_event_key_click;
    
    scui_event_retval_t ret_1 = scui_widget_event_dispatch(event);
    if (ret_1 == scui_event_retval_over && event_filter)
        return scui_event_retval_over;
    scui_event_retval_t ret_2 = scui_scene_mgr_event_dispatch(event);
    return ret_2 != scui_event_retval_quit ? ret_2 : ret_1;
}

/*@brief 事件响应(custom)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
scui_event_retval_t scui_event_respond_custom(scui_event_t *event)
{
    SCUI_LOG_INFO("");
    SCUI_ASSERT(scui_event_cb_custom != NULL);
    return scui_event_cb_custom(event);
}
