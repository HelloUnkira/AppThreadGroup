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
void scui_window_make(scui_window_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建窗口控件实例 */
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_window_t));
    memset(window, 0, sizeof(scui_window_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    scui_widget_make(&window->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_window));
    SCUI_ASSERT(widget_maker.parent == SCUI_HANDLE_INVALID);
    /* 注意:要求只能是根控件才可以创建窗口(根控件==窗口) */
    
    /* 创建surface */
    if (maker->buffer) {
        
        #if 1
        scui_pixel_cf_t p_cf = maker->format;
        #elif SCUI_MEM_SIZE_TYPE == 0
        scui_pixel_cf_t p_cf = maker->format;
        #elif SCUI_MEM_SIZE_TYPE == 1
        scui_pixel_cf_t p_cf = SCUI_PIXEL_CF_DEF_A;
        #else
        scui_pixel_cf_t p_cf = maker->format;
        #endif
        
        scui_coord_t hor_res = maker->widget.clip.w;
        scui_coord_t ver_res = maker->widget.clip.h;
        scui_widget_clip_clear(&window->widget, true);
        scui_widget_surface_create(*handle, p_cf, hor_res, ver_res);
        scui_widget_surface_refr(*handle, true);
    }
    
    window->level       = maker->level;
    window->buffer      = maker->buffer;
    window->resident    = maker->resident;
    window->hang_only   = maker->hang_only;
    window->format      = maker->format;
    
    // 初始化默认为自动切换类型
    for(scui_handle_t idx = 0; idx < 4; idx++)
        window->switch_type[idx] = scui_window_switch_auto;
}

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 */
void scui_window_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    /* 回收surface */
    if (scui_widget_surface_only(widget))
        scui_widget_surface_destroy(widget->myself);
    
    /* 销毁基础控件实例 */
    SCUI_ASSERT(widget->type == scui_widget_type_window);
    scui_widget_burn(&window->widget);
    
    /* 销毁窗口控件实例 */
    SCUI_MEM_FREE(window);
}

/*@brief 窗口配置参数获取
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_get(scui_handle_t handle, scui_handle_t sibling[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        sibling[idx] = window->sibling[idx];
}

/*@brief 窗口配置参数设置
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_set(scui_handle_t handle, scui_handle_t sibling[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        window->sibling[idx] = sibling[idx];
}

/*@brief 窗口配置参数获取
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_get(scui_handle_t handle, scui_window_switch_type_t switch_type[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        switch_type[idx] = window->switch_type[idx];
}

/*@brief 窗口配置参数设置
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_set(scui_handle_t handle, scui_window_switch_type_t switch_type[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        window->switch_type[idx] = switch_type[idx];
}
