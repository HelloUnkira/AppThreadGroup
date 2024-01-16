#ifndef SCUI_WIDGET_LIST_H
#define SCUI_WIDGET_LIST_H

/*@brief 控件列表初始化
 *@param widget    控件实例
 *@param child_num 控件子控件数量
 */
void scui_widget_list_init(scui_widget_t *widget, uint32_t child_num);

/*@brief 控件列表反初始化
 *@param widget 控件实例
 */
void scui_widget_list_deinit(scui_widget_t *widget);

/*@brief 控件列表添加子控件
 *@param widget 控件实例
 *@param handle 控件子控件
 */
void scui_widget_list_child_add(scui_widget_t *widget, scui_handle_t handle);

/*@brief 控件列表移除子控件
 *@param widget 控件实例
 *@param handle 控件子控件
 */
void scui_widget_list_child_del(scui_widget_t *widget, scui_handle_t handle);

/*@brief 控件列表清除剪切域
 *@param widget 控件实例
 */
void scui_widget_list_clip_clear(scui_widget_t *widget);









/*@brief 控件列表为指定子控件追加剪切域
 *@param widget     控件实例
 *@param clip       剪切域
 *@param widget_sub 子控件
 */
void scui_widget_list_clip_merge(scui_widget_t *widget, scui_area_t *clip, scui_widget_t *widget_sub);

/*@brief 控件列表更新画布alpha
 *@param widget 控件实例
 *@param alpha  画布alpha
 */
void scui_widget_list_alpha_update(scui_widget_t *widget, uint8_t alpha);



#endif
