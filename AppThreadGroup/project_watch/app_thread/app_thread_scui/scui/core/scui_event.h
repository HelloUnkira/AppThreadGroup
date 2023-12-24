#ifndef SCUI_EVENT_H
#define SCUI_EVENT_H

typedef struct {
    app_sys_list_dln_t dl_node;
    /* 事件包吸收回调: */
    /* 如果手动交付该回调,则使用事件包吸收功能 */
    /* 新的事件包根据回调作用到旧有的一个上去,且丢弃本事件 */
    void (*absorb)(void *event_old, void *event_new);
    /*  */
    scui_handle_t object;   /* 事件对象 */
    uint64_t type:32;       /* 事件类型 */
    uint64_t style:8;       /* 事件风格 */
    uint64_t priority:8;    /* 事件优先级(数字越大优先级越高) */
    union {                 /* 事件参数 */
        scui_area_t  area;
        scui_point_t point;
        struct {
        scui_point_t pos_s;
        scui_point_t pos_e;
        };
    };
} scui_event_t;

typedef struct {
    app_sys_list_dll_t dl_list;
    uint32_t list_num;
} scui_event_queue_t;

/* 事件响应回调 */
typedef uint32_t (*scui_event_cb_t)(scui_event_t *event);

/*@brief 事件队列同步原语
 *@param lock   同步原语:上锁
 *@param unlock 同步原语:解锁
 *@param wait   同步原语:等待
 *@param notify 同步原语:通报
 */
void scui_event_sync_ready(void (*lock)(void), void (*unlock)(void),
                           void (*wait)(void), void (*notify)(void));

/*@brief 事件同步等待
 */
void scui_event_sync_wait(void);

/*@brief 事件队列事件数量
 *@retval 事件数量
 */
uint32_t scui_event_num(void);

/*@brief 事件包匹配函数
 *@param event 事件包
 */
void scui_event_enqueue(scui_event_t *event);

/*@brief 事件包匹配函数
 *@param event 事件包
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit);

#endif
