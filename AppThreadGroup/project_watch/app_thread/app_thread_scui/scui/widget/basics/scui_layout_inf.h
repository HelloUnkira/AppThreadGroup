#ifndef SCUI_LAYOUT_INF_H
#define SCUI_LAYOUT_INF_H

/*@brief 布局控件子控件对齐
 *@param handle   布局控件句柄
 *@param handle_t 对齐目标控件(为空相对父)
 *@param handle_c 子控件句柄
 *@param align    对齐
 *@param offset   偏移
 */
void scui_layout_item_align(scui_handle_t handle, scui_handle_t handle_t,
    scui_handle_t handle_c, scui_align_t align, scui_point_t *offset);

/*@brief 布局控件(方向/对齐/间距)
 *@param handle 布局控件句柄
 *@param way    摆放方向(0:水平;1:垂直)
 *@param align  对齐(水平/垂直)
 *@param span   间距(水平/垂直)(EVNELY:-1)
 */
void scui_layout_flex_way(scui_handle_t handle, bool way, scui_opt_pos_t align, scui_point_t span);

/*@brief 布局控件子控件分组
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param group    组编号
 */
void scui_layout_flex_group(scui_handle_t handle, scui_handle_t handle_c, scui_coord_t group);

#endif