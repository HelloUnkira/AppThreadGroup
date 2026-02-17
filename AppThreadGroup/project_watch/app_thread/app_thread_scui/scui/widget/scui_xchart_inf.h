#ifndef SCUI_XCHART_INF_H
#define SCUI_XCHART_INF_H

/*@brief 图表控件数据列表更新(柱状图)
 *@param handle 图表控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_xchart_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max);

/*@brief 图表控件数据列表更新(折线图)
 *@param handle 图表控件句柄
 *@param vlist  数据列表
 */
void scui_xchart_line_data(scui_handle_t handle, scui_coord_t *vlist);

#endif
