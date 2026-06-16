/*实现目标:
 *    窗口管理器:窗口变换
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_transform_t scui_window_transform = {0};

/*@brief 调整控件画布到绘制画布的剪切域
 *@param widget   控件实例
 *@param dst_clip 绘制画布剪切域
 *@param src_clip 控件画布剪切域
 *@retval 有效或无效
 */
static bool scui_window_transform_clip(scui_widget_t *widget, scui_area_t *dst_clip, scui_area_t *src_clip)
{
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_surface_t *src_surface = widget->surface_c;
    
    SCUI_ASSERT(scui_widget_surface_only(widget));
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    SCUI_ASSERT(widget->clip_set.clip.x == 0);
    SCUI_ASSERT(widget->clip_set.clip.y == 0);
    *dst_clip = scui_frame_buffer_area();
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

/*@brief 窗口管理器画布清空
 */
void scui_window_transform_clean(void)
{
    scui_color_t dst_pixel = {0};
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    scui_draw_area_fill(true, dst_surface, dst_clip, scui_alpha_cover, SCUI_COLOR_ZEROED);
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_move(scui_widget_t **list, scui_handle_t num)
{
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
             continue;
        #endif
        
        scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cover(scui_widget_t **list, scui_handle_t num)
{
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        /* 该部分效果相对于,浮动窗口进入时的样子 */
        if (scui_window_switch_type() == scui_window_switch_cover_in) {
            
            if (list[idx]->myself == scui_window_active_curr()) {
                
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(100 - scui_window_switch_pct()));
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                src_surface->alpha = alpha;
            }
            if (list[idx]->myself != scui_window_active_curr()) {
                
                if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
                     continue;
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(scui_window_switch_pct()));
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                src_surface->alpha = alpha;
            }
        }
        
        /* 该部分效果相对于,浮动窗口退出时的样子 */
        if (scui_window_switch_type() == scui_window_switch_cover_out) {
            
            if (list[idx]->myself != scui_window_active_curr()) {
                
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(scui_window_switch_pct()));
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                src_surface->alpha = alpha;
            }
            if (list[idx]->myself == scui_window_active_curr()) {
                
                if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
                     continue;
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(100 - scui_window_switch_pct()));
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                src_surface->alpha = alpha;
            }
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_zoom(scui_widget_t **list, scui_handle_t num)
{
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        scui_area_t widget_clip = list[idx]->clip;
        
        if (scui_window_switch_type() == scui_window_switch_zoom1) {
            
            if (list[idx]->myself == scui_window_active_curr()) {
                
                if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
                     continue;
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                continue;
            }
        }
        
        scui_coord_t pct = 0;
        if (list[idx]->myself == scui_window_active_curr())
            pct = 100 - scui_window_switch_pct();
        if (list[idx]->myself != scui_window_active_curr())
            pct = scui_window_switch_pct();
        
        float scale_d = scui_map(pct, 0, 100, 80, 100) / 100.0f;
        SCUI_LOG_INFO("scale_d:%f", scale_d);
        scui_point_t src_scale = {
            .x = scale_d * SCUI_SCALE_COF,
            .y = scale_d * SCUI_SCALE_COF,
        };
        
        scui_area_t dst_clip_s = widget_clip;
        dst_clip_s.w *= scale_d;
        dst_clip_s.h *= scale_d;
        dst_clip_s.x += (widget_clip.w - dst_clip_s.w) / 2;
        dst_clip_s.y += (widget_clip.h - dst_clip_s.h) / 2;
        
        scui_point_t dst_anchor = scui_area_center(&dst_clip_s);
        scui_point_t src_center = scui_area_center(&src_clip);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_anchor.x -= seg_offset.x;
        dst_anchor.y -= seg_offset.y;
        #endif
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_2d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
            SCUI_COLOR_UNUSED, src_scale, 0, dst_anchor, src_center);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_center(scui_widget_t **list, scui_handle_t num)
{
    /* 交换窗口绘制顺序 */
    if (scui_window_switch_type() == scui_window_switch_center_out) {
        
        if (list[0]->myself != scui_window_active_curr()) {
            scui_widget_t *widget_0 = list[0];
            scui_widget_t *widget_1 = list[1];
            list[0] = widget_1;
            list[1] = widget_0;
        }
    }
    
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        float scale_d = 0;
        scui_coord_t pct = 0;
        
        if (scui_window_switch_type() == scui_window_switch_center_in) {
            pct = scui_window_switch_pct();
            scale_d = scui_map(pct, 0, 100, 100, 20) / 100.0f;
            pct = 100 - scui_window_switch_pct();
            
            if (list[idx]->myself != scui_window_active_curr()) {
                
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                continue;
            }
        }
        if (scui_window_switch_type() == scui_window_switch_center_out) {
            pct = scui_window_switch_pct();
            scale_d = scui_map(pct, 0, 100, 20, 100) / 100.0f;
            
            if (list[idx]->myself == scui_window_active_curr()) {
                
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
                continue;
            }
        }
        
        if (scui_window_switch_type() == scui_window_switch_center_inout) {
            pct = scui_window_switch_pct();
            
            if (list[idx]->myself == scui_window_active_curr() && pct > 50) continue;
            if (list[idx]->myself != scui_window_active_curr() && pct < 50) continue;
            
            if (list[idx]->myself == scui_window_active_curr())
                scale_d = scui_map(pct, 50,   0, 0, 100) / 100.0f;
            if (list[idx]->myself != scui_window_active_curr())
                scale_d = scui_map(pct, 50, 100, 0, 100) / 100.0f;
            SCUI_LOG_INFO("pct:%d, scale_d:%f", pct, scale_d);
            
            if (list[idx]->myself == scui_window_active_curr())
                pct = scui_map(pct,   0, 50, 100, 0);
            if (list[idx]->myself != scui_window_active_curr())
                pct = scui_map(pct, 100, 50, 100, 0);
            SCUI_LOG_INFO("pct:%d", pct);
        }
        
        SCUI_LOG_INFO("pct:%d, scale_d:%f", pct, scale_d);
        scui_point_t src_scale = {
            .x = scale_d * SCUI_SCALE_COF,
            .y = scale_d * SCUI_SCALE_COF,
        };
        
        scui_area_t dst_clip_s = dst_clip;
        dst_clip_s.w *= scale_d;
        dst_clip_s.h *= scale_d;
        dst_clip_s.x += (dst_clip.w - dst_clip_s.w) / 2;
        dst_clip_s.y += (dst_clip.h - dst_clip_s.h) / 2;
        
        scui_point_t dst_anchor = scui_area_center(&dst_clip_s);
        scui_point_t src_center = scui_area_center(&src_clip);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_anchor.x -= seg_offset.x;
        dst_anchor.y -= seg_offset.y;
        #endif
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_2d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
            SCUI_COLOR_UNUSED, src_scale, 0, dst_anchor, src_center);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_rotate(scui_widget_t **list, scui_handle_t num)
{
    /* 这是特殊的变换, 只有水平变换, 没有垂直变换 */
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
        scui_window_transform_move(list, num);
        return;
    }
    
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        scui_multi_t angle = 0, angle_total = 75;
        scui_coord_t pct = scui_window_switch_pct();
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ltr))
            angle = + pct * angle_total / 100;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_rtl))
            angle = - pct * angle_total / 100;
        
        if (list[idx]->myself != scui_window_active_curr()) {
            if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ltr)) angle -= angle_total;
            if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_rtl)) angle += angle_total;
        }
        
        if (list[idx]->myself == scui_window_active_curr())
            pct = 100 - pct;
        
        SCUI_LOG_INFO("pct:%d, angle:%f", pct, angle);
        angle *= SCUI_SCALE_COF;
        scui_point_t src_scale = {
            .x = 1.0 * SCUI_SCALE_COF,
            .y = 1.0 * SCUI_SCALE_COF,
        };
        
        scui_point_t dst_anchor = scui_area_center(&dst_clip);
        scui_point_t src_center = scui_area_center(&src_clip);
        dst_anchor.y += dst_clip.h;
        src_center.y += src_clip.h;
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_anchor.x -= seg_offset.x;
        dst_anchor.y -= seg_offset.y;
        #endif
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_2d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
            SCUI_COLOR_UNUSED, src_scale, angle, dst_anchor, src_center);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_rotate_1(scui_widget_t **list, scui_handle_t num)
{
    /* 这是特殊的变换, 只有水平变换, 没有垂直变换 */
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
        scui_window_transform_move(list, num);
        return;
    }
    
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        scui_multi_t angle = 0, pct = scui_window_switch_pct();
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ltr)) angle = +pct * 3.6;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_rtl)) angle = -pct * 3.6;
        
        if (list[idx]->myself == scui_window_active_curr()) if (pct >= 50) continue;
        if (list[idx]->myself != scui_window_active_curr()) if (pct <= 50) continue;
        pct = pct <= 50 ? 100 - pct * 2 : (pct - 50) * 2;
        
        SCUI_LOG_INFO("pct:%d, angle:%f", pct, angle);
        angle *= SCUI_SCALE_COF;
        scui_point_t src_scale = {
            .x = 1.0 * SCUI_SCALE_COF,
            .y = 1.0 * SCUI_SCALE_COF,
        };
        
        scui_point_t dst_anchor = scui_area_center(&dst_clip);
        scui_point_t src_center = scui_area_center(&src_clip);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        dst_anchor.x -= seg_offset.x;
        dst_anchor.y -= seg_offset.y;
        #endif
        
        scui_alpha_t alpha = src_surface->alpha;
        src_surface->alpha = scui_alpha_mix(alpha, scui_alpha_pct(pct));
        scui_draw_area_2d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
            SCUI_COLOR_UNUSED, src_scale, angle, dst_anchor, src_center);
        src_surface->alpha = alpha;
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_circle(scui_widget_t **list, scui_handle_t num)
{
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (list[idx]->myself != scui_window_active_curr()) {
            
            #if SCUI_FRAME_BUFFER_SEG
            if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                 continue;
            #endif
            
            scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
            continue;
        }
        
        scui_area_t clip_seg[16][16] = {0};
        const scui_coord_t circle_w_seg = 16;
        const scui_coord_t circle_h_seg = 16;
        
        scui_coord_t w_num = circle_w_seg;
        scui_coord_t h_num = circle_h_seg;
        scui_coord_t w_res = src_surface->hor_res;
        scui_coord_t h_res = src_surface->ver_res;
        scui_coord_t w_seg = w_res / w_num;
        scui_coord_t h_seg = h_res / h_num;
        scui_coord_t w_mod = w_res % w_num / 2;
        scui_coord_t h_mod = h_res % h_num / 2;
        SCUI_ASSERT(w_res % w_num % 2 == 0);
        SCUI_ASSERT(h_res % h_num % 2 == 0);
        
        scui_coord_t scale_d = scui_window_switch_pct();
        SCUI_LOG_INFO("scale_d:%d", scale_d);
        
        for (scui_multi_t idx_j = 0; idx_j < h_num; idx_j++)
        for (scui_multi_t idx_i = 0; idx_i < w_num; idx_i++) {
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
        
        for (scui_multi_t idx_j = 0; idx_j < h_num; idx_j++)
        for (scui_multi_t idx_i = 0; idx_i < w_num; idx_i++) {
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
                
                #if SCUI_FRAME_BUFFER_SEG
                if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                     continue;
                #endif
                
                scui_alpha_t alpha = src_surface->alpha;
                src_surface->alpha = scui_map(dist_ds, 0, dist_dr, alpha, alpha / 2);
                scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
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
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (list[idx]->myself != scui_window_active_curr()) {
            
            #if SCUI_FRAME_BUFFER_SEG
            if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                 continue;
            #endif
            
            scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
            continue;
        }
        
        scui_area_t clip_seg[4][4] = {0};
        const scui_coord_t grid_w_seg = 4;
        const scui_coord_t grid_h_seg = 4;
        
        scui_coord_t w_num = grid_w_seg;
        scui_coord_t h_num = grid_h_seg;
        scui_coord_t w_res = src_surface->hor_res;
        scui_coord_t h_res = src_surface->ver_res;
        scui_coord_t w_seg = w_res / w_num;
        scui_coord_t h_seg = h_res / h_num;
        scui_coord_t w_mod = w_res % w_num / 2;
        scui_coord_t h_mod = h_res % h_num / 2;
        SCUI_ASSERT(w_res % w_num % 2 == 0);
        SCUI_ASSERT(h_res % h_num % 2 == 0);
        
        scui_coord_t scale_d = scui_window_switch_pct();
        SCUI_LOG_INFO("scale_d:%d", scale_d);
        
        for (scui_multi_t idx_j = 0; idx_j < h_num; idx_j++)
        for (scui_multi_t idx_i = 0; idx_i < w_num; idx_i++) {
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
        
        for (scui_multi_t idx_j = 0; idx_j < h_num; idx_j++)
        for (scui_multi_t idx_i = 0; idx_i < w_num; idx_i++) {
            scui_coord_t scale_w = scui_map(scale_d, 0, 100, 0, clip_seg[idx_j][idx_i].w);
            scui_coord_t scale_h = scui_map(scale_d, 0, 100, 0, clip_seg[idx_j][idx_i].h);
            clip_seg[idx_j][idx_i].x += scale_w / 2 + scale_w % 2;
            clip_seg[idx_j][idx_i].y += scale_h / 2 + scale_h % 2;
            clip_seg[idx_j][idx_i].w -= scale_w;
            clip_seg[idx_j][idx_i].h -= scale_h;
            dst_clip = clip_seg[idx_j][idx_i];
            src_clip = clip_seg[idx_j][idx_i];
            
            #if SCUI_FRAME_BUFFER_SEG
            if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
                 continue;
            #endif
            
            scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip1(scui_widget_t **list, scui_handle_t num)
{
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        scui_face3_t face3 = {0};
        scui_area_t  area  = scui_surface_area(src_surface);
        scui_area3_by_area(&face3, &area);
        
        scui_point2_t offset = {0};
        scui_matrix_t r_matrix = {0};
        scui_matrix_identity(&r_matrix);
        
        scui_coord_t pct = scui_window_switch_pct();
        if (list[idx]->myself == scui_window_active_curr()) pct = 100 - pct;
        if (list[idx]->myself != scui_window_active_curr()) pct = pct;
        float angle = scui_map(pct, 0, 100, 180, 0.0f);
        
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor)) {
            scui_matrix_rotate_a(&r_matrix, angle, 0x02);
            offset.x = -src_surface->hor_res / 2;
        }
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
            scui_matrix_rotate_a(&r_matrix, angle, 0x01);
            offset.y = -src_surface->ver_res / 2;
        }
        /* 小于一半跳过绘制, 因为会被覆盖 */
        SCUI_LOG_INFO("angle:%f", angle);
        if (angle > 90) continue;
        
        /* 移动到轴心, 然后再旋转 */
        scui_point2_t offset1 = {.x = +offset.x, .y = +offset.y,};
        scui_point2_t offset2 = {.x = -offset.x, .y = -offset.y,};
        
        /* scui_matrix_check(&r_matrix); */
        scui_area3_offset_xy(&face3, &offset1);
        scui_area3_transform_by_matrix(&face3, &r_matrix);
        scui_area3_offset_xy(&face3, &offset2);
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = src_clip.w,.h = src_clip.h,};
        scui_matrix_affine_blit(&inv_matrix, &size2, &face3);
        /* scui_matrix_check(&inv_matrix); */
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        scui_point2_t matrix_ofs_i = {0};
        matrix_ofs_i.x = seg_offset.x;
        matrix_ofs_i.y = seg_offset.y;
        scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
        
        src_matrix = inv_matrix;
        scui_matrix_inverse(&src_matrix);
        #endif
        
        scui_draw_area_3d_blend(true, dst_surface, dst_clip,
            src_surface, src_clip, SCUI_COLOR_UNUSED,
            inv_matrix, src_matrix);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flip2(scui_widget_t **list, scui_handle_t num)
{
    /* 底图清空 */
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        scui_area_t area = scui_frame_buffer_area();
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor)) {
            area.x = dst_clip.x > 0 ? area.w / 2 : 0; area.w = area.w / 2;
        }
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
            area.y = dst_clip.y > 0 ? area.h / 2 : 0; area.h = area.h / 2;
        }
        
        dst_clip = area;
        src_clip = area;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
             continue;
        #endif
        
        scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
    }
    
    /* flip2 在 flip1 的基础上扩展生成 */
    scui_window_transform_flip1(list, num);
    
    /* 这里投机取巧,在上面变换完之后,把不显示的那部分还原的给盖住 */
    for (scui_multi_t idx = 0; idx < num; idx++) {
        /* 窗口切换满足全局目标 */
        scui_surface_t *dst_surface = scui_frame_buffer_draw();
        scui_surface_t *src_surface = list[idx]->surface_c;
        SCUI_ASSERT(src_surface->hor_res == SCUI_HOR_RES);
        SCUI_ASSERT(src_surface->ver_res == SCUI_VER_RES);
        scui_area_t dst_clip = scui_frame_buffer_area();
        scui_area_t src_clip = scui_surface_area(src_surface);
        
        if (!scui_window_transform_clip(list[idx], &dst_clip, &src_clip))
             continue;
        
        /* 小于一半跳过绘制, 因为不覆盖它 */
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
            if (src_clip.w < src_surface->hor_res / 2) continue;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
            if (src_clip.h < src_surface->ver_res / 2) continue;
        
        scui_area_t area = scui_frame_buffer_area();
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor)) {
            area.x = dst_clip.x > 0 ? area.w / 2 : 0; area.w = area.w / 2;
        }
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
            area.y = dst_clip.y > 0 ? area.h / 2 : 0; area.h = area.h / 2;
        }
        
        dst_clip = area;
        src_clip = area;
        
        #if SCUI_FRAME_BUFFER_SEG
        if (!scui_frame_buffer_clip_seg(dst_surface, &dst_clip, NULL, &src_clip, NULL))
             continue;
        #endif
        
        scui_draw_area_blend(true, dst_surface, dst_clip, src_surface, src_clip, SCUI_COLOR_UNUSED);
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flap1(scui_widget_t **list, scui_handle_t num)
{
    /* 窗口切换满足全局目标 */
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_frame_buffer_area();
    
    /* active, inactive */
    scui_handle_t  window_a = scui_window_active_curr();
    scui_widget_t *widget_a = list[0]->myself == window_a ? list[0] : list[1];
    scui_widget_t *widget_i = list[0]->myself != window_a ? list[0] : list[1];
    scui_surface_t *src_surface_a = widget_a->surface_c;
    scui_surface_t *src_surface_i = widget_i->surface_c;
    
    /* 空间建模参数 */
    scui_coord3_t angle = scui_map(scui_window_switch_pct(), 0, 100, 0, 180);
    switch (scui_window_switch_dir()) {
    case scui_opt_dir_rtl: angle = -angle; break;
    case scui_opt_dir_ltr: angle = +angle; break;
    case scui_opt_dir_dtu: angle = +angle; break;
    case scui_opt_dir_utd: angle = -angle; break;
    }
    SCUI_LOG_INFO("angle:%f", angle);
    
    scui_coord3_t scale = 1.0f;
    scui_coord3_t s_cen = scui_dist(scui_window_switch_pct(), 50);
    scale = scui_map(s_cen, 0, 50, 0.707f, 1.0f);
    
    const scui_coord3_t flap_scale_w = 0.173;
    
    /* 数字用于调整视角保证边界平稳过度 */
    scui_coord3_t s_res = scale * 0.95;
    scui_coord3_t x_res = s_res * dst_clip.w / 2;
    scui_coord3_t y_res = s_res * dst_clip.h / 2;
    scui_coord3_t z_res = s_res * flap_scale_w;
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
        z_res *= dst_clip.w / 2;
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
        z_res *= dst_clip.h / 2;
    
    /* 3D旋转矩阵 */
    scui_matrix_t r_matrix = {0};
    scui_matrix_identity(&r_matrix);
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
        scui_matrix_rotate_a(&r_matrix, angle, 0x02);
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
        scui_matrix_rotate_a(&r_matrix, angle, 0x01);
    /* scui_matrix_check(&r_matrix); */
    
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
        {.point3 = {vertex3_0, vertex3_3, vertex3_2, vertex3_1,},}, /* 0321 */
        {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},}, /* 4567 */
        {.point3 = {vertex3_5, vertex3_1, vertex3_2, vertex3_6,},}, /* 5126 */
        {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},}, /* 0473 */
        {.point3 = {vertex3_7, vertex3_6, vertex3_2, vertex3_3,},}, /* 7623 */
        {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},}, /* 0154 */
    };
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor)) {
        face3[0] = (scui_face3_t){.point3 = {vertex3_1, vertex3_0, vertex3_3, vertex3_2,},};
    }
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
        face3[0] = (scui_face3_t){.point3 = {vertex3_3, vertex3_2, vertex3_1, vertex3_0,},};
    }
    
    /* 3D空间Z轴序列 */
    scui_coord3_t center_z[6] = {0};
    scui_handle_t face_idx[6] = {0};
    
    /* 3D空间面应用变换, 获得变换后的空间状态 */
    for (scui_multi_t idx = 0; idx < 6; idx++) {
        scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
        scui_area3_offset_xy(&face3[idx], &(scui_point2_t){
            .x = dst_clip.w / 2,
            .y = dst_clip.h / 2,
        });
        scui_area3_center_z(&face3[idx], &center_z[idx]);
        face_idx[idx] = idx;
    }
    
    /* 3D空间Z轴序列排序 */
    for (scui_multi_t idx_i = 0; idx_i < 6; idx_i++)
    for (scui_multi_t idx_j = 0; idx_j < 6; idx_j++)
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
        .x = dst_clip.w / 2,
        .y = dst_clip.h / 2,
        .z = dst_clip.w * 2,
    };
    
    /* 绘制六个面 */
    for (scui_multi_t idx = 0; idx < 6; idx++) {
        scui_area_t dst_clip = scui_frame_buffer_area();
        
        /* 跳过被覆盖的面 */
        if (center_z[idx] < 0.0f)
            continue;
        
        /* 跳过上下俩个平行面(因为它们不显示) */
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
            if (scui_area3_parallel(&face3[idx], 0x02))
                continue;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
            if (scui_area3_parallel(&face3[idx], 0x01))
                continue;
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = dst_clip.w,.h = dst_clip.h,};
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        /* scui_matrix_affine_blit(&inv_matrix, &size2, &face); */
        /* scui_matrix_check(&inv_matrix); */
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        scui_point2_t matrix_ofs_i = {0};
        matrix_ofs_i.x = seg_offset.x;
        matrix_ofs_i.y = seg_offset.y;
        scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
        
        src_matrix = inv_matrix;
        scui_matrix_inverse(&src_matrix);
        #endif
        
        if (face_idx[idx] == 0 || face_idx[idx] == 1) {
            /* 绘制俩个窗口面 */
            scui_surface_t *src_surface = face_idx[idx] == 1 ? src_surface_a : src_surface_i;
            scui_area_t src_clip = scui_surface_area(src_surface);
            scui_draw_area_3d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
                SCUI_COLOR_UNUSED, inv_matrix, src_matrix);
        } else {
            #if 1
            /* 没有窗口面, 则填背景灰色 */
            scui_area_t   src_clip = scui_frame_buffer_area();
            scui_color_t src_color = SCUI_COLOR_MAKE32(false, 0x0, 0xFF1F1F1F);
            scui_draw_area_3d_fill(true, dst_surface, dst_clip, src_clip,
                scui_alpha_cover, src_color, inv_matrix, src_matrix);
            #endif
        }
        
        
        
        #if 1   /* 光影特效 */
        if (scui_window_transform.flap1_shadow != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = scui_window_transform.flap1_shadow;
            scui_image_t *shadow = scui_handle_source_check(handle);
            size2.w = scui_image_w(handle) / 2;
            size2.h = scui_image_h(handle);
            scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
            /* scui_matrix_affine_blit(&inv_matrix, &size2, &face); */
            /* scui_matrix_check(&inv_matrix); */
            src_matrix = inv_matrix;
            scui_matrix_inverse(&inv_matrix);
            
            scui_area_t  src_clip  = {.w = size2.w, .h = size2.h,};
            scui_color_t src_color = SCUI_COLOR_MAKE32(true, 0xFF000000, 0x0);
            src_clip.x = scui_window_switch_pct() * size2.w / 100;
            
            #if SCUI_FRAME_BUFFER_SEG
            scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
            src_matrix = inv_matrix;
            scui_matrix_inverse(&src_matrix);
            #endif
            
            scui_draw_image_3d(true, dst_surface, dst_clip, shadow, src_clip,
                scui_alpha_pct100, src_color, inv_matrix, src_matrix);
        }
        #endif
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_flap2(scui_widget_t **list, scui_handle_t num)
{
    /* 这是特殊的变换, 只有水平变换, 没有垂直变换 */
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver)) {
        scui_window_transform_move(list, num);
        return;
    }
    
    /* 窗口切换满足全局目标 */
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_frame_buffer_area();
    
    /* active, inactive */
    scui_handle_t  window_a = scui_window_active_curr();
    scui_widget_t *widget_a = list[0]->myself == window_a ? list[0] : list[1];
    scui_widget_t *widget_i = list[0]->myself != window_a ? list[0] : list[1];
    scui_surface_t *src_surface_a = widget_a->surface_c;
    scui_surface_t *src_surface_i = widget_i->surface_c;
    
    const scui_coord3_t flap_scale_w = 0.173;
    const scui_coord_t  flap_seg = 6;
    
    scui_coord_t seg_num = flap_seg;
    for (scui_multi_t seg_idx = 0; seg_idx < seg_num; seg_idx++) {
        
        /* 空间建模参数 */
        scui_coord3_t angle = 0.0f, flag = 1.0f;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_rtl)) flag = -flag;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ltr));
        angle  = scui_map(scui_window_switch_pct(), 0, 100, 0, 180 + 90);
        angle -= scui_map(seg_idx, 0, seg_num, 0, 90);
        angle  = scui_clamp(angle, 0, 180);
        angle *= flag;
        SCUI_LOG_INFO("flag:%f, angle:%f", flag, angle);
        
        /* 数字用于调整视角保证边界平稳过度 */
        scui_coord3_t s_res = 1.0;
        scui_coord3_t x_res = s_res * (dst_clip.w) / 2;
        scui_coord3_t y_res = s_res * (dst_clip.h / seg_num) / 2 / s_res;
        scui_coord3_t z_res = s_res * flap_scale_w;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
            z_res *= dst_clip.w / 2;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
            z_res *= dst_clip.h / 2;
        
        /* 3D旋转矩阵 */
        scui_matrix_t r_matrix = {0};
        scui_matrix_identity(&r_matrix);
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
            scui_matrix_rotate_a(&r_matrix, angle, 0x02);
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
            scui_matrix_rotate_a(&r_matrix, angle, 0x01);
        /* scui_matrix_check(&r_matrix); */
        
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
            {.point3 = {vertex3_0, vertex3_3, vertex3_2, vertex3_1,},}, /* 0321 */
            {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},}, /* 4567 */
            {.point3 = {vertex3_5, vertex3_1, vertex3_2, vertex3_6,},}, /* 5126 */
            {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},}, /* 0473 */
            {.point3 = {vertex3_7, vertex3_6, vertex3_2, vertex3_3,},}, /* 7623 */
            {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},}, /* 0154 */
        };
        face3[0] = (scui_face3_t){.point3 = {vertex3_1, vertex3_0, vertex3_3, vertex3_2,},};
        
        /* 3D空间Z轴序列 */
        scui_coord3_t center_z[6] = {0};
        scui_handle_t face_idx[6] = {0};
        
        /* 3D空间面应用变换, 获得变换后的空间状态 */
        for (scui_multi_t idx = 0; idx < 6; idx++) {
            scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
            scui_area3_offset_xy(&face3[idx], &(scui_point2_t){
                .x = dst_clip.w / 2,
                .y = dst_clip.h * seg_idx / seg_num +
                    (dst_clip.h / seg_num) / 2,
            });
            scui_area3_center_z(&face3[idx], &center_z[idx]);
            face_idx[idx] = idx;
        }
        
        /* 3D空间Z轴序列排序 */
        for (scui_multi_t idx_i = 0; idx_i < 6; idx_i++)
        for (scui_multi_t idx_j = 0; idx_j < 6; idx_j++)
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
            .x = dst_clip.w / 2,
            .y = dst_clip.h / 2,
            .z = dst_clip.w * 2,
        };
        
        /* 绘制六个面 */
        for (scui_multi_t idx = 0; idx < 6; idx++) {
            scui_area_t dst_clip = scui_frame_buffer_area();
            
            /* 跳过被覆盖的面 */
            if (center_z[idx] < 0.0f)
                continue;
            
            /* 跳过上下俩个平行面(因为它们不显示) */
            if (scui_area3_parallel(&face3[idx], 0x02))
                continue;
            
            scui_matrix_t inv_matrix = {0};
            scui_size2_t size2 = {.w = dst_clip.w,.h = dst_clip.h / seg_num,};
            scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
            /* scui_matrix_affine_blit(&inv_matrix, &size2, &face); */
            /* scui_matrix_check(&inv_matrix); */
            scui_matrix_t src_matrix = inv_matrix;
            scui_matrix_inverse(&inv_matrix);
            
            #if SCUI_FRAME_BUFFER_SEG
            scui_point_t seg_offset = {0};
            if (!scui_frame_buffer_clip_seg(dst_surface,
                 &dst_clip, NULL, NULL, &seg_offset)) continue;
            
            scui_point2_t matrix_ofs_i = {0};
            matrix_ofs_i.x = seg_offset.x;
            matrix_ofs_i.y = seg_offset.y;
            scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
            
            src_matrix = inv_matrix;
            scui_matrix_inverse(&src_matrix);
            #endif
            
            if (face_idx[idx] == 0 || face_idx[idx] == 1) {
                /* 绘制俩个窗口面 */
                scui_surface_t *src_surface = face_idx[idx] == 1 ? src_surface_a : src_surface_i;
                scui_area_t src_clip = scui_surface_area(src_surface);
                src_clip.h  = src_clip.h / seg_num;
                src_clip.y += src_clip.h * seg_idx;
                scui_draw_area_3d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
                    SCUI_COLOR_UNUSED, inv_matrix, src_matrix);
            } else {
                #if 1
                /* 没有窗口面, 则填背景灰色 */
                scui_area_t src_clip = scui_frame_buffer_area(); src_clip.h /= seg_num;
                scui_color_t src_color = SCUI_COLOR_MAKE32(false, 0x0, 0xFF1F1F1F);
                scui_draw_area_3d_fill(true, dst_surface, dst_clip, src_clip,
                    scui_alpha_cover, src_color, inv_matrix, src_matrix);
                #endif
            }
            
            
            
            #if 1   /* 光影特效 */
            if (scui_window_transform.flap2_shadow != SCUI_HANDLE_INVALID) {
                scui_handle_t handle = scui_window_transform.flap2_shadow;
                scui_image_t *shadow = scui_handle_source_check(handle);
                size2.w = scui_image_w(handle) / 2;
                size2.h = scui_image_h(handle) / seg_num;
                scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
                /* scui_matrix_affine_blit(&inv_matrix, &size2, &face); */
                /* scui_matrix_check(&inv_matrix); */
                src_matrix = inv_matrix;
                scui_matrix_inverse(&inv_matrix);
                
                scui_area_t  src_clip  = {.w = size2.w, .h = size2.h,};
                scui_color_t src_color = SCUI_COLOR_MAKE32(true, 0xFF000000, 0x0);
                src_clip.x = scui_window_switch_pct() * size2.w / 100;
                src_clip.y = size2.h * seg_idx;
                
                #if SCUI_FRAME_BUFFER_SEG
                scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
                src_matrix = inv_matrix;
                scui_matrix_inverse(&src_matrix);
                #endif
                
                scui_draw_image_3d(true, dst_surface, dst_clip, shadow, src_clip,
                    scui_alpha_pct100, src_color, inv_matrix, src_matrix);
            }
            #endif
        }
    }
}

/*@brief 窗口管理器画布混合变换
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_transform_cube(scui_widget_t **list, scui_handle_t num)
{
    /* 窗口切换满足全局目标 */
    scui_surface_t *dst_surface = scui_frame_buffer_draw();
    scui_area_t dst_clip = scui_frame_buffer_area();
    
    /* active, inactive */
    scui_handle_t  window_a = scui_window_active_curr();
    scui_widget_t *widget_a = list[0]->myself == window_a ? list[0] : list[1];
    scui_widget_t *widget_i = list[0]->myself != window_a ? list[0] : list[1];
    scui_surface_t *src_surface_a = widget_a->surface_c;
    scui_surface_t *src_surface_i = widget_i->surface_c;
    
    /* 空间建模参数 */
    scui_coord3_t angle = scui_map(scui_window_switch_pct(), 0, 100, 0, 90);
    switch (scui_window_switch_dir()) {
    case scui_opt_dir_rtl: angle = -angle; break;
    case scui_opt_dir_ltr: angle = +angle; break;
    case scui_opt_dir_dtu: angle = +angle; break;
    case scui_opt_dir_utd: angle = -angle; break;
    }
    SCUI_LOG_INFO("angle:%f", angle);
    
    scui_coord3_t scale = 1.0f;
    scui_coord3_t s_cen = scui_dist(scui_window_switch_pct(), 50);
    scale = scui_map(s_cen, 0, 50, 0.707f, 1.0f);
    
    /* 数字用于调整视角保证边界平稳过度 */
    scui_coord3_t s_res = scale * 0.78;
    scui_coord3_t x_res = s_res * dst_clip.w / 2;
    scui_coord3_t y_res = s_res * dst_clip.h / 2;
    scui_coord3_t z_res = s_res * 1.0f;
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
        z_res *= dst_clip.w / 2;
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
        z_res *= dst_clip.h / 2;
    
    /* 3D旋转矩阵 */
    scui_matrix_t r_matrix = {0};
    scui_matrix_identity(&r_matrix);
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
        scui_matrix_rotate_a(&r_matrix, angle, 0x02);
    if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
        scui_matrix_rotate_a(&r_matrix, angle, 0x01);
    /* scui_matrix_check(&r_matrix); */
    
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
        {.point3 = {vertex3_0, vertex3_3, vertex3_2, vertex3_1,},}, /* 0321 */
        {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},}, /* 4567 */
        {.point3 = {vertex3_5, vertex3_1, vertex3_2, vertex3_6,},}, /* 5126 */
        {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},}, /* 0473 */
        {.point3 = {vertex3_7, vertex3_6, vertex3_2, vertex3_3,},}, /* 7623 */
        {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},}, /* 0154 */
    };
    face3[0] = (scui_face3_t){.point3 = {vertex3_0, vertex3_1, vertex3_2, vertex3_3,},};
    
    /* 3D空间Z轴序列 */
    scui_coord3_t center_z[6] = {0};
    scui_handle_t face_idx[6] = {0};
    
    /* 3D空间面应用变换, 获得变换后的空间状态 */
    for (scui_multi_t idx = 0; idx < 6; idx++) {
        scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
        scui_area3_offset_xy(&face3[idx], &(scui_point2_t){
            .x = dst_clip.w / 2,
            .y = dst_clip.h / 2,
        });
        scui_area3_center_z(&face3[idx], &center_z[idx]);
        face_idx[idx] = idx;
    }
    
    /* 3D空间Z轴序列排序 */
    for (scui_multi_t idx_i = 0; idx_i < 6; idx_i++)
    for (scui_multi_t idx_j = 0; idx_j < 6; idx_j++)
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
        .x = dst_clip.w / 2,
        .y = dst_clip.h / 2,
        .z = dst_clip.w * 2,
    };
    
    /* 绘制六个面 */
    for (scui_multi_t idx = 0; idx < 6; idx++) {
        scui_area_t dst_clip = scui_frame_buffer_area();
        
        /* 跳过被覆盖的面 */
        if (center_z[idx] < 0.0f)
            continue;
        
        /* 跳过上下俩个平行面(因为它们不显示) */
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_hor))
            if (scui_area3_parallel(&face3[idx], 0x02))
                continue;
        if (scui_opt_bits_check(scui_window_switch_dir(), scui_opt_dir_ver))
            if (scui_area3_parallel(&face3[idx], 0x01))
                continue;
        
        bool cover_check = true;
        if (face_idx[idx] == 1 || /* 移动方向不同会导致选择的绘制面不一样 */
           (face_idx[idx] == 2 && scui_window_switch_dir() == scui_opt_dir_rtl) ||
           (face_idx[idx] == 3 && scui_window_switch_dir() == scui_opt_dir_ltr) ||
           (face_idx[idx] == 4 && scui_window_switch_dir() == scui_opt_dir_dtu) ||
           (face_idx[idx] == 5 && scui_window_switch_dir() == scui_opt_dir_utd))
            cover_check = false;
        if (cover_check)
            continue;
        
        scui_matrix_t inv_matrix = {0};
        scui_size2_t size2 = {.w = dst_clip.w,.h = dst_clip.h,};
        scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
        /* scui_matrix_affine_blit(&inv_matrix, &size2, &face3[idx]); */
        /* scui_matrix_check(&inv_matrix); */
        scui_matrix_t src_matrix = inv_matrix;
        scui_matrix_inverse(&inv_matrix);
        
        #if SCUI_FRAME_BUFFER_SEG
        scui_point_t seg_offset = {0};
        if (!scui_frame_buffer_clip_seg(dst_surface,
             &dst_clip, NULL, NULL, &seg_offset)) continue;
        
        scui_point2_t matrix_ofs_i = {0};
        matrix_ofs_i.x = seg_offset.x;
        matrix_ofs_i.y = seg_offset.y;
        scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
        
        src_matrix = inv_matrix;
        scui_matrix_inverse(&src_matrix);
        #endif
        
        /* 绘制俩个窗口面 */
        scui_surface_t *src_surface = face_idx[idx] == 1 ? src_surface_a : src_surface_i;
        scui_area_t src_clip = scui_surface_area(src_surface);
        scui_draw_area_3d_blend(true, dst_surface, dst_clip, src_surface, src_clip,
            SCUI_COLOR_UNUSED, inv_matrix, src_matrix);
        
        
        
        #if 1   /* 光影特效 */
        if (scui_window_transform.cube_shadow != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = scui_window_transform.cube_shadow;
            scui_image_t *shadow = scui_handle_source_check(handle);
            size2.w = scui_image_w(handle) / 2;
            size2.h = scui_image_h(handle);
            scui_matrix_perspective_view_blit(&inv_matrix, &size2, &face3[idx], &view);
            /* scui_matrix_affine_blit(&inv_matrix, &size2, &face); */
            /* scui_matrix_check(&inv_matrix); */
            src_matrix = inv_matrix;
            scui_matrix_inverse(&inv_matrix);
            
            scui_area_t  src_clip  = {.w = size2.w, .h = size2.h,};
            scui_color_t src_color = SCUI_COLOR_MAKE32(true, 0xFF000000, 0x0);
            src_clip.x = scui_window_switch_pct() * size2.w / 100;
            
            #if SCUI_FRAME_BUFFER_SEG
            scui_matrix_translate(&inv_matrix, &matrix_ofs_i);
            src_matrix = inv_matrix;
            scui_matrix_inverse(&src_matrix);
            #endif
            
            scui_draw_image_3d(true, dst_surface, dst_clip, shadow, src_clip,
                scui_alpha_pct100, src_color, inv_matrix, src_matrix);
        }
        #endif
    }
}

/*@brief 窗口切换参数配置
 */
void scui_window_transform_flap1_shadow(scui_handle_t shadow)
{
    scui_window_transform.flap1_shadow = shadow;
}

/*@brief 窗口切换参数配置
 */
void scui_window_transform_flap2_shadow(scui_handle_t shadow)
{
    scui_window_transform.flap2_shadow = shadow;
}

/*@brief 窗口切换参数配置
 */
void scui_window_transform_cube_shadow(scui_handle_t shadow)
{
    scui_window_transform.cube_shadow = shadow;
}
