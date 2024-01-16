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
    uint8_t indev_ptr:1;    /* 输入事件处理:ptr */
    uint8_t indev_enc:1;    /* 输入事件处理:enc */
    uint8_t indev_key:1;    /* 输入事件处理:key */
} scui_widget_style_t;

/* 控件注册事件回调响应 */
/* 任何传递给控件的事件都会尝试性的经过该三步流程进行响应 */
/* 如果三步处理均不能完全吸收则事件继续向兄弟传递或者冒泡上去 */
/* 处理中回调如果由外界提供则不走默认处理流程,否则响应默认控件事件传导 */
typedef enum {
    scui_widget_order_before,   /* 主事件处理前回调 */
    scui_widget_order_current,  /* 主事件处理中回调 */
    scui_widget_order_after,    /* 主事件处理后回调 */
} scui_widget_order_t;

/* 控件注册事件回调: */
typedef struct {
    app_sys_list_dln_t  dl_node;
    scui_event_cb_t     event_cb;
    scui_widget_order_t order;
    uint32_t            event;
} scui_widget_event_t;

/* 控件列表: */
typedef struct {
    uint32_t       child_num;
    scui_handle_t *child_list;
} scui_widget_list_t;

/* 控件绘制环境: */
typedef struct {
    scui_area_t   clip;     // 剪切域(自动转化为surface区域)
    scui_handle_t surface;  // 画布句柄
} scui_widget_gc_t;

/* 控件图片信息: */
typedef struct {
    scui_handle_t handle;
} scui_widget_image_t;

/* 控件基础信息: */
/* 控件只处理最基础绘制: */
typedef struct {
    scui_widget_type_t      type;       // 控件类型
    scui_widget_style_t     style;      // 控件状态风格
    app_sys_list_dll_t      dl_list;    // 控件事件注册回调列表
    scui_area_t             clip;       // 控件所在父容器区域
    scui_handle_t           parent;     // 控件关联属性:父容器
    scui_handle_t           handle;     // 控件关联属性:自己
    scui_widget_list_t      list;       // 控件关联属性:子控件列表
    scui_widget_gc_t        gc;         // 控件绘制环境
    scui_widget_image_t     image;      // 如果有背景图片,绘制背景图片
    scui_color_gradient_t   color;      // 如果没有图片,则绘制纯色背景
    scui_alpha_t            alpha;      // 控件透明度
} scui_widget_t;

/* 控件布局资料 */
typedef struct {
    scui_widget_type_t      type;       // 控件类型
    scui_widget_style_t     style;      // 控件状态风格
    scui_area_t             clip;       // 控件所在父容器区域
    scui_handle_t           parent;     // 控件关联属性:父容器
    scui_widget_image_t     image;      // 如果有背景图片,绘制背景图片
    scui_color_gradient_t   color;      // 如果没有图片,则绘制纯色背景
    scui_alpha_t            alpha;      // 控件透明度
} scui_widget_maker_t;

/*@brief剪切域说明:
 *      scui_widget_t->clip:        "控件在父控件的恒定剪切域"
 *      scui_widget_t->gc.clip:     "控件在当次绘制时的剪切域"
 *      scui_widget_t->gc.surface:
 *      scui_surface_t->surface:    "在画布中的剪切域"
 *      画布不是完全都存在独立资源空间
 *      只有一个独立场景可能存在资源空间
 *      其他控件的画布可能都只是相对独立场景资源空间的一个映射
 *      如果一个独立场景都不存在资源空间(映射到绘制画布资源空间)
 *      那么它们都会是相对绘制画布资源空间的一个映射
 */








/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件alpha获取
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle);

/*@brief 控件alpha设置
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha);

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle);



#endif
