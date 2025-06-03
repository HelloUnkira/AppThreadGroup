#ifndef SCUI_LINEAR_INF_H
#define SCUI_LINEAR_INF_H

typedef struct {
    scui_handle_t       draw_idx;
    scui_handle_t       handle_m;
    scui_handle_t       handle_s;
} scui_linear_item_t;

/*@brief 列表控件条目参数(获取)
 *@param handle 列表控件句柄
 *@param item   条目参数
 */
void scui_linear_item_gets(scui_handle_t handle, scui_linear_item_t *item);

/*@brief 列表控件条目参数(设置)
 *@param handle 列表控件句柄
 *@param item   条目参数
 */
void scui_linear_item_sets(scui_handle_t handle, scui_linear_item_t *item);

/*@brief 列表子控件事件响应回调(从)
 *@param event 事件
 */
void scui_linear_s_event(scui_event_t *event);

/*@brief 列表子控件事件响应回调(主)
 *@param event 事件
 */
void scui_linear_m_event(scui_event_t *event);

#endif
