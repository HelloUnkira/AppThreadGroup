#ifndef SCUI_WIDGET_H
#define SCUI_WIDGET_H

/*@brief 控件类型
 */
typedef enum {
    scui_widget_type_unknown,   /* 未知控件 */
    scui_widget_type_window,    /* 窗口控件(仅根控件) */
    scui_widget_type_custom,    /* 自定义控件 */
    scui_widget_type_scroll,    /* 可滚动控件 */
    scui_widget_type_string,    /* 字符串控件 */
    /* 扩展控件(按需补充) */
    scui_widget_type_button,    /* 纯色按钮 */
    scui_widget_type_watch,     /* 表盘指针控件 */
    scui_widget_type_chart,     /* 图表控件 */
    /* 继续补充... */
    scui_widget_type_num,
} scui_widget_type_t;

/*@brief 控件状态风格
 */
typedef struct {
    uint8_t trans:1;            /* 背景显示:0;背景透明:1; */
    uint8_t state:1;            /* 控件隐藏:0;控件显示:1; */
    uint8_t sched_anima:1;      /* 控件调度动画标记 */
    uint8_t indev_ptr:1;        /* 输入事件响应标记:ptr */
    uint8_t indev_enc:1;        /* 输入事件响应标记:enc */
    uint8_t indev_key:1;        /* 输入事件响应标记:key */
} scui_widget_style_t;

/*@brief 控件创建回调
 *@brief 控件销毁回调
 *@brief 控件布局更新回调
 */
typedef void (*scui_widget_cb_create_t)(void *maker, scui_handle_t *handle, bool layout);
typedef void (*scui_widget_cb_destroy_t)(scui_handle_t handle);
typedef void (*scui_widget_cb_layout_t)(scui_handle_t handle);

/*@brief 控件处理函数映射表
 */
typedef struct {
    scui_widget_cb_create_t  create;
    scui_widget_cb_destroy_t destroy;
    scui_widget_cb_layout_t  layout;
} scui_widget_cb_t;

/*@brief 控件基础信息:
 *       控件只处理最基础绘制(背景)
 */
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件状态风格 */
    scui_event_cb_list_t    list;           /* 控件事件回调列表 */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t          *child_list;     /* 控件关联属性:子控件列表 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件数量 */
    scui_surface_t         *surface;        /* 控件画布(图形上下文):画布实例 */
    scui_clip_set_t         clip_set;       /* 控件画布(图形上下文):画布剪切域集合 */
    scui_alpha_t            alpha;          /* 控件透明度 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_t            color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_t;

/*@brief 控件布局资料
 */
#pragma pack(push, 1)
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件状态风格 */
    scui_event_cb_t         event_cb;       /* 事件响应回调(可选字段,非控件自身字段) */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t           child_num;      /* 控件关联属性:子控件数量 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_t            color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_maker_t;
#pragma pack(pop)

/* 控件孩子列表宏迭代器(略过无效控件)(backward traverse) */
#define scui_widget_child_list_btra(widget, idx)                            \
    for (int32_t idx = 0; idx < (int32_t)widget->child_num; idx++)          \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/* 控件孩子列表宏迭代器(略过无效控件)(forward traverse) */
#define scui_widget_child_list_ftra(widget, idx)                            \
    for (int32_t idx = (int32_t)widget->child_num - 1; idx >= 0; idx--)     \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/*@brief 控件创建
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker,
                        scui_handle_t *handle, bool layout);

/*@brief 控件销毁
 *@param widget 控件实例
 */
void scui_widget_destroy(scui_widget_t *widget);

/*@brief 查找控件映射表
 *@param type 控件类型
 *@retval 控件映射表
 */
scui_widget_cb_t * scui_widget_cb_find(uint32_t type);

/*@brief 通过映射表调用创建一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 控件句柄
 */
void scui_widget_cb_create(scui_handle_t handle);

/*@brief 卸载一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_destroy(scui_handle_t handle);

/*@brief 更新一个控件布局
 *@param handle 控件句柄
 */
void scui_widget_cb_layout(scui_handle_t handle);

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle);

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle);

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

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_by_index(scui_handle_t handle, scui_handle_t index);

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件句柄
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_to_index(scui_handle_t handle, scui_handle_t child);

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

/*@brief 控件画布
 *@param handle 控件句柄
 *@retval 控件画布
 */
scui_surface_t * scui_widget_surface(scui_handle_t handle);

/*@brief 控件画布创建
 *@param handle  控件句柄
 *@param format  画布格式
 *@param hor_res 画布水平尺寸
 *@param ver_res 画布垂直尺寸
 */
void scui_widget_surface_create(scui_handle_t handle,  scui_pixel_cf_t format,
                                scui_coord_t  hor_res, scui_coord_t    ver_res);

/*@brief 控件画布销毁
 *@param handle 控件句柄
 */
void scui_widget_surface_destroy(scui_handle_t handle);

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

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point);

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param pos     对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_event_pos_t pos, scui_point_t *offset);

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param child   控件子控件句柄(为空则镜像所有子控件)
 *@param dir     镜像方向(水平镜像或垂直镜像)
 *@param recurse 递归处理(全局镜像有效)
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_handle_t child, scui_event_dir_t dir, bool recurse);

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height);

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_move_ofs_child_list(scui_handle_t handle, scui_point_t *offset);

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 */
void scui_widget_move_ofs_child_list_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range);

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param target 控件句柄(目标控件)
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_handle_t   *target,
                                scui_point_t *offset, scui_event_pos_t pos);

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_type(scui_handle_t handle);

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle);

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle);

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_style_is_hide(scui_handle_t handle);

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@retval 控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle);

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle);

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_t scui_widget_color_get(scui_handle_t handle);

/*@brief 控件透明度设置
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recurse);

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image);

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_t color);

#endif
