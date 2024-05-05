#ifndef SCUI_RING_H
#define SCUI_RING_H

typedef struct {
    scui_widget_t widget;
} scui_ring_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
} scui_ring_maker_t;
#pragma pack(pop)

/*@brief 圆环控件创建
 *@param maker  圆环控件创建参数
 *@param handle 圆环控件句柄
 *@param layout 通过布局创建
 */
void scui_ring_create(scui_ring_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 圆环控件销毁
 *@param handle 圆环控件句柄
 */
void scui_ring_destroy(scui_handle_t handle);

#endif
