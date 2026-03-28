#ifndef SCUI_XWATCH_H
#define SCUI_XWATCH_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t image[3];         /* 图片句柄(hour,minute,second) */
    scui_point_t  anchor[3];        /* 图片围绕轴心(hour,minute,second) */
    scui_point_t  center[3];        /* 图片旋转中心(hour,minute,second) */
    scui_sbitfd_t tick_mode:2;      /* 2:一帧一跳;1:一度一跳;0:一秒一跳; */
    scui_sbitfd_t use_always:1;     /* 永远使用外界时钟(完全可信) */
    
    uint8_t  (*get_h)(void);        /* 获取外界时回调 */
    uint8_t  (*get_m)(void);        /* 获取外界分回调 */
    uint8_t  (*get_s)(void);        /* 获取外界秒回调 */
    uint16_t (*get_ms)(void);       /* 获取外界毫秒回调 */
    /* 内部域: */
    scui_sbitfd_t tick_curr:30;     /* 当前时刻:时.分.秒.毫秒 */
    scui_sbitfd_t tick_last:30;     /* 前一时刻:时.分.秒.毫秒 */
    scui_sbitfd_t tick_sync:1;      /* 内部同步信号 */
} scui_xwatch_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t image[3];         /* 图片句柄(hour,minute,second) */
    scui_point_t  anchor[3];        /* 图片围绕轴心(hour,minute,second) */
    scui_point_t  center[3];        /* 图片旋转中心(hour,minute,second) */
    scui_sbitfd_t tick_mode:2;      /* 2:一帧一跳;1:一度一跳;0:一秒一跳; */
    scui_sbitfd_t use_always:1;     /* 永远使用外界时钟(完全可信) */
    
    uint8_t  (*get_h)(void);        /* 获取外界时回调 */
    uint8_t  (*get_m)(void);        /* 获取外界分回调 */
    uint8_t  (*get_s)(void);        /* 获取外界秒回调 */
    uint16_t (*get_ms)(void);       /* 获取外界毫秒回调 */
} scui_xwatch_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_xwatch_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_xwatch_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_xwatch_invoke(scui_event_t *event);

#endif
