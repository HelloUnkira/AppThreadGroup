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
    /* 创建窗口控件实例 */
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_window_t));
    memset(window, 0, sizeof(scui_window_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    scui_widget_create(&window->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_window));
    SCUI_ASSERT(widget_maker.parent == SCUI_HANDLE_INVALID);
    /* 注意:要求只能是根控件才可以创建窗口(根控件==窗口) */
    
    /* 创建surface */
    if (maker->buffer) {
        /* 注意:每个独立资源窗口是一个完整显示区域以简化逻辑与布局 */
        scui_pixel_cf_t p_cf = maker->format;
        scui_pixel_pb_t p_pb = scui_pixel_pb_shared;
        scui_coord_t hor_res = maker->widget.clip.w;
        scui_coord_t ver_res = maker->widget.clip.h;
        scui_widget_clip_clear(&window->widget, true);
        scui_widget_surface_create(*handle, p_cf, p_pb, hor_res, ver_res);
        scui_widget_surface_refr(*handle, true);
    }
    
    scui_window_cfg_def(&window->cfg);
    
    window->level       = maker->level;
    window->buffer      = maker->buffer;
    window->resident    = maker->resident;
    window->hang_only   = maker->hang_only;
    window->format      = maker->format;
}

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 回收surface */
    if (scui_widget_surface_only(widget))
        scui_widget_surface_destroy(widget->myself);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&window->widget);
    
    /* 销毁窗口控件实例 */
    SCUI_MEM_FREE(window);
}

/*@brief 窗口配置参数默认
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_def(scui_window_cfg_t *cfg)
{
    for (uint8_t idx = 0; idx < 4; idx++) {
        cfg->sibling[idx] = SCUI_HANDLE_INVALID;
        cfg->preload[idx] = false;
    }
}

/*@brief 窗口配置参数获取
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_get(scui_handle_t handle, scui_window_cfg_t *cfg)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_get(handle);
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    *cfg = window->cfg;
}

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_cfg_set(scui_handle_t handle, scui_window_cfg_t *cfg)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_get(handle);
    scui_window_t *window = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    window->cfg = *cfg;
}
