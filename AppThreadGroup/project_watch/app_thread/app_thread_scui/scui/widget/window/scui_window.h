#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    uintptr_t           level:6;            /* 窗口所在层级(数字越大越高) */
    uintptr_t           buffer:1;           /* 窗口是否使用独立画布 */
    uintptr_t           resident:1;         /* 窗口常驻标记(特殊使用) */
    uintptr_t           hang_only:1;        /* 窗口悬挂标记(特殊使用) */
    scui_pixel_cf_t     format;             /* 窗口独立画布基础像素格式 */
    /* 内部域: */
    scui_handle_t       sibling[4];         /* 临近界面句柄:0:上;1:下;2:左;3:右; */
    scui_handle_t       switch_type[4];     /* 临近界面交互风格 */
} scui_window_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    uintptr_t           level:6;            /* 窗口所在层级(数字越大越高) */
    uintptr_t           buffer:1;           /* 窗口是否使用独立画布 */
    uintptr_t           resident:1;         /* 窗口常驻标记(特殊使用) */
    uintptr_t           hang_only:1;        /* 窗口悬挂标记(特殊使用) */
    scui_pixel_cf_t     format;             /* 窗口独立画布基础像素格式 */
} scui_window_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_window_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_window_burn(scui_handle_t handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

typedef enum {
    scui_window_switch_none,            /* 无切换 */
    scui_window_switch_auto,            /* 自适应(适应部分特效) */
    scui_window_switch_single_s,
    
    scui_window_switch_move,            /* 常规移动 */
    scui_window_switch_cover_in,        /* 覆盖移动(进入) */
    scui_window_switch_cover_out,       /* 覆盖移动(退出) */
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
    
    scui_window_switch_flap1,           /* 中心轴翻版(2.5D) */
    scui_window_switch_flap2,           /* 分段式翻版(2.5D) */
    scui_window_switch_cube,            /* 正方体旋转(2.5D) */
    
    scui_window_switch_single_e,
} scui_window_switch_type_t;

typedef struct {
    // 这部分参数未定, 暂保留自由度
    scui_handle_t shadow;
} scui_window_switch_args_t;

typedef struct {
    scui_handle_t               list[SCUI_WINDOW_MGR_LIMIT];
    scui_window_switch_type_t   type;             /* 窗口切换风格(当前) */
    scui_window_switch_type_t   cfg_type;         /* 窗口切换风格(配置) */
    scui_window_switch_args_t   cfg_args;         /* 窗口切换参数(配置) */
    scui_opt_dir_t              cfg_dir;          /* 窗口切换方向(配置) */
    scui_opt_dir_t              dir;              /* 窗口切换方向(当前) */
    scui_opt_pos_t              pos;              /* 窗口切换位置(当前) */
    scui_handle_t               key;              /* 窗口切换全局锁 */
    scui_handle_t               anima;            /* 窗口切换动画 */
    scui_point_t                point;            /* 窗口切换偏移(坐标点) */
    scui_coord_t                pct;              /* 窗口切换进度(百分比) */
    scui_coord_t                ofs;              /* 窗口切换偏移(像素点) */
    uint32_t                    lock_jump:1;      /* 窗口切换锁 */
    uint32_t                    lock_move:1;      /* 窗口切换锁 */
    uint32_t                    mask_fling:1;     /* 窗口切换锁 */
    uint8_t                     hold_move:1;      /* 窗口使用锁 */
} scui_window_switch_t;

typedef struct {
    scui_handle_t stack[SCUI_WINDOW_STACK_NEST];
    scui_handle_t top;
} scui_window_stack_t;

typedef struct {
    scui_handle_t           list_num;                           /* 窗口列表数量 */
    scui_handle_t           list[SCUI_WINDOW_MGR_LIMIT];        /* 窗口管理列表 */
    scui_window_stack_t     stack_args;                         /* 窗口栈式记录 */
    scui_window_switch_t    switch_args;                        /* 窗口切换信息 */
    scui_handle_t           active_curr;                        /* 当前活跃窗口 */
    scui_handle_t           active_last;                        /* 上一活跃窗口 */
    scui_widget_t          *refr_widget;                        /* 窗口送显模式控件实例地址 */
    uint32_t                refr_switch:1;                      /* 窗口送显模式标记 */
    /* 窗口混合信息: */
    scui_widget_t          *list_0[SCUI_WINDOW_MGR_LIMIT];      /* 窗口管理列表(有独立画布) */
    scui_widget_t          *list_1[SCUI_WINDOW_MGR_LIMIT];      /* 窗口管理列表(无独立画布) */
    scui_handle_t           list_0_num;                         /* 窗口列表数量(有独立画布) */
    scui_handle_t           list_1_num;                         /* 窗口列表数量(无独立画布) */
    /*  */
} scui_window_mgr_t;

/*@brief 窗口管理器根控件列表
 *@param list 根控件列表
 */
void scui_window_list(scui_handle_t **list);

/*@brief 窗口列表添加窗口(内部使用)
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle);

/*@brief 窗口列表移除窗口(内部使用)
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle);

/*@brief 窗口刷新(仅调度使用)
 */
void scui_window_refresh(void);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口栈更新栈顶窗口(内部接口)
 *@param type   更新类型(0:cover;1:add;2:del;)
 */
void scui_window_stack_update(scui_handle_t handle, uint8_t type);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
