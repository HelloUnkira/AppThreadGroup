#ifndef SCUI_CUSTOM_H
#define SCUI_CUSTOM_H

typedef struct {
    scui_widget_t widget;
} scui_custom_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
} scui_custom_maker_t;
#pragma pack(pop)

/*@brief 自定义控件创建
 *@param maker  自定义控件创建参数
 *@param handle 自定义控件句柄
 *@param layout 通过布局创建
 */
void scui_custom_make(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 */
void scui_custom_burn(scui_handle_t handle);

/*@brief 自定义控件绘制文本
 *@param handle 自定义控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, scui_area_t *target, scui_string_args_t *args, scui_handle_t text);

#endif
