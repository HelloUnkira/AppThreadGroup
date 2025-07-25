/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_window_mgr_t scui_window_mgr = {0};

/*@brief 窗口切换风格实例
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_cfg_type(scui_window_switch_type_t **cfg_type)
{
    *cfg_type = &scui_window_mgr.switch_args.cfg_type;
}

/*@brief 窗口切换参数实例
 *@param cfg_args 实例
 */
void scui_window_switch_cfg_args(scui_window_switch_args_t **cfg_args)
{
    *cfg_args = &scui_window_mgr.switch_args.cfg_args;
}

/*@brief 窗口切换方向实例
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_cfg_dir(scui_opt_dir_t **cfg_dir)
{
    *cfg_dir = &scui_window_mgr.switch_args.cfg_dir;
}

/*@brief 窗口管理器根控件列表
 *@param list 根控件列表
 */
void scui_window_list(scui_handle_t **list)
{
    *list = scui_window_mgr.list;
}

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            SCUI_LOG_INFO("redundant operation");
            return;
        }
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID) {
            scui_window_mgr.list[idx]  = handle;
            scui_window_mgr.list_num++;
            return;
        }
    SCUI_LOG_ERROR("scene %u add fail", handle);
}

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    
    /* 如果移除的是焦点,记得清空 */
    if (scui_window_mgr.active_curr == handle)
        scui_window_mgr.active_curr  = SCUI_HANDLE_INVALID;
    if (scui_window_mgr.active_last == handle)
        scui_window_mgr.active_last  = SCUI_HANDLE_INVALID;
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            scui_window_mgr.list[idx]  = SCUI_HANDLE_INVALID;
            scui_window_mgr.list_num--;
            return;
        }
    SCUI_LOG_ERROR("scene %u del fail", handle);
}

/*@brief 窗口管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
static void scui_window_list_sort(scui_widget_t **list, scui_handle_t num)
{
    /* 要用稳定排序,这里使用冒泡排序 */
    for (scui_handle_t idx_i = 0; idx_i < num; idx_i++)
    for (scui_handle_t idx_j = 0; idx_j + 1 < num - idx_i; idx_j++) {
        scui_widget_t *widget_1 = list[idx_j];
        scui_widget_t *widget_2 = list[idx_j + 1];
        SCUI_ASSERT(widget_1->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(widget_2->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget_1->myself));
        SCUI_ASSERT(scui_handle_remap(widget_2->myself));
        
        SCUI_ASSERT(scui_widget_type_check(widget_1->myself, scui_widget_type_window));
        SCUI_ASSERT(scui_widget_type_check(widget_2->myself, scui_widget_type_window));
        scui_window_t *window_1 = (scui_window_t *)widget_1;
        scui_window_t *window_2 = (scui_window_t *)widget_2;
        
        if (window_1->level > window_2->level) {
            scui_widget_t *widget_t = list[idx_j];
            list[idx_j] = list[idx_j + 1];
            list[idx_j + 1] = widget_t;
        }
    }
    
    /* 将焦点窗口移到同级的最后面 */
    scui_handle_t idx_t = num;
    for (scui_handle_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        if (widget->myself == scui_window_active_curr()) {
            idx_t = idx;
        }
    }
    
    if (idx_t < num) {
        for (scui_handle_t idx = idx_t; idx + 1 < num; idx++) {
            scui_widget_t *widget_1 = list[idx];
            scui_widget_t *widget_2 = list[idx + 1];
            SCUI_ASSERT(widget_1->parent == SCUI_HANDLE_INVALID);
            SCUI_ASSERT(widget_2->parent == SCUI_HANDLE_INVALID);
            SCUI_ASSERT(scui_handle_remap(widget_1->myself));
            SCUI_ASSERT(scui_handle_remap(widget_2->myself));
            
            SCUI_ASSERT(scui_widget_type_check(widget_1->myself, scui_widget_type_window));
            SCUI_ASSERT(scui_widget_type_check(widget_2->myself, scui_widget_type_window));
            scui_window_t *window_1 = (scui_window_t *)widget_1;
            scui_window_t *window_2 = (scui_window_t *)widget_2;
            
            if (window_1->level >= window_2->level) {
                scui_widget_t *widget_t = list[idx];
                list[idx] = list[idx + 1];
                list[idx + 1] = widget_t;
                continue;
            }
            break;
        }
    }
    
    // 输出排序好的窗口信息
    if (num != 0)
        SCUI_LOG_INFO("window list:");
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        SCUI_LOG_INFO("object:%u<x:%d,y:%d,w:%d,h:%d><active:%d>", widget->myself,
                      widget->clip.x, widget->clip.y, widget->clip.w, widget->clip.h,
                      widget->myself == scui_window_active_curr());
    }
}

/*@brief 窗口管理器过滤根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 *@param ofs  根控件偏移
 */
static void scui_window_list_filter(scui_widget_t **list, scui_handle_t num, scui_handle_t *ofs)
{
    scui_surface_t *surface_fb = scui_frame_buffer_draw();
    
    int32_t idx = (int32_t)num - 1;
    for (idx = (int32_t)num - 1; idx >= 0; idx--) {
        scui_widget_t *widget = list[idx];
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget->myself));
        
        /* 完全覆盖,此时跳过 */
        if (widget->clip.x == 0 && widget->clip.y == 0 &&
            widget->surface->alpha  == scui_alpha_cover &&
            widget->surface->format == surface_fb->format)
            break;
    }
    
    *ofs = idx < 0 ? 0 : idx;
}

/*@brief 窗口管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       窗口管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
static void scui_window_list_blend(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(scui_window_mgr.switch_args.cfg_type > scui_window_switch_single_s);
    SCUI_ASSERT(scui_window_mgr.switch_args.cfg_type < scui_window_switch_single_e);
    
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案无独立画布 */
    return;
    #endif
    
    scui_window_switch_type_t switch_type = scui_window_mgr.switch_args.type;
    if (switch_type == scui_window_switch_none)
        switch_type  = scui_window_mgr.switch_args.cfg_type;
    
    bool mode_simple = false;
    /* 1.单一窗口直接渲染 */
    mode_simple = mode_simple || num == 1;
    /* 2.多窗口叠加不应用特效渲染 */
    #if 0   // discard, we don't need this
    mode_simple = mode_simple || scui_window_float_running();
    #endif
    /* 3.仅窗口切换时应用特效渲染 */
    mode_simple = mode_simple || scui_widget_global_scroll_flag(0x02, &scui_window_mgr.switch_args.key);
    
    // 直接渲染复用常规窗口移动变换(此时相当于不移动)
    if (mode_simple) switch_type = scui_window_switch_move;
    
    /* 底图清空 */
    scui_color_t dst_pixel = {0};
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    scui_draw_area_fill(dst_surface, &dst_clip, scui_alpha_cover, SCUI_COLOR_ZEROED);
    
    /* 内部唯一引用::: */
    void scui_window_transform_move(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_cover(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_zoom(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_center(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_rotate(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_rotate1(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_circle(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_grid(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_flip1(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_flip2(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_flap1(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_flap2(scui_widget_t **list, scui_handle_t num);
    void scui_window_transform_cube(scui_widget_t **list, scui_handle_t num);
    
    /* 多画布混合变换 */
    switch (switch_type) {
    default:
         SCUI_LOG_ERROR("unknown switch type");
         SCUI_LOG_ERROR("switch to move type");
    case scui_window_switch_move:
         scui_window_transform_move(list, num);
         break;
    case scui_window_switch_cover_in:
    case scui_window_switch_cover_out:
         scui_window_transform_cover(list, num);
         break;
    case scui_window_switch_center_in:
    case scui_window_switch_center_out:
    case scui_window_switch_center_inout:
         scui_window_transform_center(list, num);
         break;
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2:
         scui_window_transform_zoom(list, num);
         break;
    case scui_window_switch_rotate:
         scui_window_transform_rotate(list, num);
         break;
    case scui_window_switch_rotate1:
         scui_window_transform_rotate1(list, num);
         break;
    case scui_window_switch_circle:
         scui_window_transform_circle(list, num);
         break;
    case scui_window_switch_grid:
         scui_window_transform_grid(list, num);
         break;
    case scui_window_switch_flip1:
         scui_window_transform_flip1(list, num);
         break;
    case scui_window_switch_flip2:
         scui_window_transform_flip2(list, num);
         break;
    case scui_window_switch_flap1:
         scui_window_transform_flap1(list, num);
         break;
    case scui_window_switch_flap2:
         scui_window_transform_flap2(list, num);
         break;
    case scui_window_switch_cube:
         scui_window_transform_cube(list, num);
         break;
    }
}

/*@brief 窗口管理器渲染根控件列表
 *       将所有根控件渲染到绘制画布上
 *@param list 根控件列表
 *@param num  根控件数量
 */
static void scui_window_list_render(scui_widget_t **list, scui_handle_t num)
{
    #if SCUI_MEM_FEAT_MINI
    /* 底图清空 */
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    scui_draw_area_fill(dst_surface, &dst_clip, scui_alpha_cover, SCUI_COLOR_ZEROED);
    
    scui_widget_t *widget_cur = NULL;
    scui_widget_t *widget_tar = NULL;
    scui_alpha_t alpha_cur = scui_alpha_cover;
    scui_alpha_t alpha_tar = scui_alpha_cover;
    scui_point_t point = {0};
    /* 对窗口进行坐标偏移处理 */
    switch (scui_window_mgr.switch_args.type) {
    default:
    case scui_window_switch_move:
        /* 其他风格都忽略为move */
        break;
    case scui_window_switch_cover_in: {
        widget_cur = list[0];
        widget_tar = list[1];
        scui_widget_move_pos(widget_cur->myself, &point);
        // alpha_cur = scui_alpha_pct(100 - scui_window_mgr.switch_args.pct);
        // alpha_tar = scui_alpha_pct(scui_window_mgr.switch_args.pct);
        // scui_widget_alpha_mix(widget_cur->myself, alpha_cur, true);
        // scui_widget_alpha_mix(widget_tar->myself, alpha_tar, true);
        break;
    }
    case scui_window_switch_cover_out: {
        widget_cur = list[1];
        widget_tar = list[0];
        scui_widget_move_pos(widget_tar->myself, &point);
        // alpha_cur = scui_alpha_pct(100 - scui_window_mgr.switch_args.pct);
        // alpha_tar = scui_alpha_pct(scui_window_mgr.switch_args.pct);
        // scui_widget_alpha_mix(widget_cur->myself, alpha_cur, true);
        // scui_widget_alpha_mix(widget_tar->myself, alpha_tar, true);
        break;
    }
    }
    #endif
    
    
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        scui_window_t *window = (void *)widget;
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        /* 当前窗口独立于管理之外时,直接渲染 */
        if (scui_widget_surface_only(widget) && window->resident) {
            scui_surface_t *dst_surface = scui_frame_buffer_draw();
            scui_surface_t *src_surface = widget->surface;
            scui_area_t dst_clip = scui_surface_area(dst_surface);
            scui_area_t src_clip = scui_surface_area(src_surface);
            /* 独立画布将窗口偏移补充到画布上 */
            if (scui_widget_surface_only(widget)) {
                SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
                SCUI_ASSERT(widget->clip_set.clip.x == 0);
                SCUI_ASSERT(widget->clip_set.clip.y == 0);
                dst_clip.x = widget->clip.x;
                dst_clip.y = widget->clip.y;
            }
            scui_tick_calc(0x20, NULL, NULL, NULL);
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            scui_tick_calc(0x21, NULL, NULL, NULL);
            continue;
        }
        
        scui_widget_draw(handle, NULL, true);
    }
    
    
    
    #if SCUI_MEM_FEAT_MINI
    /* 对窗口进行坐标偏移处理 */
    switch (scui_window_mgr.switch_args.type) {
    default:
    case scui_window_switch_move:
        /* 其他风格都忽略为move */
        break;
    case scui_window_switch_cover_in: {
        SCUI_ASSERT(widget_cur->clip.w == SCUI_HOR_RES);
        SCUI_ASSERT(widget_cur->clip.h == SCUI_VER_RES);
        // scui_widget_alpha_undo(widget_cur->myself, alpha_cur, true);
        // scui_widget_alpha_undo(widget_tar->myself, alpha_tar, true);
        break;
    }
    case scui_window_switch_cover_out: {
        SCUI_ASSERT(widget_tar->clip.w == SCUI_HOR_RES);
        SCUI_ASSERT(widget_tar->clip.h == SCUI_VER_RES);
        // scui_widget_alpha_undo(widget_cur->myself, alpha_cur, true);
        // scui_widget_alpha_undo(widget_tar->myself, alpha_tar, true);
        break;
    }
    }
    #endif
}

/*@brief 窗口列表事件通知
 *@param event 事件
 */
void scui_window_list_event_notify(scui_event_t *event)
{
    scui_handle_t handle = event->object;
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        event->object = scui_window_mgr.list[idx];
        scui_event_notify(event);
        event->object = handle;
    }
}

/*@brief 窗口管理器混合画布模式检查
 *@param state  状态(0x00:设置标记;0x01:清除标记;0x02:检查标记;)
 *@param widget 控件实例地址
 */
static bool scui_window_surface_switch(uint8_t state, scui_widget_t **widget)
{
    switch (state) {
    case 0x00:
        scui_window_mgr.refr_switch = true;
        scui_window_mgr.refr_widget = *widget;
        return scui_window_mgr.refr_switch;
    case 0x01:
        scui_window_mgr.refr_switch = false;
        scui_window_mgr.refr_widget = *widget;
        return scui_window_mgr.refr_switch;
    case 0x02:
        *widget = scui_window_mgr.refr_widget;
        return scui_window_mgr.refr_switch;
    default:
        SCUI_LOG_ERROR("unknown state");
        return false;
    }
}

/*@brief 窗口管理器画布就绪
 *       分类整理所需处理的窗口
 */
static void scui_window_surface_ready(void)
{
    scui_window_mgr.list_0_num = 0;
    scui_window_mgr.list_1_num = 0;
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_source_check(handle);
        scui_window_t *window = (void *)widget;
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget) && !window->resident)
            scui_window_mgr.list_0[scui_window_mgr.list_0_num++] = widget;
        else
            scui_window_mgr.list_1[scui_window_mgr.list_1_num++] = widget;
        
        /* 小内存方案没有窗口独立画布混合 */
        #if SCUI_MEM_FEAT_MINI
        if (scui_widget_surface_only(widget))
            SCUI_ASSERT(false);
        #endif
    }
    
    /* 依照窗口层级进行排序 */
    scui_window_list_sort(scui_window_mgr.list_0, scui_window_mgr.list_0_num);
    scui_window_list_sort(scui_window_mgr.list_1, scui_window_mgr.list_1_num);
    
    scui_handle_t list_lvl_0_ofs = 0;
    scui_handle_t list_lvl_1_ofs = 0;
    /* 仅窗口切换时才应用特效渲染 */
    if (scui_widget_global_scroll_flag(0x02, &scui_window_mgr.switch_args.key)) {
        /* 过滤掉被覆盖的绘制界面 */
        scui_window_list_filter(scui_window_mgr.list_0, scui_window_mgr.list_0_num, &list_lvl_0_ofs);
        /* 去除掉覆盖的窗口 */
        for (scui_handle_t idx = 0; idx + list_lvl_0_ofs < scui_window_mgr.list_0_num; idx++)
            scui_window_mgr.list_0[idx] = scui_window_mgr.list_0[idx + list_lvl_0_ofs];
        scui_window_mgr.list_0_num -= list_lvl_0_ofs;
    }
}

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 */
static void scui_window_surface_blend(void)
{
    /* 切换switch模式? */
    /* 如果送显数据刚好为完整的一个surface */
    /* 此时则使用switch模式直接交换surface快速进入refr异步 */
    /* 在refr异步的开始则异步进行数据同步,还原本地的surface */
    #if SCUI_WINDOW_SWITCH_MODE
    scui_widget_t *widget_only = NULL;
    scui_window_surface_switch(0x01, &widget_only);
    if (scui_window_mgr.list_0_num == 1 && scui_window_mgr.list_1_num == 0) {
        widget_only = scui_window_mgr.list_0[0];
        
        scui_surface_t *surface_fb = scui_frame_buffer_draw();
        SCUI_ASSERT(surface_fb->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(surface_fb->hor_res == SCUI_VER_RES);
        // 类型必须匹配才可交换
        if (widget_only->surface->format  == surface_fb->format  &&
            widget_only->surface->hor_res == surface_fb->hor_res &&
            widget_only->surface->ver_res == surface_fb->ver_res) {
            
            scui_window_surface_switch(0x00, &widget_only);
            scui_widget_surface_swap(scui_window_mgr.list_0[0], surface_fb);
            SCUI_LOG_INFO("blend to switch mode");
            return;
        }
    }
    #endif
    
    /* 第一轮混合:处理所有常规独立画布 */
    scui_tick_calc(0x20, NULL, NULL, NULL);
    scui_window_list_blend(scui_window_mgr.list_0, scui_window_mgr.list_0_num);
    scui_tick_calc(0x21, NULL, NULL, NULL);
    /* 第二轮混合:处理所有特殊独立画布或共享画布 */
    scui_window_list_render(scui_window_mgr.list_1, scui_window_mgr.list_1_num);
}

/*@brief 窗口刷新(仅调度使用)
 */
void scui_window_refresh(void)
{
    #if SCUI_MEM_FEAT_MINI
    scui_window_surface_ready();
    /* 在此处启动段绘制 */
    scui_frame_buffer_seg_ready();
    for (bool seg_valid = true; seg_valid; seg_valid) {
        scui_window_surface_blend();
        /* 混合绘制刷新流程结束 */
        /* 使用绘制启动刷新流程 */
        scui_frame_buffer_refr_toggle();
        /* 等待绘制目标刷新 */
        scui_frame_buffer_draw_wait();
        seg_valid = scui_frame_buffer_seg_offset();
    }
    scui_tick_calc(0x00, NULL, NULL, NULL);
    return;
    #endif
    
    scui_window_surface_ready();
    scui_window_surface_blend();
    /* 混合绘制刷新流程结束 */
    /* 使用绘制启动刷新流程 */
    scui_frame_buffer_refr_toggle();
    /* 开始refr的switch模式同步 */
    scui_widget_t *widget = NULL;
    if (scui_window_surface_switch(0x02, &widget)) {
        SCUI_ASSERT(widget != NULL);
        SCUI_LOG_INFO("switch mode sync");
        scui_surface_t *surface_fb = scui_frame_buffer_refr();
        scui_widget_surface_sync(widget, surface_fb);
    }
    /* 等待绘制目标刷新 */
    scui_frame_buffer_draw_wait();
    scui_tick_calc(0x00, NULL, NULL, NULL);
}

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle)
{
    SCUI_ASSERT(scui_handle_remap(handle));
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_window_mgr.active_curr == handle) {
        scui_widget_draw(handle, NULL, false);
        SCUI_LOG_INFO("redundant operation");
        return;
    }
    
    /* 焦点必须在窗口列表中 */
    bool not_match_yet = true;
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            not_match_yet = false;
            break;
        }
    if (not_match_yet) {
        SCUI_LOG_INFO("unknown scene %u", handle);
        return;
    }
    
    scui_event_t event = {.style.sync = true,};
    scui_window_mgr.active_last = scui_window_mgr.active_curr;
    scui_window_mgr.active_curr = handle;
    
    /* 先失活旧的焦点窗口 */
    if (scui_handle_remap(scui_window_mgr.active_last)) {
        event.type   = scui_event_focus_lost;
        event.object = scui_window_mgr.active_last;
        SCUI_LOG_INFO("window %u focus lost", event.object);
        scui_event_notify(&event);
    }
    
    /* 后激活新的焦点窗口 */
    event.type   = scui_event_focus_get;
    event.object = scui_window_mgr.active_curr;
    SCUI_LOG_INFO("window %u focus get", event.object);
    scui_event_notify(&event);
}

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_window_mgr.active_last));
    return scui_window_mgr.active_last;
}

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_curr(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_window_mgr.active_curr));
    return scui_window_mgr.active_curr;
}
