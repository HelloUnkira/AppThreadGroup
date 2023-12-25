#ifndef SCUI_WIDGET_GC_H
#define SCUI_WIDGET_GC_H

/*@brief 控件gc重置
 *@param widget 控件实例
 */
void scui_widget_gc_reset(scui_widget_t *widget);

/*@brief  控件gc获取画布
 *@param  widget 控件实例
 *@retval 画布句柄
 */
scui_handle_t scui_widget_gc_surface_get(scui_widget_t *widget);

/*@brief 控件gc设置画布
 *@param widget 控件实例
 *@param surface 画布句柄
 */
void scui_widget_gc_surface_set(scui_widget_t *widget, scui_handle_t surface);

/*@brief 控件gc获取剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_get(scui_widget_t *widget, scui_area_t *area);

/*@brief 控件gc设置剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_set(scui_widget_t *widget, scui_area_t *area);

/*@brief 控件gc追加剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_merge(scui_widget_t *widget, scui_area_t *clip);

/*@brief 控件gc清除剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_clear(scui_widget_t *widget);

/*@brief 控件gc剪切域为空检查
 *@param widget 控件实例
 *@retval 剪切域为空
 */
bool scui_widget_gc_clip_empty(scui_widget_t *widget);

/*@brief 控件gc更新画布alpha
 *@param widget 控件实例
 *@param alpha  画布alpha
 */
void scui_widget_gc_alpha_update(scui_widget_t *widget, uint8_t alpha);





#endif
