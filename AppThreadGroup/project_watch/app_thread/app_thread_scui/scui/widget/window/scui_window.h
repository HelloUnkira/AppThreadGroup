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
    scui_sbitfd_t       preload:1;          /* 窗口预加载时(许可预加载) */
    scui_sbitfd_t       level:6;            /* 窗口所在层级(越大越高) */
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
    scui_sbitfd_t       preload:1;          /* 窗口预加载时(许可预加载) */
    scui_sbitfd_t       level:6;            /* 窗口所在层级(越大越高) */
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
    
    scui_window_switch_rotate,          /* 旋转渐变(鬼点子, 边缘点) */
    scui_window_switch_rotate_1,        /* 旋转渐变(鬼点子, 中心点) */
    scui_window_switch_circle,          /* 圈格缩放(鬼点子) */
    scui_window_switch_grid,            /* 网格缩放(鬼点子) */
    
    scui_window_switch_flip1,           /* 中心轴翻页(2.5D) */
    scui_window_switch_flip2,           /* 仿真页翻页(2.5D) */
    
    scui_window_switch_flap1,           /* 中心轴翻版(2.5D, 未细调) */
    scui_window_switch_flap2,           /* 分段式翻版(2.5D, 未细调) */
    scui_window_switch_cube,            /* 正方体旋转(2.5D, 未细调) */
    
    scui_window_switch_single_e,
} scui_window_switch_type_t;

typedef struct {
    scui_handle_t  list[SCUI_WINDOW_LIST_LIMIT];
    scui_handle_t  type;            /* 窗口切换风格(当前) */
    scui_handle_t  move_type;       /* 窗口切换风格(配置:跟手滑动) */
    scui_handle_t  jump_type;       /* 窗口切换风格(配置:跳转) */
    scui_opt_dir_t jump_dir;        /* 窗口切换方向(配置:跳转) */
    scui_opt_dir_t dir;             /* 窗口切换方向(当前) */
    scui_opt_pos_t pos;             /* 窗口切换位置(当前) */
    scui_point_t   point;           /* 窗口切换偏移(坐标点) */
    scui_coord_t   pct;             /* 窗口切换进度(百分比) */
    scui_coord_t   ofs;             /* 窗口切换偏移(像素点) */
    scui_handle_t  anima;           /* 窗口切换动画 */
    scui_coord_t   anima_speed[5];  /* 窗口切换动画速度[ptr,enc,key,auto,jump](像素点/1s) */
    scui_map_cb_t  anima_path[5];   /* 窗口切换动画轨迹[ptr,enc,key,auto,jump] */
    scui_sbitfd_t  anima_tag:5;     /* 窗口切换动画标记[ptr,enc,key,auto,jump] */
    scui_sbitfd_t  lock_jump:1;     /* 窗口切换锁 */
    scui_sbitfd_t  lock_move:1;     /* 窗口切换锁 */
    scui_sbitfd_t  mask_fling:1;    /* 窗口切换锁 */
} scui_window_switch_t;

typedef struct {
    scui_handle_t flap1_shadow;
    scui_handle_t flap2_shadow;
    scui_handle_t cube_shadow;
} scui_window_transform_t;

typedef struct {
    scui_handle_t list[SCUI_WINDOW_STACK_NEST];
    scui_handle_t list_rcd[SCUI_WINDOW_STACK_NEST];
    scui_handle_t list_bak[SCUI_WINDOW_STACK_NEST];
    scui_handle_t node_bak;
    scui_handle_t top_bak;
    scui_handle_t top;
} scui_window_stack_t;

typedef struct {
    scui_handle_t  acts_cur_num;
    scui_handle_t  acts_cur[SCUI_WINDOW_LIST_LIMIT];
    scui_handle_t  acts_rcd[SCUI_WINDOW_LIST_LIMIT];
    scui_widget_t *widget_0[SCUI_WINDOW_LIST_LIMIT];
    scui_widget_t *widget_1[SCUI_WINDOW_LIST_LIMIT];
    scui_handle_t  widget_0_num;
    scui_handle_t  widget_1_num;
    scui_widget_t *widget_refr;
    scui_sbitfd_t  switch_refr:1;
    uint32_t        anima_draw;
} scui_window_list_t;

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle);

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle);

/*@brief 窗口列表同步
 *@param list 窗口列表
 */
void scui_window_list_sync(scui_handle_t list[SCUI_WINDOW_LIST_LIMIT]);

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

#define SCUI_WINDOW_SWITCH_DECLARE(name)    void name(scui_widget_t **list, scui_handle_t num)
/*************************************************************************************************/
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_move);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_cover);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_zoom);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_center);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_rotate);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_rotate_1);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_circle);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_grid);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_flip1);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_flip2);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_flap1);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_flap2);
SCUI_WINDOW_SWITCH_DECLARE(scui_window_transform_cube);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
