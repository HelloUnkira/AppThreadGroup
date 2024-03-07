#ifndef SCUI_EVENT_SYS_H
#define SCUI_EVENT_SYS_H

/*@brief 系统事件表
 */
typedef enum {
    scui_event_none = 0,
    scui_event_invalid = scui_event_none,
    scui_event_sys_s,
    
    /* 调度事件<s> */
    scui_event_sched_s,
    scui_event_sched_all,
    scui_event_anima_elapse,    /* 动画轮转调度 */
    scui_event_focus_get,       /* 窗口焦点获取事件 */
    scui_event_focus_lost,      /* 窗口焦点失去事件 */
    scui_event_show_delay,      /* 窗口显示事件(迟延到下一系统调度) */
    scui_event_hide_delay,      /* 窗口隐藏事件(迟延到下一系统调度) */
    scui_event_show,            /* 窗口显示事件 */
    scui_event_hide,            /* 窗口隐藏事件 */
    scui_event_draw,            /* 控件绘制事件 */
    scui_event_refr,            /* 界面刷新事件 */
    scui_event_sched_e,
    /* 调度事件<e> */
    
    /* 输入设备事件<ptr,s>: */
    scui_event_ptr_s,
    scui_event_ptr_all,
    scui_event_ptr_cover,   /* 覆盖事件<> */
    scui_event_ptr_down,    /* 按下事件<point> */
    scui_event_ptr_hold,    /* 持续事件<point>:通过参数通报持续时间 */
    scui_event_ptr_click,   /* 点击事件<point>:通过参数通报点击次数 */
    scui_event_ptr_fling,   /* 轻扫事件<pos_s, pos_e> */
    scui_event_ptr_move,    /* 移动事件<pos_s, pos_e> */
    scui_event_ptr_up,      /* 抬起事件<point> */
    scui_event_ptr_e,
    /* 输入设备事件<ptr,e>: */
    
    /* 输入设备事件<enc,s> */
    scui_event_enc_s,
    scui_event_enc_all,
    scui_event_enc_clockwise,       /* 正转<coord> */
    scui_event_enc_clockwise_anti,  /* 反转<coord> */
    scui_event_enc_e,
    /* 输入设备事件<enc,e> */
    
    /* 输入设备事件<key,s> */
    scui_event_key_s,
    scui_event_key_all,
    scui_event_key_down,    /* 按下事件<coord> */
    scui_event_key_hold,    /* 持续事件<coord>:通过参数通报持续时间 */
    scui_event_key_click,   /* 单击事件<coord>:通过参数通报点击次数 */
    scui_event_key_up,      /* 抬起事件<coord> */
    scui_event_key_e,
    /* 输入设备事件<key,e> */
    
    scui_event_sys_e,
    scui_event_sys_num,
} scui_event_sys_t;

#endif
