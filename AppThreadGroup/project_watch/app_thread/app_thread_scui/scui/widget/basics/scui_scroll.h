#ifndef SCUI_SCROLL_H
#define SCUI_SCROLL_H

/*@brief 注意
 *       水平自动布局会调整所有子控件的(x,y,h)
 *       垂直自动布局会调整所有子控件的(x,y,w)
 */

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_opt_pos_t  pos;                /* 滚动停留(边界或中心) */
    scui_opt_dir_t  dir;                /* 滚动方向(水平滚动,垂直滚动,全局滚动) */
    scui_point_t    edge;               /* 滚动边距(自由布局) */
    scui_coord_t    space;              /* 控件间隙(自动布局) */
    scui_coord_t    fling_page;         /* 翻页数量 */
    scui_map_cb_t   anima_path[4];      /* 动画轨迹[ptr, enc, key, auto] */
    scui_coord_t    anima_speed[4];     /* 动画速度[ptr, enc, key, auto](像素点/1s) */
    scui_coord_t    route_enc;          /* 编码器行程 */
    scui_coord_t    route_key;          /* 按键行程 */
    scui_coord_t    keyid_fdir;         /* 按键id,对应编码器事件(clockwise) */
    scui_coord_t    keyid_bdir;         /* 按键id,对应编码器事件(clockwise_anti) */
    scui_coord_t    springback;         /* 回弹效果 */
    uintptr_t       freedom:1;          /* 自由布局(默认自动布局) */
    uintptr_t       loop:1;             /* 滚动循环(自动布局,与回弹效果互斥) */
    /* 内部域: */
    scui_handle_t   anima;              /* 移动动画 */
    scui_handle_t   key;                /* 全局滚动锁定 */
    scui_point_t   *point_rcd;          /* 子控件移动记录 */
    scui_point_t    point_cur;          /* 移动动画当前 */
    scui_point_t    point_ofs;          /* 移动动画总计 */
    bool            anima_tag[4];       /* 动画标记[ptr, enc, key, auto] */
    uintptr_t       layout:1;           /* 布局更新标记 */
    uintptr_t       over_scroll;        /* 滚动迭代状态 */
    uintptr_t       lock_move:1;        /* 滚动长留锁 */
    uintptr_t       hold_move:1;        /* 滚动长留锁 */
    uintptr_t       mask_springback:1;  /* 回弹暂留锁 */
    /* 内部域(模式): */
    union {
        /* 无方向布局(自由布局) */
        struct {
            scui_point_t    ofs_sum;    /* 记录偏移点 */
            scui_point_t    ofs_cur;    /* 当前偏移点 */
            scui_point_t    ofs_min;    /* 最小偏移点 */
            scui_point_t    ofs_max;    /* 最大偏移点 */
        };
        /* 方向布局(水平布局,垂直布局,非循环,循环) */
        struct {
            scui_coord_t    dis_sum;    /* 移动记录线 */
            scui_coord_t    dis_ofs;    /* 移动记录线 */
            scui_coord_t    dis_lim;    /* 移动限制线[0,range] */
        };
    };
} scui_scroll_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_opt_pos_t  pos;                /* 滚动停留(边界或中心) */
    scui_opt_dir_t  dir;                /* 滚动方向(水平滚动,垂直滚动,全局滚动) */
    scui_point_t    edge;               /* 滚动边距(自由布局) */
    scui_coord_t    space;              /* 控件间隙(自动布局) */
    scui_coord_t    fling_page;         /* 翻页数量 */
    scui_map_cb_t   anima_path[4];      /* 动画轨迹[ptr, enc, key, auto] */
    scui_coord_t    anima_speed[4];     /* 动画速度[ptr, enc, key, auto](像素点/1s) */
    scui_coord_t    route_enc;          /* 编码器行程 */
    scui_coord_t    route_key;          /* 按键行程 */
    scui_coord_t    keyid_fdir;         /* 按键id,对应编码器事件(clockwise) */
    scui_coord_t    keyid_bdir;         /* 按键id,对应编码器事件(clockwise_anti) */
    scui_coord_t    springback;         /* 回弹效果 */
    uintptr_t       freedom:1;          /* 自由布局(默认自动布局) */
    uintptr_t       loop:1;             /* 滚动循环(自动布局,与回弹效果互斥) */
} scui_scroll_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_scroll_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_scroll_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_scroll_event(scui_event_t *event);

#endif
