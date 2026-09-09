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

/*@brief 布局控件grid轨道(列/行模板)
 *@param handle 布局控件句柄
 *@param way    false:列模板(横向轨道); true:行模板(纵向轨道)
 *@param size   轨道尺寸数组(定值, 长度num)
 *@param num    轨道数量
 *@param gap    轨道间距
 */
void scui_layout_grid_way(scui_handle_t handle, bool way,
    const scui_coord_t *size, scui_coord_t num, scui_coord_t gap);

/*@brief 布局控件grid子控件落位
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param col      起始列
 *@param row      起始行
 *@param span     跨度(x=跨列; y=跨行)
 *@param align    cell内对齐(水平l/r/hor | 垂直u/d/ver)(STRETCH通过stretch)
 *@param stretch  拉伸方向(scui_opt_dir_hor|scui_opt_dir_ver)
 */
void scui_layout_grid_cell(scui_handle_t handle, scui_handle_t handle_c,
    scui_coord_t col, scui_coord_t row, scui_point_t span,
    scui_opt_pos_t align, scui_opt_dir_t stretch);

#endif