#ifndef SCUI_EVENT_TABLE_H
#define SCUI_EVENT_TABLE_H

/* 事件表: */
typedef enum {
    /* 系统事件 */
    scui_event_none = 0,
    scui_event_show,
    scui_event_hide,
    scui_event_refr,
    scui_event_paint,
    scui_event_paint_overlay,
    scui_event_scene_focus_get,
    scui_event_scene_focus_lost,
    scui_event_scene_res_load,
    scui_event_scene_res_unload,
    
    
    /* 自定义事件<起始> */
    
    /* 自定义事件<结束> */
    
    scui_event_invalid = 0xFFFF,
} scui_event_table_t;





#endif
