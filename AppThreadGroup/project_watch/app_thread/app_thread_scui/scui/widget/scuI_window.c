/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口控件创建
 *@param maker  窗口控件创建参数
 *@param handle 窗口控件句柄
 *@param layout 通过布局创建
 */
void scui_window_create(scui_window_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建窗口控件实例 */
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_window_t));
    memset(window, 0, sizeof(scui_window_t));
    
    /* 是否需要创建自己的surface */
    if (maker->buffer) {
        scui_coord_t hor_res = scui_disp_get_hor_res();
        scui_coord_t ver_res = scui_disp_get_hor_res();
        uint32_t surface_res = hor_res * ver_res * SCUI_PIXEL_SIZE;
        window->widget.surface.pixel = SCUI_MEM_ALLOC(scui_mem_is_image, surface_res);
    }
    
    /* 创建基础控件实例 */
    scui_widget_create(&window->widget, &maker->widget, handle, layout);
    
    /* 为窗口添加指定的事件回调 */
    scui_widget_event_t event = {0};
    event.order    = scui_widget_order_current;
    event.event_cb = maker->event_cb;
    
    event.event = scui_event_scene_show;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_scene_hide;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_scene_focus_get;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_scene_focus_lost;
    scui_widget_event_add(*handle, &event);
    
    /* 输入事件默认全局接收 */
    event.event = scui_event_ptr_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_enc_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_key_all;
    scui_widget_event_add(*handle, &event);
}

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_window_destroy(scui_handle_t handle, bool parent_way)
{
    scui_window_t *window = scui_handle_get(handle);
    SCUI_ASSERT(window != NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&window->widget, parent_way);
    
    /* 回收surface */
    if (scui_widget_surface_only(&window->widget.surface)) {
        SCUI_MEM_FREE(window->widget.surface.pixel);
    }
    
    /* 销毁窗口控件实例 */
    SCUI_MEM_FREE(window);
}
