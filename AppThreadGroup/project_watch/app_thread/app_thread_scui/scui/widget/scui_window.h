#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    uint8_t style_float:1;      // 跟手(或事件)浮动交互
    uint8_t style_normal:1;     // 跟手(或事件)常规交互(默认风格,无论设置与否)
} scui_window_style_t;

typedef struct {
    scui_widget_t       widget;
    scui_window_style_t style[4];       /* 交互风格 */
    scui_handle_t       sibling[4];     /* 0:上;1:下;2:左;3:右; */
    uint8_t             level:6;        /* 窗口所在层级(数字越大越高) */
} scui_window_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_window_style_t style[4];       /* 交互风格 */
    scui_handle_t       sibling[4];     /* 0:上;1:下;2:左;3:右; */
    uint8_t             level:6;        /* 窗口所在层级(数字越大越高) */
    uint8_t             buffer:1;       /* 窗口是否使用独立画布 */
} scui_window_maker_t;
#pragma pack(pop)

/*@brief 窗口控件创建
 *@param maker  窗口控件创建参数
 *@param handle 窗口控件句柄
 *@param layout 通过布局创建
 */
void scui_window_create(scui_window_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_destroy(scui_handle_t handle);

#endif
