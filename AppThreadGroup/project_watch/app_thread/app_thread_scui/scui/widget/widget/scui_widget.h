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
    scui_widget_type_roller,    /* 滚动轮控件 */
    /* 对象控件 */
    scui_widget_type_objbtn,    /* 对象按钮 */
    /* 扩展控件(按需补充) */
    scui_widget_type_button,    /* 纯色按钮 */
    scui_widget_type_chart,     /* 图表控件 */
    /* 继续补充... */
    scui_widget_type_num,
} scui_widget_type_t;

/*@brief 控件类包装器
 *       仅控件声明时使用
 */
#if 1
// 基类:控件
#define scui_class_widget(widget) union {scui_widget_t widget;}
#define scui_class_widget_maker(widget) union {scui_widget_maker_t widget;}
// 一级派生:基础控件
#define scui_class_window(widget, window) union {scui_widget_t widget; scui_window_t window;}
#define scui_class_custom(widget, custom) union {scui_widget_t widget; scui_custom_t custom;}
#define scui_class_scroll(widget, scroll) union {scui_widget_t widget; scui_scroll_t scroll;}
#define scui_class_string(widget, string) union {scui_widget_t widget; scui_string_t string;}
#define scui_class_window_maker(widget, window) union {scui_widget_maker_t widget; scui_window_maker_t window;}
#define scui_class_custom_maker(widget, custom) union {scui_widget_maker_t widget; scui_custom_maker_t custom;}
#define scui_class_scroll_maker(widget, scroll) union {scui_widget_maker_t widget; scui_scroll_maker_t scroll;}
#define scui_class_string_maker(widget, string) union {scui_widget_maker_t widget; scui_string_maker_t string;}
// 一级派生:对象控件
#define scui_class_objbtn(widget, objbtn) union {scui_widget_t widget; scui_objbtn_t objbtn;}
#define scui_class_objbtn_maker(widget, objbtn) union {scui_widget_maker_t widget; scui_objbtn_maker_t objbtn;}
// 一级派生:扩展控件
#define scui_class_button(widget, button) union {scui_widget_t widget; scui_button_t button;}
#define scui_class_chart(widget,  chart)  union {scui_widget_t widget; scui_chart_t  chart;}
#define scui_class_button_maker(widget, button) union {scui_widget_maker_t widget; scui_button_maker_t button;}
#define scui_class_chart_maker(widget,  chart)  union {scui_widget_maker_t widget; scui_chart_maker_t  chart;}
#endif

/*@brief 控件状态风格
 */
typedef struct {
    uint8_t state:1;            /* 控件隐藏:0;控件显示:1; */
    uint8_t trans:1;            /* 背景显示:0;背景透明:1; */
    uint8_t cover:1;            /* 未知覆盖:0;完全覆盖:1; */
    uint8_t sched_anima:1;      /* 控件调度动画标记 */
    uint8_t indev_ptr:1;        /* 输入事件响应标记:ptr */
    uint8_t indev_enc:1;        /* 输入事件响应标记:enc */
    uint8_t indev_key:1;        /* 输入事件响应标记:key */
} scui_widget_style_t;

/*@brief 控件构造回调
 *@brief 控件析构回调
 *@brief 控件布局回调
 *@brief 控件事件回调
 */
typedef void (*scui_widget_cb_make_t)(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);
typedef void (*scui_widget_cb_burn_t)(scui_handle_t handle);
typedef void (*scui_widget_cb_layout_t)(scui_handle_t handle);
typedef scui_event_cb_t scui_widget_cb_invoke_t;

/*@brief 控件处理映射表
 */
typedef struct {
    scui_handle_t           size;   // 控件实例大小
    scui_handle_t           maker;  // 控件构造器大小
    scui_widget_type_t      base;   // 控件基类
    scui_widget_cb_make_t   make;   // 控件构造
    scui_widget_cb_burn_t   burn;   // 控件析构
    scui_widget_cb_invoke_t invoke; // 控件调用
    scui_widget_cb_layout_t layout; // 控件布局
} scui_widget_map_t;

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
    for (int64_t idx = 0; idx < (int64_t)widget->child_num; idx++)          \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/* 控件孩子列表宏迭代器(略过无效控件)(forward traverse) */
#define scui_widget_child_list_ftra(widget, idx)                            \
    for (int64_t idx = (int64_t)widget->child_num - 1; idx >= 0; idx--)     \
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)                 \

/*@brief 控件构造器
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_make(scui_widget_t *widget, scui_widget_maker_t *maker,
                      scui_handle_t *handle, bool layout);

/*@brief 控件析构器
 *@param widget 控件实例
 */
void scui_widget_burn(scui_widget_t *widget);

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
bool scui_widget_is_show(scui_handle_t handle);

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_is_hide(scui_handle_t handle);

/*@brief 用户资源获取
 *@param handle 控件句柄
 *@retval 用户资源
 */
void * scui_widget_user_data_get(scui_handle_t handle);

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

/*@brief 用户资源设置
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_set(scui_handle_t handle, void *user_data);

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
