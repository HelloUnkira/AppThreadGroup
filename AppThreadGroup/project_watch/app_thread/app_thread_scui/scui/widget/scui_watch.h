#ifndef SCUI_WATCH_H
#define SCUI_WATCH_H

typedef struct {
    scui_widget_t widget;
    scui_handle_t image_h;      /* 图片句柄(hour) */
    scui_handle_t image_m;      /* 图片句柄(minute) */
    scui_handle_t image_s;      /* 图片句柄(second) */
    scui_point_t  anchor_h;     /* 图片旋转围绕轴心 */
    scui_point_t  center_h;     /* 图片旋转中心 */
    scui_point_t  anchor_m;     /* 图片旋转围绕轴心 */
    scui_point_t  center_m;     /* 图片旋转中心 */
    scui_point_t  anchor_s;     /* 图片旋转围绕轴心 */
    scui_point_t  center_s;     /* 图片旋转中心 */
    bool          tick_mode;    /* 一度一跳:1;一秒一跳:0; */
    /* 内部域: */
    uint64_t tick_ms_rcd:10;    /* 前一显示(ms) */
    uint64_t tick_ms:10;        /* 当前显示(ms) */
    uint64_t tick_s:6;          /* 当前显示(s) */
    uint64_t tick_m:6;          /* 当前显示(m) */
    uint64_t tick_h:5;          /* 当前显示(h) */
} scui_watch_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_handle_t       image_h;        /* 图片句柄(hour) */
    scui_handle_t       image_m;        /* 图片句柄(minute) */
    scui_handle_t       image_s;        /* 图片句柄(second) */
    scui_point_t        anchor_h;       /* 图片旋转围绕轴心 */
    scui_point_t        center_h;       /* 图片旋转中心 */
    scui_point_t        anchor_m;       /* 图片旋转围绕轴心 */
    scui_point_t        center_m;       /* 图片旋转中心 */
    scui_point_t        anchor_s;       /* 图片旋转围绕轴心 */
    scui_point_t        center_s;       /* 图片旋转中心 */
    bool                tick_mode;      /* 一度一跳:1;一秒一跳:0; */
} scui_watch_maker_t;
#pragma pack(pop)

/*@brief 表盘指针控件创建
 *@param maker  表盘指针控件创建参数
 *@param handle 表盘指针控件句柄
 *@param layout 通过布局创建
 */
void scui_watch_make(scui_watch_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 表盘指针控件销毁
 *@param handle 表盘指针控件句柄
 */
void scui_watch_burn(scui_handle_t handle);

/*@brief 表盘指针跳动模式
 *@param handle 滚动控件句柄
 *@param tick_mode 一度一跳:1;一秒一跳:0;
 */
void scui_watch_tick_mode(scui_handle_t handle, bool tick_mode);

/*@brief 表盘指针控件事件处理回调
 *@param event 事件
 */
void scui_watch_event(scui_event_t *event);

#endif
