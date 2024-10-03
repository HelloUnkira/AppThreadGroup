#ifndef SCUI_WINDOW_FLOAT_H
#define SCUI_WINDOW_FLOAT_H

typedef struct {
    scui_handle_t               list[4];    /* 0:临近上;1:临近下;2:临近左;3:临近右; */
    scui_handle_t               main;       /* 主窗口 */
    scui_handle_t               target;     /* 目标浮窗 */
    scui_handle_t               anima;      /* 交互动画 */
    scui_handle_t               key;        /* 全局滚动锁定 */
    scui_opt_dir_t              dir;        /* 交互方向 */
    scui_opt_pos_t              pos;        /* 相对位置 */
    uint8_t execute:1;
    uint8_t cover:1;
    uint8_t hold:1;
} scui_window_float_t;

/*@brief 窗口配置参数获取
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_float_cfg_get(scui_window_float_t *cfg);

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_float_cfg_set(scui_window_float_t *cfg);

/*@brief 窗口正在工作中
 *@retval 正在工作中
 */
bool scui_window_float_running(void);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_show(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_hide(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_key(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_ptr(scui_event_t *event);

/*@brief 窗口浮动事件检查回调
 *@param event 事件
 */
void scui_window_float_event_check_ptr(scui_event_t *event);

#endif
