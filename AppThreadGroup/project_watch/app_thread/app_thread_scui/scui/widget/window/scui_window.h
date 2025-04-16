#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct { scui_class_widget(widget)
    /* 外部域: */
    uint16_t        level:6;        /* 窗口所在层级(数字越大越高) */
    uint16_t        buffer:1;       /* 窗口是否使用独立画布 */
    uint16_t        resident:1;     /* 窗口常驻标记(特殊使用) */
    uint16_t        hang_only:1;    /* 窗口悬挂标记(特殊使用) */
    scui_pixel_cf_t format;         /* 窗口独立画布基础像素格式 */
    /* 内部域: */
    scui_handle_t sibling[4];                   /* 临近界面句柄:0:上;1:下;2:左;3:右; */
    scui_window_switch_type_t switch_type[4];   /* 临近界面交互风格 */
} scui_window_t;

#pragma pack(push, 1)
typedef struct { scui_class_widget_maker(widget)
    /* 外部域: */
    uint16_t        level:6;        /* 窗口所在层级(数字越大越高) */
    uint16_t        buffer:1;       /* 窗口是否使用独立画布 */
    uint16_t        resident:1;     /* 窗口常驻标记(特殊使用) */
    uint16_t        hang_only:1;    /* 窗口悬挂标记(特殊使用) */
    scui_pixel_cf_t format;         /* 窗口独立画布基础像素格式 */
} scui_window_maker_t;
#pragma pack(pop)

/*@brief 窗口控件创建
 *@param maker  窗口控件创建参数
 *@param handle 窗口控件句柄
 *@param layout 通过布局创建
 */
void scui_window_make(scui_window_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_burn(scui_handle_t handle);

/*@brief 窗口配置参数获取
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_get(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数设置
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_set(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数获取
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_get(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

/*@brief 窗口配置参数设置
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_set(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

#endif
