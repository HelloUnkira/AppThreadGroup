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
 */
void scui_window_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_window_t *window = widget;
    scui_window_maker_t *window_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_window));
    SCUI_ASSERT(widget_maker->parent == SCUI_HANDLE_INVALID);
    /* 注意:要求只能是根控件才可以创建窗口 */
    
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案禁用窗口独立画布 */
    window_maker->buffer = false;
    #endif
    
    /* 常配置信息 */
    window->resident = window_maker->resident;
    window->buffer   = window_maker->buffer;
    window->level    = window_maker->level;
    window->format   = window_maker->format;
    if (window->format == scui_pixel_cf_def)
        window->format  = SCUI_PIXEL_CF_DEF;
    
    /* 创建surface */
    if (window->buffer) {
        scui_surface_t surface = {
            .format  = window->format,
            .hor_res = widget->clip.w,
            .ver_res = widget->clip.h,
        };
        
        scui_widget_clip_clear(widget, true);
        scui_widget_surface_create(widget->myself, &surface);
        scui_widget_surface_refr(widget->myself, true);
    }
    
    /* sibling, switch_type, switch_enc, switch_key */
    /* 既要满足静态配置的同时也应允许过程中动态更变 */
    
    /* 配置邻近窗口: */
    for (scui_coord_t idx = 0; idx < 4; idx++)
        window->sibling[idx] = window_maker->sibling[idx];
    
    /* 配置邻近窗口切换状态: */
    for (scui_coord_t idx = 0; idx < 4; idx++)
        window->switch_type[idx] = window_maker->switch_type[idx];
    
    /* 配置邻近窗口旋钮交互: */
    window->switch_enc = window_maker->switch_enc;
    window->switch_enc_way = window_maker->switch_enc_way;
    
    /* 配置邻近窗口按键交互: */
    window->switch_key = window_maker->switch_key;
    for (scui_coord_t idx = 0; idx < 4; idx++)
        window->switch_key_id[idx] = window_maker->switch_key_id[idx];
    
    /* 初始化默认切换类型为垂直优先 */
    if (window->switch_enc_way != scui_opt_dir_hor &&
        window->switch_enc_way != scui_opt_dir_ver)
        window->switch_enc_way  = scui_opt_dir_ver;
    
    /* 初始化默认切换类型为自适应 */
    for(scui_handle_t idx = 0; idx < 4; idx++)
    if (window->switch_type[idx] == scui_window_switch_none)
        window->switch_type[idx]  = scui_window_switch_auto;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_window_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    /* 回收本地资源 */
    SCUI_MEM_FREE(window->local_res);
    
    /* 回收surface */
    if (scui_widget_surface_only(widget))
        scui_widget_surface_destroy(widget->myself);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_window_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_window_t *window = (void *)widget;
    
    /* empty */
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

/*@brief 窗口配置参数获取
 *@param handle     窗口控件句柄
 *@param switch_enc 配置参数
 */
void scui_window_switch_enc_get(scui_handle_t handle, scui_opt_pos_t *switch_enc)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    SCUI_ASSERT(switch_enc != NULL);
    *switch_enc = window->switch_enc;
}

/*@brief 窗口配置参数设置
 *@param handle     窗口控件句柄
 *@param switch_enc 配置参数
 */
void scui_window_switch_enc_set(scui_handle_t handle, scui_opt_pos_t switch_enc)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    window->switch_enc = switch_enc;
}

/*@brief 窗口配置参数获取
 *@param handle     窗口控件句柄
 *@param switch_key 配置参数
 */
void scui_window_switch_key_get(scui_handle_t handle, scui_opt_pos_t *switch_key)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    SCUI_ASSERT(switch_key != NULL);
    *switch_key = window->switch_key;
}

/*@brief 窗口配置参数设置
 *@param handle     窗口控件句柄
 *@param switch_key 配置参数
 */
void scui_window_switch_key_set(scui_handle_t handle, scui_opt_pos_t switch_key)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    window->switch_key = switch_key;
}

/*@brief 窗口配置参数获取
 *@param handle         窗口控件句柄
 *@param switch_enc_way 配置参数
 */
void scui_window_switch_enc_way_get(scui_handle_t handle, scui_opt_dir_t *switch_enc_way)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    SCUI_ASSERT(switch_enc_way != NULL);
    *switch_enc_way = window->switch_enc_way;
}

/*@brief 窗口配置参数设置
 *@param handle         窗口控件句柄
 *@param switch_enc_way 配置参数
 */
void scui_window_switch_enc_way_set(scui_handle_t handle, scui_opt_dir_t switch_enc_way)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    window->switch_enc_way = switch_enc_way;
}

/*@brief 窗口配置参数获取
 *@param handle        窗口控件句柄
 *@param switch_key_id 配置参数
 */
void scui_window_switch_key_id_get(scui_handle_t handle, scui_coord_t switch_key_id[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        switch_key_id[idx] = window->switch_key_id[idx];
}

/*@brief 窗口配置参数设置
 *@param handle        窗口控件句柄
 *@param switch_key_id 配置参数
 */
void scui_window_switch_key_id_set(scui_handle_t handle, scui_coord_t switch_key_id[4])
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    for(scui_handle_t idx = 0; idx < 4; idx++)
        window->switch_key_id[idx] = switch_key_id[idx];
}

/*@brief 窗口资源获取
 *@param handle    窗口控件句柄
 *@param local_res 窗口资源
 */
void scui_window_local_res_get(scui_handle_t handle, void **local_res)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    SCUI_ASSERT(window->local_res != NULL);
    *local_res = window->local_res;
}

/*@brief 窗口资源设置
 *@param handle 窗口控件句柄
 *@param size   窗口资源大小
 */
void scui_window_local_res_set(scui_handle_t handle, scui_handle_t size)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    SCUI_MEM_FREE(window->local_res);
    window->local_res = SCUI_MEM_ZALLOC(scui_mem_type_user, size);
}
