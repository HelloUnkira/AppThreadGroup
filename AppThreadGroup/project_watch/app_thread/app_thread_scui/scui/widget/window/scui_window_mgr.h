#ifndef SCUI_WINDOW_MGR_H
#define SCUI_WINDOW_MGR_H

typedef enum {
    scui_window_switch_none,            /* 无切换 */
    scui_window_switch_auto,            /* 自适应(适应部分特效) */
    scui_window_switch_float,           /* 透明浮动 */
    scui_window_switch_normal,          /* 常规切换 */
    scui_window_switch_center_in,       /* 中心淡入 */
    scui_window_switch_center_out,      /* 中心淡出 */
    scui_window_switch_zoom1,           /* 单边缩放 */
    scui_window_switch_zoom2,           /* 双边缩放 */
} scui_window_switch_type_t;

typedef struct {
    scui_handle_t             list[SCUI_WINDOW_MGR_LIMIT];
    scui_event_dir_t          cfg_dir;      /* 窗口切换方向(配置) */
    scui_window_switch_type_t cfg_type;     /* 窗口切换风格(配置) */
    scui_window_switch_type_t type;         /* 窗口切换风格(当前) */
    scui_event_dir_t          dir;          /* 窗口切换方向(当前) */
    scui_event_pos_t          pos;          /* 窗口切换位置(当前) */
    scui_handle_t             anima;        /* 窗口切换动画 */
    scui_coord_t              pct;          /* 窗口切换进度(百分比) */
    scui_coord_t              ofs;          /* 窗口切换偏移(像素点) */
    uint32_t                  lock:1;       /* 窗口切换全局锁 */
    uint32_t                  lock_move:1;  /* 窗口切换锁 */
    uint32_t                  flag_fling:1; /* 窗口切换锁 */
    uint8_t                   hold_move:1;  /* 窗口使用锁 */
} scui_window_switch_t;

typedef struct {
    scui_handle_t        list_num;                      /* 窗口列表数量 */
    scui_handle_t        list[SCUI_WINDOW_MGR_LIMIT];   /* 窗口管理列表 */
    scui_handle_t        active_curr;                   /* 当前活跃窗口 */
    scui_handle_t        active_last;                   /* 上一活跃窗口 */
    scui_window_switch_t switch_args;                   /* 窗口切换信息 */
    /*  */
} scui_window_mgr_t;

/*@brief 窗口管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       窗口管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_mix_list(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_sort_list(scui_widget_t **list, scui_handle_t num);

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 *       将绘制画布生成刷新事件
 */
void scui_window_mix_surface(void);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event);

#endif
