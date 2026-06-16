/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_list_t scui_window_list = {0};

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_list.acts_cur[idx] == handle) {
            SCUI_LOG_INFO("redundant operation");
            return;
        }
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_list.acts_cur[idx] == SCUI_HANDLE_INVALID) {
            scui_window_list.acts_cur[idx]  = handle;
            scui_window_list.acts_cur_num++;
            return;
        }
    SCUI_LOG_ERROR("window %u add fail", handle);
}

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_window));
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_list.acts_cur[idx] == handle) {
            scui_window_list.acts_cur[idx]  = SCUI_HANDLE_INVALID;
            scui_window_list.acts_cur_num--;
            return;
        }
    SCUI_LOG_ERROR("window %u del fail", handle);
}

/*@brief 窗口列表同步
 *@param list 窗口列表
 */
void scui_window_list_sync(scui_handle_t list[SCUI_WINDOW_LIST_LIMIT])
{
    SCUI_ASSERT(list != NULL);
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        list[idx] = scui_window_list.acts_cur[idx];
}

/*@brief 窗口管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
static void scui_window_list_sort(scui_widget_t **list, scui_handle_t num)
{
    /* 要用稳定排序,这里使用冒泡排序 */
    for (scui_multi_t idx_i = 0; idx_i < num; idx_i++)
    for (scui_multi_t idx_j = 0; idx_j + 1 < num - idx_i; idx_j++) {
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
    for (scui_multi_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        if (widget->myself == scui_window_active_curr()) {
            idx_t = idx;
        }
    }
    
    if (idx_t < num) {
        for (scui_multi_t idx = idx_t; idx + 1 < num; idx++) {
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
    
    /* 输出排序好的窗口信息 */
    if (num != 0)
        SCUI_LOG_INFO("window list:");
    
    for (scui_multi_t idx = 0; idx < num; idx++) {
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
    scui_multi_t idx = (scui_multi_t)num - 1;
    
    for (idx = idx; idx >= 0; idx--) {
        scui_widget_t *widget = list[idx];
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget->myself));
        
        /* 完全覆盖,此时跳过 */
        if (widget->clip.x == 0 && widget->clip.y == 0 &&
            widget->surface_c->alpha  == scui_alpha_cover &&
            widget->surface_c->format == surface_fb->format)
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
    /* 底图清空 */
    void scui_window_transform_clean(void);
    scui_window_transform_clean();
    
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案无独立画布 */
    SCUI_ASSERT(num == 0);
    return;
    #endif
    
    scui_window_switch_type_t switch_type = scui_window_switch_type();
    if (switch_type == scui_window_switch_none)
        switch_type  = scui_window_switch_move;
    
    /* 单一窗口直接渲染 */
    bool mode_simple = num == 1;
    /* 仅窗口切换时应用特效渲染 */
    mode_simple = mode_simple || !scui_widget_scroll_state(0x02);
    /* 直接渲染复用常规窗口移动变换(此时相当于不移动) */
    if (mode_simple) switch_type = scui_window_switch_move;
    
    /* 目前所有的画布混合都只有俩个, move除外 */
    if (switch_type >= scui_window_switch_single_s &&
        switch_type <= scui_window_switch_single_e &&
        switch_type != scui_window_switch_move)
        SCUI_ASSERT(num == 2);
    
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
    case scui_window_switch_rotate_1:
         scui_window_transform_rotate_1(list, num);
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
    scui_widget_t *widget_cur = NULL;
    scui_widget_t *widget_tar = NULL;
    scui_alpha_t alpha_cur = scui_alpha_cover;
    scui_alpha_t alpha_tar = scui_alpha_cover;
    scui_point_t point = {0};
    /* 对窗口进行坐标偏移处理 */
    switch (scui_window_switch_type()) {
    default:
    case scui_window_switch_move:
        /* 其他风格都忽略为move */
        break;
    case scui_window_switch_cover_in: {
        widget_cur = list[0];
        widget_tar = list[1];
        scui_widget_move_pos(widget_cur->myself, &point);
        /* alpha_cur = scui_alpha_pct(100 - scui_window_switch_pct()); */
        /* alpha_tar = scui_alpha_pct(scui_window_switch_pct()); */
        /* scui_widget_alpha_mix(widget_cur->myself, alpha_cur, true); */
        /* scui_widget_alpha_mix(widget_tar->myself, alpha_tar, true); */
        break;
    }
    case scui_window_switch_cover_out: {
        widget_cur = list[1];
        widget_tar = list[0];
        scui_widget_move_pos(widget_tar->myself, &point);
        /* alpha_cur = scui_alpha_pct(100 - scui_window_switch_pct()); */
        /* alpha_tar = scui_alpha_pct(scui_window_switch_pct()); */
        /* scui_widget_alpha_mix(widget_cur->myself, alpha_cur, true); */
        /* scui_widget_alpha_mix(widget_tar->myself, alpha_tar, true); */
        break;
    }
    }
    #endif
    
    
    
    for (scui_multi_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        scui_window_t *window = (void *)widget;
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        /* 当前窗口独立于管理之外时, 直接渲染 */
        if (scui_widget_surface_only(widget) && window->resident) {
            scui_surface_t *dst_surface = scui_frame_buffer_draw();
            scui_surface_t *src_surface = widget->surface_c;
            scui_area_t dst_clip = scui_surface_area(dst_surface);
            scui_area_t src_clip = scui_surface_area(src_surface);
            /* 独立画布将窗口偏移补充到画布上 */
            SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
            SCUI_ASSERT(widget->clip_set.clip.x == 0);
            SCUI_ASSERT(widget->clip_set.clip.y == 0);
            dst_clip.x = widget->clip.x;
            dst_clip.y = widget->clip.y;
            
            /* 给目标独立画布追加段偏移 */
            #if SCUI_FRAME_BUFFER_SEG
            if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                 continue;
            #endif
            
            scui_tick_stat(scui_tick_stat_draw_rcd);
            scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
            scui_tick_stat(scui_tick_stat_draw_sum);
            continue;
        }
        
        scui_widget_draw(handle, NULL, true);
    }
    
    
    
    #if SCUI_MEM_FEAT_MINI
    /* 对窗口进行坐标偏移处理 */
    switch (scui_window_switch_type()) {
    default:
    case scui_window_switch_move:
        /* 其他风格都忽略为move */
        break;
    case scui_window_switch_cover_in: {
        SCUI_ASSERT(widget_cur->clip.w == SCUI_HOR_RES);
        SCUI_ASSERT(widget_cur->clip.h == SCUI_VER_RES);
        /* scui_widget_alpha_undo(widget_cur->myself, alpha_cur, true); */
        /* scui_widget_alpha_undo(widget_tar->myself, alpha_tar, true); */
        break;
    }
    case scui_window_switch_cover_out: {
        SCUI_ASSERT(widget_tar->clip.w == SCUI_HOR_RES);
        SCUI_ASSERT(widget_tar->clip.h == SCUI_VER_RES);
        /* scui_widget_alpha_undo(widget_cur->myself, alpha_cur, true); */
        /* scui_widget_alpha_undo(widget_tar->myself, alpha_tar, true); */
        break;
    }
    }
    #endif
}

/*@brief 窗口画布更新(swap模式)
 *       交换窗口独立画布与目标画布的pixel指针
 *       同时同步更新独立画布句柄内的surface.pixel字段
 *       调用前提:窗口必须拥有独立画布(surface句柄有效)
 *@param widget  窗口控件实例
 *@param surface 目标画布(通常为framebuffer)
 */
static void scui_window_surface_swap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface_c->pixel   != surface->pixel);
    SCUI_ASSERT(widget->surface_c->format  == surface->format);
    SCUI_ASSERT(widget->surface_c->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface_c->ver_res == surface->ver_res);
    
    /* 窗口必须有独立画布 */
    SCUI_ASSERT(widget->surface != SCUI_HANDLE_INVALID);
    SCUI_ASSERT(widget->surface_c == scui_widget_surface(widget->myself));
    
    /* 交换 surface_c 与目标画布的 pixel */
    uint8_t *pixel = widget->surface_c->pixel;
    widget->surface_c->pixel = surface->pixel;
    surface->pixel = pixel;
}

/*@brief 窗口画布同步(sync模式)
 *       将目标画布内容拷贝到窗口独立画布
 *       调用前提: 窗口必须拥有独立画布(surface句柄有效)
 *@param widget  窗口控件实例
 *@param surface 源画布(通常为refr framebuffer)
 */
static void scui_window_surface_sync(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface_c->pixel   != surface->pixel);
    SCUI_ASSERT(widget->surface_c->format  == surface->format);
    SCUI_ASSERT(widget->surface_c->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface_c->ver_res == surface->ver_res);
    
    scui_surface_t *dst_surface = widget->surface_c;
    scui_surface_t *src_surface = surface;
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    scui_area_t src_clip = scui_surface_area(src_surface);
    scui_draw_area_copy(true, dst_surface, dst_clip, src_surface, src_clip);
}

/*@brief 窗口管理器画布就绪
 *       分类整理所需处理的窗口
 */
static void scui_window_surface_ready(void)
{
    scui_window_list.widget_0_num = 0;
    scui_window_list.widget_1_num = 0;
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (scui_window_list.acts_cur[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_list.acts_cur[idx];
        scui_widget_t *widget = scui_handle_source_check(handle);
        scui_window_t *window = (void *)widget;
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget) && !window->resident)
            scui_window_list.widget_0[scui_window_list.widget_0_num++] = widget;
        else
            scui_window_list.widget_1[scui_window_list.widget_1_num++] = widget;
        
        #if SCUI_MEM_FEAT_MINI
        /* 小内存方案没有窗口独立画布混合 */
        if (scui_widget_surface_only(widget))
            SCUI_ASSERT(false);
        #endif
    }
    
    /* 依照窗口层级进行排序 */
    scui_window_list_sort(scui_window_list.widget_0, scui_window_list.widget_0_num);
    scui_window_list_sort(scui_window_list.widget_1, scui_window_list.widget_1_num);
    
    scui_handle_t list_lvl_0_ofs = 0;
    scui_handle_t list_lvl_1_ofs = 0;
    /* 仅窗口切换时才应用特效渲染 */
    if (!scui_widget_scroll_state(0x02)) {
        /* 过滤掉被覆盖的绘制窗口 */
        scui_window_list_filter(scui_window_list.widget_0, scui_window_list.widget_0_num, &list_lvl_0_ofs);
        /* 去除掉覆盖的窗口 */
        for (scui_multi_t idx = 0; idx + list_lvl_0_ofs < scui_window_list.widget_0_num; idx++)
            scui_window_list.widget_0[idx] = scui_window_list.widget_0[idx + list_lvl_0_ofs];
        scui_window_list.widget_0_num -= list_lvl_0_ofs;
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
        scui_window_list.switch_refr = true;
        scui_window_list.widget_refr = *widget;
        return scui_window_list.switch_refr;
    case 0x01:
        scui_window_list.switch_refr = false;
        scui_window_list.widget_refr = *widget;
        return scui_window_list.switch_refr;
    case 0x02:
        *widget = scui_window_list.widget_refr;
        return scui_window_list.switch_refr;
    default:
        SCUI_LOG_ERROR("unknown state");
        return false;
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
    scui_widget_t *widget_o = NULL;
    scui_window_surface_switch(0x01, &widget_o);
    if (scui_window_list.widget_0_num == 1 && scui_window_list.widget_1_num == 0) {
        widget_o = scui_window_list.widget_0[0];
        
        scui_surface_t *surface_fb = scui_frame_buffer_draw();
        SCUI_ASSERT(surface_fb->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(surface_fb->hor_res == SCUI_VER_RES);
        /* 类型必须匹配才可交换(通过独立画布句柄获取surface实例) */
        if (widget_o->surface_c->format  == surface_fb->format  &&
            widget_o->surface_c->hor_res == surface_fb->hor_res &&
            widget_o->surface_c->ver_res == surface_fb->ver_res) {
            
            scui_window_surface_switch(0x00, &widget_o);
            scui_window_surface_swap(scui_window_list.widget_0[0], surface_fb);
            SCUI_LOG_INFO("blend to switch mode");
            return;
        }
    }
    #endif
    
    /* 第一轮混合:处理所有常规独立画布 */
    scui_tick_stat(scui_tick_stat_draw_rcd);
    scui_window_list_blend(scui_window_list.widget_0, scui_window_list.widget_0_num);
    scui_tick_stat(scui_tick_stat_draw_sum);
    /* 第二轮混合:处理所有特殊独立画布或共享画布 */
    scui_window_list_render(scui_window_list.widget_1, scui_window_list.widget_1_num);
}

/*@brief 窗口刷新
 */
static void scui_window_surface_refresh(void)
{
    #if SCUI_FRAME_BUFFER_SEG
    scui_window_surface_ready();
    /* 在此处启动段绘制 */
    scui_frame_buffer_seg_ready();
    for (bool seg_valid = true; seg_valid; seg_valid) {
        /* 混合绘制子段 */
        scui_window_surface_blend();
        /* 使用绘制启动刷新流程 */
        scui_frame_buffer_refr_toggle();
        /* 等待绘制目标刷新 */
        scui_frame_buffer_draw_wait();
        seg_valid = scui_frame_buffer_seg_offset();
    }
    scui_tick_stat(scui_tick_stat_refr);
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
        scui_window_surface_sync(widget, surface_fb);
    }
    /* 等待绘制目标刷新 */
    scui_frame_buffer_draw_wait();
    scui_tick_stat(scui_tick_stat_refr);
}

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event)
{
    /* 活跃窗口的事件处理 */
    if (event->object != SCUI_HANDLE_SYSTEM) {
        
        /* 活跃窗口的切换调度 */
        scui_window_switch_event(event);
        
        return;
    }
    
    
    
    /* 窗口列表的组合调度:整体处理 */
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        #if SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL
        if (scui_widget_scroll_state(0x02))
            return;
        #endif
        break;
    }
    case scui_event_refr: {
        scui_window_surface_refresh();
        return;
    }
    case scui_event_lang_change: {
        /* 清扫一遍cache以让旧资源快速回收 */
        scui_cache_font_rectify();
        scui_cache_glyph_rectify();
        break;
    }
    }
    
    /* 窗口列表的组合调度:元素处理 */
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (scui_window_list.acts_cur[idx] == SCUI_HANDLE_INVALID) continue;
        
        scui_handle_t  handle = scui_window_list.acts_cur[idx];
        scui_widget_t *widget = scui_handle_source_check(handle);
        scui_window_t *window = (void *)widget;
        event->object = handle;
        
        switch (event->type) {
        case scui_event_anima_elapse: {
            
            /* 动画驱动:全局刷新窗口 */
            #if SCUI_WIDGET_ANIMA_DRAW_AUTO
            if (scui_window_active_curr() == handle) {
                uint32_t anima_draw = scui_window_list.anima_draw;
                anima_draw += event->tick;
                
                if (anima_draw >= SCUI_WIDGET_ANIMA_DRAW_TIME) {
                    anima_draw -= SCUI_WIDGET_ANIMA_DRAW_TIME;
                    scui_widget_draw(handle, NULL, false);
                }
                scui_window_list.anima_draw = anima_draw;
            }
            #endif
            
            /* 通知焦点窗口或常驻窗口 */
            if (scui_window_active_curr() == handle || window->resident) {
                event->style.sync = true;
                scui_event_notify(event);
            }
            
            break;
        }
        case scui_event_lang_change:
            /* 通知所有窗口 */
            scui_event_notify(event);
            break;
        }
    }
    
    event->object = SCUI_HANDLE_SYSTEM;
}

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle)
{
    if (handle != SCUI_HANDLE_SYSTEM) {
        SCUI_ASSERT(scui_handle_remap(handle));
        scui_widget_t *widget = scui_handle_source_check(handle);
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    }
    
    if (scui_window_list.acts_rcd[0] == handle) {
        scui_widget_draw(handle, NULL, false);
        SCUI_LOG_INFO("redundant operation");
        return;
    }
    
    /* 焦点必须在窗口列表中 */
    bool acts_cur_check = handle == SCUI_HANDLE_SYSTEM;
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (scui_window_list.acts_cur[idx] != handle) continue;
        if (acts_cur_check) break;
        acts_cur_check = true;
        break;
    }
    SCUI_ASSERT(acts_cur_check);
    
    
    /* 清除当前活跃窗口, 后记录新的窗口 */
    bool acts_valid_last = true;
    bool acts_valid_curr = handle != SCUI_HANDLE_SYSTEM;
    if (scui_window_list.acts_rcd[0] == SCUI_HANDLE_SYSTEM) {
        scui_window_list.acts_rcd[0]  = handle;
        acts_valid_last  = false;
    } else {
        for (scui_multi_t idx = SCUI_WINDOW_LIST_LIMIT - 1; idx > 0; idx--)
            scui_window_list.acts_rcd[idx] = scui_window_list.acts_rcd[idx - 1];
            scui_window_list.acts_rcd[0] = handle;
    }
    
    scui_handle_t handle_last = scui_window_list.acts_rcd[1];
    scui_handle_t handle_curr = scui_window_list.acts_rcd[0];
    
    /* 先失活旧的焦点窗口 */
    if (acts_valid_last && scui_handle_remap(handle_last)) {
        scui_event_define(event, handle_last, true, scui_event_focus_lost, NULL);
        SCUI_LOG_WARN("window %u focus lost", event.object);
        scui_event_notify(&event);
    }
    
    /* 后激活新的焦点窗口 */
    if (acts_valid_curr && scui_handle_remap(handle_curr)) {
        scui_event_define(event, handle_curr, true, scui_event_focus_get, NULL);
        SCUI_LOG_WARN("window %u focus get", event.object);
        scui_event_notify(&event);
    }
    
    /* 切到新窗口时 */
    /* 快速刷新权重淘汰旧资源 */
    scui_cache_image_rectify();
}

/*@brief 获得活跃窗口句柄
 *@param index 0:当前活跃;1:上一活跃;2~n以此类推
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(scui_handle_t index)
{
    SCUI_ASSERT(index < SCUI_WINDOW_LIST_LIMIT);
    /* SCUI_ASSERT(scui_handle_remap(scui_window_list.acts_rcd[index])); */
    if (index != 0) return scui_window_list.acts_rcd[index];
    
    /* 活跃记录(特殊情况,临时失活): */
    if (scui_window_list.acts_rcd[0] != SCUI_HANDLE_SYSTEM)
        return scui_window_list.acts_rcd[0];
    
    SCUI_ASSERT(scui_window_list.acts_rcd[1] != SCUI_HANDLE_SYSTEM);
    return scui_window_list.acts_rcd[1];
}
