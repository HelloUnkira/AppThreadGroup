#ifndef SCUI_WIDGET_ATTR_H
#define SCUI_WIDGET_ATTR_H

/*@brief 控件添加子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_add(scui_handle_t handle, scui_handle_t child);

/*@brief 控件移除子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_del(scui_handle_t handle, scui_handle_t child);

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle);

/*@brief 控件透明度设置
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha);

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle);

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image);

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_gradient_t scui_widget_color_get(scui_handle_t handle);

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_gradient_t color);

/*@brief 控件清除剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_clear(scui_widget_t *widget);

/*@brief 控件还原剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_reset(scui_widget_t *widget);

#endif