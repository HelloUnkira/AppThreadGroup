#include "lvgl_view_sched.h"

extern lvgl_view_sched_t lvgl_view_sched;

static void lvgl_view_transform_move(void)
{
    #if LVGL_VIEW_CVS_USE

    #if 0

    // 俩种写法都行, 哪个能加速就用哪个
    if ((lvgl_view_sched.slide_dir & LV_DIR_HOR) != 0)
    {
        // x支持负偏移以得到正确计算
        lv_canvas_transform(lvgl_view_sched.view_cvs, &lvgl_view_sched.view_sps_c, 0,
            LV_IMG_ZOOM_NONE,
            lvgl_view_sched.point_c.x, lvgl_view_sched.point_c.y, 0, 0, true);
        lv_canvas_transform(lvgl_view_sched.view_cvs, &lvgl_view_sched.view_sps_t, 0,
            LV_IMG_ZOOM_NONE,
            lvgl_view_sched.point_t.x, lvgl_view_sched.point_t.y, 0, 0, true);
    }
    else
    {
        // y不支持负偏移图像错位
        lv_img_dsc_t view_sps_c = lvgl_view_sched.view_sps_c;
        lv_img_dsc_t view_sps_t = lvgl_view_sched.view_sps_t;
        lv_coord_t point_c_y = lvgl_view_sched.point_c.y;
        lv_coord_t point_t_y = lvgl_view_sched.point_t.y;
        
        if (point_c_y < 0)
        {
            int32_t skip_line_px = (int32_t)LV_ABS(point_c_y) * view_sps_c.header.w;
            int32_t skip_line_bytes = skip_line_px * lv_img_cf_get_px_size(
                    view_sps_c.header.cf);
            view_sps_c.header.h -= (int32_t)LV_ABS(point_c_y);
            view_sps_c.data -= skip_line_bytes;
            view_sps_c.data_size -= skip_line_bytes;
            point_c_y = 0;
        }
        
        if (point_t_y < 0)
        {
            int32_t skip_line_px = (int32_t)LV_ABS(point_t_y) * view_sps_t.header.w;
            int32_t skip_line_bytes = skip_line_px * lv_img_cf_get_px_size(
                    view_sps_t.header.cf);
            view_sps_t.header.h -= (int32_t)LV_ABS(point_t_y);
            view_sps_t.data -= skip_line_bytes;
            view_sps_t.data_size -= skip_line_bytes;
            point_t_y = 0;
        }
        
        // 淦~! 这玩意有问题, 不懂
        lv_canvas_transform(lvgl_view_sched.view_cvs, &view_sps_c, 0, LV_IMG_ZOOM_NONE,
            lvgl_view_sched.point_c.x, point_c_y, 0, 0, true);
        lv_canvas_transform(lvgl_view_sched.view_cvs, &view_sps_t, 0, LV_IMG_ZOOM_NONE,
            lvgl_view_sched.point_t.x, point_t_y, 0, 0, true);
    }
    
    #else
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, lvgl_view_sched.point_c.x,
        lvgl_view_sched.point_c.y,
        &lvgl_view_sched.view_sps_c, &draw_dsc_c);
    
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, lvgl_view_sched.point_t.x,
        lvgl_view_sched.point_t.y,
        &lvgl_view_sched.view_sps_t, &draw_dsc_t);
    #endif
    
    #else
    lv_obj_set_pos(lvgl_view_sched.view_c, lvgl_view_sched.point_c.x,
        lvgl_view_sched.point_c.y);
    lv_obj_set_pos(lvgl_view_sched.view_t, lvgl_view_sched.point_t.x,
        lvgl_view_sched.point_t.y);
    #endif
    
}

static void lvgl_view_transform_cover(void)
{
    /* 该部分效果相对于,浮动窗口进入时的样子 */
    if (lvgl_view_sched.transform_type == lvgl_view_tr_cover_in)
    {
    
        #if LVGL_VIEW_CVS_USE
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        draw_dsc_c.opa = (uint32_t)(100 - lvgl_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
            &lvgl_view_sched.view_sps_c, &draw_dsc_c);
            
        lv_draw_img_dsc_t draw_dsc_t = {0};
        lv_draw_img_dsc_init(&draw_dsc_t);
        draw_dsc_t.opa = (uint32_t)(lvgl_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(lvgl_view_sched.view_cvs, lvgl_view_sched.point_t.x,
            lvgl_view_sched.point_t.y,
            &lvgl_view_sched.view_sps_t, &draw_dsc_t);
        #else
        lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
        lv_obj_set_pos(lvgl_view_sched.view_t, lvgl_view_sched.point_t.x,
            lvgl_view_sched.point_t.y);
            
        lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_0, LV_OPA_100);
        lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
        lv_obj_move_foreground(lvgl_view_sched.view_t);
        #endif
    }
    
    /* 该部分效果相对于,浮动窗口退出时的样子 */
    if (lvgl_view_sched.transform_type == lvgl_view_tr_cover_out)
    {
    
        #if LVGL_VIEW_CVS_USE
        lv_draw_img_dsc_t draw_dsc_t = {0};
        lv_draw_img_dsc_init(&draw_dsc_t);
        draw_dsc_t.opa = (uint32_t)(lvgl_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
            &lvgl_view_sched.view_sps_t, &draw_dsc_t);
            
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        draw_dsc_c.opa = (uint32_t)(100 - lvgl_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(lvgl_view_sched.view_cvs, lvgl_view_sched.point_c.x,
            lvgl_view_sched.point_c.y,
            &lvgl_view_sched.view_sps_c, &draw_dsc_c);
        #else
        lv_obj_set_pos(lvgl_view_sched.view_c, lvgl_view_sched.point_c.x,
            lvgl_view_sched.point_c.y);
        lv_obj_set_pos(lvgl_view_sched.view_t, 0, 0);
            
        lv_opa_t opa_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_0);
        lv_obj_set_style_opa(lvgl_view_sched.view_c, opa_c, 0);
        lv_obj_move_foreground(lvgl_view_sched.view_c);
        #endif
    }
}

static void lvgl_view_transform_zoom(void)
{
    #if LVGL_VIEW_CVS_USE
    lv_coord_t zoom_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE,
            LV_IMG_ZOOM_NONE / 2);
    lv_coord_t zoom_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2,
            LV_IMG_ZOOM_NONE);
            
    // 缩放旋转效果不能支持by:lv_canvas_transform
    if ((lvgl_view_sched.slide_dir & LV_DIR_VER) != 0)
    {
        lvgl_view_transform_move();
        return;
    }
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_zoom1)
    {
    
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        lv_canvas_draw_img(lvgl_view_sched.view_cvs, lvgl_view_sched.point_c.x,
            lvgl_view_sched.point_c.y,
            &lvgl_view_sched.view_sps_c, &draw_dsc_c);
            
        lv_canvas_transform(lvgl_view_sched.view_cvs, &lvgl_view_sched.view_sps_t, 0,
            zoom_t,
            lvgl_view_sched.point_t.x, lvgl_view_sched.point_t.y,
            lvgl_view_sched.view_sps_t.header.w / 2,
            lvgl_view_sched.view_sps_t.header.h / 2, true);
    }
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_zoom2)
    {
    
        lv_canvas_transform(lvgl_view_sched.view_cvs, &lvgl_view_sched.view_sps_c, 0,
            zoom_c,
            lvgl_view_sched.point_c.x, lvgl_view_sched.point_c.y,
            lvgl_view_sched.view_sps_c.header.w / 2,
            lvgl_view_sched.view_sps_c.header.h / 2, true);
            
        lv_canvas_transform(lvgl_view_sched.view_cvs, &lvgl_view_sched.view_sps_t, 0,
            zoom_t,
            lvgl_view_sched.point_t.x, lvgl_view_sched.point_t.y,
            lvgl_view_sched.view_sps_t.header.w / 2,
            lvgl_view_sched.view_sps_t.header.h / 2, true);
    }
    
    #else
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_zoom1)
    {
        lv_obj_set_pos(lvgl_view_sched.view_c, lvgl_view_sched.point_c.x,
            lvgl_view_sched.point_c.y);
        lv_obj_set_pos(lvgl_view_sched.view_t, lvgl_view_sched.point_t.x,
            lvgl_view_sched.point_t.y);
    
        lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_root);
        lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_root);
    
        lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
        lv_coord_t zoom_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2,
                LV_IMG_ZOOM_NONE);
        lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(lvgl_view_sched.view_t, zoom_t, 0);
        lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
    }
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_zoom2)
    {
        lv_obj_set_pos(lvgl_view_sched.view_c, lvgl_view_sched.point_c.x,
            lvgl_view_sched.point_c.y);
        lv_obj_set_pos(lvgl_view_sched.view_t, lvgl_view_sched.point_t.x,
            lvgl_view_sched.point_t.y);
    
        lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_root);
        lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_root);
    
        lv_opa_t opa_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_50);
        lv_coord_t zoom_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE,
                LV_IMG_ZOOM_NONE / 2);
        lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_c, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_c, h / 2, 0);
        lv_obj_set_style_transform_zoom(lvgl_view_sched.view_c, zoom_c, 0);
        lv_obj_set_style_opa(lvgl_view_sched.view_c, opa_c, 0);
    
        lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
        lv_coord_t zoom_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2,
                LV_IMG_ZOOM_NONE);
        lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(lvgl_view_sched.view_t, zoom_t, 0);
        lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
    }
    
    #endif
    
}

static void lvgl_view_transform_center(void)
{
    #if LVGL_VIEW_CVS_USE
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    draw_dsc_t.opa = (uint32_t)(lvgl_view_sched.pct) * 255 / 100;
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
        &lvgl_view_sched.view_sps_t, &draw_dsc_t);
        
    lv_coord_t zoom_c = LV_IMG_ZOOM_NONE;
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_center_in)
    {
        zoom_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE,
                LV_IMG_ZOOM_NONE / 8);
    }
    
    if (lvgl_view_sched.transform_type == lvgl_view_tr_center_out)
    {
        zoom_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE,
                LV_IMG_ZOOM_NONE * 2);
    }
    
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    draw_dsc_c.zoom = zoom_c;
    draw_dsc_c.pivot.x = lvgl_view_sched.view_sps_c.header.w / 2;
    draw_dsc_c.pivot.y = lvgl_view_sched.view_sps_c.header.h / 2;
    draw_dsc_c.opa = (uint32_t)(100 - lvgl_view_sched.pct) * 255 / 100;
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
        &lvgl_view_sched.view_sps_c, &draw_dsc_c);
    #else
        
    if (lvgl_view_sched.transform_type == lvgl_view_tr_center_in ||
        lvgl_view_sched.transform_type == lvgl_view_tr_center_out)
    {
        lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
        lv_obj_set_pos(lvgl_view_sched.view_t, 0, 0);
        
        lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_root);
        lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_root);
        
        lv_coord_t zoom_t = LV_IMG_ZOOM_NONE;
        
        if (lvgl_view_sched.transform_type == lvgl_view_tr_center_in)
        {
            zoom_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE * 2,
                    LV_IMG_ZOOM_NONE);
        }
        
        if (lvgl_view_sched.transform_type == lvgl_view_tr_center_out)
        {
            zoom_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 8,
                    LV_IMG_ZOOM_NONE);
        }
        
        // lv_opa_t opa_c = lv_map(lvgl_view_sched.pct, 100, 0, LV_OPA_100, LV_OPA_0);
        // lv_obj_set_style_opa(view_t, opa_c, 0);
        
        lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_0, LV_OPA_100);
        lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(lvgl_view_sched.view_t, zoom_t, 0);
        lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
        lv_obj_move_foreground(lvgl_view_sched.view_t);
    }
        
    if (lvgl_view_sched.transform_type == lvgl_view_tr_center_inout)
    {
        lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
        lv_obj_set_pos(lvgl_view_sched.view_t, 0, 0);
        
        if (lvgl_view_sched.pct < 50)
        {
        
            lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_root);
            lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_root);
        
            lv_opa_t opa_c = lv_map(lvgl_view_sched.pct, 0, 50, LV_OPA_100, LV_OPA_0);
            lv_coord_t zoom_c = lv_map(lvgl_view_sched.pct, 0, 50, LV_IMG_ZOOM_NONE,
                    LV_IMG_ZOOM_NONE / 5);
            lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_c, w / 2, 0);
            lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_c, h / 2, 0);
            lv_obj_set_style_transform_zoom(lvgl_view_sched.view_c, zoom_c, 0);
            lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_c, 0);
            lv_obj_move_foreground(lvgl_view_sched.view_c);
        
            lv_obj_clear_flag(lvgl_view_sched.view_c, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lvgl_view_sched.view_t, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
        
            lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_root);
            lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_root);
        
            lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 50, 100, LV_OPA_0, LV_OPA_100);
            lv_coord_t zoom_t = lv_map(lvgl_view_sched.pct, 50, 100, LV_IMG_ZOOM_NONE / 5,
                    LV_IMG_ZOOM_NONE);
            lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_t, w / 2, 0);
            lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_t, h / 2, 0);
            lv_obj_set_style_transform_zoom(lvgl_view_sched.view_t, zoom_t, 0);
            lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
            lv_obj_move_foreground(lvgl_view_sched.view_t);
        
            lv_obj_clear_flag(lvgl_view_sched.view_t, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lvgl_view_sched.view_c, LV_OBJ_FLAG_HIDDEN);
        }
    }
        
    #endif
}

static void lvgl_view_transform_rotate(void)
{
    if ((lvgl_view_sched.slide_dir & LV_DIR_VER) != 0)
    {
        lvgl_view_transform_move();
        return;
    }
    
    lv_coord_t angle_total = 75, angle = 0, pct = lvgl_view_sched.pct;
    
    if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
    {
        angle = + pct * angle_total / 100;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
    {
        angle = - pct * angle_total / 100;
    }
    
    lv_coord_t angle_c = angle;
    lv_coord_t angle_t = angle;
    
    if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
    {
        angle_t -= angle_total;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
    {
        angle_t += angle_total;
    }
    
    LV_LOG_INFO("angle_c:%d", angle_c);
    LV_LOG_INFO("angle_c:%d", angle_t);
    
    #if LVGL_VIEW_CVS_USE
    int32_t radius  = LV_VER_RES;
    int32_t point_x = radius * lv_trigo_cos((int32_t)angle) / 32768;
    int32_t point_y = radius * lv_trigo_sin((int32_t)angle) / 32768;
    point_x = LV_HOR_RES / 2     + point_x;
    point_y = LV_VER_RES * 3 / 2 + point_y;
    #else
    int32_t radius  = LV_VER_RES;
    int32_t point_x = lv_obj_get_width(lvgl_view_sched.view_root) / 2;
    int32_t point_y = lv_obj_get_height(lvgl_view_sched.view_root) / 2 + radius;
    #endif
    
    #if LVGL_VIEW_CVS_USE
    // 缩放旋转效果不能支持by:lv_canvas_transform
    
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    draw_dsc_c.angle = angle_c * 10;
    draw_dsc_c.pivot.x = point_x; // lvgl_view_sched.view_sps_c.header.w / 2;
    draw_dsc_c.pivot.y = point_y; // lvgl_view_sched.view_sps_c.header.h / 2;
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
        &lvgl_view_sched.view_sps_c, &draw_dsc_c);
        
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    draw_dsc_t.angle = angle_t * 10;
    draw_dsc_t.pivot.x = point_x; // lvgl_view_sched.view_sps_c.header.w / 2;
    draw_dsc_t.pivot.y = point_y; // lvgl_view_sched.view_sps_c.header.h / 2;
    lv_canvas_draw_img(lvgl_view_sched.view_cvs, 0, 0,
        &lvgl_view_sched.view_sps_t, &draw_dsc_t);
    #else
    lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
    lv_obj_set_pos(lvgl_view_sched.view_t, 0, 0);
        
    lv_opa_t opa_c = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_50);
    lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_c, point_x, 0);
    lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_c, point_y, 0);
    lv_obj_set_style_transform_angle(lvgl_view_sched.view_c, angle_c * 10, 0);
    // lv_obj_set_style_opa(lvgl_view_sched.view_c, opa_c, 0);
        
    lv_opa_t opa_t = lv_map(lvgl_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
    lv_obj_set_style_transform_pivot_x(lvgl_view_sched.view_t, point_x, 0);
    lv_obj_set_style_transform_pivot_y(lvgl_view_sched.view_t, point_y, 0);
    lv_obj_set_style_transform_angle(lvgl_view_sched.view_t, angle_t * 10, 0);
    // lv_obj_set_style_opa(lvgl_view_sched.view_t, opa_t, 0);
    #endif
}

void lvgl_view_transform_reset(void)
{
    #if LVGL_VIEW_CVS_USE == 0
    lv_obj_set_style_opa(lvgl_view_sched.view_c, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(lvgl_view_sched.view_t, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_angle(lvgl_view_sched.view_c, 0, 0);
    lv_obj_set_style_transform_angle(lvgl_view_sched.view_t, 0, 0);
    lv_obj_set_style_transform_zoom(lvgl_view_sched.view_c, LV_IMG_ZOOM_NONE, 0);
    lv_obj_set_style_transform_zoom(lvgl_view_sched.view_t, LV_IMG_ZOOM_NONE, 0);
    #endif
}

/*该接口完成以下目标
 *给定现在滑动的状态比例
 *将俩个画布图形变换后
 *渲染到一个新画布上
 */
void lvgl_view_transform_async_cb(void *user_data)
{
    // 异步只需要发生在动画持续时
    if (!lvgl_view_sched.anim_lock)
    {
        return;
    }
    
    // LV_LOG_WARN("\nview_c:<%d, %d>  ", lvgl_view_sched.point_c.x, lvgl_view_sched.point_c.y);
    // LV_LOG_WARN("\nview_t:<%d, %d>\n", lvgl_view_sched.point_t.x, lvgl_view_sched.point_t.y);
    
    // 底图填成黑色底面
    #if LVGL_VIEW_CVS_USE
    lv_draw_rect_dsc_t draw_rect = {0};
    lv_draw_rect_dsc_init(&draw_rect);
    draw_rect.bg_opa = LV_OPA_COVER;
    draw_rect.bg_color = lv_color_black();
    // 其他部分都是空的, 只要黑色底面
    lv_coord_t w = lv_obj_get_width(lvgl_view_sched.view_cvs);
    lv_coord_t h = lv_obj_get_height(lvgl_view_sched.view_cvs);
    lv_canvas_draw_rect(lvgl_view_sched.view_cvs, 0, 0, w, h, &draw_rect);
    #endif
    
    #if 0
    // 仅仅测试固定界面所用, 长久屏蔽
    lvgl_view_sched.transform_type = lv_rand(1, 100) % lvgl_view_tr_num;
    lvgl_view_sched.transform_type = lvgl_view_tr_rotate;
    #endif
    
    switch (lvgl_view_sched.transform_type)
    {
    default:
    case lvgl_view_tr_move:
        lvgl_view_transform_move();
        break;
        
    case lvgl_view_tr_cover_in:
    case lvgl_view_tr_cover_out:
        lvgl_view_transform_cover();
        break;
        
    case lvgl_view_tr_zoom1:
    case lvgl_view_tr_zoom2:
        lvgl_view_transform_zoom();
        break;
        
    case lvgl_view_tr_center_in:
    case lvgl_view_tr_center_out:
    case lvgl_view_tr_center_inout:
        lvgl_view_transform_center();
        break;
        
    case lvgl_view_tr_rotate:
        lvgl_view_transform_rotate();
        break;
    }
    
    //
    lv_obj_invalidate(lvgl_view_sched.view_root);
    lv_disp_trig_activity(NULL);
}
