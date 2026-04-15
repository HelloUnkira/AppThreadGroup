#ifndef SCUI_CANVAS_INF_H
#define SCUI_CANVAS_INF_H

/*@brief 画布控件图
 *@param handle 画布控件句柄
 *@param image  画布图资源
 */
void scui_canvas_image(scui_handle_t handle, scui_handle_t *image);

/*@brief 画布控件取控件树快照
 *@param handle   画布控件句柄
 *@param handle_t 目标控件树句柄
 */
void scui_canvas_snapshot(scui_handle_t handle, scui_handle_t handle_t);

#endif
