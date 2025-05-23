#ifndef SCUI_WIDGET_OPT_H
#define SCUI_WIDGET_OPT_H

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 *@param dirty  脏标记(批量移动控件使用, 默认true)
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point, bool dirty);

/*@brief 控件移动
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param dirty  脏标记
 */
void scui_widget_move_ofs(scui_handle_t handle, scui_point_t *offset, bool dirty);

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param align   对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_opt_align_t align, scui_point_t *offset);

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param child   控件子控件句柄(为空则镜像所有子控件)
 *@param dir     镜像方向(水平镜像或垂直镜像)
 *@param recurse 递归处理(全局镜像有效)
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_handle_t child, scui_opt_dir_t dir, bool recurse);

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height);

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param dirty  脏标记
 */
void scui_widget_move_ofs_child_list(scui_handle_t handle, scui_point_t *offset, bool dirty);

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 *@param dirty  脏标记
 */
void scui_widget_move_ofs_child_list_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range, bool dirty);

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param target 控件句柄(目标控件)
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_handle_t *target, scui_point_t *offset, scui_opt_pos_t pos);

/*@prief 事件滚动状态检查更新
 *@param type 0x00:锁定; 0x01:解锁; 0x02:检查
 *@param key  锁定标记句柄(浮动校验密钥)
 */
bool scui_widget_global_scroll_flag(uint8_t state, scui_handle_t *key);

#endif
