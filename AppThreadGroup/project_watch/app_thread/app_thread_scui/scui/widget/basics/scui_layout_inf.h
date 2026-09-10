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

/*@brief 布局控件(方向/间距/对齐)
 *@param handle 布局控件句柄
 *@param way    轨道方向(0:水平;1:垂直)
 *@param span   间距(水平/垂直)(EVNELY:-1)
 *@param alig_o 轨道间对齐
 *@param alig_i 轨道内对齐
 */
void scui_layout_flex_way(scui_handle_t handle, bool way, scui_point_t span,
    scui_opt_pos_t alig_o, scui_opt_pos_t alig_i);

/*@brief 布局控件子控件分组
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param group    组编号
 */
void scui_layout_flex_group(scui_handle_t handle, scui_handle_t handle_c, scui_coord_t group);

/*@brief 布局控件(列/行模板)
 *@param handle 布局控件句柄
 *@param way    轨道(0:横向/列; 1:纵向/行)
 *@param size   轨道尺寸数组(长度num)
 *@param num    轨道数量
 *@param gap    轨道间距
 */
void scui_layout_grid_way(scui_handle_t handle, bool way,
    const scui_coord_t *size, scui_coord_t num, scui_coord_t gap);

/*@brief 布局控件落位
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param pos      行列(x列;y行)
 *@param span     跨度(x列;y行)
 *@param align    对齐(cell)
 *@param stretch  拉伸方向(hor|ver)
 */
void scui_layout_grid_cell(scui_handle_t handle, scui_handle_t handle_c,
    scui_point_t pos, scui_point_t span, scui_opt_pos_t align, scui_opt_dir_t stretch);

#endif