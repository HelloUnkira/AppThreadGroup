#ifndef SCUI_WIDGET_H
#define SCUI_WIDGET_H

/*@brief 控件类型
 */
typedef enum {
    scui_widget_type_unknown,   /* 未知控件 */
    scui_widget_type_window,    /* 窗口控件(仅根控件) */
    scui_widget_type_custom,    /* 自定义控件 */
    scui_widget_type_scroll,    /* 滚动控件 */
    scui_widget_type_string,    /* 字符串控件 */
    scui_widget_type_linear,    /* 列表控件 */
    scui_widget_type_linear_m,  /* 列表控件(子, 主) */
    scui_widget_type_linear_s,  /* 列表控件(子, 从) */
    scui_widget_type_roller,    /* 滚动控件 */
    scui_widget_type_xvedio,	/* 图帧控件 */
    /* 扩展控件(按需补充) */
    scui_widget_type_button,    /* 纯色按钮 */
    scui_widget_type_chart,     /* 图表控件 */
    /* 继续补充... */
    scui_widget_type_num,
} scui_widget_type_t;

/*@brief 字段继承(控件声明时使用)
 */
#define SCUI_EXTEND_FIELD_S     union {
#define SCUI_EXTEND_FIELD_E     };

/*@brief 控件状态风格
 */
typedef struct {
    scui_sbitfd_t fixed:1;              /* 控件移动禁止(悬浮) */
    scui_sbitfd_t fully_bg:1;           /* 背景覆盖:1;背景透明:0; */
    scui_sbitfd_t cover_fg:1;           /* 前景覆盖:1;前景透明:0; */
    scui_sbitfd_t order_draw:1;         /* 控件绘制顺序(0:顺向;1:逆向;) */
    scui_sbitfd_t sched_anima:1;        /* 控件调度帧动画标记 */
    scui_sbitfd_t sched_widget:1;       /* 控件专属事件响应标记 */
    scui_sbitfd_t indev_ptr:1;          /* 输入事件响应标记:ptr */
    scui_sbitfd_t indev_ptr_move:1;     /* 输入事件响应标记:ptr move 自动跟随 */
    scui_sbitfd_t indev_enc:1;          /* 输入事件响应标记:enc */
    scui_sbitfd_t indev_key:1;          /* 输入事件响应标记:key */
} scui_widget_style_t;

/*@brief 控件状态
 */
typedef struct {
    scui_sbitfd_t view:1;               /* 控件隐藏:0;控件显示:1; */
    scui_sbitfd_t indev_ptr_hold:1;     /* 输入事件响应维持:ptr */
    scui_sbitfd_t indev_enc_hold:1;     /* 输入事件响应维持:enc */
    scui_sbitfd_t indev_key_hold:1;     /* 输入事件响应维持:key */
} scui_widget_state_t;

/*@brief 控件动画
 */
typedef struct {
    scui_handle_t  handle;          /* 监听动画 */
    scui_handle_t  object;          /* 动画对象 */
    scui_handle_t  number;          /* 动画数量 */
    scui_handle_t *list;            /* 动画列表 */
    scui_handle_t *step;            /* 工步序列 */
    scui_handle_t  step_lim;        /* 工步极限 */
    scui_handle_t  iter;            /* 当前工步 */
} scui_widget_anima_t;

/*@brief 控件构造回调
 *@brief 控件析构回调
 *@brief 控件事件回调
 */
typedef void (*scui_widget_cb_make_t)(void *inst, void *inst_maker, scui_handle_t *handle);
typedef void (*scui_widget_cb_burn_t)(scui_handle_t handle);
typedef scui_event_cb_t scui_widget_cb_invoke_t;

/*@brief 控件处理映射表
 */
typedef struct {
    scui_handle_t           size;       // 控件实例大小
    scui_handle_t           maker;      // 控件构造器大小
    scui_sbitfd_t           inherit:1;  // 控件可继承标记
    scui_widget_type_t      base;       // 控件基类
    scui_widget_cb_make_t   make;       // 控件构造
    scui_widget_cb_burn_t   burn;       // 控件析构
    scui_widget_cb_invoke_t invoke;     // 控件调用
    const char             *name;       // 控件名称
} scui_widget_map_t;

/*@brief 控件基础信息:
 *       控件只处理最基础绘制(背景)
 */
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件风格 */
    scui_widget_state_t     state;          /* 控件状态 */
    scui_event_cb_list_t    list;           /* 控件事件回调列表 */
    scui_area_t             clip;           /* 控件有效区域 */
    scui_handle_t           anima;          /* 控件动画集合 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t          *child_list;     /* 控件关联属性:子控件列表 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件最大数量 */
    scui_handle_t           child_now;      /* 控件关联属性:子控件当前数量 */
    scui_surface_t         *surface;        /* 控件画布(图形上下文):画布实例 */
    scui_clip_set_t         clip_set;       /* 控件画布(图形上下文):画布剪切域集合 */
    scui_handle_t           user_data;      /* 扩展字段:用户资源句柄 */
    scui_alpha_t            alpha;          /* 控件透明度 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_t            color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_t;

/*@brief 控件布局资料
 */
#pragma pack(push, 1)
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件风格 */
    scui_event_cb_t         event_cb;       /* 事件响应回调 */
    scui_area_t             clip;           /* 控件有效区域 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件最大数量 */
    scui_handle_t           anima_num;      /* 控件动画数量:最大限制 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_t            color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_maker_t;
#pragma pack(pop)

/* 控件孩子列表宏迭代器(略过无效控件)(backward traverse) */
#define scui_widget_child_list_btra(widget, idx)                            \
    for (int64_t idx = 0; idx < (int64_t)widget->child_num; idx++)          \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/* 控件孩子列表宏迭代器(略过无效控件)(forward traverse) */
#define scui_widget_child_list_ftra(widget, idx)                            \
    for (int64_t idx = (int64_t)widget->child_num - 1; idx >= 0; idx--)     \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/*@brief 控件构造器
 *@param widget 控件实例
 *@param maker  控件构造实例
 *@param handle 控件句柄
 */
void scui_widget_make(scui_widget_t *widget, void *maker, scui_handle_t *handle);

/*@brief 控件析构器
 *@param widget 控件实例
 */
void scui_widget_burn(scui_widget_t *widget);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件添加子控件
 *@param widget 控件实例
 *@param child  控件子控件句柄
 */
void scui_widget_child_add(scui_widget_t *widget, scui_handle_t child);

/*@brief 控件移除子控件
 *@param widget 控件实例
 *@param child  控件子控件句柄
 */
void scui_widget_child_del(scui_widget_t *widget, scui_handle_t child);

/*@brief 控件统计剪切域大小
 *@param handle 控件句柄
 *@param size   剪切域大小
 */
void scui_widget_clip_sizes(scui_handle_t handle, scui_multi_t *size);

/*@brief 控件检查剪切域
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_clip_check(scui_handle_t handle, bool recurse);

/*@brief 控件清除剪切域
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_clip_clear(scui_widget_t *widget, bool recurse);

/*@brief 控件还原剪切域
 *@param widget  控件实例
 *@param clip    剪切域
 *@param recurse 递归处理
 */
void scui_widget_clip_reset(scui_widget_t *widget, scui_area_t *clip, bool recurse);

/*@brief 控件全覆盖剪切域检查
 *@param widget 控件实例
 */
bool scui_widget_clip_cover(scui_widget_t *widget);

/*@brief 控件更新剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_update(scui_widget_t *widget);

/*@brief 控件画布重映射
 *@param handle  控件句柄
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_handle_t handle, scui_surface_t *surface);

/*@brief 控件画布剪切域刷新
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_handle_t handle, bool recurse);

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget);

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件显示状态更新
 *@param handle  控件句柄
 *@param view    状态更新
 *@param recurse 递归处理
 */
void scui_widget_state_view(scui_handle_t handle, bool view, bool recurse);

/*@brief 控件透明度设置撤销
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_undo(scui_handle_t handle, scui_alpha_t alpha, bool recurse);

/*@brief 控件透明度设置融合
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_mix(scui_handle_t handle, scui_alpha_t alpha, bool recurse);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_dispatch(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件类型检查
 *@param handle 控件句柄
 *@param type   控件类型
 *@retval 控件类型检查
 */
bool scui_widget_type_check(scui_handle_t handle, scui_widget_type_t type);

/*@brief 查找控件映射表
 *@param type       控件类型
 *@param widget_map 控件映射表
 */
void scui_widget_map_find(scui_widget_type_t type, scui_widget_map_t **widget_map);

/*@brief 创建控件树(句柄映射表)
 *@param handle 根控件句柄
 */
void scui_widget_create_layout_tree(scui_handle_t handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_clist_move_ofs(scui_handle_t handle, scui_point_t *offset);

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 */
void scui_widget_clist_move_ofs_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range);

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param target 控件句柄(目标控件)
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_handle_t *target, scui_point_t *offset, scui_opt_pos_t pos);

/*@prief 事件滚动状态检查更新
 *@param type 0x00:锁定; 0x01:解锁; 0x02:检查
 *@retval state:0x02时是否滚动
 */
bool scui_widget_scroll_state(uint8_t state);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
