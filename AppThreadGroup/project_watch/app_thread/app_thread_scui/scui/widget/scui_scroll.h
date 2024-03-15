#ifndef SCUI_SCROLL_H
#define SCUI_SCROLL_H

/*@brief 注意
 *       水平自动布局会调整所有子控件的(x,y,h)
 *       垂直自动布局会调整所有子控件的(x,y,w)
 */

typedef struct {
    scui_widget_t       widget;
    scui_event_dir_t    dir;            /* 滚动方向(自动布局) */
    scui_event_pos_t    pos;            /* 滚动停留(边界或中心) */
    scui_coord_t        space;          /* 控件间隙(自动布局) */
    scui_coord_t        springback;     /* 回弹效果 */
    uint8_t             loop:1;         /* 滚动循环(自动布局,与回弹效果互斥) */
    /* 内部域: */
    scui_handle_t       anima;          /* 移动动画 */
    scui_point_t        point_s;        /* 移动动画起始点 */
    scui_point_t        point_c;        /* 移动动画当前点 */
    scui_point_t        point_e;        /* 移动动画结束点 */
    /* 无方向布局(自由布局) */
    scui_point_t        ofs_cur;        /* 当前偏移点 */
    scui_point_t        ofs_min;        /* 最小偏移点 */
    scui_point_t        ofs_max;        /* 最大偏移点 */
    /* 方向布局(水平布局,垂直布局) */
    scui_point_t        dist_ofs;       /* 移动偏移线 */
    scui_coord_t        dist_map;       /* 移动映射线 */
    
    uint8_t             layout:1;       /* 布局更新标记 */
} scui_scroll_t;

typedef struct {
    scui_widget_maker_t widget;
    scui_event_dir_t    dir;            /* 滚动方向(自动布局) */
    scui_event_pos_t    pos;            /* 滚动停留(边界或中心) */
    scui_coord_t        space;          /* 控件间隙(自动布局) */
    scui_coord_t        springback;     /* 回弹效果 */
    uint8_t             loop:1;         /* 滚动循环(自动布局,与回弹效果互斥) */
} scui_scroll_maker_t;

/*@brief 滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 滚动控件销毁
 *@param handle 可滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle);

#if 0

/*@brief 滚动控件布局更新
 *@param handle 可滚动控件句柄
 */
void scui_scroll_layout(scui_handle_t handle);

/*@brief 滚动控件子控件位置调整
 *@param handle 窗口控件句柄
 *@param point  窗口移动坐标
 */
void scui_scroll_adjust_offset(scui_handle_t handle, scui_point_t *offset);

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_start(void *instance);

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_ready(void *instance);

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_expired(void *instance);

/*@brief 滚动控件动画自动化
 */
void scui_scroll_anima_auto(scui_handle_t handle, int32_t value_s, int32_t value_e, uint32_t peroid);

#endif

/*@brief 滚动控件更新布局回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scroll_update_layout(scui_event_t *event);

/*@brief 滚动控件事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scroll_event(scui_event_t *event);

#endif
