/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_window_mgr_t scui_window_mgr = {0};

/*@brief 窗口切换风格(配置)
 *@retval 窗口切换风格
 */
scui_window_switch_type_t scui_window_switch_type_cfg_get(void)
{
    return scui_window_mgr.switch_args.cfg_type;
}

/*@brief 窗口切换方向(配置)
 *@retval 窗口切换方向
 */
scui_opt_dir_t scui_window_switch_dir_cfg_get(void)
{
    return scui_window_mgr.switch_args.cfg_dir;
}

/*@brief 窗口切换风格(配置)
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_type_cfg_set(scui_window_switch_type_t switch_type)
{
    SCUI_ASSERT(switch_type != scui_window_switch_auto);
    scui_window_mgr.switch_args.cfg_type = switch_type;
}

/*@brief 窗口切换方向(配置)
 *@param switch_dir 窗口切换方向
 */
void scui_window_switch_dir_cfg_set(scui_opt_dir_t switch_dir)
{
    scui_window_mgr.switch_args.cfg_dir = switch_dir;
}

/*@brief 窗口管理器根控件列表
 *@param list 根控件列表
 */
void scui_window_list(scui_handle_t **list)
{
    *list = scui_window_mgr.list;
}

/*@brief 窗口隐藏
 *@param handle 窗口句柄
 *@param any    所有或者仅含有资源的窗口
 */
void scui_window_list_hide_without(scui_handle_t handle, bool any)
{
    SCUI_LOG_INFO("");
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] != SCUI_HANDLE_INVALID &&
            scui_window_mgr.list[idx] != handle) {
            scui_widget_t *widget = scui_handle_get(scui_window_mgr.list[idx]);
            scui_window_t *window = (void *)widget;
            SCUI_ASSERT(widget != NULL);
            SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
            if (any || (scui_widget_surface_only(widget) && !window->resident))
                scui_widget_hide(scui_window_mgr.list[idx], false);
        }
}

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle)
{
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            SCUI_LOG_INFO("redundant operation");
            return;
        }
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
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
    /* 如果移除的是焦点,记得清空 */
    if (scui_window_mgr.active_curr == handle)
        scui_window_mgr.active_curr  = SCUI_HANDLE_INVALID;
    if (scui_window_mgr.active_last == handle)
        scui_window_mgr.active_last  = SCUI_HANDLE_INVALID;
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
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
void scui_window_list_sort(scui_widget_t **list, scui_handle_t num)
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
void scui_window_list_filter(scui_widget_t **list, scui_handle_t num, scui_handle_t *ofs)
{
    scui_surface_t *surface_fb = scui_surface_fb_draw();
    
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
void scui_window_list_blend(scui_widget_t **list, scui_handle_t num)
{
    bool mode_simple = false;
    /* 1.单一窗口直接渲染 */
    mode_simple = mode_simple || num == 1;
    /* 2.多窗口叠加不应用特效渲染 */
    mode_simple = mode_simple || scui_window_float_running();
    /* 3.仅窗口切换时应用特效渲染 */
    mode_simple = mode_simple || scui_widget_event_scroll_flag(0x02, &scui_window_mgr.switch_args.key);
    
    // 直接渲染复用常规窗口移动变换(此时相当于不移动)
    scui_window_switch_type_t switch_type = mode_simple ? scui_window_switch_move : scui_window_mgr.switch_args.type;
    
    /* 多画布混合变换 */
    switch (switch_type) {
    case scui_window_switch_move:
         scui_window_transform_move(list, num);
         break;
    case scui_window_switch_cover:
         scui_window_transform_cover(list, num);
         break;
    case scui_window_switch_center_in:
    case scui_window_switch_center_out:
         scui_window_transform_center_in_out(list, num);
         break;
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2:
         scui_window_transform_zoom(list, num);
         break;
    case scui_window_switch_circle:
         scui_window_transform_circle(list, num);
         break;
    case scui_window_switch_grid:
         scui_window_transform_grid(list, num);
         break;
    case scui_window_switch_flip:
         scui_window_transform_flip(list, num);
         break;
    case scui_window_switch_cube:
         scui_window_transform_cube(list, num);
         break;
    default:
         SCUI_LOG_ERROR("unknown switch type");
         SCUI_ASSERT(false);
         break;
    }
}

/*@brief 窗口管理器渲染根控件列表
 *       将所有根控件渲染到绘制画布上
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_list_render(scui_widget_t **list, scui_handle_t num)
{
    for (scui_handle_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        scui_window_t *window = (void *)widget;
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        /* 当前窗口独立于管理之外时,直接渲染 */
        if (scui_widget_surface_only(widget) && window->hang_only) {
            scui_surface_t *dst_surface = scui_surface_fb_draw();
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
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, (scui_color_t){0});
            continue;
        }
        
        scui_widget_draw(handle, NULL, true);
    }
}

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 */
void scui_window_surface_blend(void)
{
    scui_handle_t  list_lvl_0_num = 0;
    scui_handle_t  list_lvl_1_num = 0;
    scui_widget_t *list_lvl_0[SCUI_WINDOW_MGR_LIMIT] = {0};
    scui_widget_t *list_lvl_1[SCUI_WINDOW_MGR_LIMIT] = {0};
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        scui_window_t *window = (void *)widget;
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget) && !window->hang_only)
            list_lvl_0[list_lvl_0_num++] = widget;
        else
            list_lvl_1[list_lvl_1_num++] = widget;
    }
    
    /* 依照窗口层级进行排序 */
    scui_window_list_sort(list_lvl_0, list_lvl_0_num);
    scui_window_list_sort(list_lvl_1, list_lvl_1_num);
    
    scui_handle_t ofs_idx = 0;
    /* 仅窗口切换时才应用特效渲染 */
    if (scui_widget_event_scroll_flag(0x02, &scui_window_mgr.switch_args.key)) {
        /* 过滤掉被覆盖的绘制界面 */
        scui_window_list_filter(list_lvl_0, list_lvl_0_num, &ofs_idx);
        list_lvl_0_num -= ofs_idx;
    }
    
    /* 切换switch模式? */
    /* 如果送显数据刚好为完整的一个surface */
    /* 此时则使用switch模式直接交换surface快速进入refr异步 */
    /* 在refr异步的开始则异步进行数据同步,还原本地的surface */
    #if SCUI_WINDOW_MGR_SWITCH_MODE
    scui_widget_t *widget_only = NULL;
    scui_window_surface_switch(0x01, &widget_only);
    if (list_lvl_0_num == 1 && list_lvl_1_num == 0) {
        widget_only = list_lvl_0[ofs_idx];
        
        scui_surface_t *surface_fb = scui_surface_fb_draw();
        // 类型必须匹配才可交换
        if (widget_only->surface->format  == surface_fb->format  &&
            widget_only->surface->hor_res == surface_fb->hor_res &&
            widget_only->surface->ver_res == surface_fb->ver_res) {
            
            scui_window_surface_switch(0x00, &widget_only);
            scui_widget_surface_swap(list_lvl_0[ofs_idx], surface_fb);
            SCUI_LOG_INFO("blend to switch mode");
            return;
        }
    }
    #endif
    
    /* 第一轮混合:处理所有常规独立画布 */
    scui_window_list_blend(&list_lvl_0[ofs_idx], list_lvl_0_num);
    /* 第二轮混合:处理所有特殊独立画布或共享画布 */
    scui_window_list_render(&list_lvl_1[ofs_idx], list_lvl_1_num);
}

/*@brief 窗口管理器混合画布模式检查
 *@param state  状态(0x00:设置标记;0x01:清除标记;0x02:检查标记;)
 *@param widget 控件实例地址
 */
bool scui_window_surface_switch(uint8_t state, scui_widget_t **widget)
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

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle)
{
    SCUI_ASSERT(scui_handle_remap(handle));
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_window_mgr.active_curr == handle) {
        scui_widget_draw(handle, NULL, false);
        SCUI_LOG_INFO("redundant operation");
        return;
    }
    
    /* 焦点必须在窗口列表中 */
    bool not_match_yet = true;
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
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
        scui_event_notify(&event);
    }
    
    /* 后激活新的焦点窗口 */
    event.type   = scui_event_focus_get;
    event.object = scui_window_mgr.active_curr;
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
