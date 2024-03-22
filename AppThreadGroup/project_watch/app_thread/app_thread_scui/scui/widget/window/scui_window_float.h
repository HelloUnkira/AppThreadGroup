#ifndef SCUI_WINDOW_FLOAT_H
#define SCUI_WINDOW_FLOAT_H

typedef struct {
    scui_handle_t               list[4];    /* 0:临近上;1:临近下;2:临近左;3:临近右; */
    scui_handle_t               main;       /* 主窗口 */
    scui_handle_t               target;     /* 目标浮窗 */
    scui_event_dir_t            dir;        /* 交互方向 */
    scui_event_pos_t            pos;        /* 相对位置 */
    scui_handle_t               anima;      /* 交互动画 */
    uint8_t execute:1;
    uint8_t cover:1;
    uint8_t lock:1;
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

/*@brief 窗口工作状态
 *@retval 工作状态
 */
bool scui_window_float_status(void);

/*@brief 窗口移动伴随透明度更新
 *@param handle 窗口控件句柄
 *@param point  窗口移动坐标
 */
void scui_window_float_move_with_alpha(scui_handle_t handle, scui_point_t *point);

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_start(void *instance);

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_ready(void *instance);

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_expired(void *instance);

/*@brief 窗口浮动动画自动化
 */
void scui_window_float_anima_auto(int32_t value_s, int32_t value_e, uint32_t peroid);

/*@brief 窗口浮动动画回调
 *@param handle 窗口句柄
 *@param inout  true:进入;false:退出;
 */
void scui_window_float_anima_inout(scui_handle_t handle, bool inout);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_float_event_grasp_show(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_float_event_grasp_hide(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_float_event_grasp_key(scui_event_t *event);

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_float_event_grasp_ptr(scui_event_t *event);

/*@brief 窗口浮动事件检查回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_float_event_check_ptr(scui_event_t *event);

#endif
