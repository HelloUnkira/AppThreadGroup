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
    scui_color_mix_t color = {0};
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
        
        /* 多窗口叠加不应用特效渲染 */
        if (scui_window_float_running()) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
            continue;
        }
        
        /* 仅窗口切换时才应用特效渲染 */
        if (scui_widget_event_scroll_flag(0x02, &scui_window_mgr.switch_args.key)) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
            continue;
        }
        
        if (num == 1) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
            continue;
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_in ||
            scui_window_mgr.switch_args.type == scui_window_switch_center_out) {
            if (widget->myself == scui_window_active_curr()) {
                scui_coord_t pct = scui_window_mgr.switch_args.pct;
                float scale_d = 0;
                if (scui_window_mgr.switch_args.type == scui_window_switch_center_in)
                    scale_d = scui_map(pct, 100, 0,  50, 100) / 100.0f;
                if (scui_window_mgr.switch_args.type == scui_window_switch_center_out)
                    scale_d = scui_map(pct, 100, 0, 150, 100) / 100.0f;
                SCUI_LOG_INFO("pct:%d, scale_d:%f", pct, scale_d);
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
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_zoom1) {
            if (widget->myself == scui_window_active_curr()) {
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
                continue;
            }
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_zoom1 ||
            scui_window_mgr.switch_args.type == scui_window_switch_zoom2) {
            /* 如果底图透明度更新, 则清空底色防止错误混合 */
            if (src_surface->alpha != scui_alpha_cover) {
                SCUI_PIXEL_TYPE dst_pixel = {0};
                scui_draw_area_fill(dst_surface, &dst_clip, &dst_pixel, scui_alpha_cover);
            }
            float scale_d = 0;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                scale_d = scui_map(src_clip.w, 0, src_surface->hor_res, 50, 100) / 100.0f;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                scale_d = scui_map(src_clip.h, 0, src_surface->ver_res, 50, 100) / 100.0f;
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
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_flip) {
            
            /* 小于一半跳过绘制, 因为会被覆盖 */
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0) {
                if (src_clip.w <  src_surface->hor_res / 2)
                    continue;
                if (src_clip.w == src_surface->hor_res / 2)
                if (widget->myself != scui_window_active_curr())
                    continue;
            }
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0) {
                if (src_clip.h <  src_surface->ver_res / 2)
                    continue;
                if (src_clip.h == src_surface->ver_res / 2)
                if (widget->myself != scui_window_active_curr())
                    continue;
            }
            /* 如果底图透明度更新, 则清空底色防止错误混合 */
            SCUI_PIXEL_TYPE dst_pixel = {0};
            dst_clip = (scui_area_t){
                .w = dst_surface->hor_res,
                .h = dst_surface->ver_res,
            };
            scui_draw_area_fill(dst_surface, &dst_clip, &dst_pixel, scui_alpha_cover);
            
            float angle = 0;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                angle = scui_map(src_clip.w - src_surface->hor_res / 2, 0, src_surface->hor_res / 2, 90, 0);
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                angle = scui_map(src_clip.h - src_surface->ver_res / 2, 0, src_surface->ver_res / 2, 90, 0);
            SCUI_LOG_INFO("angle:%f", angle);
            
            scui_matrix_t r_matrix = {0};
            scui_matrix_identity(&r_matrix);
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                scui_matrix_rotate(&r_matrix, angle, 0x02);
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                scui_matrix_rotate(&r_matrix, angle, 0x01);
            // scui_matrix_check(&r_matrix);
            scui_face3_t face = {.point3 = {{0},
                {.x = src_surface->hor_res - 1},
                {.x = src_surface->hor_res - 1, .y = src_surface->ver_res - 1},
                {.y = src_surface->ver_res - 1},
            }};
            scui_point3_transform_by_matrix(&face.point3[0], &r_matrix);
            scui_point3_transform_by_matrix(&face.point3[1], &r_matrix);
            scui_point3_transform_by_matrix(&face.point3[2], &r_matrix);
            scui_point3_transform_by_matrix(&face.point3[3], &r_matrix);
            
            scui_point2_t offset = {0};
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                offset.x = (src_surface->hor_res - scui_dist(face.point3[0].x, face.point3[1].x)) / 2;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                offset.y = (src_surface->ver_res - scui_dist(face.point3[0].y, face.point3[2].y)) / 2;
            
            SCUI_LOG_INFO("offset:%f", offset);
            for (uint8_t idx_v = 0; idx_v < 4; idx_v++)
                scui_point3_offset_xy(&face.point3[idx_v], &offset);
            
            scui_matrix_t inv_matrix = {0};
            scui_matrix_affine_blit(&inv_matrix, src_surface->hor_res, src_surface->ver_res, &face);
            // scui_matrix_check(&inv_matrix);
            scui_matrix_inverse(&inv_matrix);
            dst_clip = (scui_area_t){
                .w = dst_surface->hor_res,
                .h = dst_surface->ver_res,
            };
            src_clip = (scui_area_t){
                .w = src_surface->hor_res,
                .h = src_surface->ver_res,
            };
            scui_draw_area_blit_by_matrix(dst_surface, &dst_clip, src_surface, &src_clip, &inv_matrix);
            continue;
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_cube) {
            
            /* 想想看怎么写 */
            
            continue;
        }
        
        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
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
            
            SCUI_ASSERT(widget_1->type == scui_widget_type_window);
            SCUI_ASSERT(widget_2->type == scui_widget_type_window);
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
    scui_window_switch_event(event);
}
