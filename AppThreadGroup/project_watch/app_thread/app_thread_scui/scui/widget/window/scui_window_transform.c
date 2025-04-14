/*实现目标:
 *    窗口管理器:窗口变换
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;



/* 这里偷个懒不做额外参数配给了 */
#define SCUI_WINDOW_TRANSFORM_CIRCLE_W_SEG     16
#define SCUI_WINDOW_TRANSFORM_CIRCLE_H_SEG     16
/* 这里偷个懒不做额外参数配给了 */
#define SCUI_WINDOW_TRANSFORM_GRID_W_SEG        4
#define SCUI_WINDOW_TRANSFORM_GRID_H_SEG        4
/* 这里偷个懒不做额外参数配给了 */
#define SCUI_WINDOW_TRANSFORM_FLIP_W_SEG        5
/* 这里偷个懒不做额外参数配给了 */
#define SCUI_WINDOW_TRANSFORM_FLAP_WIDTH        (0.173)



/*@brief 调整控件画布到绘制画布的剪切域
 *@param widget   控件实例
 *@param dst_clip 绘制画布剪切域
 *@param src_clip 控件画布剪切域
 *@retval 有效或无效
 */
static bool scui_window_transform_clip(scui_widget_t *widget, scui_area_t *dst_clip, scui_area_t *src_clip)
{
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_surface_t *src_surface = widget->surface;
    
    SCUI_ASSERT(scui_widget_surface_only(widget));
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    SCUI_ASSERT(widget->clip_set.clip.x == 0);
    SCUI_ASSERT(widget->clip_set.clip.y == 0);
    *dst_clip = scui_surface_area(dst_surface);
    *src_clip = widget->clip_set.clip;
    
    /* 独立画布将窗口偏移补充到画布上 */
    scui_point_t dst_offset = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    if (!scui_area_limit_offset(dst_clip, &dst_offset))
         return false;
    
    scui_point_t src_offset = {
        .x = -dst_offset.x,
        .y = -dst_offset.y,
    };
    if (!scui_area_limit_offset(src_clip, &src_offset))
         return false;
    
    return true;
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_move(scui_widget_t **list, scui_handle_t num)
{
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = {0}, src_clip = {0};
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cover(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        if (list[idx]->myself == scui_window_mgr.active_curr) {
            
            scui_area_t dst_clip = scui_surface_area(dst_surface);
            scui_area_t src_clip = scui_surface_area(src_surface);
            
            scui_alpha_t alpha = src_surface->alpha;
            src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(100 - scui_window_mgr.switch_args.pct));
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            src_surface->alpha = alpha;
        } else {
            
            scui_area_t dst_clip = {0}, src_clip = {0};
            if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
                 continue;
            
            scui_alpha_t alpha = src_surface->alpha;
            src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(scui_window_mgr.switch_args.pct));
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            src_surface->alpha = alpha;
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_zoom(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = {0}, src_clip = {0};
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        float scale_d = 0;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
            scale_d = scui_map(src_clip.w, 0, src_surface->hor_res, 50, 100) / 100.0f;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
            scale_d = scui_map(src_clip.h, 0, src_surface->ver_res, 50, 100) / 100.0f;
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_zoom1)
        if (list[idx]->myself == scui_window_mgr.active_curr) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            continue;
        }
        
        SCUI_LOG_INFO("scale_d:%f", scale_d);
        scui_matrix_t inv_matrix = {0};
        scui_matrix_identity(&inv_matrix);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +dst_clip.x,    .y = +dst_clip.y,});
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +dst_clip.w / 2,.y = +dst_clip.h / 2,});
        scui_matrix_scale(&inv_matrix, &(scui_point2_t){.x = scale_d,.y = scale_d,});
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -src_clip.w / 2,.y = -src_clip.h / 2,});
        
        #if 1 // 扩充源区域的取点范围,保证到达边界
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0) {
            scui_coord_t delta = (scui_multi_t)src_clip.w * SCUI_SCALE_COF / (scale_d * SCUI_SCALE_COF) - src_clip.w;
            src_clip.w = scui_min(src_surface->hor_res, src_clip.w + delta);
            
            if (src_clip.x > 0) {
                scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -delta,});
                src_clip.x = scui_max(0, src_clip.x - delta);
            }
        }
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
            scui_coord_t delta = (scui_multi_t)src_clip.h * SCUI_SCALE_COF / (scale_d * SCUI_SCALE_COF) - src_clip.h;
            src_clip.h = scui_min(src_surface->ver_res, src_clip.h + delta);
            
            if (src_clip.y > 0) {
                scui_matrix_translate(&inv_matrix, &(scui_point2_t){.y = -delta,});
                src_clip.y = scui_max(0, src_clip.y - delta);
            }
        }
        #endif
        
        // scui_matrix_check(&inv_matrix);
        // scui_matrix_t matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_coord_t pct = 0;
        if (list[idx]->myself == scui_window_mgr.active_curr)
            pct = 100 - scui_window_mgr.switch_args.pct;
        if (list[idx]->myself != scui_window_mgr.active_curr)
            pct = scui_window_mgr.switch_args.pct;
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                    SCUI_COLOR_UNUSED, &inv_matrix, NULL);
        src_surface->alpha = alpha;
        continue;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cinout(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        float scale_d = 0;
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_in)
            scale_d = scui_map(scui_window_mgr.switch_args.pct, 100, 0,   0, 100) / 100.0f;
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_out)
            scale_d = scui_map(scui_window_mgr.switch_args.pct, 100, 0, 200, 100) / 100.0f;
        SCUI_LOG_INFO("pct:%d, scale_d:%f", scui_window_mgr.switch_args.pct, scale_d);
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_in ||
            scui_window_mgr.switch_args.type == scui_window_switch_center_out)
        if (list[idx]->myself != scui_window_mgr.active_curr) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            continue;
        }
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_inout) {
            
            if (list[idx]->myself == scui_window_mgr.active_curr)
                if (scui_window_mgr.switch_args.pct > 50)
                    continue;
            if (list[idx]->myself != scui_window_mgr.active_curr)
                if (scui_window_mgr.switch_args.pct < 50)
                    continue;
            
            if (list[idx]->myself == scui_window_mgr.active_curr)
                scale_d = scui_map(scui_window_mgr.switch_args.pct,  50,  0, 0, 100) / 100.0f;
            if (list[idx]->myself != scui_window_mgr.active_curr)
                scale_d = scui_map(scui_window_mgr.switch_args.pct, 50, 100, 0, 100) / 100.0f;
            SCUI_LOG_INFO("pct:%d, scale_d:%f", scui_window_mgr.switch_args.pct, scale_d);
        }
        
        scui_matrix_t inv_matrix = {0};
        scui_matrix_identity(&inv_matrix);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +dst_clip.w / 2,.y = +dst_clip.h / 2,});
        scui_matrix_scale(&inv_matrix, &(scui_point2_t){.x = scale_d,.y = scale_d,});
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -src_clip.w / 2,.y = -src_clip.h / 2,});
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_coord_t pct = 100 - scui_window_mgr.switch_args.pct;
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_center_inout) {
            
            if (list[idx]->myself == scui_window_mgr.active_curr)
                pct = scui_map(scui_window_mgr.switch_args.pct,   0, 50, 100, 0);
            if (list[idx]->myself != scui_window_mgr.active_curr)
                pct = scui_map(scui_window_mgr.switch_args.pct, 100, 50, 100, 0);
            SCUI_LOG_INFO("pct:%d", pct);
        }
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                    SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_rotate(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    // 这是特殊的变换, 只有水平变换, 没有垂直变换
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
        scui_window_transform_move(list, num);
        return;
    }
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        scui_coord_t angle = 0, pct = scui_window_mgr.switch_args.pct;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_r) != 0)
            angle = +pct * 3.6;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_l) != 0)
            angle = -pct * 3.6;
        
        if (list[idx]->myself == scui_window_mgr.active_curr)
            if (pct >= 50)
                continue;
        if (list[idx]->myself != scui_window_mgr.active_curr)
            if (pct <= 50)
                continue;
        
        pct = pct <= 50 ? 100 - pct * 2 : (pct - 50) * 2;
        
        scui_matrix_t inv_matrix = {0};
        scui_matrix_identity(&inv_matrix);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +dst_clip.w / 2,.y = +dst_clip.h / 2,});
        scui_matrix_rotate(&inv_matrix, angle, 0x00);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -src_clip.w / 2,.y = -src_clip.h / 2,});
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                    SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_rotate1(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    // 这是特殊的变换, 只有水平变换, 没有垂直变换
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
        scui_window_transform_move(list, num);
        return;
    }
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        scui_coord_t angle_total = 75, angle = 0, pct = scui_window_mgr.switch_args.pct;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_r) != 0)
            angle = 270 + pct * angle_total / 100;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_l) != 0)
            angle = 270 - pct * angle_total / 100;
        
        if (list[idx]->myself != scui_window_mgr.active_curr) {
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_r) != 0)
                angle -= angle_total;
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_l) != 0)
                angle += angle_total;
        }
        
        scui_multi_t radius  = dst_clip.h;
        scui_multi_t point_x = radius * scui_cos4096((int32_t)angle) >> 12;
        scui_multi_t point_y = radius * scui_sin4096((int32_t)angle) >> 12;
        point_x = dst_clip.w / 2     + point_x;
        point_y = dst_clip.h * 3 / 2 + point_y;
        
        scui_matrix_t inv_matrix = {0};
        scui_matrix_identity(&inv_matrix);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +point_x,.y = +point_y,});
        scui_matrix_rotate(&inv_matrix, angle - 270, 0x00);
        scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -src_clip.w / 2,.y = -src_clip.h / 2,});
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                    SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_circle(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    scui_coord_t pct = scui_window_mgr.switch_args.pct;
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (list[idx]->myself != scui_window_mgr.active_curr) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
        }
        
        scui_area_t clip_seg
        [SCUI_WINDOW_TRANSFORM_CIRCLE_W_SEG]
        [SCUI_WINDOW_TRANSFORM_CIRCLE_H_SEG] = {0};
        
        scui_coord_t w_num = SCUI_WINDOW_TRANSFORM_CIRCLE_W_SEG;
        scui_coord_t h_num = SCUI_WINDOW_TRANSFORM_CIRCLE_H_SEG;
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
            scui_coord_t center_rx = w_res / 2;
            scui_coord_t center_ry = h_res / 2;
            scui_coord_t center_sx = clip_seg[idx_j][idx_i].x + clip_seg[idx_j][idx_i].w / 2;
            scui_coord_t center_sy = clip_seg[idx_j][idx_i].y + clip_seg[idx_j][idx_i].h / 2;
            scui_multi_t dist_drx = center_rx * (100 - scale_d) / 100;
            scui_multi_t dist_dry = center_ry * (100 - scale_d) / 100;
            scui_multi_t dist_dsx = scui_dist(center_rx, center_sx);
            scui_multi_t dist_dsy = scui_dist(center_ry, center_sy);
            scui_multi_t dist_dr = dist_drx * dist_drx + dist_dry * dist_dry;
            scui_multi_t dist_ds = dist_dsx * dist_dsx + dist_dsy * dist_dsy;
            
            dst_clip = clip_seg[idx_j][idx_i];
            src_clip = clip_seg[idx_j][idx_i];
            
            if (dist_dr >= dist_ds) {
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_map(dist_ds, 0, dist_dr, alpha, alpha / 2);
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
                src_surface->alpha = alpha;
                continue;
            }
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_grid(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    scui_coord_t pct = scui_window_mgr.switch_args.pct;
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (list[idx]->myself != scui_window_mgr.active_curr) {
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
            continue;
        }
        
        scui_area_t clip_seg
        [SCUI_WINDOW_TRANSFORM_GRID_W_SEG]
        [SCUI_WINDOW_TRANSFORM_GRID_H_SEG] = {0};
        
        scui_coord_t w_num = SCUI_WINDOW_TRANSFORM_GRID_W_SEG;
        scui_coord_t h_num = SCUI_WINDOW_TRANSFORM_GRID_H_SEG;
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
            scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip1(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = {0}, src_clip = {0};
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        scui_face3_t face3 = {0};
        scui_area_t  area  = scui_surface_area(src_surface);
        scui_area3_by_area(&face3, &area);
        
        float angle = 0;
        scui_point2_t offset = {0};
        scui_matrix_t r_matrix = {0};
        scui_matrix_identity(&r_matrix);
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0) {
            angle = scui_map(src_clip.w, 0, src_surface->hor_res, 180, 0);
            scui_matrix_rotate(&r_matrix, angle, 0x02);
            offset.x = -src_surface->hor_res / 2;
        }
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
            angle = scui_map(src_clip.h, 0, src_surface->ver_res, 180, 0);
            scui_matrix_rotate(&r_matrix, angle, 0x01);
            offset.y = -src_surface->ver_res / 2;
        }
        /* 小于一半跳过绘制, 因为会被覆盖 */
        SCUI_LOG_INFO("angle:%f", angle);
        if (angle > 90)
            continue;
        
        // 移动到轴心, 然后再旋转
        scui_point2_t offset1 = {.x = +offset.x, .y = +offset.y,};
        scui_point2_t offset2 = {.x = -offset.x, .y = -offset.y,};
        
        // scui_matrix_check(&r_matrix);
        scui_area3_offset_xy(&face3, &offset1);
        scui_area3_transform_by_matrix(&face3, &r_matrix);
        scui_area3_offset_xy(&face3, &offset2);
        
        dst_clip = scui_surface_area(dst_surface);
        src_clip = scui_surface_area(src_surface);
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = src_clip.w,.h = src_clip.h,};
        scui_matrix_affine_blit(&inv_matrix, &size2, &face3);
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_draw_area_matrix_blend(dst_surface, &dst_clip,
            src_surface, &src_clip, SCUI_COLOR_UNUSED,
            &inv_matrix, &src_matrix);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip2(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    /* 底图清空 */
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = {0}, src_clip = {0};
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        scui_area_t area = scui_surface_area(dst_surface);
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0) {
            area.x = dst_clip.x > 0 ? dst_surface->hor_res / 2 : 0;
            area.w = dst_surface->hor_res / 2;
        }
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
            area.y = dst_clip.y > 0 ? src_surface->ver_res / 2 : 0;
            area.h = dst_surface->ver_res / 2;
        }
        
        dst_clip = area; src_clip = area;
        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
    }
    
    // flip2 在 flip1 的基础上扩展生成
    scui_window_transform_flip1(list, num);
    
    // 这里投机取巧,在上面变换完之后,把不显示的那部分还原的给盖住
    for (scui_handle_t idx = 0; idx < num; idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface;
        SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
        SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
        
        scui_area_t dst_clip = {0}, src_clip = {0};
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        /* 小于一半跳过绘制, 因为不覆盖它 */
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
            if (src_clip.w < src_surface->hor_res / 2)
                continue;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
            if (src_clip.h < src_surface->ver_res / 2)
                continue;
        
        scui_area_t area = scui_surface_area(dst_surface);
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0) {
            area.x = dst_clip.x > 0 ? dst_surface->hor_res / 2 : 0;
            area.w = dst_surface->hor_res / 2;
        }
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
            area.y = dst_clip.y > 0 ? dst_surface->ver_res / 2 : 0;
            area.h = dst_surface->ver_res / 2;
        }
        
        dst_clip = area; src_clip = area;
        scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip, SCUI_COLOR_UNUSED);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip3(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    // 这是特殊的变换, 只有水平变换, 没有垂直变换
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0) {
        scui_window_transform_flip1(list, num);
        return;
    }
    
    scui_coord_t  seg_w_num = SCUI_WINDOW_TRANSFORM_FLIP_W_SEG;
    scui_matrix_t seg_src_matrix[SCUI_WINDOW_TRANSFORM_FLIP_W_SEG] = {0};
    scui_matrix_t seg_inv_matrix[SCUI_WINDOW_TRANSFORM_FLIP_W_SEG] = {0};
    scui_coord_t  seg_src_angle[SCUI_WINDOW_TRANSFORM_FLIP_W_SEG] = {0};
    scui_coord_t  seg_src_index[SCUI_WINDOW_TRANSFORM_FLIP_W_SEG] = {0};
    scui_area_t   seg_src_area[SCUI_WINDOW_TRANSFORM_FLIP_W_SEG] = {0};
    // 第一次循环保存起来
    
    for (scui_handle_t seg_idx = 0; seg_idx < seg_w_num; seg_idx++) {
        
        for (scui_handle_t idx = 0; idx < num; idx++) {
            /* 场景切换满足全局目标 */
            scui_surface_t *dst_surface = scui_frame_buffer_draw();
            scui_surface_t *src_surface = list[idx]->surface;
            SCUI_ASSERT(src_surface->hor_res == scui_disp_get_hor_res());
            SCUI_ASSERT(src_surface->ver_res == scui_disp_get_ver_res());
            
            scui_area_t seg_w_clip = scui_surface_area(src_surface);
            seg_w_clip.h  = seg_w_clip.h / seg_w_num;
            seg_w_clip.y += seg_w_clip.h * seg_idx;
            
            scui_area_t dst_clip = {0}, src_clip = {0};
            if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
                 continue;
            
            scui_face3_t face3 = {0};
            scui_area3_by_area(&face3, &seg_w_clip);
            
            #if 1
            // 旋转控制逻辑
            
            scui_coord3_t angle = 0.0f, flag = 1.0f;
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_l) != 0);
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_r) != 0)
                flag = -flag;
            
            angle  = scui_map(scui_window_mgr.switch_args.pct, 0, 100, 0, 180 + 90);
            angle -= scui_map(seg_idx, 0, seg_w_num, 0, 90);
            angle  = scui_clamp(angle, 0, 180);
            angle *= flag;
            
            SCUI_LOG_INFO("flag:%f, angle:%f", flag, angle);
            /* 覆盖检测,跳过覆盖的绘制部分 */
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_l) != 0) {
                if (list[idx]->myself == scui_window_mgr.active_curr)
                    if (angle > +180.0f / 2)
                        continue;
                if (list[idx]->myself != scui_window_mgr.active_curr)
                    if (angle < +180.0f / 2)
                        continue;
            }
            if ((scui_window_mgr.switch_args.dir & scui_opt_dir_to_r) != 0) {
                if (list[idx]->myself == scui_window_mgr.active_curr)
                    if (angle < -180.0f / 2)
                        continue;
                if (list[idx]->myself != scui_window_mgr.active_curr)
                    if (angle > -180.0f / 2)
                        continue;
            }
            
            // 对非焦点窗口做一次镜像
            if (list[idx]->myself != scui_window_mgr.active_curr)
                angle += 180.0f;
            
            #endif
            
            scui_matrix_t r_matrix = {0};
            scui_matrix_identity(&r_matrix);
            scui_matrix_rotate(&r_matrix, angle, 0x02);
            
            // 移动到轴心, 然后再旋转
            scui_point2_t offset1 = {.x = -src_surface->hor_res / 2,};
            scui_point2_t offset2 = {.x = +src_surface->hor_res / 2,};
            // scui_matrix_check(&r_matrix);
            scui_area3_offset_xy(&face3, &offset1);
            scui_area3_transform_by_matrix(&face3, &r_matrix);
            scui_area3_offset_xy(&face3, &offset2);
            
            scui_view3_t view = {
                .x = +seg_w_clip.x + seg_w_clip.w / 2,
                .y = +seg_w_clip.y + seg_w_clip.h / 2,
                .z = -dst_surface->hor_res * 2,
            };
            
            scui_matrix_t inv_matrix = {0};
            scui_size2_t size2 = {.w = seg_w_clip.w, .h = seg_w_clip.h,};
            scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3, &view);
            // scui_matrix_affine_blit(&inv_matrix, &size2, &face3);
            // scui_matrix_check(&inv_matrix);
            scui_matrix_t src_matrix = inv_matrix;
            scui_matrix_inverse(&inv_matrix);
            
            dst_clip = scui_surface_area(dst_surface);
            src_clip = scui_surface_area(src_surface);
            if (!scui_area_inter2(&src_clip, &seg_w_clip))
                 continue;
            
            seg_src_matrix[seg_idx] = src_matrix;
            seg_inv_matrix[seg_idx] = inv_matrix;
            seg_src_angle[seg_idx]  = angle;
            seg_src_index[seg_idx]  = idx;
            seg_src_area[seg_idx]   = src_clip;
            
            continue;
            // 这里不绘制了, 放到之后进行统一绘制
            scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                        SCUI_COLOR_UNUSED, &inv_matrix, NULL);
        }
    }
    
    // 因为有被覆盖的可能性, 以angle相对中心线的距离排序, 距离越小越后绘制
    for (scui_handle_t seg_idx = 0; seg_idx < seg_w_num; seg_idx++) {
        if (seg_src_angle[seg_idx] > 0)
            seg_src_angle[seg_idx] = scui_dist(seg_src_angle[seg_idx], +90);
        if (seg_src_angle[seg_idx] < 0)
            seg_src_angle[seg_idx] = scui_dist(seg_src_angle[seg_idx], -90);
    }
    for (scui_handle_t seg_idx_i = 0; seg_idx_i < seg_w_num; seg_idx_i++)
    for (scui_handle_t seg_idx_j = 0; seg_idx_j < seg_w_num; seg_idx_j++)
        if (seg_src_angle[seg_idx_i] > seg_src_angle[seg_idx_j]) {
            
            scui_matrix_t inv_matrix  = seg_inv_matrix[seg_idx_i];
            seg_inv_matrix[seg_idx_i] = seg_inv_matrix[seg_idx_j];
            seg_inv_matrix[seg_idx_j] = inv_matrix;
            
            scui_matrix_t src_matrix  = seg_src_matrix[seg_idx_i];
            seg_src_matrix[seg_idx_i] = seg_src_matrix[seg_idx_j];
            seg_src_matrix[seg_idx_j] = src_matrix;
            
            scui_coord_t src_angle   = seg_src_angle[seg_idx_i];
            seg_src_angle[seg_idx_i] = seg_src_angle[seg_idx_j];
            seg_src_angle[seg_idx_j] = src_angle;
            
            scui_coord_t src_index   = seg_src_index[seg_idx_i];
            seg_src_index[seg_idx_i] = seg_src_index[seg_idx_j];
            seg_src_index[seg_idx_j] = src_index;
            
            scui_area_t  src_area   = seg_src_area[seg_idx_i];
            seg_src_area[seg_idx_i] = seg_src_area[seg_idx_j];
            seg_src_area[seg_idx_j] = src_area;
        }
    
    // 第二次循环再进行迭代绘制, 
    for (scui_handle_t seg_idx = 0; seg_idx < seg_w_num; seg_idx++) {
        /* 场景切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[seg_src_index[seg_idx]]->surface;
        
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        scui_area_t src_clip = seg_src_area[seg_idx];
        scui_matrix_t inv_matrix = seg_inv_matrix[seg_idx];
        scui_matrix_t src_matrix = seg_src_matrix[seg_idx];
        
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
                                    SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flap1(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    // active, inactive
    scui_handle_t  window_a = scui_window_active_curr();
    scui_widget_t *widget_a = list[0]->myself == window_a ? list[0] : list[1];
    scui_widget_t *widget_i = list[0]->myself != window_a ? list[0] : list[1];
    
    /* 场景切换满足全局目标 */
    scui_surface_t *dst_surface   = scui_frame_buffer_draw();
    scui_surface_t *src_surface_a = widget_a->surface;
    scui_surface_t *src_surface_i = widget_i->surface;
    SCUI_ASSERT(src_surface_a->hor_res == dst_surface->hor_res);
    SCUI_ASSERT(src_surface_a->ver_res == dst_surface->ver_res);
    SCUI_ASSERT(src_surface_i->hor_res == dst_surface->hor_res);
    SCUI_ASSERT(src_surface_i->ver_res == dst_surface->ver_res);
    
    /* 空间建模参数 */
    scui_coord3_t angle = scui_map(scui_window_mgr.switch_args.pct, 0, 100, 0, 180);
    switch (scui_window_mgr.switch_args.dir) {
    case scui_opt_dir_to_l: angle = -angle; break;
    case scui_opt_dir_to_r: angle = +angle; break;
    case scui_opt_dir_to_u: angle = +angle; break;
    case scui_opt_dir_to_d: angle = -angle; break;
    }
    SCUI_LOG_INFO("angle:%f", angle);
    
    scui_coord3_t scale = 1.0f;
    scui_coord3_t s_cen = scui_dist(scui_window_mgr.switch_args.pct, 50);
    scale = scui_map(s_cen, 0, 50, 0.707f, 1.0f);
    
    // 数字用于调整视角保证边界平稳过度
    scui_coord3_t s_res = scale * 0.95;
    scui_coord3_t x_res = s_res * dst_surface->hor_res / 2;
    scui_coord3_t y_res = s_res * dst_surface->ver_res / 2;
    scui_coord3_t z_res = s_res * SCUI_WINDOW_TRANSFORM_FLAP_WIDTH;
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
        z_res *= dst_surface->hor_res / 2;
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
        z_res *= dst_surface->ver_res / 2;
    
    /* 3D旋转矩阵 */
    scui_matrix_t r_matrix = {0};
    scui_matrix_identity(&r_matrix);
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
        scui_matrix_rotate(&r_matrix, angle, 0x02);
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
        scui_matrix_rotate(&r_matrix, angle, 0x01);
    // scui_matrix_check(&r_matrix);
    
    /* 3D空间建模(8个顶点) */
    scui_vertex3_t vertex3_0 = {-1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_1 = {+1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_2 = {+1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_3 = {-1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_4 = {-1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_5 = {+1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_6 = {+1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_7 = {-1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
    
    /* 3D空间建模(6个面) */
    scui_face3_t face3[6] = {
        {.point3 = {vertex3_1, vertex3_0, vertex3_3, vertex3_2,},},
        {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},},
        {.point3 = {vertex3_5, vertex3_1, vertex3_2, vertex3_6,},},
        {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},},
        {.point3 = {vertex3_7, vertex3_6, vertex3_2, vertex3_3,},},
        {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},},
    };
    
    /* 3D空间Z轴序列 */
    scui_coord3_t center_z[6] = {0};
    scui_handle_t face_idx[6] = {0};
    
    /* 3D空间面应用变换, 获得变换后的空间状态 */
    for (scui_handle_t idx = 0; idx < 6; idx++) {
        scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
        scui_area3_offset_xy(&face3[idx], &(scui_point2_t){
            .x = dst_surface->hor_res / 2,
            .y = dst_surface->ver_res / 2,
        });
        scui_area3_center_z(&face3[idx], &center_z[idx]);
        face_idx[idx] = idx;
    }
    
    /* 3D空间Z轴序列排序 */
    for (scui_handle_t idx_i = 0; idx_i < 6; idx_i++)
    for (scui_handle_t idx_j = 0; idx_j < 6; idx_j++)
        if (center_z[idx_i] < center_z[idx_j] && idx_i != idx_j) {
            
            scui_face3_t face3_t1 = face3[idx_i];
            scui_face3_t face3_t2 = face3[idx_j];
            face3[idx_i] = face3_t2;
            face3[idx_j] = face3_t1;
            
            scui_coord3_t center_z_t1 = center_z[idx_i];
            scui_coord3_t center_z_t2 = center_z[idx_j];
            center_z[idx_i] = center_z_t2;
            center_z[idx_j] = center_z_t1;
            
            scui_handle_t face_idx_t1 = face_idx[idx_i];
            scui_handle_t face_idx_t2 = face_idx[idx_j];
            face_idx[idx_i] = face_idx_t2;
            face_idx[idx_j] = face_idx_t1;
        }
    
    /* 公共视点 */
    scui_view3_t view = {
        .x = dst_surface->hor_res / 2,
        .y = dst_surface->ver_res / 2,
        .z = dst_surface->hor_res * 2,
    };
    
    /* 绘制六个面 */
    for (scui_handle_t idx = 0; idx < 6; idx++) {
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        
        // 跳过被覆盖的面
        if (center_z[idx] < 0.0f)
            continue;
        
        // 跳过上下俩个平行面(因为它们不显示)
        if (scui_area3_parallel(&face3[idx], 0x02))
            continue;
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = dst_clip.w,.h = dst_clip.h,};
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        // scui_matrix_affine_blit(&inv_matrix, &size2, &face);
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        if (face_idx[idx] == 0 || face_idx[idx] == 1) {
            // 绘制俩个窗口面
            scui_surface_t *src_surface = face_idx[idx] == 1 ? src_surface_a : src_surface_i;
            scui_area_t src_clip = scui_surface_area(src_surface);
            scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip, 
                SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
        } else {
            #if 1
            // 没有窗口面, 则填背景灰色
            scui_area_t  src_clip  = dst_clip;
            scui_color_t src_color = SCUI_COLOR_MAKE32(false, 0x0, 0xFF1F1F1F);
            scui_draw_area_matrix_fill(dst_surface, &dst_clip, &src_clip,
                scui_alpha_cover, src_color, &inv_matrix, &src_matrix);
            #endif
        }
        
        
        
        #if 0   // 光影特效
        scui_handle_t handle = scui_window_mgr.switch_args.cfg_args.cube.shadow;
        scui_image_t *shadow = scui_handle_source_check(handle);
        size2.w = scui_image_w(handle) / 2;
        size2.h = scui_image_h(handle);
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        // scui_matrix_affine_blit(&inv_matrix, &size2, &face);
        // scui_matrix_check(&inv_matrix);
        
        scui_area_t src_clip = {0};
        src_clip.w = size2.w;
        src_clip.h = size2.h;
        src_clip.x = scui_window_mgr.switch_args.pct * size2.w / 100;
        src_clip.y = 0;
        src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_color_t src_color = {
            .filter = true, .color_f.full = 0xFF000000,
        };
        
        scui_draw_image_matrix_blend(dst_surface, &dst_clip,
            shadow, &src_clip, scui_alpha_pct100, src_color, &inv_matrix, &src_matrix);
        #endif
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cube(scui_widget_t **list, scui_handle_t num)
{
    SCUI_ASSERT(num == 2);
    
    // active, inactive
    scui_handle_t  window_a = scui_window_active_curr();
    scui_widget_t *widget_a = list[0]->myself == window_a ? list[0] : list[1];
    scui_widget_t *widget_i = list[0]->myself != window_a ? list[0] : list[1];
    
    /* 场景切换满足全局目标 */
    scui_surface_t *dst_surface   = scui_frame_buffer_draw();
    scui_surface_t *src_surface_a = widget_a->surface;
    scui_surface_t *src_surface_i = widget_i->surface;
    SCUI_ASSERT(src_surface_a->hor_res == dst_surface->hor_res);
    SCUI_ASSERT(src_surface_a->ver_res == dst_surface->ver_res);
    SCUI_ASSERT(src_surface_i->hor_res == dst_surface->hor_res);
    SCUI_ASSERT(src_surface_i->ver_res == dst_surface->ver_res);
    
    /* 空间建模参数 */
    scui_coord3_t angle = scui_map(scui_window_mgr.switch_args.pct, 0, 100, 0, 90);
    switch (scui_window_mgr.switch_args.dir) {
    case scui_opt_dir_to_l: angle = -angle; break;
    case scui_opt_dir_to_r: angle = +angle; break;
    case scui_opt_dir_to_u: angle = +angle; break;
    case scui_opt_dir_to_d: angle = -angle; break;
    }
    SCUI_LOG_INFO("angle:%f", angle);
    
    scui_coord3_t scale = 1.0f;
    scui_coord3_t s_cen = scui_dist(scui_window_mgr.switch_args.pct, 50);
    scale = scui_map(s_cen, 0, 50, 0.707f, 1.0f);
    
    // 数字用于调整视角保证边界平稳过度
    scui_coord3_t s_res = scale * 0.78;
    scui_coord3_t x_res = s_res * dst_surface->hor_res / 2;
    scui_coord3_t y_res = s_res * dst_surface->ver_res / 2;
    scui_coord3_t z_res = s_res * 1.0f;
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
        z_res *= dst_surface->hor_res / 2;
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
        z_res *= dst_surface->ver_res / 2;
    
    /* 3D旋转矩阵 */
    scui_matrix_t r_matrix = {0};
    scui_matrix_identity(&r_matrix);
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
        scui_matrix_rotate(&r_matrix, angle, 0x02);
    if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
        scui_matrix_rotate(&r_matrix, angle, 0x01);
    // scui_matrix_check(&r_matrix);
    
    /* 3D空间建模(8个顶点) */
    scui_vertex3_t vertex3_0 = {-1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_1 = {+1.0f * x_res, -1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_2 = {+1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_3 = {-1.0f * x_res, +1.0f * y_res, -1.0f * z_res,};
    scui_vertex3_t vertex3_4 = {-1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_5 = {+1.0f * x_res, -1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_6 = {+1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
    scui_vertex3_t vertex3_7 = {-1.0f * x_res, +1.0f * y_res, +1.0f * z_res,};
    
    /* 3D空间建模(6个面) */
    scui_face3_t face3[6] = {
        {.point3 = {vertex3_0, vertex3_1, vertex3_2, vertex3_3,},},
        {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},},
        {.point3 = {vertex3_5, vertex3_1, vertex3_2, vertex3_6,},},
        {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},},
        {.point3 = {vertex3_7, vertex3_6, vertex3_2, vertex3_3,},},
        {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},},
    };
    
    /* 3D空间Z轴序列 */
    scui_coord3_t center_z[6] = {0};
    scui_handle_t face_idx[6] = {0};
    
    /* 3D空间面应用变换, 获得变换后的空间状态 */
    for (scui_handle_t idx = 0; idx < 6; idx++) {
        scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
        scui_area3_offset_xy(&face3[idx], &(scui_point2_t){
            .x = dst_surface->hor_res / 2,
            .y = dst_surface->ver_res / 2,
        });
        scui_area3_center_z(&face3[idx], &center_z[idx]);
        face_idx[idx] = idx;
    }
    
    /* 3D空间Z轴序列排序 */
    for (scui_handle_t idx_i = 0; idx_i < 6; idx_i++)
    for (scui_handle_t idx_j = 0; idx_j < 6; idx_j++)
        if (center_z[idx_i] < center_z[idx_j] && idx_i != idx_j) {
            
            scui_face3_t face3_t1 = face3[idx_i];
            scui_face3_t face3_t2 = face3[idx_j];
            face3[idx_i] = face3_t2;
            face3[idx_j] = face3_t1;
            
            scui_coord3_t center_z_t1 = center_z[idx_i];
            scui_coord3_t center_z_t2 = center_z[idx_j];
            center_z[idx_i] = center_z_t2;
            center_z[idx_j] = center_z_t1;
            
            scui_handle_t face_idx_t1 = face_idx[idx_i];
            scui_handle_t face_idx_t2 = face_idx[idx_j];
            face_idx[idx_i] = face_idx_t2;
            face_idx[idx_j] = face_idx_t1;
        }
    
    /* 公共视点 */
    scui_view3_t view = {
        .x = dst_surface->hor_res / 2,
        .y = dst_surface->ver_res / 2,
        .z = dst_surface->hor_res * 2,
    };
    
    /* 绘制六个面 */
    for (scui_handle_t idx = 0; idx < 6; idx++) {
        scui_area_t dst_clip = scui_surface_area(dst_surface);
        
        // 跳过被覆盖的面
        if (center_z[idx] < 0.0f)
            continue;
        
        // 跳过上下俩个平行面(因为它们不显示)
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_hor) != 0)
            if (scui_area3_parallel(&face3[idx], 0x02))
                continue;
        if ((scui_window_mgr.switch_args.dir & scui_opt_dir_ver) != 0)
            if (scui_area3_parallel(&face3[idx], 0x01))
                continue;
        
        bool cover_check = true;
        if (face_idx[idx] == 1 || // 移动方向不同会导致选择的绘制面不一样
           (face_idx[idx] == 2 && scui_window_mgr.switch_args.dir == scui_opt_dir_to_l) ||
           (face_idx[idx] == 3 && scui_window_mgr.switch_args.dir == scui_opt_dir_to_r) ||
           (face_idx[idx] == 4 && scui_window_mgr.switch_args.dir == scui_opt_dir_to_u) ||
           (face_idx[idx] == 5 && scui_window_mgr.switch_args.dir == scui_opt_dir_to_d))
            cover_check = false;
        if (cover_check)
            continue;
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = dst_clip.w,.h = dst_clip.h,};
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        // scui_matrix_affine_blit(&inv_matrix, &size2, &face3[idx]);
        // scui_matrix_check(&inv_matrix);
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        // 绘制俩个窗口面
        scui_surface_t *src_surface = face_idx[idx] == 1 ? src_surface_a : src_surface_i;
        scui_area_t src_clip = scui_surface_area(src_surface);
        scui_draw_area_matrix_blend(dst_surface, &dst_clip, src_surface, &src_clip,
            SCUI_COLOR_UNUSED, &inv_matrix, &src_matrix);
        
        
        
        #if 0   // 光影特效
        scui_handle_t handle = scui_window_mgr.switch_args.cfg_args.cube.shadow;
        scui_image_t *shadow = scui_handle_source_check(handle);
        size2.w = scui_image_w(handle) / 2;
        size2.h = scui_image_h(handle);
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        // scui_matrix_affine_blit(&inv_matrix, &size2, &face);
        // scui_matrix_check(&inv_matrix);
        
        src_clip.w = size2.w;
        src_clip.h = size2.h;
        src_clip.x = scui_window_mgr.switch_args.pct * size2.w / 100;
        src_clip.y = 0;
        src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        scui_color_t src_color = {
            .filter = true, .color_f.full = 0xFF000000,
        };
        
        scui_draw_image_matrix_blend(dst_surface, &dst_clip,
            shadow, &src_clip, scui_alpha_pct100, src_color, &inv_matrix, &src_matrix);
        #endif
    }
}
