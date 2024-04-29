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
    scui_coord_t        fling_page;     /* 翻页数量 */
    scui_coord_t        route_enc;      /* 编码器行程 */
    scui_coord_t        route_key;      /* 按键行程 */
    scui_coord_t        keyid_fdir;     /* 按键id,对应编码器事件(clockwise) */
    scui_coord_t        keyid_bdir;     /* 按键id,对应编码器事件(clockwise_anti) */
    scui_coord_t        springback;     /* 回弹效果 */
    uint8_t             loop:1;         /* 滚动循环(自动布局,与回弹效果互斥) */
    /* 内部域: */
    scui_handle_t       anima;              /* 移动动画 */
    scui_handle_t       key;                /* 全局滚动锁定 */
    scui_point_t        point_cur;          /* 移动动画当前 */
    scui_point_t        point_ofs;          /* 移动动画总计 */
    uint8_t             layout:1;           /* 布局更新标记 */
    uint8_t             lock_move:1;        /* 滚动长留锁 */
    uint8_t             hold_move:1;        /* 滚动长留锁 */
    uint8_t             mask_springback:1;  /* 回弹暂留锁 */
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

typedef struct {
    scui_widget_maker_t widget;
    scui_event_dir_t    dir;            /* 滚动方向(自动布局) */
    scui_event_pos_t    pos;            /* 滚动停留(边界或中心) */
    scui_coord_t        space;          /* 控件间隙(自动布局) */
    scui_coord_t        fling_page;     /* 翻页数量 */
    scui_coord_t        route_enc;      /* 编码器行程 */
    scui_coord_t        route_key;      /* 按键行程 */
    scui_coord_t        keyid_fdir;     /* 按键id,对应编码器事件(clockwise) */
    scui_coord_t        keyid_bdir;     /* 按键id,对应编码器事件(clockwise_anti) */
    scui_coord_t        springback;     /* 回弹效果 */
    uint8_t             loop:1;         /* 滚动循环(自动布局,与回弹效果互斥) */
} scui_scroll_maker_t;

/*@brief 滚动控件创建
 *@param maker  滚动控件创建参数
 *@param handle 滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 滚动控件销毁
 *@param handle 滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle);

/*@brief 滚动控件设置偏移量
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 */
void scui_scroll_offset(scui_handle_t handle, scui_point_t *offset);

/*@brief 滚动控件布局更新
 *@param handle 滚动控件句柄
 */
void scui_scroll_layout(scui_handle_t handle);

/*@brief 滚动控件翻页数更新
 *@param handle 滚动控件句柄
 *@param fling_page 翻页数
 */
void scui_scroll_fling_page(scui_handle_t handle, scui_coord_t fling_page);

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

/*@brief 滚动控件更新布局回调
 *@param event 事件
 */
void scui_scroll_update_layout(scui_event_t *event);

/*@brief 滚动控件事件流程合并
 *@param event 事件
 *@param type  自动事件类型
 *             0x00 动画打断事件(自动布局, 非循环, 循环)
 *             0x01 动画重置事件(自动布局, 非循环, 循环)
 *             0x02 动画重置事件(自动布局, 非循环, 循环)
 *             0x10 动画打断事件(自由布局)
 *             0x11 动画重置事件(自由布局)
 *             0x12 动画重置事件(自由布局)
 *             0xAA 动画回弹事件(自动布局, 自由布局, 非循环, 循环)
 */
void scui_scroll_event_auto_merge(scui_event_t *event, uint8_t type);

/*@brief 滚动控件事件处理回调
 *@param event 事件
 */
void scui_scroll_event(scui_event_t *event);

#endif
