#ifndef SCUI_SCENE_MGR_H
#define SCUI_SCENE_MGR_H

typedef enum {
    scui_window_switch_none,         /* 无切换 */
    scui_window_switch_auto,         /* 自适应(适应部分特效) */
    scui_window_switch_normal,       /* 常规切换 */
    scui_window_switch_center_in,    /* 中心淡入 */
    scui_window_switch_center_out,   /* 中心淡出 */
    scui_window_switch_zoom1,        /* 单边缩放 */
    scui_window_switch_zoom2,        /* 双边缩放 */
} scui_window_switch_type_t;

typedef struct {
    scui_handle_t list_num;
    scui_handle_t list[SCUI_SCENE_MGR_LIMIT];
    scui_handle_t active_curr;  /* 当前活跃窗口 */
    scui_handle_t active_last;  /* 上一活跃窗口 */
    /* 窗口切换信息 */
    scui_handle_t            switch_list[SCUI_SCENE_MGR_LIMIT];
    scui_window_switch_type_t switch_type_cfg;  /* 窗口切换风格(配置) */
    scui_window_switch_type_t switch_type_cur;  /* 窗口切换风格(当前) */
    scui_event_dir_t         switch_dir_cfg;    /* 窗口切换方向(配置) */
    scui_event_dir_t         switch_dir_cur;    /* 窗口切换方向(当前) */
    scui_handle_t            switch_anima;      /* 窗口切换动画 */
    scui_coord_t             switch_pct;        /* 窗口切换进度(百分比) */
    scui_coord_t             switch_ofs;        /* 窗口切换偏移(像素点) */
    uint32_t                 switch_lock:1;     /* 窗口切换锁 */
    /*  */
} scui_window_mgr_t;

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 *       将绘制画布生成刷新事件
 */
void scui_window_mix_surface(void);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_event_dispatch(scui_event_t *event);

#endif
