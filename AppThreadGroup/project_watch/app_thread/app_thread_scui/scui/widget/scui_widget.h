#ifndef SCUI_WIDGET_H
#define SCUI_WIDGET_H

/* 控件类型: */
typedef enum {
    /* 基础控件 */
    scui_widget_type_window,    /* 窗口控件(根控件) */
    scui_widget_type_custom,    /* 自定义控件(绘制图形) */
    scui_widget_type_scroll,    /* 可滚动控件 */
    scui_widget_type_image,     /* 图片控件 */
    scui_widget_type_label,     /* 文本控件 */
    /* 扩展控件 */
    scui_widget_type_num,
} scui_widget_type_t;

/* 控件状态风格: */
typedef struct {
    uint8_t trans:1;        /* 背景透明 */
    uint8_t cover:1;        /* 覆盖更新 */
    uint8_t state:1;        /* 控件隐藏:0;控件显示:1; */
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

/* 控件事件回调: */
typedef struct {
    app_sys_list_dln_t      dl_node;        /* 控件事件回调列表 */
    scui_widget_order_t     order;          /* 控件事件回调响应优先级 */
    scui_event_cb_t         event_cb;       /* 控件事件回调 */
    uint32_t                event;          /* 控件事件 */
} scui_widget_event_t;

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
    app_sys_list_dll_t      dl_list;        /* 控件事件回调列表 */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           myself;         /* 控件关联属性:自己 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    scui_handle_t          *child_list;     /* 控件关联属性:子控件列表 */
    uint32_t                child_num;      /* 控件关联属性:子控件数量 */
    scui_area_t             surface_clip;   /* 控件画布(图形上下文):画布剪切域 */
    scui_surface_t          surface;        /* 控件画布(图形上下文):画布实例 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_gradient_t   color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_t;

/* 控件布局资料: */
typedef struct {
    scui_widget_type_t      type;           /* 控件类型 */
    scui_widget_style_t     style;          /* 控件状态风格 */
    scui_area_t             clip;           /* 控件所在父控件区域 */
    scui_handle_t           parent;         /* 控件关联属性:父控件 */
    uint32_t                child_num;      /* 控件关联属性:子控件数量 */
    scui_handle_t           image;          /* 背景图片(如果有背景图片,优先绘制) */
    scui_color_gradient_t   color;          /* 纯色背景(如果没背景图片,颜色绘制) */
} scui_widget_maker_t;

/*@brief 控件创建
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 */
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker, scui_handle_t handle);

/*@brief 控件销毁
 *@param widget     控件实例
 *@param parent_way 来自父控件的销毁
 */
void scui_widget_destroy(scui_widget_t *widget, bool parent_way);

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle);

/*@brief 控件透明度设置
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha);

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle);

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image);

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_gradient_t scui_widget_color_get(scui_handle_t handle);

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_gradient_t color);

/*@brief 控件清除剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_clear(scui_widget_t *widget);

/*@brief 控件还原剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_reset(scui_widget_t *widget);

/*@brief 控件添加子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_add(scui_handle_t handle, scui_handle_t child);

/*@brief 控件移除子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_del(scui_handle_t handle, scui_handle_t child);

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件移动到目标坐标
 *@param handle 控件句柄
 */
void scui_widget_move(scui_handle_t handle, scui_point_t *point);

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

#endif
