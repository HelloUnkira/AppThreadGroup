#ifndef SCUI_WIDGET_SURFACE_H
#define SCUI_WIDGET_SURFACE_H

/*@brief 控件画布创建
 *@param widget  控件实例
 *@param format  画布格式
 *@param hor_res 画布水平尺寸
 *@param ver_res 画布垂直尺寸
 */
void scui_widget_surface_create(scui_widget_t *widget,  scui_pixel_cf_t format,
                                scui_coord_t   hor_res, scui_coord_t    ver_res);

/*@brief 控件画布销毁
 *@param widget 控件实例
 */
void scui_widget_surface_destroy(scui_widget_t *widget);

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget);

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse);

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件画布重映射
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_widget_t *widget, scui_surface_t *surface);

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface);

#endif
