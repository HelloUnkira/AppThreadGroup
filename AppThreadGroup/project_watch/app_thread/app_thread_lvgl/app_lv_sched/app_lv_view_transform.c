#include "app_lv_view_sched.h"

extern app_lv_view_sched_t app_lv_view_sched;

static void app_lv_view_transform_move(void)
{
    lv_obj_t *view_c = app_lv_view_sched.view_c;
    lv_obj_t *view_t = app_lv_view_sched.view_t;
    
    #if 0
    if ((app_lv_view_sched.slide_dir & LV_DIR_HOR) != 0) {
        // x支持负偏移以得到正确计算
        lv_canvas_transform(app_lv_view_sched.canvas, &app_lv_view_sched.snapshot_c, 0, LV_IMG_ZOOM_NONE,
            app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y, 0, 0, true);
        lv_canvas_transform(app_lv_view_sched.canvas, &app_lv_view_sched.snapshot_t, 0, LV_IMG_ZOOM_NONE,
            app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y, 0, 0, true);
    } else {
        // y不支持负偏移图像错位
        lv_img_dsc_t snapshot_c = app_lv_view_sched.snapshot_c;
        lv_img_dsc_t snapshot_t = app_lv_view_sched.snapshot_t;
        lv_coord_t point_c_y = app_lv_view_sched.point_c.y;
        lv_coord_t point_t_y = app_lv_view_sched.point_t.y;
        
        if (point_c_y < 0) {
            int32_t skip_line_px = (int32_t)LV_ABS(point_c_y) * snapshot_c.header.w;
            int32_t skip_line_bytes = skip_line_px * lv_img_cf_get_px_size(snapshot_c.header.cf);
            snapshot_c.header.h -= (int32_t)LV_ABS(point_c_y);
            snapshot_c.data -= skip_line_bytes;
            snapshot_c.data_size -= skip_line_bytes;
            point_c_y = 0;
        }
        if (point_t_y < 0) {
            int32_t skip_line_px = (int32_t)LV_ABS(point_t_y) * snapshot_t.header.w;
            int32_t skip_line_bytes = skip_line_px * lv_img_cf_get_px_size(snapshot_t.header.cf);
            snapshot_t.header.h -= (int32_t)LV_ABS(point_t_y);
            snapshot_t.data -= skip_line_bytes;
            snapshot_t.data_size -= skip_line_bytes;
            point_t_y = 0;
        }
        
        // 淦~! 这玩意有问题, 不懂
        lv_canvas_transform(app_lv_view_sched.canvas, &snapshot_c, 0, LV_IMG_ZOOM_NONE,
            app_lv_view_sched.point_c.x, point_c_y, 0, 0, true);
        lv_canvas_transform(app_lv_view_sched.canvas, &snapshot_t, 0, LV_IMG_ZOOM_NONE,
            app_lv_view_sched.point_t.x, point_t_y, 0, 0, true);
    }
    #endif
    
    #if 0
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    lv_canvas_draw_img(app_lv_view_sched.canvas, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y,
        &app_lv_view_sched.snapshot_c, &draw_dsc_c);
    
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    lv_canvas_draw_img(app_lv_view_sched.canvas, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y,
        &app_lv_view_sched.snapshot_t, &draw_dsc_t);
    #else
    lv_obj_set_pos(view_c, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y);
    lv_obj_set_pos(view_t, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y);
    #endif
}

static void app_lv_view_transform_cover(void)
{
    lv_obj_t *view_c = app_lv_view_sched.view_c;
    lv_obj_t *view_t = app_lv_view_sched.view_t;
    
    /* 该部分效果相对于,浮动窗口进入时的样子 */
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_cover_in) {
        
        #if 0
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        draw_dsc_c.opa = (uint32_t)(100 - app_lv_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
            &app_lv_view_sched.snapshot_c, &draw_dsc_c);
        
        lv_draw_img_dsc_t draw_dsc_t = {0};
        lv_draw_img_dsc_init(&draw_dsc_t);
        draw_dsc_t.opa = (uint32_t)(app_lv_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(app_lv_view_sched.canvas, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y,
            &app_lv_view_sched.snapshot_t, &draw_dsc_t);
        #else
        lv_obj_set_pos(view_c, 0, 0);
        lv_obj_set_pos(view_t, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y);
        
        lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_0, LV_OPA_100);
        lv_obj_set_style_opa(view_t, opa_t, 0);
        lv_obj_move_foreground(view_t);
        #endif
    }
    
    /* 该部分效果相对于,浮动窗口退出时的样子 */
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_cover_out) {
        
        #if 0
        lv_draw_img_dsc_t draw_dsc_t = {0};
        lv_draw_img_dsc_init(&draw_dsc_t);
        draw_dsc_t.opa = (uint32_t)(app_lv_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
            &app_lv_view_sched.snapshot_t, &draw_dsc_t);
        
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        draw_dsc_c.opa = (uint32_t)(100 - app_lv_view_sched.pct) * 255 / 100;
        lv_canvas_draw_img(app_lv_view_sched.canvas, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y,
            &app_lv_view_sched.snapshot_c, &draw_dsc_c);
        #else
        lv_obj_set_pos(view_c, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y);
        lv_obj_set_pos(view_t, 0, 0);
        
        lv_opa_t opa_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_0);
        lv_obj_set_style_opa(view_c, opa_c, 0);
        lv_obj_move_foreground(view_c);
        #endif
    }
}

static void app_lv_view_transform_zoom(void)
{
    lv_obj_t *view_c = app_lv_view_sched.view_c;
    lv_obj_t *view_t = app_lv_view_sched.view_t;
    
    #if 0
    lv_coord_t zoom_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 2);
    lv_coord_t zoom_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2, LV_IMG_ZOOM_NONE);
    
    // 缩放旋转效果不能支持by:lv_canvas_transform
    if ((app_lv_view_sched.slide_dir & LV_DIR_VER) != 0) {
        app_lv_view_transform_move();
        return;
    }
    
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_zoom1) {
        
        lv_draw_img_dsc_t draw_dsc_c = {0};
        lv_draw_img_dsc_init(&draw_dsc_c);
        lv_canvas_draw_img(app_lv_view_sched.canvas, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y,
            &app_lv_view_sched.snapshot_c, &draw_dsc_c);
        
        lv_canvas_transform(app_lv_view_sched.canvas, &app_lv_view_sched.snapshot_t, 0, zoom_t,
            app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y,
            app_lv_view_sched.snapshot_t.header.w / 2, app_lv_view_sched.snapshot_t.header.h / 2, true);
    }
    
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_zoom2) {
        
        lv_canvas_transform(app_lv_view_sched.canvas, &app_lv_view_sched.snapshot_c, 0, zoom_c,
            app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y,
            app_lv_view_sched.snapshot_c.header.w / 2, app_lv_view_sched.snapshot_c.header.h / 2, true);
        
        lv_canvas_transform(app_lv_view_sched.canvas, &app_lv_view_sched.snapshot_t, 0, zoom_t,
            app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y,
            app_lv_view_sched.snapshot_t.header.w / 2, app_lv_view_sched.snapshot_t.header.h / 2, true);
    }
    #else
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_zoom1) {
        lv_obj_set_pos(view_c, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y);
        lv_obj_set_pos(view_t, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y);
        
        lv_coord_t w = lv_obj_get_width(app_lv_view_sched.slide_effect);
        lv_coord_t h = lv_obj_get_height(app_lv_view_sched.slide_effect);
        
        lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
        lv_coord_t zoom_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2, LV_IMG_ZOOM_NONE);
        lv_obj_set_style_transform_pivot_x(view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(view_t, zoom_t, 0);
        lv_obj_set_style_opa(view_t, opa_t, 0);
    }
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_zoom2) {
        lv_obj_set_pos(view_c, app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y);
        lv_obj_set_pos(view_t, app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y);
        
        lv_coord_t w = lv_obj_get_width(app_lv_view_sched.slide_effect);
        lv_coord_t h = lv_obj_get_height(app_lv_view_sched.slide_effect);
        
        lv_opa_t opa_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_50);
        lv_coord_t zoom_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 2);
        lv_obj_set_style_transform_pivot_x(view_c, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(view_c, h / 2, 0);
        lv_obj_set_style_transform_zoom(view_c, zoom_c, 0);
        lv_obj_set_style_opa(view_c, opa_c, 0);
        
        lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
        lv_coord_t zoom_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 2, LV_IMG_ZOOM_NONE);
        lv_obj_set_style_transform_pivot_x(view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(view_t, zoom_t, 0);
        lv_obj_set_style_opa(view_t, opa_t, 0);
    }
    #endif
    
}

static void app_lv_view_transform_center(void)
{
    lv_obj_t *view_c = app_lv_view_sched.view_c;
    lv_obj_t *view_t = app_lv_view_sched.view_t;
    
    #if 0
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    draw_dsc_t.opa = (uint32_t)(app_lv_view_sched.pct) * 255 / 100;
    lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
        &app_lv_view_sched.snapshot_t, &draw_dsc_t);
    
    lv_coord_t zoom_c = LV_IMG_ZOOM_NONE;
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_in)
        zoom_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 8);
    
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_out)
        zoom_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 2);
    
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    draw_dsc_c.zoom = zoom_c;
    draw_dsc_c.pivot.x = app_lv_view_sched.snapshot_c.header.w / 2;
    draw_dsc_c.pivot.y = app_lv_view_sched.snapshot_c.header.h / 2;
    draw_dsc_c.opa = (uint32_t)(100 - app_lv_view_sched.pct) * 255 / 100;
    lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
        &app_lv_view_sched.snapshot_c, &draw_dsc_c);
    #else
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_in ||
        app_lv_view_sched.transform_type == app_lv_view_transform_type_center_out) {
        lv_obj_set_pos(view_c, 0, 0);
        lv_obj_set_pos(view_t, 0, 0);
        
        lv_coord_t w = lv_obj_get_width(app_lv_view_sched.slide_effect);
        lv_coord_t h = lv_obj_get_height(app_lv_view_sched.slide_effect);
        
        lv_coord_t zoom_t = LV_IMG_ZOOM_NONE;
        if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_in)
            zoom_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE * 2, LV_IMG_ZOOM_NONE);
        if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_out)
            zoom_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_IMG_ZOOM_NONE / 8, LV_IMG_ZOOM_NONE);
        
        // lv_opa_t opa_c = lv_map(app_lv_view_sched.pct, 100, 0, LV_OPA_100, LV_OPA_0);
        // lv_obj_set_style_opa(view_t, opa_c, 0);
        
        lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_0, LV_OPA_100);
        lv_obj_set_style_transform_pivot_x(view_t, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(view_t, h / 2, 0);
        lv_obj_set_style_transform_zoom(view_t, zoom_t, 0);
        lv_obj_set_style_opa(view_t, opa_t, 0);
        lv_obj_move_foreground(view_t);
    }
    if (app_lv_view_sched.transform_type == app_lv_view_transform_type_center_inout) {
        lv_obj_set_pos(view_c, 0, 0);
        lv_obj_set_pos(view_t, 0, 0);
        
        if (app_lv_view_sched.pct < 50) {
            
            lv_coord_t w = lv_obj_get_width(app_lv_view_sched.slide_effect);
            lv_coord_t h = lv_obj_get_height(app_lv_view_sched.slide_effect);
            
            lv_opa_t opa_c = lv_map(app_lv_view_sched.pct, 0, 50, LV_OPA_100, LV_OPA_0);
            lv_coord_t zoom_c = lv_map(app_lv_view_sched.pct, 0, 50, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 5);
            lv_obj_set_style_transform_pivot_x(view_c, w / 2, 0);
            lv_obj_set_style_transform_pivot_y(view_c, h / 2, 0);
            lv_obj_set_style_transform_zoom(view_c, zoom_c, 0);
            lv_obj_set_style_opa(view_t, opa_c, 0);
            lv_obj_move_foreground(view_c);
            
            lv_obj_clear_flag(view_c, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(view_t, LV_OBJ_FLAG_HIDDEN);
        } else {
            
            lv_coord_t w = lv_obj_get_width(app_lv_view_sched.slide_effect);
            lv_coord_t h = lv_obj_get_height(app_lv_view_sched.slide_effect);
            
            lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 50, 100, LV_OPA_0, LV_OPA_100);
            lv_coord_t zoom_t = lv_map(app_lv_view_sched.pct, 50, 100, LV_IMG_ZOOM_NONE / 5, LV_IMG_ZOOM_NONE);
            lv_obj_set_style_transform_pivot_x(view_t, w / 2, 0);
            lv_obj_set_style_transform_pivot_y(view_t, h / 2, 0);
            lv_obj_set_style_transform_zoom(view_t, zoom_t, 0);
            lv_obj_set_style_opa(view_t, opa_t, 0);
            lv_obj_move_foreground(view_t);
            
            lv_obj_clear_flag(view_t, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(view_c, LV_OBJ_FLAG_HIDDEN);
        }
    }
    #endif
}

static void app_lv_view_transform_rotate(void)
{
    lv_obj_t *view_c = app_lv_view_sched.view_c;
    lv_obj_t *view_t = app_lv_view_sched.view_t;
    
    if ((app_lv_view_sched.slide_dir & LV_DIR_VER) != 0) {
        app_lv_view_transform_move();
        return;
    }
    
    lv_coord_t angle_total = 75, angle = 0, pct = app_lv_view_sched.pct;
    if (app_lv_view_sched.slide_dir == LV_DIR_RIGHT)
        angle = + pct * angle_total / 100;
    if (app_lv_view_sched.slide_dir == LV_DIR_LEFT)
        angle = - pct * angle_total / 100;
    
    lv_coord_t angle_c = angle;
    lv_coord_t angle_t = angle;
    if (app_lv_view_sched.slide_dir == LV_DIR_RIGHT) angle_t -= angle_total;
    if (app_lv_view_sched.slide_dir == LV_DIR_LEFT)  angle_t += angle_total;
    LV_LOG_INFO("angle_c:%d", angle_c);
    LV_LOG_INFO("angle_c:%d", angle_t);
    
    #if 0
    int32_t radius  = LV_VER_RES;
    int32_t point_x = radius * lv_trigo_cos((int32_t)angle) / 32768;
    int32_t point_y = radius * lv_trigo_sin((int32_t)angle) / 32768;
    point_x = LV_HOR_RES / 2     + point_x;
    point_y = LV_VER_RES * 3 / 2 + point_y;
    #else
    int32_t radius  = LV_VER_RES;
    int32_t point_x = lv_obj_get_width(app_lv_view_sched.slide_effect) / 2;
    int32_t point_y = lv_obj_get_height(app_lv_view_sched.slide_effect) / 2 + radius;
    #endif
    
    #if 0
    // 缩放旋转效果不能支持by:lv_canvas_transform
    
    lv_draw_img_dsc_t draw_dsc_c = {0};
    lv_draw_img_dsc_init(&draw_dsc_c);
    draw_dsc_c.angle = angle_c * 10;
    draw_dsc_c.pivot.x = point_x; // app_lv_view_sched.snapshot_c.header.w / 2;
    draw_dsc_c.pivot.y = point_y; // app_lv_view_sched.snapshot_c.header.h / 2;
    lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
        &app_lv_view_sched.snapshot_c, &draw_dsc_c);
    
    lv_draw_img_dsc_t draw_dsc_t = {0};
    lv_draw_img_dsc_init(&draw_dsc_t);
    draw_dsc_t.angle = angle_t * 10;
    draw_dsc_t.pivot.x = point_x; // app_lv_view_sched.snapshot_c.header.w / 2;
    draw_dsc_t.pivot.y = point_y; // app_lv_view_sched.snapshot_c.header.h / 2;
    lv_canvas_draw_img(app_lv_view_sched.canvas, 0, 0,
        &app_lv_view_sched.snapshot_t, &draw_dsc_t);
    #else
    lv_obj_set_pos(view_c, 0, 0);
    lv_obj_set_pos(view_t, 0, 0);
    
    lv_opa_t opa_c = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_100, LV_OPA_50);
    lv_obj_set_style_transform_pivot_x(view_c, point_x, 0);
    lv_obj_set_style_transform_pivot_y(view_c, point_y, 0);
    lv_obj_set_style_transform_angle(view_c, angle_c * 10, 0);
    // lv_obj_set_style_opa(view_c, opa_c, 0);
    
    lv_opa_t opa_t = lv_map(app_lv_view_sched.pct, 0, 100, LV_OPA_50, LV_OPA_100);
    lv_obj_set_style_transform_pivot_x(view_t, point_x, 0);
    lv_obj_set_style_transform_pivot_y(view_t, point_y, 0);
    lv_obj_set_style_transform_angle(view_t, angle_t * 10, 0);
    // lv_obj_set_style_opa(view_t, opa_t, 0);
    #endif
}

void app_lv_view_transform_reset(void)
{
    lv_obj_set_style_opa(app_lv_view_sched.view_c, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(app_lv_view_sched.view_t, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_angle(app_lv_view_sched.view_c, 0, 0);
    lv_obj_set_style_transform_angle(app_lv_view_sched.view_t, 0, 0);
    lv_obj_set_style_transform_zoom(app_lv_view_sched.view_c, LV_IMG_ZOOM_NONE, 0);
    lv_obj_set_style_transform_zoom(app_lv_view_sched.view_t, LV_IMG_ZOOM_NONE, 0);
}

/*该接口完成以下目标
 *给定现在滑动的状态比例
 *将俩个画布图形变换后
 *渲染到一个新画布上
 */
void app_lv_view_transform_async_cb(void *user_data)
{
    // 异步只需要发生在动画持续时
    if (!app_lv_view_sched.anim_lock) return;
    
    // LV_LOG_WARN("\nview_c:<%d, %d>  ", app_lv_view_sched.point_c.x, app_lv_view_sched.point_c.y);
    // LV_LOG_WARN("\nview_t:<%d, %d>\n", app_lv_view_sched.point_t.x, app_lv_view_sched.point_t.y);
    
    #if 0
    // 底图填成黑色底面
    lv_draw_rect_dsc_t draw_rect = {0};
    lv_draw_rect_dsc_init(&draw_rect);
    draw_rect.bg_opa = LV_OPA_COVER;
    draw_rect.bg_color = lv_color_black();
    // 其他部分都是空的, 只要黑色底面
    draw_rect.border_opa = draw_rect.shadow_opa  =
    draw_rect.bg_img_opa = draw_rect.outline_opa = LV_OPA_TRANSP;
    lv_coord_t w = lv_obj_get_width(app_lv_view_sched.canvas);
    lv_coord_t h = lv_obj_get_height(app_lv_view_sched.canvas);
    lv_canvas_draw_rect(app_lv_view_sched.canvas, 0, 0, w, h, &draw_rect);
    #endif
    
    #if 0
    // 仅仅测试固定界面所用, 长久屏蔽
    app_lv_view_sched.transform_type = lv_rand(1, 100) % app_lv_view_transform_type_num;
    app_lv_view_sched.transform_type = app_lv_view_transform_type_rotate;
    #endif
    
    switch (app_lv_view_sched.transform_type) {
    default:
    case app_lv_view_transform_type_move:
        app_lv_view_transform_move();
        break;
    case app_lv_view_transform_type_cover_in:
    case app_lv_view_transform_type_cover_out:
        app_lv_view_transform_cover();
        break;
    case app_lv_view_transform_type_zoom1:
    case app_lv_view_transform_type_zoom2:
        app_lv_view_transform_zoom();
        break;
    case app_lv_view_transform_type_center_in:
    case app_lv_view_transform_type_center_out:
    case app_lv_view_transform_type_center_inout:
        app_lv_view_transform_center();
        break;
    case app_lv_view_transform_type_rotate:
        app_lv_view_transform_rotate();
        break;
    }
    
    // 
    lv_obj_invalidate(app_lv_view_sched.slide_effect);
}
