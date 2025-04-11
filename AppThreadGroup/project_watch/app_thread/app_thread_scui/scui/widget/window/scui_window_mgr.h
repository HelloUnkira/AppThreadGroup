#ifndef SCUI_WINDOW_MGR_H
#define SCUI_WINDOW_MGR_H

typedef enum {
    scui_window_switch_none,            /* 无切换 */
    scui_window_switch_auto,            /* 自适应(适应部分特效) */
    scui_window_switch_single_s,
    
    scui_window_switch_move,            /* 常规移动 */
    scui_window_switch_cover,           /* 覆盖移动 */
    scui_window_switch_zoom1,           /* 单边缩放 */
    scui_window_switch_zoom2,           /* 双边缩放 */
    scui_window_switch_center_in,       /* 中心淡入 */
    scui_window_switch_center_out,      /* 中心淡出 */
    scui_window_switch_center_inout,    /* 中心淡入淡出 */
    
    scui_window_switch_rotate,          /* 旋转渐变(鬼点子, 中心点) */
    scui_window_switch_rotate1,         /* 旋转渐变(鬼点子, 边缘点) */
    scui_window_switch_circle,          /* 圈格缩放(鬼点子) */
    scui_window_switch_grid,            /* 网格缩放(鬼点子) */
    
    scui_window_switch_flip1,           /* 中心轴翻页(2.5D) */
    scui_window_switch_flip2,           /* 仿真页翻页(2.5D) */
    scui_window_switch_flip3,           /* 分段式翻页(2.5D) */
    scui_window_switch_cube,            /* 正方体旋转(2.5D) */

    scui_window_switch_single_e,
} scui_window_switch_type_t;

typedef union {
    struct {
        scui_handle_t shadow;
    } cube;
} scui_window_switch_args_t;

typedef struct {
    scui_handle_t             list[SCUI_WINDOW_MGR_LIMIT];
    scui_window_switch_type_t type;         /* 窗口切换风格(当前) */
    scui_window_switch_type_t cfg_type;     /* 窗口切换风格(配置) */
    scui_window_switch_args_t cfg_args;     /* 窗口切换参数(配置) */
    scui_opt_dir_t            cfg_dir;      /* 窗口切换方向(配置) */
    scui_opt_dir_t            dir;          /* 窗口切换方向(当前) */
    scui_opt_pos_t            pos;          /* 窗口切换位置(当前) */
    scui_handle_t             key;          /* 窗口切换全局锁 */
    scui_handle_t             anima;        /* 窗口切换动画 */
    scui_point_t              point;        /* 窗口切换偏移(坐标点) */
    scui_coord_t              pct;          /* 窗口切换进度(百分比) */
    scui_coord_t              ofs;          /* 窗口切换偏移(像素点) */
    uint32_t                  lock_jump:1;  /* 窗口切换锁 */
    uint32_t                  lock_move:1;  /* 窗口切换锁 */
    uint32_t                  mask_fling:1; /* 窗口切换锁 */
    uint8_t                   hold_move:1;  /* 窗口使用锁 */
} scui_window_switch_t;

typedef struct {
    scui_handle_t        list_num;                      /* 窗口列表数量 */
    scui_handle_t        list[SCUI_WINDOW_MGR_LIMIT];   /* 窗口管理列表 */
    scui_window_switch_t switch_args;                   /* 窗口切换信息 */
    scui_handle_t        active_curr;                   /* 当前活跃窗口 */
    scui_handle_t        active_last;                   /* 上一活跃窗口 */
    scui_widget_t       *refr_widget;                   /* 窗口送显模式控件实例地址 */
    uint32_t             refr_switch:1;                 /* 窗口送显模式标记 */
    /*  */
} scui_window_mgr_t;

/*@brief 窗口切换风格实例
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_cfg_type(scui_window_switch_type_t **cfg_type);

/*@brief 窗口切换参数实例
 *@param cfg_args 实例
 */
void scui_window_switch_cfg_args(scui_window_switch_args_t **cfg_args);

/*@brief 窗口切换方向实例
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_cfg_dir(scui_opt_dir_t **cfg_dir);

/*@brief 窗口管理器根控件列表
 *@param list 根控件列表
 */
void scui_window_list(scui_handle_t **list);

/*@brief 窗口隐藏
 *@param handle 窗口句柄
 */
void scui_window_list_hide_without(scui_handle_t handle, bool any);

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle);

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle);

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 *       将绘制画布生成刷新事件
 */
void scui_window_surface_blend(void);

/*@brief 窗口管理器混合画布模式检查
 *@param state  状态(0x00:设置标记;0x01:清除标记;0x02:检查标记;)
 *@param widget 控件实例地址
 */
bool scui_window_surface_switch(uint8_t state, scui_widget_t **widget);

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle);

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(void);

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_curr(void);

#endif
