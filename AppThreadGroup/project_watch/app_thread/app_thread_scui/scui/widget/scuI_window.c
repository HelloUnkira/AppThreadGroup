/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口控件创建
 *@param maker  窗口控件创建参数
 *@param handle 窗口控件句柄
 *@param layout 通过布局创建
 */
void scui_window_create(scui_window_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_window);
    SCUI_ASSERT(maker->widget.parent == SCUI_HANDLE_INVALID);
    
    /* 创建窗口控件实例 */
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_window_t));
    memset(window, 0, sizeof(scui_window_t));
    
    scui_window_cfg_def(&window->cfg);
    
    window->level  = maker->level;
    window->buffer = maker->buffer;
    
    /* 是否需要创建自己的surface */
    if (maker->buffer) {
        scui_coord_t hor_res = scui_disp_get_hor_res();
        scui_coord_t ver_res = scui_disp_get_ver_res();
        /* 窗口是全屏的绘制区域 */
        SCUI_ASSERT(hor_res == maker->widget.clip.w);
        SCUI_ASSERT(ver_res == maker->widget.clip.h);
        uint32_t surface_res = hor_res * ver_res * SCUI_PIXEL_SIZE;
        window->widget.surface.pixel = SCUI_MEM_ALLOC(scui_mem_is_image, surface_res);
        window->widget.surface.line  = hor_res;
    }
    
    /* 创建基础控件实例 */
    scui_widget_create(&window->widget, &maker->widget, handle, layout);
    
    /* 为窗口控件添加指定的事件回调 */
    scui_widget_event_t event = {0};
    event.order    = scui_widget_order_current;
    event.event_cb = maker->widget.event_cb;
    
    /* 事件默认全局接收 */
    event.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_ptr_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_enc_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_key_all;
    scui_widget_event_add(*handle, &event);
}

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    scui_window_t *window = (void *)widget;
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&window->widget);
    
    /* 回收surface */
    if (scui_widget_surface_only(&window->widget.surface)) {
        SCUI_MEM_FREE(window->widget.surface.pixel);
    }
    
    /* 销毁窗口控件实例 */
    SCUI_MEM_FREE(window);
}

/*@brief 窗口配置参数默认
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_def(scui_window_cfg_t *cfg)
{
    for (uint8_t idx = 0; idx < 6; idx++) {
        cfg->sibling[idx] = SCUI_HANDLE_INVALID;
        cfg->sibling_attr[idx].floating = false;
        cfg->sibling_attr[idx].preload = true;
    }
}

/*@brief 窗口配置参数获取
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_get(scui_handle_t handle, scui_window_cfg_t *cfg)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    scui_window_t *window = (void *)widget;
    
    *cfg = window->cfg;
}

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_set(scui_handle_t handle, scui_window_cfg_t *cfg)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    scui_window_t *window = (void *)widget;
    
    window->cfg = *cfg;
}
