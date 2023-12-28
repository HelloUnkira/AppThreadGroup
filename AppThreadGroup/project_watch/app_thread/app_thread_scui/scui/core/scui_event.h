#ifndef SCUI_EVENT_H
#define SCUI_EVENT_H

/* 系统事件表: */
typedef enum {
    scui_event_none = 0,
    scui_event_invalid = scui_event_none,
    
    /* 调度事件<s> */
    scui_event_sched_s,
    scui_event_anima_elapse,
    scui_event_show,
    scui_event_hide,
    scui_event_refr,
    scui_event_paint,
    scui_event_paint_overlay,
    scui_event_scene_focus_get,
    scui_event_scene_focus_lost,
    scui_event_scene_res_load,
    scui_event_scene_res_unload,
    scui_event_language_change,
    scui_event_sched_e,
    /* 调度事件<e> */
    
    /* 输入设备事件<ptr,s>: */
    scui_event_ptr_s,
    scui_event_ptr_cover,   /* 覆盖事件<>: */
    scui_event_ptr_down,    /* 按下事件<point>: */
    scui_event_ptr_single,  /* 单击事件<point>: */
    scui_event_ptr_double,  /* 双击事件<point>: */
    scui_event_ptr_fling,   /* 轻扫事件<pos_s, pos_e>: */
    scui_event_ptr_move,    /* 移动事件<pos_s, pos_e>: */
    scui_event_ptr_long,    /* 长按事件<point>: */
    scui_event_ptr_hold,    /* 持续事件<point>: */
    scui_event_ptr_up,      /* 抬起事件<point>: */
    scui_event_ptr_e,
    /* 输入设备事件<ptr,e>: */
    
    /* 输入设备事件<enc,s> */
    scui_event_enc_s,
    scui_event_enc_clockwise,       /* 正转<coord> */
    scui_event_enc_clockwise_anti,  /* 反转<coord> */
    scui_event_enc_e,
    /* 输入设备事件<enc,e> */
    
    /* 输入设备事件<key,s> */
    scui_event_key_s,
    scui_event_key_down,    /* 按下事件<coord> */
    scui_event_key_single,  /* 单击事件<coord> */
    scui_event_key_double,  /* 双击事件<coord> */
    scui_event_key_hold,    /* 持续事件<coord> */
    scui_event_key_long,    /* 长按事件<coord> */
    scui_event_key_up,      /* 抬起事件<coord> */
    scui_event_key_e,
    /* 输入设备事件<key,e> */
    
    scui_event_sys_num,
} scui_event_sys_t;

typedef enum {
    scui_event_style_async,     /* 常规异步调度 */
    scui_event_style_sync,      /* 就地同步调度 */
} scui_event_style_t;

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
        /* scui定制化事件数据: */
        scui_area_t  area;
        scui_point_t point;
        scui_coord_t coord;
        struct {
        scui_point_t pos_s;
        scui_point_t pos_e;
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

typedef struct {
    app_sem_t sem;
    app_mutex_t mutex;
    app_sys_list_dll_t dl_list;
    uint32_t list_num;
} scui_event_queue_t;

/* 事件响应回调 */
typedef uint32_t (*scui_event_cb_t)(scui_event_t *event);
/* 事件响应回调返回值 */
#define SCUI_EVENT_DEFAULT      0   /* 无效值 */
#define SCUI_EVENT_BREAK        1   /* 终止事件冒泡 */
#define SCUI_EVENT_CONTINUE     2   /* 继续事件冒泡 */

/*@brief 事件队列初始化
 */
void scui_event_ready(void);

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
