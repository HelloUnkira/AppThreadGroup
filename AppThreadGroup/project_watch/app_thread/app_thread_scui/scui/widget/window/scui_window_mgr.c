/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_window_mgr_t scui_window_mgr = {0};

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

/*@brief 窗口管理器过滤根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 *@param ofs  根控件偏移
 */
void scui_window_list_filter(scui_widget_t **list, scui_handle_t num, scui_handle_t *ofs)
{
    int32_t idx = (int32_t)num - 1;
    for (idx = (int32_t)num - 1; idx >= 0; idx--) {
        scui_widget_t *widget = list[idx];
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget->myself));
        
        /* 完全覆盖,此时跳过 */
        if (widget->clip.x == 0 && widget->clip.y == 0 &&
            widget->surface->alpha == scui_alpha_cover)
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
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_grid) {
            if (widget->myself != scui_window_active_curr()) {
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
                continue;
            }
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_grid) {
            /* 这里偷个懒不做额外参数配给了 */
            #define SCUI_WINDOW_SWITCH_GRID_W_SEG   4
            #define SCUI_WINDOW_SWITCH_GRID_H_SEG   4
            
            scui_area_t clip_seg
            [SCUI_WINDOW_SWITCH_GRID_W_SEG]
            [SCUI_WINDOW_SWITCH_GRID_H_SEG] = {0};
            
            scui_coord_t w_num = SCUI_WINDOW_SWITCH_GRID_W_SEG;
            scui_coord_t h_num = SCUI_WINDOW_SWITCH_GRID_H_SEG;
            scui_coord_t w_res = src_surface->hor_res;
            scui_coord_t h_res = src_surface->ver_res;
            scui_coord_t w_seg = w_res / w_num;
            scui_coord_t h_seg = h_res / h_num;
            scui_coord_t w_mod = w_res % w_num / 2;
            scui_coord_t h_mod = h_res % h_num / 2;
            SCUI_ASSERT(w_res % w_num % 2 == 0);
            SCUI_ASSERT(h_res % h_num % 2 == 0);
            
            scui_coord_t scale_d = scui_window_mgr.switch_args.pct;
            SCUI_LOG_INFO("scale_d:%d", scale_d);
            
            for (uint8_t idx_j = 0; idx_j < h_num; idx_j++)
            for (uint8_t idx_i = 0; idx_i < w_num; idx_i++) {
                clip_seg[idx_j][idx_i].w = w_seg;
                clip_seg[idx_j][idx_i].h = h_seg;
                
                if (idx_i == w_num / 2 - 1 || idx_i == w_num / 2)
                    clip_seg[idx_j][idx_i].w += w_mod;
                if (idx_j == h_num / 2 - 1 || idx_j == h_num / 2)
                    clip_seg[idx_j][idx_i].h += h_mod;
                
                if (idx_i != 0) {
                    clip_seg[idx_j][idx_i].x += clip_seg[idx_j][idx_i - 1].x;
                    clip_seg[idx_j][idx_i].x += clip_seg[idx_j][idx_i - 1].w;
                }
                
                if (idx_j != 0) {
                    clip_seg[idx_j][idx_i].y += clip_seg[idx_j - 1][idx_i].y;
                    clip_seg[idx_j][idx_i].y += clip_seg[idx_j - 1][idx_i].h;
                }
            }
            
            for (uint8_t idx_j = 0; idx_j < h_num; idx_j++)
            for (uint8_t idx_i = 0; idx_i < w_num; idx_i++) {
                scui_coord_t scale_w = scui_map(scale_d, 0, 100, 0, clip_seg[idx_j][idx_i].w);
                scui_coord_t scale_h = scui_map(scale_d, 0, 100, 0, clip_seg[idx_j][idx_i].h);
                
                clip_seg[idx_j][idx_i].x += scale_w / 2 + scale_w % 2;
                clip_seg[idx_j][idx_i].y += scale_h / 2 + scale_h % 2;
                clip_seg[idx_j][idx_i].w -= scale_w;
                clip_seg[idx_j][idx_i].h -= scale_h;
                
                dst_clip = clip_seg[idx_j][idx_i];
                src_clip = clip_seg[idx_j][idx_i];
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
            }
            continue;
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_flip) {
            
            /* 小于一半跳过绘制, 因为会被覆盖 */
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0) {
                if (src_clip.w < src_surface->hor_res / 2)
                    continue;
                if (src_clip.w == src_surface->hor_res / 2)
                if (widget->myself != scui_window_active_curr())
                    continue;
            }
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0) {
                if (src_clip.h < src_surface->ver_res / 2)
                    continue;
                if (src_clip.h == src_surface->ver_res / 2)
                if (widget->myself != scui_window_active_curr())
                    continue;
            }
            /* 清空底色防止错误混合 */
            SCUI_PIXEL_TYPE dst_pixel = {0};
            dst_clip.x = 0;
            dst_clip.y = 0;
            dst_clip.w = dst_surface->hor_res;
            dst_clip.h = dst_surface->ver_res;
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
            scui_face3_t face = {
                .point3[1].x = src_surface->hor_res - 1,
                .point3[2].x = src_surface->hor_res - 1,
                .point3[2].y = src_surface->ver_res - 1,
                .point3[3].y = src_surface->ver_res - 1,
            };
            scui_area3_transform_by_matrix(&face, &r_matrix);
            
            scui_point2_t offset = {0};
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                offset.x = (src_surface->hor_res - scui_dist(face.point3[0].x, face.point3[1].x)) / 2;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                offset.y = (src_surface->ver_res - scui_dist(face.point3[0].y, face.point3[2].y)) / 2;
            
            SCUI_LOG_INFO("offset:%f", offset);
            for (uint8_t idx_v = 0; idx_v < 4; idx_v++)
                scui_point3_offset_xy(&face.point3[idx_v], &offset);
            
            scui_matrix_t inv_matrix = {0};
            scui_size2_t size2 = {.w = src_surface->hor_res,.h = src_surface->ver_res,};
            scui_matrix_affine_blit(&inv_matrix, &size2, &face);
            // scui_matrix_check(&inv_matrix);
            scui_matrix_inverse(&inv_matrix);
            dst_clip.x = 0;
            dst_clip.y = 0;
            dst_clip.w = dst_surface->hor_res;
            dst_clip.h = dst_surface->ver_res;
            src_clip.x = 0;
            src_clip.y = 0;
            src_clip.w = src_surface->hor_res;
            src_clip.h = src_surface->ver_res;
            scui_draw_area_blit_by_matrix(dst_surface, &dst_clip, src_surface, &src_clip, &inv_matrix);
            continue;
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_cube) {
            
            /* 清空底色防止错误混合 */
            if (idx == 0) {
                SCUI_PIXEL_TYPE dst_pixel = {0};
                dst_clip.x = 0;
                dst_clip.y = 0;
                dst_clip.w = dst_surface->hor_res;
                dst_clip.h = dst_surface->ver_res;
                scui_draw_area_fill(dst_surface, &dst_clip, &dst_pixel, scui_alpha_cover);
            }
            
            float angle = 0;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0) {
                angle = scui_map(src_clip.w, 0, src_surface->hor_res, 90, 0);
                if (src_clip.x == 0)
                    angle = 90 - angle;
            }
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0) {
                angle = scui_map(src_clip.h, 0, src_surface->ver_res, 90, 0);
                if (src_clip.y == 0)
                    angle = 90 - angle;
                angle = -angle;
            }
            SCUI_LOG_INFO("angle:%f", angle);
            
            scui_matrix_t r_matrix = {0};
            scui_matrix_identity(&r_matrix);
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                scui_matrix_rotate(&r_matrix, angle, 0x02);
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                scui_matrix_rotate(&r_matrix, angle, 0x01);
            // scui_matrix_check(&r_matrix);
            
            scui_coord_t x_res = src_surface->hor_res / 2;
            scui_coord_t y_res = src_surface->ver_res / 2;
            scui_coord_t z_res = 0;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                z_res = src_surface->hor_res / 2;
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                z_res = src_surface->ver_res / 2;
            
            scui_vertex3_t vertex3_0 = {-1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
            scui_vertex3_t vertex3_1 = {+1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
            scui_vertex3_t vertex3_2 = {+1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
            scui_vertex3_t vertex3_3 = {-1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
            scui_vertex3_t vertex3_4 = {-1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
            scui_vertex3_t vertex3_5 = {+1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
            scui_vertex3_t vertex3_6 = {+1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
            scui_vertex3_t vertex3_7 = {-1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
            
            scui_face3_t face3[6] = {
                {.point3 = {vertex3_0, vertex3_1, vertex3_2, vertex3_3,},}, // 0321
                {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},}, // 4567
                {.point3 = {vertex3_1, vertex3_5, vertex3_6, vertex3_2,},}, // 1265
                {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},}, // 0473
                {.point3 = {vertex3_3, vertex3_2, vertex3_6, vertex3_7,},}, // 2376
                {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},}, // 0154
            };
            
            scui_face3_t face = {0};
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
                face = src_clip.x > 0 ? face3[0] : face3[2];
            if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
                face = src_clip.y > 0 ? face3[0] : face3[4];
            
            scui_area3_transform_by_matrix(&face, &r_matrix);
            scui_area3_offset_xy(&face, &(scui_point2_t){.x = x_res, .y = y_res,});
            
            scui_matrix_t inv_matrix = {0};
            scui_size2_t size2 = {.w = src_surface->hor_res,.h = src_surface->ver_res,};
            scui_matrix_affine_blit(&inv_matrix, &size2, &face);
            // scui_matrix_check(&inv_matrix);
            scui_matrix_inverse(&inv_matrix);
            dst_clip.x = 0;
            dst_clip.y = 0;
            dst_clip.w = dst_surface->hor_res;
            dst_clip.h = dst_surface->ver_res;
            src_clip.x = 0;
            src_clip.y = 0;
            src_clip.w = src_surface->hor_res;
            src_clip.h = src_surface->ver_res;
            scui_draw_area_blit_by_matrix(dst_surface, &dst_clip, src_surface, &src_clip, &inv_matrix);
            continue;
        }
        
        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, color);
    }
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
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget))
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
        scui_window_surface_switch(0x00, &widget_only);
        scui_surface_t *surface_fb = scui_surface_fb_draw();
        scui_widget_surface_swap(list_lvl_0[ofs_idx], surface_fb);
        SCUI_LOG_INFO("blend to switch mode");
        return;
    }
    #endif
    
    /* 第一轮混合:处理所有独立画布 */
    scui_window_list_blend(&list_lvl_0[ofs_idx], list_lvl_0_num);
    
    /* 第二轮混合:处理所有独立画布 */
    for (scui_handle_t idx = 0; idx < list_lvl_1_num; idx++) {
        scui_widget_t *widget = list_lvl_1[idx];
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
