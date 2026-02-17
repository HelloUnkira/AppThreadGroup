/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t big_3_slave;
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_ACTIVITY_BAR_ARC;
        break;
    }
    case scui_event_widget_scroll_layout:
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_keep:
    case scui_event_widget_scroll_over: {
        
        scui_coord_t scroll_pct = 0;
        scui_scroll_percent_get(event->object, &scroll_pct);
        scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
        scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        break;
    }
    default:
        break;
    }
    
    
    
    // 转移至控件调度
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_bar_arc_event_proc(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ring_big_3_slave_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_handle_t image_bg   = scui_image_prj_image_src_03_activity_ring_big_max_01_bjbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_03_activity_ring_big_max_02_bj_00bmp;
        scui_handle_t image_ring = scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp;
        
        scui_widget_draw_image(event->object, NULL, image_bg, NULL, SCUI_COLOR_UNUSED);
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += (466 - 458) / 2;
        clip.y += (466 - 458) / 2;
        clip.w = 458;
        clip.h = 458;
        
        // 困惑, 滑动的重绘会导致这里显示异常
        scui_coord_t pct_r = scui_map(scui_presenter.get_kcal_cur(), scui_presenter.get_kcal_min(), scui_presenter.get_kcal_max(), 0, 100);
        scui_coord_t pct_g = scui_map(scui_presenter.get_step_cur(), scui_presenter.get_step_min(), scui_presenter.get_step_max(), 0, 100);
        scui_coord_t pct_b = scui_map(scui_presenter.get_dist_cur(), scui_presenter.get_dist_min(), scui_presenter.get_dist_max(), 0, 100);
        scui_color_t color_r = {.filter = true,.color_s.ch.r = 0xFF,.color_e.ch.r = 0xFF,};
        scui_color_t color_g = {.filter = true,.color_s.ch.g = 0xFF,.color_e.ch.g = 0xFF,};
        scui_color_t color_b = {.filter = true,.color_s.ch.b = 0xFF,.color_e.ch.b = 0xFF,};
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  -81, color_r,  +22, pct_r, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  +39, color_g, +141, pct_g, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, +158, color_b, +262, pct_b, image_edge);
        
        scui_handle_t image_pic_kcal = scui_image_prj_image_src_03_activity_04_icon_kcalpng;
        scui_handle_t image_pic_step = scui_image_prj_image_src_03_activity_06_icon_stepspng;
        scui_handle_t image_pic_dist = scui_image_prj_image_src_03_activity_02_icon_dispng;
        /* 绘制圆环前景图: */
        clip = scui_widget_clip(event->object); clip.x += 242; clip.y +=  10;
        scui_widget_draw_image(event->object, &clip, image_pic_kcal, NULL, SCUI_COLOR_UNUSED);
        clip = scui_widget_clip(event->object); clip.x += 376; clip.y += 341;
        scui_widget_draw_image(event->object, &clip, image_pic_step, NULL, SCUI_COLOR_UNUSED);
        clip = scui_widget_clip(event->object); clip.x +=  22; clip.y += 291;
        scui_widget_draw_image(event->object, &clip, image_pic_dist, NULL, SCUI_COLOR_UNUSED);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ring_big_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        // 清空图像资源缓存
        scui_image_cache_clear();
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
        custom_maker.widget.type        = scui_widget_type_custom;
        custom_maker.widget.clip.w      = SCUI_HOR_RES;
        custom_maker.widget.clip.h      = SCUI_VER_RES;
        custom_maker.widget.event_cb    = scui_ui_scene_activity_scroll_ring_big_3_slave_event_proc;
        scui_widget_create(&custom_maker, &custom_handle);
        scui_ui_res_local->big_3_slave = custom_handle;
        
        // 创建画布,用于绘制
        scui_surface_t surface = {
            .format  = SCUI_PIXEL_CF_DEF,
            .hor_res = custom_maker.widget.clip.w,
            .ver_res = custom_maker.widget.clip.h,
        };
        scui_widget_surface_create(scui_ui_res_local->big_3_slave, &surface);
        scui_widget_surface_refr(scui_ui_res_local->big_3_slave, true);
        
        scui_widget_show(scui_ui_res_local->big_3_slave, false);
        scui_widget_draw(scui_ui_res_local->big_3_slave, NULL, true);
        break;
    }
    case scui_event_destroy: {
        
        scui_widget_surface_destroy(scui_ui_res_local->big_3_slave);
        scui_widget_hide(scui_ui_res_local->big_3_slave, false);
        scui_ui_res_local->big_3_slave = SCUI_HANDLE_INVALID;
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_handle_t  custom = scui_ui_res_local->big_3_slave;
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = scui_widget_clip(custom).w,
            .pixel.height   = scui_widget_clip(custom).h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_UNUSED);
        scui_handle_clear(image);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ditail_title_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
        string_maker.widget.type                = scui_widget_type_string;
        string_maker.widget.parent              = event->object;
        string_maker.widget.clip.w              = SCUI_HOR_RES;
        string_maker.widget.clip.h              = 40;
        string_maker.widget.clip.y              = (scui_widget_clip(event->object).h - string_maker.widget.clip.h) / 2;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.mode_scroll           = 1;
        string_maker.args.color.color_s.full    = 0xFFFFFFFF;
        string_maker.args.color.color_e.full    = 0xFFFFFFFF;
        string_maker.args.color.filter          = true;
        string_maker.text                       = SCUI_MULTI_LANG_0X0017;
        string_maker.font_idx                   = SCUI_FONT_IDX_36;
        scui_widget_create(&string_maker, &string_handle);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ditail_sum_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        break;
    }
    case scui_event_lang_change:
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        uint8_t week_idx = scui_presenter.get_week();
        /* 文字和背景小圆圈:日,一,二,三,四,五,六 */
        for (int16_t idx = 0; idx < 7; idx++) {
            
            uint32_t text_week[] = {
                SCUI_MULTI_LANG_0X0016, /* 日 */
                SCUI_MULTI_LANG_0X0010, /* 一 */
                SCUI_MULTI_LANG_0X0011, /* 二 */
                SCUI_MULTI_LANG_0X0012, /* 三 */
                SCUI_MULTI_LANG_0X0013, /* 四 */
                SCUI_MULTI_LANG_0X0014, /* 五 */
                SCUI_MULTI_LANG_0X0015, /* 六 */
            };
            
            /* 红色小圆圈绘制 */
            if (idx == week_idx) {
                scui_handle_t image = scui_image_prj_image_src_03_activity_01_dotbmp;
                scui_area_t clip_image = clip;
                clip_image.w = 44; clip_image.x += 36 + idx * (44 + 12);
                clip_image.h = 44; clip_image.y += 113 - 90;
                scui_widget_draw_image(event->object, &clip_image, image, NULL, SCUI_COLOR_UNUSED);
            }
            
            /* 单字符文本 */
            scui_string_args_t args = {0};
            args.color.color_s.full = 0xFFFFFFFF;
            args.color.color_e.full = 0xFFFFFFFF;
            args.color.filter = true;
            args.align_hor = 2;
            args.align_ver = 2;
            
            scui_multi_lang_get(&args.lang);
            args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
            args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
            
            scui_area_t clip_str = clip;
            clip_str.w = 44; clip_str.x += 36 + idx * (44 + 12);
            clip_str.h = 44; clip_str.y += 113 - 90;
            scui_custom_draw_text(event->object, &clip_str, &args, text_week[idx]);
        }
        
        uint32_t day7_kcal[7] = {0};
        uint32_t day7_step[7] = {0};
        uint32_t day7_dist[7] = {0};
        scui_presenter.get_kcal_day7(day7_kcal);
        scui_presenter.get_step_day7(day7_step);
        scui_presenter.get_dist_day7(day7_dist);
        /* 过去六天加今天的小三环:日,一,二,三,四,五,六 */
        for (int16_t idx = 0; idx < 7; idx++)
        {
            scui_area_t clip_icon = clip;
            clip_icon.w = 50; clip_icon.x += 36 + idx * (50 + 6);
            clip_icon.h = 50; clip_icon.y += 173 - 90;
            
            scui_coord_t pct_kcal = 0;
            scui_coord_t pct_step = 0;
            scui_coord_t pct_dist = 0;
            
            if (idx == week_idx) {
                pct_kcal = scui_map(scui_presenter.get_kcal_cur(), scui_presenter.get_kcal_min(), scui_presenter.get_kcal_max(), 0, 100);
                pct_step = scui_map(scui_presenter.get_step_cur(), scui_presenter.get_step_min(), scui_presenter.get_step_max(), 0, 100);
                pct_dist = scui_map(scui_presenter.get_dist_cur(), scui_presenter.get_dist_min(), scui_presenter.get_dist_max(), 0, 100);
            } else {
                pct_kcal = scui_map(day7_kcal[idx], scui_presenter.get_kcal_min(), scui_presenter.get_kcal_max(), 0, 100);
                pct_step = scui_map(day7_step[idx], scui_presenter.get_step_min(), scui_presenter.get_step_max(), 0, 100);
                pct_dist = scui_map(day7_dist[idx], scui_presenter.get_dist_min(), scui_presenter.get_dist_max(), 0, 100);
            }
            
            /* 限制到0-9 */
            pct_kcal = 9;scui_clamp(pct_kcal / 10, 0, 9);
            pct_step = 9;scui_clamp(pct_step / 10, 0, 9);
            pct_dist = 9;scui_clamp(pct_dist / 10, 0, 9);
            /* 先画背景 */
            scui_handle_t image = scui_image_prj_image_src_03_activity_ring_small_min_01_bjbmp;
            scui_widget_draw_image(event->object, &clip_icon, image, NULL, SCUI_COLOR_UNUSED);
            /* 画三个环 */
            if (pct_kcal != 0) {
                scui_handle_t  image = scui_image_prj_image_src_03_activity_ring_small_min_calories_01_01png;
                scui_area_t clip_min = clip_icon;
                clip_min.x += 14; clip_min.y -= 8;
                clip_min.x += (clip_min.w - scui_image_w(image)) / 2;
                clip_min.y += (clip_min.h - scui_image_h(image)) / 2;
                scui_widget_draw_image(event->object, &clip_min, image + pct_kcal, NULL, SCUI_COLOR_UNUSED);
            }
            if (pct_step != 0) {
                scui_handle_t  image = scui_image_prj_image_src_03_activity_ring_small_min_steps_01_01png;
                scui_area_t clip_min = clip_icon;
                clip_min.y += 16;
                clip_min.x += (clip_min.w - scui_image_w(image)) / 2;
                clip_min.y += (clip_min.h - scui_image_h(image)) / 2;
                scui_widget_draw_image(event->object, &clip_min, image + pct_step, NULL, SCUI_COLOR_UNUSED);
            }
            if (pct_dist != 0) {
                scui_handle_t  image = scui_image_prj_image_src_03_activity_ring_small_min_distance_01_01png;
                scui_area_t clip_min = clip_icon;
                clip_min.x -= 14; clip_min.y -= 8;
                clip_min.x += (clip_min.w - scui_image_w(image)) / 2;
                clip_min.y += (clip_min.h - scui_image_h(image)) / 2;
                scui_widget_draw_image(event->object, &clip_min, image + pct_dist, NULL, SCUI_COLOR_UNUSED);
            }
        }
        //分割线
        scui_area_t clip_line = clip;
        clip_line.w = 394; clip_line.x += 36;
        clip_line.h =   2; clip_line.y += 243 - 90;
        scui_area_m_to_s(&clip_line, &clip_line);
        
        scui_draw_dsc_t draw_graph = {0};
        draw_graph.type = scui_draw_type_pixel_line;
        draw_graph.src_color.color.full = 0xFF333333;
        draw_graph.src_width = 2;
        draw_graph.src_pos_1.x = clip_line.x1;
        draw_graph.src_pos_1.y = clip_line.y1;
        draw_graph.src_pos_2.x = clip_line.x2;
        draw_graph.src_pos_2.y = clip_line.y1;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ditail_kcal_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        // chart hist
        scui_xchart_maker_t xchart_maker = {0};
        scui_handle_t xchart_handle = SCUI_HANDLE_INVALID;
        xchart_maker.widget.type = scui_widget_type_xchart;
        xchart_maker.widget.clip.x = 0;
        xchart_maker.widget.clip.y = 100;
        xchart_maker.widget.clip.w = SCUI_HOR_RES;
        xchart_maker.widget.clip.h = 86;
        xchart_maker.widget.parent = event->object;
        xchart_maker.type = scui_xchart_type_hist;
        xchart_maker.hist.edge = scui_image_prj_image_src_repeat_06_dotbmp;
        xchart_maker.hist.value_min = 0;
        xchart_maker.hist.value_max = 100;
        xchart_maker.hist.number    = 24;
        xchart_maker.hist.offset.x  = 42;
        xchart_maker.hist.height    = 86;
        xchart_maker.hist.space     = 4;
        xchart_maker.hist.color.color.full = 0xFFF9104F;
        scui_widget_create(&xchart_maker, &xchart_handle);
        
        uint32_t day7_24[24] = {0};
        scui_presenter.get_kcal_day7_24(scui_presenter.get_week(), day7_24);
        
        scui_coord_t vlist_min[24] = {0};
        scui_coord_t vlist_max[24] = {0};
        for (uint32_t idx  = 0; idx < 24; idx++) {
            vlist_min[idx] = 0;
            vlist_max[idx] = scui_map(day7_24[idx], scui_presenter.get_kcal_min(), scui_presenter.get_kcal_max(), 0, 100);
        }
        scui_xchart_hist_data(xchart_handle, vlist_min, vlist_max);
        break;
    }
    case scui_event_lang_change:
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        
        /* 文本 */
        scui_string_args_t args = {0};
        args.color.color_s.full = 0XFFDA2069;
        args.color.color_e.full = 0XFFDA2069;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_str = clip;
        clip_str.w = SCUI_HOR_RES - 40 * 2; clip_str.x += 40;
        clip_str.h = 40; clip_str.y += 25 - 16;
        scui_custom_draw_text(event->object, &clip_str, &args, SCUI_MULTI_LANG_0X000a);
        
        scui_handle_t image_digit = scui_image_prj_image_src_num_44_white_24x34_04_03png;
        uint8_t char_digit[10] = {0};
        scui_coord_t digit_num = snprintf(char_digit, sizeof(char_digit), "%d", scui_presenter.get_kcal_cur());
        scui_area_t clip_digit = clip; clip_digit.x += 40; clip_digit.y += 52;
        scui_handle_t image_digit_list[10] = {0};
        for (scui_coord_t idx = 0; idx < digit_num; idx++)
            image_digit_list[idx] = image_digit + char_digit[idx] - '0';
        scui_custom_draw_image_text(event, &clip_digit, image_digit_list, SCUI_COLOR_FILTER_TRANS, 3, digit_num, 0);
        
        args.color.color_s.full = 0XFFFFFFFF;
        args.color.color_e.full = 0XFFFFFFFF;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_unit = clip;
        clip_unit.y += 52; clip_unit.x += 40 + (scui_image_w(image_digit) + 3) * digit_num;
        clip_unit.h  = 40; clip_unit.w = SCUI_HOR_RES - (clip_unit.x - clip.x) - 40;
        scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_MULTI_LANG_0X000d);
        
        for (int16_t idx = 0; idx <= 24; idx++) {
            
            // 在0, 6, 12, 18, 24点前面画线
            if (idx % 6 == 0)
            {
                scui_area_t clip_line = clip;
                clip_line.w =   1; clip_line.x += 42 + idx * (12 + 4) - 2;
                clip_line.h = 110; clip_line.y += 113;
                scui_area_m_to_s(&clip_line, &clip_line);
                
                scui_draw_dsc_t draw_graph = {0};
                draw_graph.type = scui_draw_type_pixel_line;
                draw_graph.src_color.color.full = 0xFF333333;
                draw_graph.src_width = 2;
                draw_graph.src_pos_1.x = clip_line.x1;
                draw_graph.src_pos_1.y = clip_line.y1;
                draw_graph.src_pos_2.x = clip_line.x1;
                draw_graph.src_pos_2.y = clip_line.y2;
                scui_widget_draw_graph(event->object, NULL, &draw_graph);
            }
            
            if (idx == 24)
                break;
            
            if (idx % 6 == 0) {
                char digit_str[10] = {0};
                sprintf(digit_str, "%d", idx);
                
                args.color.color_s.full = 0xFF333333;
                args.color.color_e.full = 0xFF333333;
                args.color.filter = true;
                args.align_hor = 0;
                args.align_ver = 2;
                args.name = SCUI_FONT_TYPE_24_ASCII;
                args.utf8 = digit_str;
                
                scui_area_t clip_unit = clip;
                clip_unit.y += 206 - 8; clip_unit.x += 42 + idx * (12 + 4) + 2;
                clip_unit.h  = 40; clip_unit.w = 80;
                scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_HANDLE_INVALID);
            }
            
            scui_handle_t  image = scui_image_prj_image_src_03_activity_bar_02_dot_caloriesbmp;
            scui_area_t clip_dot = clip;
            clip_dot.x += 42 + idx * (12 + 4); clip_dot.y += 187 - 12;
            scui_widget_draw_image(event->object, &clip_dot, image, NULL, SCUI_COLOR_UNUSED);
        }
        
        //分割线
        scui_area_t clip_line = clip;
        clip_line.w = 394; clip_line.x += 36;
        clip_line.h =   2; clip_line.y += 248;
        scui_area_m_to_s(&clip_line, &clip_line);
        
        scui_draw_dsc_t draw_graph = {0};
        draw_graph.type = scui_draw_type_pixel_line;
        draw_graph.src_color.color.full = 0xFF333333;
        draw_graph.src_width = 2;
        draw_graph.src_pos_1.x = clip_line.x1;
        draw_graph.src_pos_1.y = clip_line.y1;
        draw_graph.src_pos_2.x = clip_line.x2;
        draw_graph.src_pos_2.y = clip_line.y1;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ditail_step_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        // chart hist
        scui_xchart_maker_t xchart_maker = {0};
        scui_handle_t xchart_handle = SCUI_HANDLE_INVALID;
        xchart_maker.widget.type = scui_widget_type_xchart;
        xchart_maker.widget.clip.x = 0;
        xchart_maker.widget.clip.y = 100;
        xchart_maker.widget.clip.w = SCUI_HOR_RES;
        xchart_maker.widget.clip.h = 86;
        xchart_maker.widget.parent = event->object;
        xchart_maker.type = scui_xchart_type_hist;
        xchart_maker.hist.edge = scui_image_prj_image_src_repeat_06_dotbmp;
        xchart_maker.hist.value_min = 0;
        xchart_maker.hist.value_max = 100;
        xchart_maker.hist.number    = 24;
        xchart_maker.hist.offset.x  = 42;
        xchart_maker.hist.height    = 86;
        xchart_maker.hist.space     = 4;
        xchart_maker.hist.color.color.full = 0xFFE1CC00;
        scui_widget_create(&xchart_maker, &xchart_handle);
        
        uint32_t day7_24[24] = {0};
        scui_presenter.get_step_day7_24(scui_presenter.get_week(), day7_24);
        
        scui_coord_t vlist_min[24] = {0};
        scui_coord_t vlist_max[24] = {0};
        for (uint32_t idx  = 0; idx < 24; idx++) {
            vlist_min[idx] = 0;
            vlist_max[idx] = scui_map(day7_24[idx], scui_presenter.get_step_min(), scui_presenter.get_step_max(), 0, 100);
        }
        scui_xchart_hist_data(xchart_handle, vlist_min, vlist_max);
        break;
    }
    case scui_event_lang_change:
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        
        /* 文本 */
        scui_string_args_t args = {0};
        args.color.color_s.full = 0XFFDFC026;
        args.color.color_e.full = 0XFFDFC026;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_str = clip;
        clip_str.w = SCUI_HOR_RES - 40 * 2; clip_str.x += 40;
        clip_str.h = 40; clip_str.y += 25 - 16;
        scui_custom_draw_text(event->object, &clip_str, &args, SCUI_MULTI_LANG_0X000c);
        
        scui_handle_t image_digit = scui_image_prj_image_src_num_44_white_24x34_04_03png;
        uint8_t char_digit[10] = {0};
        scui_coord_t digit_num = snprintf(char_digit, sizeof(char_digit), "%d", scui_presenter.get_kcal_cur());
        scui_area_t clip_digit = clip; clip_digit.x += 40; clip_digit.y += 52;
        scui_handle_t image_digit_list[10] = {0};
        for (scui_coord_t idx = 0; idx < digit_num; idx++)
            image_digit_list[idx] = image_digit + char_digit[idx] - '0';
        scui_custom_draw_image_text(event, &clip_digit, image_digit_list, SCUI_COLOR_FILTER_TRANS, 3, digit_num, 0);
        
        args.color.color_s.full = 0XFFFFFFFF;
        args.color.color_e.full = 0XFFFFFFFF;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_unit = clip;
        clip_unit.y += 52; clip_unit.x += 40 + (scui_image_w(image_digit) + 3) * digit_num;
        clip_unit.h  = 40; clip_unit.w = SCUI_HOR_RES - (clip_unit.x - clip.x) - 40;
        scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_MULTI_LANG_0X000f);
        
        for (int16_t idx = 0; idx <= 24; idx++) {
            
            // 在0, 6, 12, 18, 24点前面画线
            if (idx % 6 == 0)
            {
                scui_area_t clip_line = clip;
                clip_line.w =   1; clip_line.x += 42 + idx * (12 + 4) - 2;
                clip_line.h = 110; clip_line.y += 113;
                scui_area_m_to_s(&clip_line, &clip_line);
                
                scui_draw_dsc_t draw_graph = {0};
                draw_graph.type = scui_draw_type_pixel_line;
                draw_graph.src_color.color.full = 0xFF333333;
                draw_graph.src_width = 2;
                draw_graph.src_pos_1.x = clip_line.x1;
                draw_graph.src_pos_1.y = clip_line.y1;
                draw_graph.src_pos_2.x = clip_line.x1;
                draw_graph.src_pos_2.y = clip_line.y2;
                scui_widget_draw_graph(event->object, NULL, &draw_graph);
            }
            
            if (idx == 24)
                break;
            
            if (idx % 6 == 0) {
                char digit_str[10] = {0};
                sprintf(digit_str, "%d", idx);
                
                args.color.color_s.full = 0xFF333333;
                args.color.color_e.full = 0xFF333333;
                args.color.filter = true;
                args.align_hor = 0;
                args.align_ver = 2;
                args.name = SCUI_FONT_TYPE_24_ASCII;
                args.utf8 = digit_str;
                
                scui_area_t clip_unit = clip;
                clip_unit.y += 206 - 8; clip_unit.x += 42 + idx * (12 + 4) + 2;
                clip_unit.h  = 40; clip_unit.w = 80;
                scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_HANDLE_INVALID);
            }
            
            scui_handle_t  image = scui_image_prj_image_src_03_activity_bar_04_dot_stepsbmp;
            scui_area_t clip_dot = clip;
            clip_dot.x += 42 + idx * (12 + 4); clip_dot.y += 187 - 12;
            scui_widget_draw_image(event->object, &clip_dot, image, NULL, SCUI_COLOR_UNUSED);
        }
        
        //分割线
        scui_area_t clip_line = clip;
        clip_line.w = 394; clip_line.x += 36;
        clip_line.h =   2; clip_line.y += 248;
        scui_area_m_to_s(&clip_line, &clip_line);
        
        scui_draw_dsc_t draw_graph = {0};
        draw_graph.type = scui_draw_type_pixel_line;
        draw_graph.src_color.color.full = 0xFF333333;
        draw_graph.src_width = 2;
        draw_graph.src_pos_1.x = clip_line.x1;
        draw_graph.src_pos_1.y = clip_line.y1;
        draw_graph.src_pos_2.x = clip_line.x2;
        draw_graph.src_pos_2.y = clip_line.y1;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_activity_scroll_ditail_dist_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        // chart hist
        scui_xchart_maker_t xchart_maker = {0};
        scui_handle_t xchart_handle = SCUI_HANDLE_INVALID;
        xchart_maker.widget.type = scui_widget_type_xchart;
        xchart_maker.widget.clip.x = 0;
        xchart_maker.widget.clip.y = 100;
        xchart_maker.widget.clip.w = SCUI_HOR_RES;
        xchart_maker.widget.clip.h = 86;
        xchart_maker.widget.parent = event->object;
        xchart_maker.type = scui_xchart_type_hist;
        xchart_maker.hist.edge = scui_image_prj_image_src_repeat_06_dotbmp;
        xchart_maker.hist.value_min = 0;
        xchart_maker.hist.value_max = 100;
        xchart_maker.hist.number    = 24;
        xchart_maker.hist.offset.x  = 42;
        xchart_maker.hist.height    = 86;
        xchart_maker.hist.space     = 4;
        xchart_maker.hist.color.color.full = 0xFF00B7FF;
        scui_widget_create(&xchart_maker, &xchart_handle);
        
        uint32_t day7_24[24] = {0};
        scui_presenter.get_dist_day7_24(scui_presenter.get_week(), day7_24);
        
        scui_coord_t vlist_min[24] = {0};
        scui_coord_t vlist_max[24] = {0};
        for (uint32_t idx  = 0; idx < 24; idx++) {
            vlist_min[idx] = 0;
            vlist_max[idx] = scui_map(day7_24[idx], scui_presenter.get_dist_min(), scui_presenter.get_dist_max(), 0, 100);
        }
        scui_xchart_hist_data(xchart_handle, vlist_min, vlist_max);
        break;
    }
    case scui_event_lang_change:
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        
        /* 文本 */
        scui_string_args_t args = {0};
        args.color.color_s.full = 0XFF06ABFB;
        args.color.color_e.full = 0XFF06ABFB;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_str = clip;
        clip_str.w = SCUI_HOR_RES - 40 * 2; clip_str.x += 40;
        clip_str.h = 40; clip_str.y += 25 - 16;
        scui_custom_draw_text(event->object, &clip_str, &args, SCUI_MULTI_LANG_0X000b);
        
        scui_handle_t image_digit = scui_image_prj_image_src_num_44_white_24x34_04_03png;
        uint8_t char_digit[10] = {0};
        scui_coord_t digit_num = snprintf(char_digit, sizeof(char_digit), "%d", scui_presenter.get_kcal_cur());
        scui_area_t clip_digit = clip; clip_digit.x += 40; clip_digit.y += 52;
        scui_handle_t image_digit_list[10] = {0};
        for (scui_coord_t idx = 0; idx < digit_num; idx++)
            image_digit_list[idx] = image_digit + char_digit[idx] - '0';
        scui_custom_draw_image_text(event, &clip_digit, image_digit_list, SCUI_COLOR_FILTER_TRANS, 3, digit_num, 0);
        
        args.color.color_s.full = 0XFFFFFFFF;
        args.color.color_e.full = 0XFFFFFFFF;
        args.color.filter = true;
        args.align_hor = 0;
        args.align_ver = 2;
        
        scui_multi_lang_get(&args.lang);
        args.size = scui_font_size_match(SCUI_FONT_IDX_32, 0);
        args.name = scui_font_name_match(SCUI_FONT_IDX_32, args.lang);
        
        scui_area_t clip_unit = clip;
        clip_unit.y += 52; clip_unit.x += 40 + (scui_image_w(image_digit) + 3) * digit_num;
        clip_unit.h  = 40; clip_unit.w = SCUI_HOR_RES - (clip_unit.x - clip.x) - 40;
        scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_MULTI_LANG_0X000e);
        
        for (int16_t idx = 0; idx <= 24; idx++) {
            
            // 在0, 6, 12, 18, 24点前面画线
            if (idx % 6 == 0)
            {
                scui_area_t clip_line = clip;
                clip_line.w =   1; clip_line.x += 42 + idx * (12 + 4) - 2;
                clip_line.h = 110; clip_line.y += 113;
                scui_area_m_to_s(&clip_line, &clip_line);
                
                scui_draw_dsc_t draw_graph = {0};
                draw_graph.type = scui_draw_type_pixel_line;
                draw_graph.src_color.color.full = 0xFF333333;
                draw_graph.src_width = 2;
                draw_graph.src_pos_1.x = clip_line.x1;
                draw_graph.src_pos_1.y = clip_line.y1;
                draw_graph.src_pos_2.x = clip_line.x1;
                draw_graph.src_pos_2.y = clip_line.y2;
                scui_widget_draw_graph(event->object, NULL, &draw_graph);
            }
            
            if (idx == 24)
                break;
            
            if (idx % 6 == 0) {
                char digit_str[10] = {0};
                sprintf(digit_str, "%d", idx);
                
                args.color.color_s.full = 0xFF333333;
                args.color.color_e.full = 0xFF333333;
                args.color.filter = true;
                args.align_hor = 0;
                args.align_ver = 2;
                args.name = SCUI_FONT_TYPE_24_ASCII;
                args.utf8 = digit_str;
                
                scui_area_t clip_unit = clip;
                clip_unit.y += 206 - 8; clip_unit.x += 42 + idx * (12 + 4) + 2;
                clip_unit.h  = 40; clip_unit.w = 80;
                scui_custom_draw_text(event->object, &clip_unit, &args, SCUI_HANDLE_INVALID);
            }
            
            scui_handle_t  image = scui_image_prj_image_src_03_activity_bar_03_dot_distancebmp;
            scui_area_t clip_dot = clip;
            clip_dot.x += 42 + idx * (12 + 4); clip_dot.y += 187 - 12;
            scui_widget_draw_image(event->object, &clip_dot, image, NULL, SCUI_COLOR_UNUSED);
        }
        
        //分割线
        scui_area_t clip_line = clip;
        clip_line.w = 394; clip_line.x += 36;
        clip_line.h =   2; clip_line.y += 248;
        scui_area_m_to_s(&clip_line, &clip_line);
        
        scui_draw_dsc_t draw_graph = {0};
        draw_graph.type = scui_draw_type_pixel_line;
        draw_graph.src_color.color.full = 0xFF333333;
        draw_graph.src_width = 2;
        draw_graph.src_pos_1.x = clip_line.x1;
        draw_graph.src_pos_1.y = clip_line.y1;
        draw_graph.src_pos_2.x = clip_line.x2;
        draw_graph.src_pos_2.y = clip_line.y1;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        break;
    }
}
