#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       sibling[4];         /* 临近界面句柄:0:上;1:下;2:左;3:右; */
    scui_handle_t       switch_type[4];     /* 临近界面交互风格 */
    scui_opt_pos_t      switch_enc;         /* 临近界面旋钮交互(上下左右) */
    scui_opt_pos_t      switch_key;         /* 临近界面按键交互(上下左右) */
    scui_opt_dir_t      switch_enc_way;     /* 临近界面旋钮方向(水平垂直) */
    scui_coord_t        switch_key_id[4];   /* 临近界面按键交互id */
    scui_sbitfd_t       resident:1;         /* 窗口常驻标记(特殊使用) */
    scui_sbitfd_t       buffer:1;           /* 窗口独立画布(是否使用) */
    scui_sbitfd_t       level:6;            /* 窗口所在层级(越大越高) */
    scui_pixel_cf_t     format;             /* 窗口像素格式(独立画布) */
    /* 内部域: */
    scui_sbitfd_t       draw_lock:1;        /* 窗口绘制锁 */
    void               *local_res;          /* 窗口资源实例 */
} scui_window_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       sibling[4];         /* 临近界面句柄:0:上;1:下;2:左;3:右; */
    scui_handle_t       switch_type[4];     /* 临近界面交互风格 */
    scui_opt_pos_t      switch_enc;         /* 临近界面旋钮交互(上下左右) */
    scui_opt_pos_t      switch_key;         /* 临近界面按键交互(上下左右) */
    scui_opt_dir_t      switch_enc_way;     /* 临近界面旋钮方向(水平垂直) */
    scui_coord_t        switch_key_id[4];   /* 临近界面按键交互id */
    scui_sbitfd_t       resident:1;         /* 窗口常驻标记(特殊使用) */
    scui_sbitfd_t       buffer:1;           /* 窗口独立画布(是否使用) */
    scui_sbitfd_t       level:6;            /* 窗口所在层级(越大越高) */
    scui_pixel_cf_t     format;             /* 窗口像素格式(独立画布) */
} scui_window_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_window_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_window_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_window_invoke(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

typedef enum {
    scui_window_switch_none,            /* 无切换 */
    scui_window_switch_auto,            /* 自适应(全局特效) */
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

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle);

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle);

/*@brief 窗口列表隐藏所有窗口
 *@param handle 窗口句柄(不隐藏:忽略它)
 *@param source 仅隐藏有资源窗口
 */
void scui_window_list_hide(scui_handle_t handle, bool source);

/*@brief 窗口事件通知
 *@param event 事件
 */
void scui_window_event_notify(scui_event_t *event);

/*@brief 窗口刷新(仅调度使用)
 */
void scui_window_refresh(void);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口栈更新栈顶窗口(内部接口)
 *@param handle 窗口句柄
 */
void scui_window_stack_switch(scui_handle_t handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口切换风格
 *@retval 窗口切换风格
 */
scui_window_switch_type_t scui_window_switch_type(void);

/*@brief 窗口切换方向
 *@retval 窗口切换方向
 */
scui_opt_dir_t scui_window_switch_dir(void);

/*@brief 窗口切换进度
 *@retval 窗口切换进度
 */
scui_coord_t scui_window_switch_pct(void);

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
void scui_window_switch_event(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
