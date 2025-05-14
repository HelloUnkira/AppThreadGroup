/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_window_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_window_t *window = widget;
    scui_window_maker_t *window_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_window));
    SCUI_ASSERT(widget_maker->parent == SCUI_HANDLE_INVALID);
    /* 注意:要求只能是根控件才可以创建窗口 */
    
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案禁用窗口独立画布 */
    window_maker->buffer = false;
    #endif
    
    /* 创建surface */
    if (window_maker->buffer) {
        
        scui_pixel_cf_t p_cf = window_maker->format; // SCUI_PIXEL_CF_DEF_A;
        scui_coord_t hor_res = widget->clip.w;
        scui_coord_t ver_res = widget->clip.h;
        scui_widget_clip_clear(widget, true);
        scui_widget_surface_create(*handle, p_cf, hor_res, ver_res);
        scui_widget_surface_refr(*handle, true);
    }
    
    window->level       = window_maker->level;
    window->buffer      = window_maker->buffer;
    window->resident    = window_maker->resident;
    window->hang_only   = window_maker->hang_only;
    window->format      = window_maker->format;
    
    // 初始化默认切换类型为自适应
    for(scui_handle_t idx = 0; idx < 4; idx++)
        window->switch_type[idx] = scui_window_switch_auto;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_window_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    /* 回收surface */
    if (scui_widget_surface_only(widget))
        scui_widget_surface_destroy(widget->myself);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
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
