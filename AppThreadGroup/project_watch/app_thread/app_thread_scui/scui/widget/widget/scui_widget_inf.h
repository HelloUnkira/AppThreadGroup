#ifndef SCUI_WIDGET_INF_H
#define SCUI_WIDGET_INF_H

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_type(scui_handle_t handle);

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle);

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle);

/*@brief 子控件当前数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_now(scui_handle_t handle);

/*@brief 控件指定索引子控件
 *@param handle 控件句柄
 *@param index  控件指定索引([0:child_now): 映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_by_index(scui_handle_t handle, scui_handle_t index);

/*@brief 控件在父控件中索引
 *@param handle 控件句柄
 *@retval 父控件指定索引([0:child_now): 映射点)
 */
scui_handle_t scui_widget_child_to_index(scui_handle_t handle);

/*@brief 控件移动到父控件指定索引
 *@param handle 控件句柄
 *@param index  父控件指定索引([0:child_now): 映射点)
 */
void scui_widget_child_move_index(scui_handle_t handle, scui_handle_t index);

/*@brief 控件移动到父控件索引最前面
 *@param handle 控件句柄
 */
void scui_widget_child_move_foreground(scui_handle_t handle);

/*@brief 控件移动到父控件索引最后面
 *@param handle 控件句柄
 */
void scui_widget_child_move_background(scui_handle_t handle);

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle);

/*@brief 控件画布
 *@param handle 控件句柄
 *@retval 控件画布
 */
scui_surface_t * scui_widget_surface(scui_handle_t handle);

/*@brief 控件画布创建
 *@param handle  控件句柄
 *@param surface 画布参数(.format;.hor_res;.ver_res;)
 */
void scui_widget_surface_create(scui_handle_t handle, scui_surface_t *surface);

/*@brief 控件画布销毁
 *@param handle 控件句柄
 */
void scui_widget_surface_destroy(scui_handle_t handle);

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_is_show(scui_handle_t handle);

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_is_hide(scui_handle_t handle);

/*@brief 用户资源获取
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_get(scui_handle_t handle, void **user_data);

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_get(scui_handle_t handle, scui_alpha_t *alpha);

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_get(scui_handle_t handle, scui_handle_t *image);

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_get(scui_handle_t handle, scui_color_t *color);

/*@brief 用户资源设置
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_set(scui_handle_t handle, void *user_data);

/*@brief 控件透明度设置
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recurse);

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image);

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_t color);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 销毁所有子控件
 *@param handle 控件句柄
 */
void scui_widget_clean(scui_handle_t handle);

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle);

/*@brief 创建控件
 *@param maker  控件构造实例
 *@param handle 控件句柄
 */
void scui_widget_create(void *maker, scui_handle_t *handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point);

/*@brief 控件移动
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_move_ofs(scui_handle_t handle, scui_point_t *offset);

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

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param clip   剪切域
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, scui_area_t *clip, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件显示
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_show(scui_handle_t handle, bool delay);

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_hide(scui_handle_t handle, bool delay);

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 */
void scui_widget_event_clear(scui_handle_t handle);

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 控件事件包含检查
 *@param event 事件
 *@retval 包含不包含
 */
bool scui_widget_event_inside(scui_event_t *event);

/*@brief 控件事件响应转移
 *@param event 事件
 */
void scui_widget_event_shift(scui_event_t *event);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 对象动画停止
 *@param handle 对象句柄
 */
void scui_widget_anima_stop(scui_handle_t handle);

/*@brief 对象动画启动
 *@param handle 对象句柄
 */
void scui_widget_anima_start(scui_handle_t handle);

/*@brief 对象动画清理
 *@param handle 对象句柄
 */
void scui_widget_anima_clear(scui_handle_t handle);

/*@brief 对象动画销毁
 *@param handle 对象句柄
 */
void scui_widget_anima_destroy(scui_handle_t handle);

/*@brief 对象动画创建
 *@param handle 对象句柄
 *@param number 动画数量
 */
void scui_widget_anima_create(scui_handle_t handle, scui_handle_t number);

/*@brief 对象动画提交
 *@param handle 对象句柄
 *@param anima  动画句柄
 *@param step   动画工步(范围:[1, 100])
 */
void scui_widget_anima_submit(scui_handle_t handle, scui_handle_t anima, scui_handle_t step);

/*************************************************************************************************/

/*@brief 控件动画回调(透明渐入)
 */
scui_handle_t scui_widget_anima_fade_in(scui_handle_t handle, int32_t time, int32_t delay);

/*@brief 控件动画回调(透明渐出)
 */
scui_handle_t scui_widget_anima_fade_out(scui_handle_t handle, int32_t time, int32_t delay);

/*@brief 控件动画回调(缩小宽度)
 */
scui_handle_t scui_widget_anima_zoom_out_w(scui_handle_t handle, int32_t time, int32_t delay);

/*@brief 控件动画回调(缩小高度)
 */
scui_handle_t scui_widget_anima_zoom_out_h(scui_handle_t handle, int32_t time, int32_t delay);

#endif
