/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

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
        case scui_window_switch_normal:
        case scui_window_switch_zoom1:
        case scui_window_switch_zoom2: {
            scui_color_gradient_t color = {0};
            scui_surface_t *dst_surface = scui_surface_fb_draw();
            for (scui_handle_t idx = 0; idx < num; idx++) {
                scui_widget_t  *widget = list[idx];
                scui_surface_t *src_surface = widget->surface;
                scui_area_t src_clip = widget->clip_set.clip;
                /* 独立画布将窗口偏移补充到画布上 */
                if (scui_widget_surface_only(widget)) {
                    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
                    SCUI_ASSERT(widget->clip_set.clip.x == 0);
                    SCUI_ASSERT(widget->clip_set.clip.y == 0);
                    src_clip.x = widget->clip.x;
                    src_clip.y = widget->clip.y;
                }
                scui_area_t dst_clip = {
                    .w = dst_surface->hor_res,
                    .h = dst_surface->ver_res,
                };
                /* 与显示区域做一次交集运算 */
                scui_area_t tmp_clip = src_clip;
                scui_area_t out_clip = {0};
                if (!scui_area_inter(&out_clip, &dst_clip, &src_clip))
                     continue;
                if (src_clip.x > 0 || src_clip.y > 0) {
                    tmp_clip.x = 0;
                    tmp_clip.y = 0;
                    tmp_clip.w -= src_clip.x;
                    tmp_clip.h -= src_clip.y;
                }
                if (src_clip.x < 0 || src_clip.y < 0) {
                    tmp_clip.x = -src_clip.x;
                    tmp_clip.y = -src_clip.y;
                    tmp_clip.w += src_clip.x;
                    tmp_clip.h += src_clip.y;
                }
                dst_clip = out_clip;
                src_clip = tmp_clip;
                
                /* 如果底图透明度更新, 则清空底色防止错误混合 */
                if (src_surface->alpha != scui_alpha_cover) {
                    SCUI_PIXEL_TYPE dst_pixel = {0};
                    scui_draw_area_fill(dst_surface, &dst_clip, &dst_pixel, scui_alpha_cover);
                }
                
                if (scui_window_mgr.switch_args.cfg_type == scui_window_switch_zoom1) {
                    if (widget->myself == scui_window_active_curr()) {
                        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
                        continue;
                    }
                }
                
                if (scui_window_mgr.switch_args.cfg_type == scui_window_switch_zoom1 ||
                    scui_window_mgr.switch_args.cfg_type == scui_window_switch_zoom2) {
                    float scale_d = scui_map(src_clip.w, 0, src_surface->hor_res, 50, 100) / 100.0f;
                    SCUI_LOG_INFO("scale_d:%f", scale_d);
                    scui_matrix_t inv_matrix = {0};
                    scui_matrix_identity(&inv_matrix);
                    scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +dst_clip.w / 2,.y = +dst_clip.h / 2,});
                    scui_matrix_scale(&inv_matrix, &(scui_point2_t){.x = scale_d,.y = scale_d,});
                    scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -dst_clip.w / 2,.y = -dst_clip.h / 2,});
                    // scui_matrix_check(&inv_matrix);
                    scui_matrix_inverse(&inv_matrix);
                    scui_draw_area_blit_by_matrix(dst_surface, &dst_clip, src_surface, &src_clip, &inv_matrix);
                    continue;
                }
                
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
            }
            break;
        }
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
    if (num != 0)
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
    scui_handle_t  list_num = 0;
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
        
        scui_widget_draw(handle, NULL, true);
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    scui_widget_event_mask_execute(event);
    scui_window_switch_event(event);
}
