/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_window_mgr_t scui_window_mgr = {0};

/*@brief 窗口管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       窗口管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_mix_list(scui_widget_t **list, scui_handle_t num)
{
    switch (scui_window_mgr.switch_args.cfg_type) {
        case scui_window_switch_none:
        case scui_window_switch_center_in:
        case scui_window_switch_center_out:
        case scui_window_switch_normal: {
            scui_surface_t *dst_surface = scui_surface_fb_draw();
            for (scui_handle_t idx = 0; idx < num; idx++) {
                scui_area_t dst_clip = {
                    .w = scui_disp_get_hor_res(),
                    .h = scui_disp_get_ver_res(),
                };
                scui_surface_t *src_surface = &list[idx]->surface;
                scui_area_t src_clip = src_surface->clip;
                scui_area_t rcd_clip = src_surface->clip;
                /* 与显示区域做一次交集运算 */
                scui_area_t out_clip = {0};
                if (!scui_area_inter(&out_clip, &dst_clip, &src_clip))
                     continue;
                dst_clip = out_clip;
                if (src_clip.x > 0 || src_clip.y > 0) {
                    src_surface->clip.x = 0;
                    src_surface->clip.y = 0;
                    src_surface->clip.w -= src_clip.x;
                    src_surface->clip.h -= src_clip.y;
                }
                if (src_clip.x < 0 || src_clip.y < 0) {
                    src_surface->clip.x = -src_clip.x;
                    src_surface->clip.y = -src_clip.y;
                    src_surface->clip.w += src_clip.x;
                    src_surface->clip.h += src_clip.y;
                }
                src_clip = src_surface->clip;
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip);
                src_surface->clip = rcd_clip;
            }
            break;
        }
        case scui_window_switch_zoom1:
        case scui_window_switch_zoom2:
            break;
    }
}

/*@brief 窗口管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_sort_list(scui_widget_t **list, scui_handle_t num)
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
        
        SCUI_ASSERT(widget_1->type == scui_widget_type_window);
        SCUI_ASSERT(widget_2->type == scui_widget_type_window);
        scui_window_t *window_1 = (scui_window_t *)widget_1;
        scui_window_t *window_2 = (scui_window_t *)widget_2;
        
        if (window_1->level > window_2->level) {
            scui_widget_t *widget_t = list[idx_j];
            list[idx_j] = list[idx_j + 1];
            list[idx_j + 1] = widget_t;
        }
    }
    
    // 输出排序好的窗口信息
    SCUI_LOG_INFO("window list:");
    for (scui_handle_t idx = 0; idx < num; idx++) {
        scui_widget_t *widget = list[idx];
        SCUI_LOG_INFO("object:%u<x:%d,y:%d,w:%d,h:%d>", widget->myself,
                      widget->clip.x, widget->clip.y, widget->clip.w, widget->clip.h);
    }
}

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 */
void scui_window_mix_surface(void)
{
    scui_handle_t list_num = 0;
    scui_widget_t *list[SCUI_WINDOW_MGR_LIMIT] = {0};
    
    /* 第一轮混合:处理所有独立画布 */
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget))
            list[list_num++] = widget;
    }
    scui_window_sort_list(list, list_num);
    scui_window_mix_list(list, list_num);
    list_num = 0;
    
    /* 第二轮混合:处理所有独立画布 */
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (!scui_widget_surface_only(widget))
             list[list_num++] = widget;
    }
    scui_window_sort_list(list, list_num);
    
    for (scui_handle_t idx = 0; idx < list_num; idx++) {
        scui_widget_t *widget = list[idx];
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        scui_widget_draw(handle, true);
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_event_dispatch(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    scui_event_retval_t ret = scui_event_retval_quit;
    ret = scui_window_switch_event(event);
    return ret;
}
