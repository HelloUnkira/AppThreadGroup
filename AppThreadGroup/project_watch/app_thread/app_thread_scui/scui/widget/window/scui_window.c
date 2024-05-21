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
    
    /* 注意:要求只能是根控件才可以创建窗口(根控件==窗口) */
    SCUI_ASSERT(maker->widget.parent == SCUI_HANDLE_INVALID);
    
    /* 创建窗口控件实例 */
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_type_none, sizeof(scui_window_t));
    memset(window, 0, sizeof(scui_window_t));
    
    /* 创建surface */
    if (maker->buffer) {
        scui_coord_t hor_res = scui_disp_get_hor_res();
        scui_coord_t ver_res = scui_disp_get_ver_res();
        uint32_t surface_res = hor_res * ver_res * SCUI_PIXEL_SIZE;
        /* 注意:每个独立资源窗口是一个完整显示区域以简化逻辑与布局 */
        SCUI_ASSERT(maker->widget.clip.x == 0 && maker->widget.clip.w == hor_res);
        SCUI_ASSERT(maker->widget.clip.y == 0 && maker->widget.clip.h == ver_res);
        window->widget.surface          = SCUI_MEM_ALLOC(scui_mem_type_none,  sizeof(scui_surface_t));
        window->widget.surface->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_res);
        window->widget.surface->format  = SCUI_PIXEL_FORMAT;
        window->widget.surface->hor_res = hor_res;
        window->widget.surface->ver_res = ver_res;
        window->widget.surface->alpha   = scui_alpha_cover;
        
        /* 添加一个检测机制,如果surface不可创建时,清空image内存重新创建 */
        if (window->widget.surface->pixel == NULL) {
            SCUI_LOG_WARN("memory deficit was caught");
            scui_image_cache_clear();
        }
        if (window->widget.surface->pixel == NULL) {
            window->widget.surface->pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_res);
            SCUI_ASSERT(window->widget.surface->pixel != NULL);
        }
    }
    
    /* 创建基础控件实例 */
    scui_widget_create(&window->widget, &maker->widget, handle, layout);
    
    scui_window_cfg_def(&window->cfg);
    
    window->level  = maker->level;
    window->buffer = maker->buffer;
    
    /* 为窗口控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = maker->widget.event_cb,};
    
    /* 事件默认全局接收 */
    cb_node.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &cb_node);
}

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&window->widget);
    
    /* 回收surface */
    if (scui_widget_surface_only(widget)) {
        SCUI_MEM_FREE(window->widget.surface->pixel);
        SCUI_MEM_FREE(window->widget.surface);
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
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    
    *cfg = window->cfg;
}

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_set(scui_handle_t handle, scui_window_cfg_t *cfg)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    
    window->cfg = *cfg;
}
