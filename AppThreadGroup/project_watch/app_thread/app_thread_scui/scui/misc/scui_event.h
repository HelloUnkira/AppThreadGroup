#ifndef SCUI_EVENT_H
#define SCUI_EVENT_H

/*@brief 事件类型
 */
typedef uint16_t scui_event_type_t;

/*@brief 事件优先级(数字越大优先级越高)
 */
typedef enum {
    scui_event_priority_none            = 0x00,
    scui_event_priority_default         = scui_event_priority_none,
    scui_event_priority_lowest          = 0x01,
    scui_event_priority_highest         = 0xFE,
    scui_event_priority_normal          = 0xFF / 2,
    scui_event_priority_normal_below    = scui_event_priority_normal - 0x0F,
    scui_event_priority_normal_above    = scui_event_priority_normal + 0x0F,
    scui_event_priority_real_time       = 0xFF,
} scui_event_priority_t;

/*@brief 事件状态风格
 */
typedef struct {
    uint64_t sync:1;            /* 事件响应调度: 同步调度,异步调度 */
    uint64_t order:2;           /* 事件响应顺序: 0x00:准备步; 0x01:处理步; 0x02:完成步; */
    uint64_t result:2;          /* 事件响应结果: 0x00:未处理; 0x01:已问询; 0x02:已吸收; */
    uint64_t priority:8;        /* 事件优先级别: 数字越大优先级越高 */
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
        /* 动画嘀嗒次数 */
        /* 输入设备数据<ptr> */
        struct {
            scui_point_t ptr_c;
            scui_coord_t ptr_cnt;
            scui_coord_t ptr_tick;
        };
        struct {
            scui_point_t ptr_s;
            scui_point_t ptr_e;
        };
        /* 输入设备数据<enc> */
        struct {
            scui_coord_t enc_diff;
        };
        /* 输入设备数据<key> */
        struct {
            scui_coord_t key_id;
            scui_coord_t key_val;
            scui_coord_t key_cnt;
            scui_coord_t key_tick;
        };
        /* scui定制化事件数据: */
        scui_area_t  area;
        scui_point_t point;
        scui_coord_t coord;
        scui_coord_t tick;
        /* scui定制调度事件数据: */
        struct {
            void (*sched)(scui_handle_t handle);
            scui_handle_t handle;
        };
        /* 通用结构信息(数据量不定,支持任意类型和种类的数据): */
        struct {
            uint64_t dynamic:1;     /* 协议数据流是动态生成,使用完毕要回收 */
            uint64_t size:24;       /* 协议数据流大小(16M) */
            void    *data;          /* 协议数据流(浅拷贝) */
        };
        /* 扩展迷你信息(数据量极少,通常表示状态类数据): */
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
