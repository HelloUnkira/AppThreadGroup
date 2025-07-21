#ifndef SCUI_EVENT_H
#define SCUI_EVENT_H

/*@brief 事件类型
 */
typedef uint16_t scui_event_type_t;

/*@brief 事件优先级(数字越大优先级越高)
 */
typedef enum {
    scui_event_prior_none   = 0x00,
    scui_event_prior_real   = 0xFF,
    scui_event_prior_normal = 0xFF / 2,
    scui_event_prior_below  = scui_event_prior_normal - 0x01,
    scui_event_prior_above  = scui_event_prior_normal + 0x01,
    scui_event_prior_low    = scui_event_prior_none + 0x01,
    scui_event_prior_high   = scui_event_prior_real - 0x01,
} scui_event_prior_t;

/*@brief 事件状态风格
 */
typedef struct {
    uintptr_t sync:1;           /* 事件响应调度: 同步调度,异步调度 */
    uintptr_t prior:8;          /* 事件优先级别: 数字越大优先级越高 */
    uintptr_t order:2;          /* 事件响应顺序: 0x00:准备步; 0x01:处理步; 0x02:完成步; */
    uintptr_t result:2;         /* 事件响应结果: 0x00:未处理; 0x01:已问询; 0x02:已吸收; */
    uintptr_t bubble:1;         /* 事件响应冒泡: 备注:!!特殊事件使用; false:不冒泡; true:冒泡; */
} scui_event_style_t;

/*@brief 事件
 */
typedef struct {
    scui_list_dln_t dl_node;
    /* 系统基本字段 */
    scui_handle_t      object;  /* 事件对象 */
    scui_event_style_t style;   /* 事件状态风格 */
    scui_event_type_t  type;    /* 事件类型 */
    /* 事件包吸收回调: */
    /* 如果手动交付该回调,则使用事件包吸收功能 */
    /* 新的事件包根据回调作用到旧有的一个上去,且丢弃本事件 */
    void (*absorb)(void *evt_old, void *evt_new);
    /* 扩展字段 */
    union {
        /* 输入事件数据<ptr> */
        struct {
            scui_point_t ptr_c;
            scui_coord_t ptr_cnt;
            scui_coord_t ptr_tick;
        };
        struct {
            scui_point_t ptr_s;
            scui_point_t ptr_e;
        };
        /* 输入事件数据<enc> */
        struct {
            scui_coord_t enc_diff;
        };
        /* 输入事件数据<key> */
        struct {
            scui_coord_t key_id;
            scui_coord_t key_val;
            scui_coord_t key_cnt;
            scui_coord_t key_tick;
        };
        /* scui迟延调度事件信息: */
        struct {
            void (*sched)(scui_handle_t handle);
            scui_handle_t handle;
        };
        /* scui动画事件嘀嗒次数 */
        scui_coord_t tick;
        /* scui其余事件信息: */
        // scui_area_t  area;
        // scui_point_t point;
        // scui_coord_t coord;
        
        /* 通用结构信息(协议数据流): */
        struct {
            uint64_t size;  /* 协议数据流大小 */
            void    *data;  /* 协议数据流 */
        };
        /* 扩展迷你信息(状态类数据): */
        uintptr_t byte_align;
        uint64_t  byte_fixed;
    };
} scui_event_t;

/*@brief 事件队列
 */
typedef struct {
    scui_sem_t sem;
    scui_mutex_t mutex;
    scui_list_dll_t dl_list;
    uint32_t list_num;
} scui_event_queue_t;

/*@brief 事件响应回调
 */
typedef void (*scui_event_cb_t)(scui_event_t *event);

/*@brief 事件回调列表
 */
typedef struct {
    scui_list_dll_t   dl_list;      /* 事件回调列表 */
} scui_event_cb_list_t;

/*@brief 事件回调节点
 */
typedef struct {
    scui_list_dln_t   dl_node;      /* 事件回调节点 */
    scui_event_cb_t   event_cb;     /* 事件回调 */
    scui_event_type_t event;        /* 事件 */
} scui_event_cb_node_t;

/* 标记事件访问状态 */
static inline void scui_event_mask_quit(scui_event_t *event)        {(event->style.result  = 0x00);}
static inline void scui_event_mask_keep(scui_event_t *event)        {(event->style.result |= 0x01);}
static inline void scui_event_mask_over(scui_event_t *event)        {(event->style.result |= 0x02);}
/* 标记事件访问流程 */
static inline void scui_event_mask_prepare(scui_event_t *event)     {event->style.order = 0x00;}
static inline void scui_event_mask_execute(scui_event_t *event)     {event->style.order = 0x01;}
static inline void scui_event_mask_finish(scui_event_t *event)      {event->style.order = 0x02;}

/* 检查事件访问状态 */
static inline bool scui_event_check_quit(scui_event_t *event)       {return (event->style.result) == 0x00;}
static inline bool scui_event_check_keep(scui_event_t *event)       {return (event->style.result & 0x01) != 0;}
static inline bool scui_event_check_over(scui_event_t *event)       {return (event->style.result & 0x02) != 0;}
/* 检查事件访问流程 */
static inline bool scui_event_check_prepare(scui_event_t *event)    {return event->style.order == 0x00;}
static inline bool scui_event_check_execute(scui_event_t *event)    {return event->style.order == 0x01;}
static inline bool scui_event_check_finish(scui_event_t *event)     {return event->style.order == 0x02;}

/*@brief 事件吸收回调(空吸收)
 *@param evt_old 旧事件
 *@param evt_new 新事件
 */
void scui_event_absorb_none(void *evt_old, void *evt_new);

/*@brief 事件队列初始化
 */
void scui_event_ready(void);

/*@brief 事件同步等待
 */
void scui_event_wait(void);

/*@brief 事件包入列函数
 *@param event 事件包
 */
void scui_event_enqueue(scui_event_t *event);

/*@brief 事件包出列函数
 *@param event 事件包
 *@param hit   命中指定对象(指定事件)
 *@param any   命中指定对象(非指定事件)
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit, bool any);

/*@brief 事件队列事件数量
 *@retval 事件数量
 */
uint32_t scui_event_num(void);

/*@brief 事件回调列表事件准备
 *@param cb_list 事件回调列表
 */
void scui_event_cb_ready(scui_event_cb_list_t *cb_list);

/*@brief 事件回调列表事件清空
 *@param cb_list 事件回调列表
 */
void scui_event_cb_clear(scui_event_cb_list_t *cb_list);

/*@brief 事件回调列表事件查找
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_find(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node);

/*@brief 事件回调列表事件添加
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_add(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node);

/*@brief 事件回调列表事件移除
 *@param cb_list 事件回调列表
 *@param cb_node 事件回调节点
 */
void scui_event_cb_del(scui_event_cb_list_t *cb_list, scui_event_cb_node_t *cb_node);

#endif
