#ifndef SCUI_EVENT_H
#define SCUI_EVENT_H

/*@brief 事件类型
 */
typedef uint32_t scui_event_type_t;

/*@brief 事件风格
 */
typedef enum {
    scui_event_style_async,     /* 常规异步调度 */
    scui_event_style_sync,      /* 就地同步调度 */
} scui_event_style_t;

/*@brief 事件优先级(数字越大优先级越高)
 */
typedef enum {
    scui_event_priority_none            = 0x00,
    scui_event_priority_default         = scui_event_priority_none,
    scui_event_priority_lowest          = 0x00,
    scui_event_priority_normal_below    = 0x0F,
    scui_event_priority_normal          = 0x8F,
    scui_event_priority_normal_above    = 0xF0,
    scui_event_priority_highest         = 0xFF,
    scui_event_priority_real_time       = 0xFF,
} scui_event_priority_t;

/*@brief 事件
 */
typedef struct {
    scui_list_dln_t dl_node;
    /* 系统基本字段 */
    scui_handle_t object;   /* 事件对象 */
    uint32_t type:20;       /* 事件类型 */
    uint32_t style:1;       /* 事件风格 */
    uint32_t priority:8;    /* 事件优先级(数字越大优先级越高) */
    /* 事件包吸收回调: */
    /* 如果手动交付该回调,则使用事件包吸收功能 */
    /* 新的事件包根据回调作用到旧有的一个上去,且丢弃本事件 */
    void (*absorb)(void *evt_old, void *evt_new);
    /* 扩展字段 */
    union {
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
        scui_area_t   area;
        scui_point_t  point;
        scui_coord_t  coord;
        scui_handle_t handle;
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

/*@brief 事件响应回调返回值
 */
typedef enum {
    scui_event_retval_none = 0,     /* 无效值:占位符 */
    scui_event_retval_quit,         /* 未处理:继续事件响应 */
    scui_event_retval_keep,         /* 已问询:继续事件响应 */
    scui_event_retval_over,         /* 已吸收:终止事件响应 */
} scui_event_retval_t;

/*@brief 事件响应回调
 */
typedef scui_event_retval_t (*scui_event_cb_t)(scui_event_t *event);

/*@brief 事件操作方向
 */
typedef enum {
    scui_event_dir_none = (0x00),
    scui_event_dir_to_u = (1 << 0),   // 向up
    scui_event_dir_to_d = (1 << 1),   // 向down
    scui_event_dir_to_l = (1 << 2),   // 向left
    scui_event_dir_to_r = (1 << 3),   // 向right
    scui_event_dir_ver  = (scui_event_dir_to_u | scui_event_dir_to_d),
    scui_event_dir_hor  = (scui_event_dir_to_l | scui_event_dir_to_r),
    scui_event_dir_all  = (scui_event_dir_ver  | scui_event_dir_hor ),
} scui_event_dir_t;

/*@brief 事件操作位置
 *       与事件操作方向单向透明切换
 */
typedef enum {
    scui_event_pos_none = scui_event_dir_none,
    scui_event_pos_u    = scui_event_dir_to_d,
    scui_event_pos_d    = scui_event_dir_to_u,
    scui_event_pos_l    = scui_event_dir_to_r,
    scui_event_pos_r    = scui_event_dir_to_l,
} scui_event_pos_t;

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
 *@retval 提取到有效事件包
 */
bool scui_event_dequeue(scui_event_t *event, bool hit);

/*@brief 事件队列事件数量
 *@retval 事件数量
 */
uint32_t scui_event_num(void);

#endif
