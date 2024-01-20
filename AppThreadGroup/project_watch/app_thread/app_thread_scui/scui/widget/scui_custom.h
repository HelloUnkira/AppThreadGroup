#ifndef SCUI_CUSTOM_H
#define SCUI_CUSTOM_H

typedef struct {
    scui_widget_t widget;
} scui_custom_t;

typedef struct {
    scui_widget_maker_t widget;
} scui_custom_maker_t;

/*@brief 自定义控件创建
 *@param maker  自定义控件创建参数
 *@param handle 自定义控件句柄
 *@param layout 通过布局创建
 */
void scui_custom_create(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_custom_destroy(scui_handle_t handle, bool parent_way);

#endif
