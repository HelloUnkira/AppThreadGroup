#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    scui_widget_t widget;
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
} scui_string_maker_t;
#pragma pack(pop)

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_create(scui_string_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 */
void scui_string_destroy(scui_handle_t handle);

#endif
