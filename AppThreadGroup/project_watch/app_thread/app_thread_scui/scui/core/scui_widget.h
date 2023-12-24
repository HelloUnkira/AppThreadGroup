#ifndef SCUI_WIDGET_H
#define SCUI_WIDGET_H

/* 控件注册事件回调响应顺序 */
typedef enum {
    scui_widget_order_before,   /* 主事件处理前回调 */
    scui_widget_order_after,    /* 主事件处理后回调 */
} scui_widget_order_t;

/* 控件注册事件回调: */
typedef struct {
    app_sys_list_dln_t dl_node;
    scui_widget_order_t order;
    uint32_t event;
} scui_widget_event_cb_t;

/* 控件类型: */
typedef enum {
    scui_widget_type_custom,
    scui_widget_type_window,
    scui_widget_type_num,
} scui_widget_type_t;

/* 控件状态: */
typedef enum {
    scui_widget_status_show,
    scui_widget_status_hide,
} scui_widget_status_t;

/* 控件风格: */
typedef enum {
    scui_widget_style_is_normal      = 0x00,
    scui_widget_style_is_transparent = 0x01,
    scui_widget_style_is_overlay     = 0x10,
} scui_widget_style_t;

/* 控件列表: */
typedef struct {
    uint32_t child_num;
    scui_handle_t *child_list;
} scui_widget_list_t;

/* 控件绘制环境: */
typedef struct {
    scui_area_t   clip;     // 剪切域(自动转化为canvas区域)
    scui_handle_t canvas;   // 画布句柄
} scui_widget_gc_t;

/* 控件颜色: */
typedef struct {
    scui_color8888_t color;         // 前景色
    scui_color8888_t color_render;  // 渐变色
} scui_widget_color_t;

/* 控件图片信息: */
typedef struct {
    scui_handle_t handle;
} scui_widget_image_t;

/* 控件 */
/* 控件基础信息: */
/* 控件只处理最基础绘制: */
typedef struct {
    scui_widget_type_t   type;      // 控件类型
    scui_widget_style_t  style;     // 控件风格
    scui_widget_status_t status;    // 控件状态
    scui_area_t          clip;      // 控件所在父容器区域
    scui_event_cb_t      event_cb;  // 控件事件响应回调(主回调)
    app_sys_list_dll_t   dl_list;   // 控件事件注册回调列表
    scui_handle_t        parent;    // 控件关联属性:父容器
    scui_handle_t        handle;    // 控件关联属性:自己
    scui_widget_list_t   list;      // 控件关联属性:子控件列表
    scui_widget_gc_t     gc;        // 控件绘制环境
    scui_alpha_t         alpha;     // 控件透明度
    scui_widget_color_t  bg_color;  // 如果没有图片,则绘制纯色背景
    scui_widget_image_t  bg_image;  // 如果有背景图片,绘制背景图片
} scui_widget_t;

/*@brief 控件alpha获取
 *@param widget 控件实例
 *@param alpha  控件alpha
 */
scui_alpha_t scui_widget_alpha_get(scui_widget_t *widget);

/*@brief 控件alpha设置
 *@param widget 控件实例
 *@param alpha  控件alpha
 */
void scui_widget_alpha_set(scui_widget_t *widget, scui_alpha_t alpha);



#endif
