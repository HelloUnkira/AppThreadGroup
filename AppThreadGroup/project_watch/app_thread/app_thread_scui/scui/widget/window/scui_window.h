#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    scui_handle_t sibling[6];           /* 0:临近上;1:临近下;2:临近左;3:临近右;4:上一层;5:下一层; */
    struct {
        uint8_t preload:1;              /* 是否预加载,默认预加载 */
    } sibling_attr[6];
} scui_window_cfg_t;

typedef struct {
    scui_widget_t     widget;
    scui_window_cfg_t cfg;              /* 窗口配置属性 */
    uint8_t           level:6;          /* 窗口所在层级(数字越大越高) */
    uint8_t           buffer:1;         /* 窗口是否使用独立画布 */
} scui_window_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
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

/*@brief 窗口配置参数默认
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_def(scui_window_cfg_t *cfg);

/*@brief 窗口配置参数获取
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_get(scui_handle_t handle, scui_window_cfg_t *cfg);

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_set(scui_handle_t handle, scui_window_cfg_t *cfg);

#endif
