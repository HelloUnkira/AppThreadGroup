#ifndef SCUI_WIDGET_H
#define SCUI_WIDGET_H

/*@brief 控件类型
 */
typedef enum {
    scui_widget_type_unknown,   /* 未知控件 */
    scui_widget_type_window,    /* 窗口控件(仅根控件) */
    scui_widget_type_custom,    /* 自定义控件 */
    scui_widget_type_scroll,    /* 可滚动控件 */
    scui_widget_type_image,     /* 图片控件 */
    scui_widget_type_label,     /* 文本控件 */
    /* 继续补充... */
    scui_widget_type_num,
} scui_widget_type_t;

/*@brief 控件状态风格
 */
typedef struct {
    uint8_t trans:1;        /* 背景透明 */
    uint8_t state:1;        /* 控件隐藏:0;控件显示:1; */
    uint8_t anima_sched:1;  /* 控件调度动画标记 */
    uint8_t indev_ptr:1;    /* 输入事件响应标记:ptr */
    uint8_t indev_enc:1;    /* 输入事件响应标记:enc */
    uint8_t indev_key:1;    /* 输入事件响应标记:key */
} scui_widget_style_t;

/*@brief 控件事件回调响应优先级
 *       任何传递给控件的事件都会尝试性的经过该三步流程进行响应
 *       如果三步处理均不能完全吸收则事件继续向兄弟传递或者冒泡上去
 *       处理中回调如果由外界提供则不走默认处理流程,否则响应默认控件事件传导
 */
typedef enum {
    scui_widget_order_before,   /* 主事件处理前回调 */
    scui_widget_order_current,  /* 主事件处理中回调 */
    scui_widget_order_after,    /* 主事件处理后回调 */
} scui_widget_order_t;

/*@brief 控件事件回调
 */
typedef struct {
    scui_list_dln_t         dl_node;    /* 控件事件回调列表 */
    scui_event_cb_t         event_cb;   /* 控件事件回调 */
    scui_event_type_t       event;      /* 控件事件 */
    scui_widget_order_t     order;      /* 控件事件回调响应优先级 */
} scui_widget_event_t;

/*@brief 控件创建回调
 *@brief 控件销毁回调
 */
typedef void (*scui_widget_cb_create_t)(void *maker, scui_handle_t *handle, bool layout);
typedef void (*scui_widget_cb_destroy_t)(scui_handle_t handle);

/*@brief 控件处理函数映射表
 */
typedef struct {
    scui_widget_cb_create_t  create;
    scui_widget_cb_destroy_t destroy;
} scui_widget_cb_t;

/*@brief 控件基础信息:
 *       控件只处理最基础绘制(背景)
 *剪切域:
 *     scui_widget_t->clip:         控件在父控件的恒定剪切域
 *     scui_widget_t->surface_clip: 控件在当次绘制时的剪切域
 *     scui_widget_t->surface.clip: 控件在画布中的剪切域(画布相对窗口,控件相对画布)
 *画布:
 *     画布不是完全都存在独立资源空间
 *     只有一个独立场景可能存在资源空间
 *     其他控件的画布可能都只是相对独立场景资源空间的一个映射
 *     如果一个独立场景都不存在资源空间(映射到绘制画布资源空间)
 *     那么它们都会是相对绘制画布资源空间的一个映射
 */
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件状态风格 */
    scui_list_dll_t         dl_list;        /* 控件事件回调列表 */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t          *child_list;     /* 控件关联属性:子控件列表 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件数量 */
    scui_clip_set_t         clip_set;       /* 控件画布(图形上下文):画布剪切域集合 */
    scui_surface_t          surface;        /* 控件画布(图形上下文):画布实例 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_gradient_t   color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_t;

/*@brief 控件布局资料
 */
#pragma pack(push, 1)
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件状态风格 */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件数量 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_gradient_t   color;          /* 纯色背景(如果没背景图片,颜色绘制) */
    scui_event_cb_t         event_cb;       /* 事件响应回调(可选字段,非控件自身字段) */
} scui_widget_maker_t;
#pragma pack(pop)

/* 控件孩子列表宏迭代器(略过无效控件)(backward traverse) */
#define scui_widget_child_list_btra(widget, idx)                            \
    for (int32_t idx = 0; idx < (int32_t)widget->child_num; idx++)          \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/* 控件孩子列表宏迭代器(略过无效控件)(forward traverse) */
#define scui_widget_child_list_ftra(widget, idx)                            \
    for (int32_t idx = (int32_t)widget->child_num - 1; idx > 0; idx--)      \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/*@brief 查找控件映射表
 *@param type 控件类型
 *@retval 控件映射表
 */
scui_widget_cb_t * scui_widget_cb_find(uint32_t type);

/*@brief 控件创建
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 控件销毁
 *@param widget 控件实例
 */
void scui_widget_destroy(scui_widget_t *widget);

/*@brief 通过映射表调用创建一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 控件句柄
 */
void scui_widget_cb_create(scui_handle_t handle);

/*@brief 卸载一个控件树
 *       从指定控件开始到它的所有子控件
 *@param handle 控件句柄
 */
void scui_widget_cb_destroy(scui_handle_t handle);

/*@brief 控件添加子控件
 *@param handle 控件句柄
 *@param child  控件子控件句柄
 */
void scui_widget_child_add(scui_handle_t handle, scui_handle_t child);

/*@brief 控件移除子控件
 *@param handle 控件句柄
 *@param child  控件子控件句柄
 */
void scui_widget_child_del(scui_handle_t handle, scui_handle_t child);

/*@brief 控件检查剪切域
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_clip_check(scui_widget_t *widget, bool recurse);

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

/*@brief 控件更新剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_update(scui_widget_t *widget);

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_resize(scui_handle_t handle, scui_coord_t width, scui_coord_t height);

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_repos(scui_handle_t handle, scui_point_t *point);

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param child  控件子控件句柄
 */
void scui_widget_reofs_children(scui_handle_t handle, scui_point_t *offset);

#endif