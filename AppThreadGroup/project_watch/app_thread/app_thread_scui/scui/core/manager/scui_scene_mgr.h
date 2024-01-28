#ifndef SCUI_SCENE_MGR_H
#define SCUI_SCENE_MGR_H

typedef enum {
    scui_scene_switch_none,         /* 无切换 */
    scui_scene_switch_auto,         /* 自适应(适应部分特效) */
    scui_scene_switch_normal,       /* 常规切换 */
    scui_scene_switch_center_in,    /* 中心淡入 */
    scui_scene_switch_center_out,   /* 中心淡出 */
    scui_scene_switch_zoom1,        /* 单边缩放 */
    scui_scene_switch_zoom2,        /* 双边缩放 */
} scui_scene_switch_type_t;

typedef struct {
    scui_handle_t list_num;
    scui_handle_t list[SCUI_SCENE_MGR_LIMIT];
    scui_handle_t active_curr;  /* 当前活跃窗口 */
    scui_handle_t active_last;  /* 上一活跃窗口 */
    /* 场景切换信息 */
    scui_handle_t            switch_list[SCUI_SCENE_MGR_LIMIT];
    scui_scene_switch_type_t switch_type_cfg;   /* 场景切换风格(配置) */
    scui_scene_switch_type_t switch_type_cur;   /* 场景切换风格(当前) */
    scui_event_dir_t         switch_dir_cfg;    /* 场景切换方向(配置) */
    scui_event_dir_t         switch_dir_cur;    /* 场景切换方向(当前) */
    scui_handle_t            switch_anima;      /* 场景切换动画 */
    scui_coord_t             switch_pct;        /* 场景切换进度(百分比) */
    scui_coord_t             switch_ofs;        /* 场景切换偏移(像素点) */
    uint32_t                 switch_lock:1;     /* 场景切换锁 */
    /*  */
} scui_scene_mgr_t;

/*@brief 场景切换风格(配置)
 *@param switch_type 场景切换风格
 */
void scui_scene_switch_type_cfg(scui_scene_switch_type_t switch_type);

/*@brief 场景切换方向(配置)
 *@param switch_dir 场景切换方向
 */
void scui_scene_switch_dir_cfg(scui_event_dir_t switch_dir);

/*@brief 场景管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       场景管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_mix_list(scui_widget_t **list, scui_handle_t num);

/*@brief 场景管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_sort_list(scui_widget_t **list, scui_handle_t num);

/*@brief 场景管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 *       将绘制画布生成刷新事件
 */
void scui_scene_mix_surface(void);

#endif
