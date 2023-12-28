/*实现目标:
 *    事件管理器
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
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
        APP_SYS_ASSERT(retval);
        scui_event_respond(&event);
    }
}

/*@brief 事件响应
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond(scui_event_t *event)
{
    uint32_t retval = SCUI_EVENT_CONTINUE;
    uint32_t retval_b = SCUI_EVENT_CONTINUE;
    uint32_t retval_r = SCUI_EVENT_CONTINUE;
    
    /* 事件前响应回调 */
    if (scui_event_cb_before != NULL) {
        retval_b = scui_event_cb_before(event);
        if (retval_b == SCUI_EVENT_BREAK)
            return SCUI_EVENT_BREAK;
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
    
    if (retval == SCUI_EVENT_BREAK)
        return SCUI_EVENT_BREAK;
    
    /* 事件后响应回调 */
    if (scui_event_cb_after != NULL) {
        retval_r = scui_event_cb_after(event);
        if (retval_r == SCUI_EVENT_BREAK)
            return SCUI_EVENT_BREAK;
    }
    
    if (retval != SCUI_EVENT_DEFAULT)
        return retval;
    
    /* 参考事件区间 */
    APP_SYS_LOG_ERROR("scui_event_sched_s:%u", scui_event_sched_s);
    APP_SYS_LOG_ERROR("scui_event_sched_e:%u", scui_event_sched_e);
    APP_SYS_LOG_ERROR("scui_event_ptr_s:%u", scui_event_ptr_s);
    APP_SYS_LOG_ERROR("scui_event_ptr_e:%u", scui_event_ptr_e);
    APP_SYS_LOG_ERROR("scui_event_enc_s:%u", scui_event_enc_s);
    APP_SYS_LOG_ERROR("scui_event_enc_e:%u", scui_event_enc_e);
    APP_SYS_LOG_ERROR("scui_event_key_s:%u", scui_event_key_s);
    APP_SYS_LOG_ERROR("scui_event_key_e:%u", scui_event_key_e);
    APP_SYS_LOG_ERROR("scui_event_custom_s:%u", scui_event_custom_s);
    APP_SYS_LOG_ERROR("scui_event_custom_e:%u", scui_event_custom_e);
    /* 未定义事件响应 */
    APP_SYS_LOG_ERROR("catch unknown event:");
    APP_SYS_LOG_ERROR("event->type:%u",     event->type);
    APP_SYS_LOG_ERROR("event->style:%u",    event->style);
    APP_SYS_LOG_ERROR("event->object:%u",   event->object);
    APP_SYS_LOG_ERROR("event->priority:%u", event->priority);
    return SCUI_EVENT_BREAK;
}

/*@brief 事件响应(sched)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_sched(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_anima_update();
        return SCUI_EVENT_BREAK;
    
    default:
        return SCUI_EVENT_DEFAULT;
    }
}

/*@brief 事件响应(ptr)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_ptr(scui_event_t *event)
{
    switch (event->type) {
    default:
        return SCUI_EVENT_DEFAULT;
    }
}

/*@brief 事件响应(enc)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_enc(scui_event_t *event)
{
    switch (event->type) {
    default:
        return SCUI_EVENT_DEFAULT;
    }
}

/*@brief 事件响应(key)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_key(scui_event_t *event)
{
    switch (event->type) {
    default:
        return SCUI_EVENT_DEFAULT;
    }
}

/*@brief 事件响应(custom)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_custom(scui_event_t *event)
{
    if (scui_event_cb_custom != NULL)
        return scui_event_cb_custom(event);
    return SCUI_EVENT_DEFAULT;
}
