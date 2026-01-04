/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t     linear;
    scui_ui_bar_arc_t bar_arc;
    // 结构相关数据
    scui_handle_t title_music;
    scui_handle_t data_hr;
    scui_handle_t data_spo2;
    scui_handle_t data_stress;
    scui_handle_t passby_hr;
    scui_handle_t passby_spo2;
    scui_handle_t passby_stress;
    scui_handle_t data_weather_cur;
    scui_handle_t data_weather_lim;
    scui_handle_t data_alarm;
    scui_handle_t data_alarm_date;
    scui_handle_t data_sleep;
    scui_handle_t data_sport_record;
    scui_handle_t data_sport_record_time;
    scui_handle_t data_women_health;
    scui_handle_t data_women_health_time;
    scui_handle_t data_compass;
    scui_handle_t data_compass_info;
    scui_handle_t data_altimeter_info1;
    scui_handle_t data_altimeter_info2;
    scui_handle_t data_stopwatch;
    scui_handle_t data_countdown[4];
    scui_handle_t data_world_time1_time;
    scui_handle_t data_world_time1_unit;
    scui_handle_t data_world_time2_time;
    scui_handle_t data_world_time2_unit;
    
} * scui_ui_res_local = NULL;



/* 将第一个指定符号移到末尾后变成'\0'(目前用于将#移到末尾变成'\0') */
static void scui_ui_str_remove_discard_char(char *str, char t_char)
{
    int32_t len = strlen(str);
    char *ofs = NULL;
    ofs = strchr(str, t_char);
    if(ofs == NULL)
    {
        SCUI_LOG_DEBUG("ofs is NULL");
        return;
    }
    for (; ofs < str + len; ofs++)
    {
        *ofs = *(ofs + 1);
    }
}



/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_mini_card_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_s_event_proc(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_s_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_ui_scene_mini_card_type_t type = scui_ui_scene_mini_card_type[linear_item.draw_idx];
        scui_area_t clip = scui_widget_clip(event->object);
        
        /* 先绘制背景 */
        #if 1
        switch (type) {
        case scui_ui_scene_mini_card_type_music_control: {
            scui_handle_t image = scui_image_prj_image_src_19_widget_music_01_bgbmp;
            scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_UNUSED);
            break;
        }
        default: {
            scui_area_t   clip = scui_widget_clip(event->object);
            scui_color_t  color = {.color.full = 0xFF282828,};
            scui_handle_t image[4] = {
                scui_image_prj_image_src_repeat_card_04_r36_1bmp,
                scui_image_prj_image_src_repeat_card_05_r36_2bmp,
                scui_image_prj_image_src_repeat_card_06_r36_3bmp,
                scui_image_prj_image_src_repeat_card_07_r36_4bmp,
            };
            scui_custom_draw_image_crect4(event, &clip, image, color, -1);
            break;
        }
        }
        #endif
        
        
        
        /* 后绘制前景 */
        #if 1
        switch (type) {
        case scui_ui_scene_mini_card_type_daily_exercise: {
            scui_area_t clip = scui_widget_clip(event->object);
            
            // 三环绘制
            scui_area_t clip_bg = clip; clip_bg.x += 250; clip_bg.y += 22;
            scui_area_t clip_fg = clip; clip_fg.x += 250; clip_fg.y += 22;
            scui_coord_t pct_kcal = scui_map(scui_presenter.get_kcal_cur(), scui_presenter.get_kcal_min(), scui_presenter.get_kcal_max(), 0, 100);
            scui_coord_t pct_step = scui_map(scui_presenter.get_step_cur(), scui_presenter.get_step_min(), scui_presenter.get_step_max(), 0, 100);
            scui_coord_t pct_dist = scui_map(scui_presenter.get_dist_cur(), scui_presenter.get_dist_min(), scui_presenter.get_dist_max(), 0, 100);
            scui_color_t color_kcal = {.filter = true,.color_s.full = 0xFFDA2069,.color_e.full = 0xFFDA2069,};
            scui_color_t color_step = {.filter = true,.color_s.full = 0xFFDFC026,.color_e.full = 0xFFDFC026,};
            scui_color_t color_dist = {.filter = true,.color_s.full = 0xFF06AEFF,.color_e.full = 0xFF06AEFF,};
            scui_handle_t image_bg   = scui_image_prj_image_src_19_widget_activity_01_bgbmp;
            scui_handle_t image_edge = scui_image_prj_image_src_19_widget_activity_04_dotbmp;
            scui_handle_t image_ring = scui_image_prj_image_src_19_widget_activity_05_ringbmp;
            scui_widget_draw_image(event->object, &clip_bg, image_bg, NULL, SCUI_COLOR_FILTER_TRANS);
            scui_widget_draw_ring(event->object, &clip_fg, image_ring, NULL,  -81, color_kcal,  +22, pct_kcal, image_edge);
            scui_widget_draw_ring(event->object, &clip_fg, image_ring, NULL,  +39, color_step, +141, pct_step, image_edge);
            scui_widget_draw_ring(event->object, &clip_fg, image_ring, NULL, +158, color_dist, +262, pct_dist, image_edge);
            
            // 绘制小图标
            scui_area_t clip_icon_kcal = clip; clip_icon_kcal.x += 25; clip_icon_kcal.y += 23;
            scui_area_t clip_icon_step = clip; clip_icon_step.x += 25; clip_icon_step.y += 73;
            scui_area_t clip_icon_dist = clip; clip_icon_dist.x += 25; clip_icon_dist.y += 123;
            scui_handle_t image_icon_kcal = scui_image_prj_image_src_19_widget_activity_02_caloriesbmp;
            scui_handle_t image_icon_step = scui_image_prj_image_src_19_widget_activity_08_stepsbmp;
            scui_handle_t image_icon_dist = scui_image_prj_image_src_19_widget_activity_03_distancebmp;
            scui_widget_draw_image(event->object, &clip_icon_kcal, image_icon_kcal, NULL, SCUI_COLOR_FILTER_TRANS);
            scui_widget_draw_image(event->object, &clip_icon_step, image_icon_step, NULL, SCUI_COLOR_FILTER_TRANS);
            scui_widget_draw_image(event->object, &clip_icon_dist, image_icon_dist, NULL, SCUI_COLOR_FILTER_TRANS);
            
            // 绘制数字图标
            scui_handle_t image_digit_kcal = scui_image_prj_image_src_19_widget_activity_num_pink_04_03bmp;
            scui_handle_t image_digit_step = scui_image_prj_image_src_19_widget_activity_num_yellow_04_03bmp;
            scui_handle_t image_digit_dist = scui_image_prj_image_src_19_widget_activity_num_blue_04_03bmp;
            uint8_t char_digit_kcal[10] = {0};
            uint8_t char_digit_step[10] = {0};
            uint8_t char_digit_dist[10] = {0};
            scui_coord_t digit_kcal_num = snprintf(char_digit_kcal, sizeof(char_digit_kcal), "%d", scui_presenter.get_kcal_cur());
            scui_coord_t digit_step_num = snprintf(char_digit_step, sizeof(char_digit_step), "%d", scui_presenter.get_kcal_cur());
            scui_coord_t digit_dist_num = snprintf(char_digit_dist, sizeof(char_digit_dist), "%d", scui_presenter.get_kcal_cur());
            scui_area_t clip_digit_kcal = clip_icon_kcal; clip_digit_kcal.x += scui_image_w(image_icon_kcal) + 10;
            scui_area_t clip_digit_step = clip_icon_step; clip_digit_step.x += scui_image_w(image_icon_step) + 10;
            scui_area_t clip_digit_dist = clip_icon_dist; clip_digit_dist.x += scui_image_w(image_icon_dist) + 10;
            scui_handle_t image_digit_kcal_list[10] = {0};
            scui_handle_t image_digit_step_list[10] = {0};
            scui_handle_t image_digit_dist_list[10] = {0};
            for (scui_coord_t idx = 0; idx < digit_kcal_num; idx++)
                image_digit_kcal_list[idx] = image_digit_kcal + char_digit_kcal[idx] - '0';
            for (scui_coord_t idx = 0; idx < digit_step_num; idx++)
                image_digit_step_list[idx] = image_digit_step + char_digit_step[idx] - '0';
            for (scui_coord_t idx = 0; idx < digit_dist_num; idx++)
                image_digit_dist_list[idx] = image_digit_dist + char_digit_dist[idx] - '0';
            scui_custom_draw_image_text(event, &clip_digit_kcal, image_digit_kcal_list, SCUI_COLOR_FILTER_TRANS, 3, digit_kcal_num, 0);
            scui_custom_draw_image_text(event, &clip_digit_step, image_digit_step_list, SCUI_COLOR_FILTER_TRANS, 3, digit_step_num, 0);
            scui_custom_draw_image_text(event, &clip_digit_dist, image_digit_dist_list, SCUI_COLOR_FILTER_TRANS, 3, digit_dist_num, 0);
            
            break;
        }
        case scui_ui_scene_mini_card_type_music_control: {
            
            // 在绘制中顺便更新词条
            scui_string_update_text(scui_ui_res_local->title_music, SCUI_MULTI_LANG_0X010b);
            
            // 不同状态显示不同图标
            scui_handle_t image0 = scui_image_prj_image_src_19_widget_music_04_upbmp;
            scui_handle_t image1 = scui_image_prj_image_src_19_widget_music_05_pausedbmp;
            scui_handle_t image2 = scui_image_prj_image_src_19_widget_music_02_nextbmp;
            
            scui_area_t btn_clip_0 = clip;
            scui_area_t btn_clip_1 = clip;
            scui_area_t btn_clip_2 = clip;
            btn_clip_0.x += 60; btn_clip_1.x += 160; btn_clip_2.x += 308;
            btn_clip_0.y += 99; btn_clip_1.y +=  77; btn_clip_2.y +=  99;
            scui_widget_draw_image(event->object, &btn_clip_0, image0, NULL, SCUI_COLOR_UNUSED);
            scui_widget_draw_image(event->object, &btn_clip_1, image1, NULL, SCUI_COLOR_UNUSED);
            scui_widget_draw_image(event->object, &btn_clip_2, image2, NULL, SCUI_COLOR_UNUSED);
            
            break;
        }
        case scui_ui_scene_mini_card_type_hr:
        case scui_ui_scene_mini_card_type_spo2:
        case scui_ui_scene_mini_card_type_stress: {
            
            scui_handle_t image = scui_image_prj_image_src_19_widget_06_retrybmp;
            scui_area_t image_clip = clip; image_clip.x += 298; image_clip.y += 66;
            scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            
            char data_unit[50] = {0};
            scui_handle_t unit_text = SCUI_HANDLE_INVALID;
            uint32_t passby_last = 0;
            
            if (type == scui_ui_scene_mini_card_type_hr) {
                unit_text = SCUI_MULTI_LANG_0X0021;
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                const char *lang_str = scui_multi_lang_str(unit_text, lang_type);
                
                snprintf(data_unit, sizeof(data_unit), "%d %s",
                    scui_presenter.get_hr_cur(), lang_str);
                scui_string_update_str(scui_ui_res_local->data_hr, data_unit);
                passby_last = scui_presenter.get_hr_passby();
            }
            if (type == scui_ui_scene_mini_card_type_spo2) {
                
                snprintf(data_unit, sizeof(data_unit), "%d %s",
                    scui_presenter.get_spo2_cur(), "%");
                scui_string_update_str(scui_ui_res_local->data_spo2, data_unit);
                passby_last = scui_presenter.get_spo2_passby();
            }
            if (type == scui_ui_scene_mini_card_type_stress) {
                unit_text = SCUI_MULTI_LANG_0X004e;
                if (scui_presenter.get_stress_cur() < 70) unit_text = SCUI_MULTI_LANG_0X004d;
                if (scui_presenter.get_stress_cur() < 50) unit_text = SCUI_MULTI_LANG_0X004c;
                if (scui_presenter.get_stress_cur() < 30) unit_text = SCUI_MULTI_LANG_0X004b;
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                const char *lang_str = scui_multi_lang_str(unit_text, lang_type);
                
                snprintf(data_unit, sizeof(data_unit), "%d %s",
                    scui_presenter.get_stress_cur(), lang_str);
                scui_string_update_str(scui_ui_res_local->data_stress, data_unit);
                passby_last = scui_presenter.get_stress_passby();
            }
            
            if (passby_last < 1) unit_text = SCUI_MULTI_LANG_0X001d;
            else if (passby_last < 59) unit_text = SCUI_MULTI_LANG_0X001e;
            else if (passby_last < 60 * 24) unit_text = SCUI_MULTI_LANG_0X001f;
            else if (passby_last < 60 * 24 * 7) unit_text = SCUI_MULTI_LANG_0X0020;
            else unit_text = SCUI_MULTI_LANG_0X01c4;
            
            data_unit[0] = '\0';
            if (passby_last != 0 && passby_last < 60 * 24 * 7) {
                if (passby_last > 60 * 24) passby_last /= 60 * 24;
                else if (passby_last > 60) passby_last /= 60;
                snprintf(data_unit, sizeof(data_unit), "%d ", passby_last);
            }
            
            scui_multi_lang_type_t lang_type = 0;
            scui_multi_lang_get(&lang_type);
            const char *lang_str = scui_multi_lang_str(unit_text, lang_type);
            
            snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit), "%s", lang_str);
            scui_ui_str_remove_discard_char(data_unit, '#');
            
            if (type == scui_ui_scene_mini_card_type_hr)
                scui_string_update_str(scui_ui_res_local->passby_hr, data_unit);
            if (type == scui_ui_scene_mini_card_type_spo2)
                scui_string_update_str(scui_ui_res_local->passby_spo2, data_unit);
            if (type == scui_ui_scene_mini_card_type_stress)
                scui_string_update_str(scui_ui_res_local->passby_stress, data_unit);
            
            break;
        }
        case scui_ui_scene_mini_card_type_weather: {
            
            scui_handle_t image = scui_presenter.get_temp_icon();
            scui_area_t image_clip = clip; image_clip.x += 346; image_clip.y += 69;
            scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            
            char data_unit[50] = {0};
            scui_handle_t unit_text = SCUI_HANDLE_INVALID;
            
            snprintf(data_unit, sizeof(data_unit), "%d %s",
                scui_presenter.get_temp_cur(),
                scui_presenter.get_temp_unit() ? "/C" : "/F");
            scui_string_update_str(scui_ui_res_local->data_weather_cur, data_unit);
            
            snprintf(data_unit, sizeof(data_unit), "%d/%d %s",
                scui_presenter.get_temp_min(),
                scui_presenter.get_temp_max(),
                scui_presenter.get_temp_unit() ? "/C" : "/F");
            scui_string_update_str(scui_ui_res_local->data_weather_lim, data_unit);
            
            break;
        }
        case scui_ui_scene_mini_card_type_alarm: {
            
            if (scui_presenter.alarm_none()) {
                scui_handle_t image = scui_image_prj_image_src_19_widget_01_arrowbmp;
                scui_area_t image_clip = clip; image_clip.x += 24; image_clip.y += 129 + 10;
                scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            } else {
                
                scui_handle_t image_on  = scui_image_prj_image_src_repeat_switch_01_alarmspng;
                scui_handle_t image_off = scui_image_prj_image_src_repeat_switch_05_offpng;
                scui_handle_t image_dot = scui_image_prj_image_src_repeat_switch_03_dotpng;
                
                if (scui_presenter.alarm_near_state()) {
                    scui_area_t image_on_clip = clip; image_on_clip.x += 316; image_on_clip.y += 71;
                    scui_widget_draw_image(event->object, &image_on_clip, image_on, NULL, SCUI_COLOR_UNUSED);
                    scui_area_t image_dot_clip = clip; image_dot_clip.x += 316 + 30; image_dot_clip.y += 71 + 3;
                    scui_widget_draw_image(event->object, &image_dot_clip, image_dot, NULL, SCUI_COLOR_UNUSED);
                } else {
                    scui_area_t image_on_clip = clip; image_on_clip.x += 316; image_on_clip.y += 71;
                    scui_widget_draw_image(event->object, &image_on_clip, image_on, NULL, SCUI_COLOR_UNUSED);
                    scui_area_t image_dot_clip = clip; image_dot_clip.x += 316 + 3; image_dot_clip.y += 71 + 3;
                    scui_widget_draw_image(event->object, &image_dot_clip, image_dot, NULL, SCUI_COLOR_UNUSED);
                }
                
                char data_unit[200] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d:%02d",
                    scui_presenter.alarm_near_hour(),
                    scui_presenter.alarm_near_minute());
                scui_string_update_str(scui_ui_res_local->data_alarm, data_unit);
                
                data_unit[0] = '\0';
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                
                if ((scui_presenter.alarm_near_date() & 0x1000) != 0)
                    snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit),
                        "%s", scui_multi_lang_str(SCUI_MULTI_LANG_0X00c5, lang_type));
                if ((scui_presenter.alarm_near_date() & 0x2000) != 0)
                    snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit),
                        "%s", scui_multi_lang_str(SCUI_MULTI_LANG_0X00f8, lang_type));
                if ((scui_presenter.alarm_near_date() & 0x8000) != 0)
                    snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit),
                        "%s", scui_multi_lang_str(SCUI_MULTI_LANG_0X00f2, lang_type));
                if ((scui_presenter.alarm_near_date() & 0x7f) == 0x7f)
                    snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit),
                        "%s", scui_multi_lang_str(SCUI_MULTI_LANG_0X00f1, lang_type));
                
                if (strlen(data_unit) == 0)
                if ((scui_presenter.alarm_near_date() & 0x7f) != 0) {
                    uint32_t text_week[] = {
                        SCUI_MULTI_LANG_0X006d, /* 周日 */
                        SCUI_MULTI_LANG_0X0067, /* 周一 */
                        SCUI_MULTI_LANG_0X0068, /* 周二 */
                        SCUI_MULTI_LANG_0X0069, /* 周三 */
                        SCUI_MULTI_LANG_0X006a, /* 周四 */
                        SCUI_MULTI_LANG_0X006b, /* 周五 */
                        SCUI_MULTI_LANG_0X006c, /* 周六 */
                    };
                    uint8_t week = scui_presenter.alarm_near_date();
                    
                    scui_multi_lang_type_t lang_type = 0;
                    scui_multi_lang_get(&lang_type);
                    
                    for (uint8_t idx = 0; idx < 7; idx++) {
                        if ((week & (1 << idx)) == 0)
                            continue;
                        snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit),
                            "%s ", scui_multi_lang_str(text_week[idx], lang_type));
                    }
                }
                
                scui_string_update_str(scui_ui_res_local->data_alarm_date, data_unit);
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_sleep: {
            
            scui_coord_t width = 354;
            
            if (scui_presenter.sleep_none()) {
                
                scui_handle_t image_l = scui_image_prj_image_src_22_sleep_home_bar_03_grey_00bmp;
                scui_handle_t image_r = scui_image_prj_image_src_22_sleep_home_bar_04_grey_01bmp;
                scui_area_t image_clip_l = clip; image_clip_l.y += 137; image_clip_l.x += 28;
                scui_area_t image_clip_r = clip; image_clip_r.y += 137; image_clip_r.x += 28 + width - scui_image_w(image_r);
                scui_widget_draw_image(event->object, &image_clip_l, image_l, NULL, SCUI_COLOR_UNUSED);
                scui_widget_draw_image(event->object, &image_clip_r, image_r, NULL, SCUI_COLOR_UNUSED);
                scui_area_t image_clip_c = clip; image_clip_c.y += 137; image_clip_c.x += 28 + scui_image_w(image_l);
                image_clip_c.h = scui_image_h(image_l); image_clip_c.w = width - scui_image_w(image_l) - scui_image_w(image_r);
                scui_widget_draw_color(event->object, &image_clip_c, SCUI_COLOR_MAKE32(false, 0x0, 0xFF6A6A6A));
                
            } else {
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d%s%02d%s",
                    scui_presenter.sleep_hour(),   scui_multi_lang_str(SCUI_MULTI_LANG_0X013b, lang_type),
                    scui_presenter.sleep_minute(), scui_multi_lang_str(SCUI_MULTI_LANG_0X013c, lang_type));
                scui_string_update_str(scui_ui_res_local->data_sleep, data_unit);
                
                uint32_t color4[4] = {
                    0xFF6A7FFF, 0xFF36B7FF,
                    0xFF00F0EC, 0xFFFF923E,
                };
                scui_handle_t image_l4[4] = {
                    scui_image_prj_image_src_22_sleep_home_bar_09_zi_00bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_01_blue_00bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_07_qing_00bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_05_prange_00bmp,
                };
                scui_handle_t image_r4[4] = {
                    scui_image_prj_image_src_22_sleep_home_bar_10_zi_01bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_02_blue_01bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_08_qing_01bmp,
                    scui_image_prj_image_src_22_sleep_home_bar_06_prange_01bmp,
                };
                
                uint8_t pct4[4] = {0};
                scui_coord_t pos_x4[4] = {0};
                scui_coord_t width4[4] = {0};
                scui_presenter.sleep_pct4(pct4);
                /* 计算宽度 */
                for (uint8_t idx = 0; idx < 4; idx++)
                    width4[idx] = width * pct4[idx] / 100;
                /* 计算宽度(对齐补足到第一个非零值) */
                for (uint8_t idx = 0; idx < 4; idx++) {
                    if (width4[idx] == 0)
                        continue;
                    scui_coord_t width_sum = 0;
                    for (uint8_t idx_sum = 0; idx_sum < 4; idx_sum++)
                        width_sum += width4[idx_sum];
                    width4[idx] += width - width_sum;
                }
                /* 计算坐标偏移 */
                pos_x4[0] = 28;
                for (uint8_t idx = 1; idx < 4; idx++)
                    pos_x4[idx] = pos_x4[idx - 1] + width4[idx - 1];
                
                
                /* 循环绘制四个条目即可 */
                for (uint8_t idx = 0; idx < 4; idx++) {
                    if (width4[idx] == 0)
                        continue;
                    
                    scui_handle_t image_l = image_l4[idx];
                    scui_handle_t image_r = image_r4[idx];
                    scui_area_t image_clip_l = clip; image_clip_l.y += 137;
                    scui_area_t image_clip_r = clip; image_clip_r.y += 137;
                    image_clip_l.x += pos_x4[idx];
                    image_clip_r.x += pos_x4[idx] + width4[idx] - scui_image_w(image_r);
                    scui_widget_draw_image(event->object, &image_clip_l, image_l, NULL, SCUI_COLOR_UNUSED);
                    scui_widget_draw_image(event->object, &image_clip_r, image_r, NULL, SCUI_COLOR_UNUSED);
                    scui_area_t image_clip_c = clip; image_clip_c.y += 137; image_clip_c.h = scui_image_h(image_l);
                    image_clip_c.x += pos_x4[idx] + scui_image_w(image_l);
                    image_clip_c.w = width4[idx] - scui_image_w(image_l) - scui_image_w(image_r);
                    scui_widget_draw_color(event->object, &image_clip_c, SCUI_COLOR_MAKE32(false, 0x0, color4[idx]));
                    
                }
                
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_sport_record: {
            
            if (scui_presenter.sport_record_none()) {
                
            } else {
                
                scui_handle_t text = scui_presenter.sport_record_type();
                scui_string_update_text(scui_ui_res_local->data_sport_record, text);
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d%s%02d%s",
                    scui_presenter.sport_record_hour(),   scui_multi_lang_str(SCUI_MULTI_LANG_0X013b, lang_type),
                    scui_presenter.sport_record_minute(), scui_multi_lang_str(SCUI_MULTI_LANG_0X013c, lang_type));
                scui_string_update_str(scui_ui_res_local->data_sport_record_time, data_unit);
                
                scui_handle_t image = scui_presenter.sport_record_icon();
                scui_area_t image_clip = clip; image_clip.x += 298; image_clip.y += 66;
                scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
                
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_women_health: {
            
            if (scui_presenter.women_health_none()) {
                
            } else {
                
                scui_handle_t text = scui_presenter.women_health_type();
                scui_string_update_text(scui_ui_res_local->data_women_health, text);
                
                scui_multi_lang_type_t lang_type = 0;
                scui_multi_lang_get(&lang_type);
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d%s%02d%s",
                    scui_presenter.women_health_hour(),   scui_multi_lang_str(SCUI_MULTI_LANG_0X013b, lang_type),
                    scui_presenter.women_health_minute(), scui_multi_lang_str(SCUI_MULTI_LANG_0X013c, lang_type));
                scui_string_update_str(scui_ui_res_local->data_women_health_time, data_unit);
                
                scui_handle_t image = scui_presenter.women_health_icon();
                scui_area_t image_clip = clip; image_clip.x += 298; image_clip.y += 66;
                scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
                
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_compass: {
            
            /* 指北针背景表图 */
            scui_handle_t image = scui_image_prj_image_src_19_widget_compass_02_bgbmp;
            scui_area_t image_clip = clip; image_clip.x += 298; image_clip.y += 73;
            scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            
            if (scui_presenter.compass_invalid()) {
                
                scui_handle_t image = scui_image_prj_image_src_19_widget_01_arrowbmp;
                scui_area_t image_clip = clip; image_clip.x += 24; image_clip.y += 136;
                scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            } else {
                
                scui_handle_t image = scui_image_prj_image_src_19_widget_compass_03_arrowpng;
                
                scui_point_t center = {
                    .x = scui_image_w(image) / 2,
                    .y = scui_image_h(image) / 2,
                };
                scui_point_t anchor = {
                    .x = clip.x + 326 + center.x,
                    .y = clip.y + 100 + center.y,
                };
                scui_widget_draw_image_rotate(event->object, NULL, image, NULL,
                    anchor, center, scui_presenter.compass_angle() * SCUI_SCALE_COF);
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%d %%",
                    scui_presenter.compass_angle());
                scui_string_update_str(scui_ui_res_local->data_compass, data_unit);
                
                struct
                {
                    uint16_t angle_s;   // 起始角度
                    uint16_t angle_e;   // 结束角度
                    uint16_t text;      // 文本
                } dir_table[] =
                {
                    // 注意:因为北区垮分界线,所以作为默认选项
                    { (315) + (45 / 2), (0)   + (45 / 2), SCUI_MULTI_LANG_0X011e },
                    { (0)   + (45 / 2), (45)  + (45 / 2), SCUI_MULTI_LANG_0X0123 },
                    { (45)  + (45 / 2), (90)  + (45 / 2), SCUI_MULTI_LANG_0X0121 },
                    { (90)  + (45 / 2), (135) + (45 / 2), SCUI_MULTI_LANG_0X0122 },
                    { (135) + (45 / 2), (180) + (45 / 2), SCUI_MULTI_LANG_0X0120 },
                    { (180) + (45 / 2), (225) + (45 / 2), SCUI_MULTI_LANG_0X0125 },
                    { (225) + (45 / 2), (270) + (45 / 2), SCUI_MULTI_LANG_0X011f },
                    { (270) + (45 / 2), (315) + (45 / 2), SCUI_MULTI_LANG_0X0124 }
                };
                
                uint16_t angle = scui_presenter.compass_angle();
                uint16_t text_handle = dir_table[0].text;
                for (uint8_t idx = 1; idx < scui_arr_len(dir_table); idx++)
                    if (dir_table[idx].angle_s <= angle &&
                        dir_table[idx].angle_e >  angle) {
                        text_handle = dir_table[idx].text;
                        break;
                    }
                
                scui_string_update_text(scui_ui_res_local->data_compass_info, text_handle);
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_altimeter: {
            
            char data_unit[50] = {0};
            
            snprintf(data_unit, sizeof(data_unit), "%.2f",
                scui_presenter.altimeter_pressure());
            scui_string_update_str(scui_ui_res_local->data_altimeter_info1, data_unit);
            
            snprintf(data_unit, sizeof(data_unit), "%.2f",
                scui_presenter.altimeter_altitude());
            scui_string_update_str(scui_ui_res_local->data_altimeter_info2, data_unit);
            
            scui_area_t color_clip = clip;
            color_clip.x += 204; color_clip.w = 2;
            color_clip.y +=  85; color_clip.h = 68;
            scui_widget_draw_color(event->object, &color_clip, SCUI_COLOR_MAKE32(false, 0x0, 0xFF4D4D4D));
            
            break;
        }
        case scui_ui_scene_mini_card_type_stopwatch: {
            
            scui_handle_t image_digit = scui_image_prj_image_src_num_44_white_24x34_04_03png;
            
            uint8_t char_digit[50] = {0};
            scui_coord_t digit_num = snprintf(char_digit, sizeof(char_digit), "%02d:%02d.%02d", 0, 0, 0);
            scui_area_t clip_digit = clip; clip_digit.x += 24; clip_digit.y += 69;
            scui_handle_t image_digit_list[10] = {0};
            for (scui_coord_t idx = 0; idx < digit_num; idx++) {
                if (char_digit[idx] >= '0' && char_digit[idx] <= '9')
                    image_digit_list[idx] = image_digit + char_digit[idx] - '0';
                if (char_digit[idx] == ':')
                    image_digit_list[idx] = image_digit + 10;
                if (char_digit[idx] == '.')
                    image_digit_list[idx] = image_digit - 2;
            }
            scui_custom_draw_image_text(event, &clip_digit, image_digit_list, SCUI_COLOR_FILTER_TRANS, 3, digit_num, 0);
            
            scui_handle_t image = scui_image_prj_image_src_19_widget_02_pausedbmp;
            scui_area_t image_clip = clip; image_clip.x += 298; image_clip.y += 66;
            scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            
            break;
        }
        case scui_ui_scene_mini_card_type_countdown: {
            
            char data_unit[50] = {0};
            uint8_t num[4] = {1, 2, 5, 10};
            scui_point_t image_clip4[4] = {
                [0] = {.x =  22, .y = 69, },
                [1] = {.x = 116, .y = 69, },
                [2] = {.x = 212, .y = 69, },
                [3] = {.x = 310, .y = 69, },
            };
            
            scui_multi_lang_type_t lang_type = 0;
            scui_multi_lang_get(&lang_type);
            const char *lang_str = scui_multi_lang_str(SCUI_MULTI_LANG_0X013c, lang_type);
            
            scui_handle_t image = scui_image_prj_image_src_19_widget_timer_01_bgbmp;
            for (uint8_t idx = 0; idx < scui_arr_len(image_clip4); idx++) {
                scui_area_t image_clip = clip;
                image_clip.x += image_clip4[idx].x;
                image_clip.y += image_clip4[idx].y;
                scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
                
                snprintf(data_unit, sizeof(data_unit), "%d %s", num[idx], lang_str);
                scui_string_update_str(scui_ui_res_local->data_countdown[idx], data_unit);
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_world_time: {
            
            /* 背景图 */
            scui_handle_t image = scui_image_prj_image_src_19_widget_02_bgbmp;
            scui_area_t image_clip = clip;
            image_clip.x += clip.w - scui_image_w(image);
            image_clip.y += clip.h - scui_image_h(image);
            scui_widget_draw_image(event->object, &image_clip, image, NULL, SCUI_COLOR_UNUSED);
            
            uint8_t world_time_cnt = 2;
            /* 在这里获取第一个要显示的世界时钟 */
            uint8_t world_time1_hour   = 12;
            uint8_t world_time1_minute = 00;
            char   *world_time1_str    = "New York";
            /* 在这里获取第二个要显示的世界时钟 */
            uint8_t world_time2_hour   = 8;
            uint8_t world_time2_minute = 32;
            char   *world_time2_str    = "London";
            
            if (world_time_cnt > 0) {
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d:%02d", world_time1_hour, world_time1_minute);
                scui_string_update_str(scui_ui_res_local->data_world_time1_time, data_unit);
                scui_string_update_str(scui_ui_res_local->data_world_time1_unit, world_time1_str);
            }
            
            if (world_time_cnt > 1) {
                
                char data_unit[50] = {0};
                snprintf(data_unit, sizeof(data_unit), "%02d:%02d", world_time2_hour, world_time2_minute);
                scui_string_update_str(scui_ui_res_local->data_world_time2_time, data_unit);
                scui_string_update_str(scui_ui_res_local->data_world_time2_unit, world_time2_str);
            }
            
            if (world_time_cnt > 1) {
                
                scui_area_t color_clip = clip;
                color_clip.x += 204; color_clip.w = 2;
                color_clip.y +=  85; color_clip.h = 68;
                scui_widget_draw_color(event->object, &color_clip, SCUI_COLOR_MAKE32(false, 0x0, 0xFF4D4D4D));
            }
            
            break;
        }
        
        default:
            break;
        }
        #endif
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_m_event_proc(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_m_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_handle_t  custom = linear_item.handle_s;
        scui_area_t  src_clip = scui_widget_clip(custom);
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        // 计算当前控件中心到父控件中心距离
        scui_area_t clip_p = scui_widget_clip(scui_widget_parent(event->object));
        scui_area_t clip_w = scui_widget_clip(event->object);
        
        #if 1
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
        offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
        
        scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
        scui_multi_t dist_y = (1024 * scui_min(rad_rr, offset.y) / rad_rr);
        scui_multi_t cos_a2 = (1024 * 1024) - dist_y * dist_y;
        scui_multi_t cos_ia = 0, cos_fa = 0;
        scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
        scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
        SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
        
        dist_x  = scui_min(dist_x, clip_p.w / 2);
        percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
        percent = scui_min(scui_max(percent, 30), 100);
        SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        
        scui_alpha_t alpha_raw = scui_alpha_cover;
        scui_widget_alpha_get(event->object, &alpha_raw);
        #if 0   // 更新alpha通道
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, scui_alpha_cover, true);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_UNUSED);
        scui_widget_alpha_set(event->object, alpha, true);
        #endif
        
        scui_point_t img_scale = {0};
        scui_opt_pos_t img_pos = scui_opt_pos_c;
        img_scale.x = 1024 * (scui_multi_t)percent / 100;
        img_scale.y = 1024 * (scui_multi_t)percent / 100;
        
        scui_widget_draw_image_scale(event->object, NULL, image, NULL, img_scale, img_pos);
        scui_widget_alpha_set(event->object, alpha_raw, true);
        #else
        clip_w.x += (clip_p.w - src_clip.w) / 2;
        scui_widget_draw_image(event->object, &clip_w, image, NULL, SCUI_COLOR_UNUSED);
        #endif
        
        scui_handle_clear(image);
        break;
    }
    case scui_event_ptr_click: {
        
        scui_alpha_t alpha = scui_alpha_trans;
        scui_widget_alpha_get(event->object, &alpha);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_event_mask_over(event);
        scui_handle_t parent = scui_widget_parent(event->object);
        scui_handle_t index  = scui_widget_child_to_index(parent, event->object) - 1;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_handle_t  custom = linear_item.handle_m;
        SCUI_LOG_WARN("click idx:%d", index);
        
        // 不是点击到中心子控件, 聚焦它
        scui_handle_t target = SCUI_HANDLE_INVALID;
        scui_scroll_center_get(parent, &target);
        if (target != event->object) {
            scui_scroll_center(parent, event->object, true);
            return;
        }
        
        // 重定向到类型
        scui_ui_scene_mini_card_type_t type = scui_ui_scene_mini_card_type[index];
        scui_area_t clip = scui_widget_clip(event->object);
        scui_point_t point_click = event->ptr_c;
        point_click.x -= clip.x + (clip.w - scui_widget_clip(custom).w) / 2;
        point_click.y -= clip.y + (clip.h - scui_widget_clip(custom).h) / 2;
        
        switch (type) {
        case scui_ui_scene_mini_card_type_music_control: {
            scui_area_t btn_clip_0 = {
                .w = 42,.x = 60,
                .h = 46,.y = 99,
            };
            scui_area_t btn_clip_1 = {
                .w = 90,.x = 160,
                .h = 90,.y = 77,
            };
            scui_area_t btn_clip_2 = {
                .w = 42,.x = 308,
                .h = 46,.y = 99,
            };
            if (scui_area_point(&btn_clip_0, &point_click)) {
                SCUI_LOG_WARN("click: |<");
                break;
            }
            if (scui_area_point(&btn_clip_1, &point_click)) {
                SCUI_LOG_WARN("click: |>");
                break;
            }
            if (scui_area_point(&btn_clip_2, &point_click)) {
                SCUI_LOG_WARN("click: >|");
                break;
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_hr:
        case scui_ui_scene_mini_card_type_spo2:
        case scui_ui_scene_mini_card_type_stress: {
            scui_area_t btn_clip = {
                .w = 88,.x = 298,
                .h = 88,.y = 66,
            };
            if (scui_area_point(&btn_clip, &point_click)) {
                SCUI_LOG_WARN("click: retry");
                break;
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_alarm: {
            
            if (scui_presenter.alarm_none()) {
                
                scui_area_t btn_clip = {
                    .w = 50,.x = 24,
                    .h = 40,.y = 129 + 10,
                };
                if (scui_area_point(&btn_clip, &point_click)) {
                    SCUI_LOG_WARN("click: alarm none");
                    break;
                }
            } else {
                
                scui_area_t btn_clip = {
                    .w = 70,.x = 316,
                    .h = 42,.y = 71,
                };
                if (scui_area_point(&btn_clip, &point_click)) {
                    SCUI_LOG_WARN("click: alarm onoff");
                    break;
                }
            }
            
            break;
        }
        case scui_ui_scene_mini_card_type_countdown: {
            
            for (uint8_t idx = 0; idx < 4; idx++) {
                
                scui_area_t btn_clip4[4] = {
                    [0] = {.x =  22, .y = 69, .w = 82, .h = 82, },
                    [1] = {.x = 116, .y = 69, .w = 82, .h = 82, },
                    [2] = {.x = 212, .y = 69, .w = 82, .h = 82, },
                    [3] = {.x = 310, .y = 69, .w = 82, .h = 82, },
                };
                if (scui_area_point(&btn_clip4[idx], &point_click)) {
                    SCUI_LOG_WARN("click: countdown %d", idx);
                    break;
                }
            }
            
            break;
        }
        
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_mini_card_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle             = SCUI_HANDLE_INVALID;
        custom_maker.widget.type                = scui_widget_type_custom;
        custom_maker.widget.style.sched_anima   = true;
        custom_maker.widget.clip.w              = SCUI_HOR_RES;
        custom_maker.widget.parent              = event->object;
        
        // 上半部分留白占用
        custom_maker.widget.style.indev_ptr     = false;
        custom_maker.widget.clip.h              = SCUI_VER_RES / 2 - 10 - 180 / 2;
        custom_maker.widget.event_cb            = NULL;
        scui_widget_create(&custom_maker, &custom_handle);
        
        // list的各个子控件
        custom_maker.widget.style.indev_ptr     = true;
        custom_maker.widget.clip.h              = 180;
        custom_maker.widget.event_cb            = scui_ui_scene_item_m_event_proc;
        
        // 创建子控件(主)
        scui_custom_maker_t custom_m_maker = custom_maker;
        scui_handle_t custom_m_handle = SCUI_HANDLE_INVALID;
        // 创建子控件(从)
        scui_custom_maker_t custom_s_maker = {0};
        scui_handle_t custom_s_handle = SCUI_HANDLE_INVALID;
        custom_s_maker.widget.type        = scui_widget_type_custom;
        custom_s_maker.widget.clip.w      = 410;
        custom_s_maker.widget.clip.h      = 180;
        custom_s_maker.widget.child_num   = 5;
        custom_s_maker.widget.event_cb    = scui_ui_scene_item_s_event_proc;
        
        for (uint8_t idx = 0; idx < scui_ui_scene_mini_card_num; idx++) {
            // 创建子控件(主)(从)
            scui_widget_create(&custom_m_maker, &custom_m_handle);
            scui_widget_create(&custom_s_maker, &custom_s_handle);
            // 绑定子控件(主)(从)
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            linear_item.handle_m = custom_m_handle;
            linear_item.handle_s = custom_s_handle;
            scui_linear_item_sets(event->object, &linear_item);
            // 绑定子控件(从)
            scui_handle_t *handle_m = NULL;
            scui_custom_handle_m(custom_s_handle, &handle_m);
            *handle_m = linear_item.handle_m;
        }
        
        // 下半部分留白占用
        custom_maker.widget.style.indev_ptr     = false;
        custom_maker.widget.clip.h              = SCUI_VER_RES / 2 - 10 - 180 / 2;
        custom_maker.widget.event_cb            = NULL;
        scui_widget_create(&custom_maker, &custom_handle);
        
        
        
        // list的各个子控件树
        for (uint8_t idx = 0; idx < scui_ui_scene_mini_card_num; idx++) {
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            scui_linear_item_gets(event->object, &linear_item);
            custom_handle = linear_item.handle_s;
            
            
            
            scui_string_maker_t string_maker = {0};
            scui_handle_t string_handle     = SCUI_HANDLE_INVALID;
            string_maker.widget.type        = scui_widget_type_string;
            string_maker.widget.parent      = custom_handle;
            string_maker.widget.clip.w      = custom_maker.widget.clip.w;
            string_maker.widget.clip.h      = custom_maker.widget.clip.h;
            string_maker.args.align_hor     = 2;
            string_maker.args.align_ver     = 2;
            string_maker.args.color.filter  = true;
            // string_maker.draw_cache         = true;
            string_maker.font_idx           = SCUI_FONT_IDX_32;
            
            // 特定卡片给自己添加额外控件
            scui_ui_scene_mini_card_type_t type = scui_ui_scene_mini_card_type[idx];
            switch (type) {
            case scui_ui_scene_mini_card_type_music_control: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                string_t_maker.text = SCUI_MULTI_LANG_0X0176;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_ui_res_local->title_music = string_handle;
                break;
            }
            case scui_ui_scene_mini_card_type_hr:
            case scui_ui_scene_mini_card_type_spo2:
            case scui_ui_scene_mini_card_type_stress: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                
                if (type == scui_ui_scene_mini_card_type_hr) {
                    string_t_maker.args.color.color_s.full = 0xFFFF0000;
                    string_t_maker.args.color.color_e.full = 0xFFFF0000;
                    string_t_maker.text = SCUI_MULTI_LANG_0X0019;
                }
                if (type == scui_ui_scene_mini_card_type_spo2) {
                    string_t_maker.args.color.color_s.full = 0xFFFF0000;
                    string_t_maker.args.color.color_e.full = 0xFFFF0000;
                    string_t_maker.text = SCUI_MULTI_LANG_0X0040;
                }
                if (type == scui_ui_scene_mini_card_type_stress) {
                    string_t_maker.args.color.color_s.full = 0xFF00F4EA;
                    string_t_maker.args.color.color_e.full = 0xFF00F4EA;
                    string_t_maker.text = SCUI_MULTI_LANG_0X0049;
                }
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                string_t_maker.widget.clip.y = 69;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                
                if (type == scui_ui_scene_mini_card_type_hr)
                    scui_ui_res_local->data_hr = string_handle;
                if (type == scui_ui_scene_mini_card_type_spo2)
                    scui_ui_res_local->data_spo2 = string_handle;
                if (type == scui_ui_scene_mini_card_type_stress)
                    scui_ui_res_local->data_stress = string_handle;
                
                string_t_maker.widget.clip.y = 129;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                
                if (type == scui_ui_scene_mini_card_type_hr)
                    scui_ui_res_local->passby_hr = string_handle;
                if (type == scui_ui_scene_mini_card_type_spo2)
                    scui_ui_res_local->passby_spo2 = string_handle;
                if (type == scui_ui_scene_mini_card_type_stress)
                    scui_ui_res_local->passby_stress = string_handle;
                
                break;
            }
            case scui_ui_scene_mini_card_type_weather: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFF06AEFF;
                string_t_maker.args.color.color_e.full = 0xFF06AEFF;
                string_t_maker.text = SCUI_MULTI_LANG_0X0092;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                string_t_maker.widget.clip.y = 69;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_weather_cur = string_handle;
                
                string_t_maker.widget.clip.y = 129;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_weather_lim = string_handle;
                
                break;
            }
            case scui_ui_scene_mini_card_type_alarm: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFFDE8926;
                string_t_maker.args.color.color_e.full = 0xFFDE8926;
                string_t_maker.text = SCUI_MULTI_LANG_0X00e6;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                if (scui_presenter.alarm_none()) {
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    string_t_maker.text = SCUI_MULTI_LANG_0X00f0;
                    scui_widget_create(&string_t_maker, &string_handle);
                    
                    #if 1
                    scui_handle_t image = scui_image_prj_image_src_repeat_arrow_01_backbmp;
                    
                    string_t_maker.widget.clip.x += scui_image_w(image) + 10;
                    string_t_maker.widget.clip.y  = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    string_t_maker.text = SCUI_MULTI_LANG_0X00e7;
                    scui_widget_create(&string_t_maker, &string_handle);
                    #endif
                } else {
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_alarm = string_handle;
                    
                    string_t_maker.widget.clip.y = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_alarm_date = string_handle;
                    
                }
                
                break;
            }
            case scui_ui_scene_mini_card_type_sleep: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFF06E2ED;
                string_t_maker.args.color.color_e.full = 0xFF06E2ED;
                string_t_maker.text = SCUI_MULTI_LANG_0X004f;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                if (scui_presenter.sleep_none()) {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    string_t_maker.text = SCUI_MULTI_LANG_0X0051;
                    scui_widget_create(&string_t_maker, &string_handle);
                    
                } else {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_sleep = string_handle;
                    
                }
                
                break;
            }
            case scui_ui_scene_mini_card_type_sport_record: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFFF83600;
                string_t_maker.args.color.color_e.full = 0xFFF83600;
                string_t_maker.text = SCUI_MULTI_LANG_0X01c5;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                if (scui_presenter.sport_record_none()) {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    string_t_maker.text = SCUI_MULTI_LANG_0X01c4;
                    scui_widget_create(&string_t_maker, &string_handle);
                    
                } else {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_sport_record = string_handle;
                    
                    string_t_maker.widget.clip.y = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_sport_record_time = string_handle;
                    
                }
                
                break;
            }
            case scui_ui_scene_mini_card_type_women_health: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFFD83371;
                string_t_maker.args.color.color_e.full = 0xFFD83371;
                string_t_maker.text = SCUI_MULTI_LANG_0X0110;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                if (scui_presenter.women_health_none()) {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    string_t_maker.text = SCUI_MULTI_LANG_0X01c4;
                    scui_widget_create(&string_t_maker, &string_handle);
                    
                } else {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_women_health = string_handle;
                    
                    string_t_maker.widget.clip.y = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_women_health_time = string_handle;
                    
                }
                
                break;
            }
            case scui_ui_scene_mini_card_type_compass: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFFFD2627;
                string_t_maker.args.color.color_e.full = 0xFFFD2627;
                string_t_maker.text = SCUI_MULTI_LANG_0X011c;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                if (scui_presenter.compass_invalid()) {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "--");
                    
                    #if 1
                    scui_handle_t image = scui_image_prj_image_src_repeat_arrow_01_backbmp;
                    
                    string_t_maker.widget.clip.x += scui_image_w(image) + 10;
                    string_t_maker.widget.clip.y = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    string_t_maker.text = SCUI_MULTI_LANG_0X00f6;
                    scui_widget_create(&string_t_maker, &string_handle);
                    #endif
                } else {
                    
                    string_t_maker.widget.clip.y = 69;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_compass = string_handle;
                    
                    string_t_maker.widget.clip.y = 129;
                    string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                    string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_compass_info = string_handle;
                    
                }
                
                break;
            }
            case scui_ui_scene_mini_card_type_altimeter: {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFF98FF02;
                string_t_maker.args.color.color_e.full = 0xFF98FF02;
                string_t_maker.text = SCUI_MULTI_LANG_0X012a;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                string_t_maker.widget.clip.x = 36;
                string_t_maker.widget.clip.y = 77;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_altimeter_info1 = string_handle;
                
                string_t_maker.widget.clip.x = 410 / 2 + 36;
                string_t_maker.widget.clip.y = 77;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_altimeter_info2 = string_handle;
                
                string_t_maker.widget.clip.x = 36;
                string_t_maker.widget.clip.y = 127;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.text = SCUI_MULTI_LANG_0X0127;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                
                string_t_maker.widget.clip.x = 410 / 2 + 36;
                string_t_maker.widget.clip.y = 127;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.text = SCUI_MULTI_LANG_0X0126;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                
                break;
            }
            case scui_ui_scene_mini_card_type_stopwatch : {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFF00E85E;
                string_t_maker.args.color.color_e.full = 0xFF00E85E;
                string_t_maker.text = SCUI_MULTI_LANG_0X013f;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                
                
                break;
            }
            case scui_ui_scene_mini_card_type_countdown : {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFFDF7331;
                string_t_maker.args.color.color_e.full = 0xFFDF7331;
                string_t_maker.text = SCUI_MULTI_LANG_0X0139;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                #if 1
                scui_handle_t image = scui_image_prj_image_src_19_widget_timer_01_bgbmp;
                scui_coord_t  image_w = scui_image_w(image);
                scui_coord_t  image_h = scui_image_h(image);
                scui_area_t   image_clip4[4] = {
                    [0] = {.x =  22 + (10) / 2, .y = 69 + image_h / 2 - 36 / 2, .w = (image_w - 10), .h = 36,},
                    [1] = {.x = 116 + (10) / 2, .y = 69 + image_h / 2 - 36 / 2, .w = (image_w - 10), .h = 36,},
                    [2] = {.x = 212 + (10) / 2, .y = 69 + image_h / 2 - 36 / 2, .w = (image_w - 10), .h = 36,},
                    [3] = {.x = 310 + (10) / 2, .y = 69 + image_h / 2 - 36 / 2, .w = (image_w - 10), .h = 36,},
                };
                for (uint8_t idx = 0; idx < 4; idx++) {
                    string_t_maker.widget.clip = image_clip4[idx];
                    string_t_maker.args.align_hor = 2;
                    string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                    string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                    string_t_maker.font_idx = 0;
                    scui_widget_create(&string_t_maker, &string_handle);
                    scui_string_update_str(string_handle, "");
                    scui_ui_res_local->data_countdown[idx] = string_handle;
                }
                
                #endif
                
                break;
            }
            case scui_ui_scene_mini_card_type_world_time : {
                
                scui_string_maker_t string_t_maker = string_maker;
                string_t_maker.widget.clip.x = 24;
                string_t_maker.widget.clip.y = 22;
                string_t_maker.widget.clip.w = 410 - 24 * 2;
                string_t_maker.widget.clip.h = 40;
                string_t_maker.args.align_hor = 0;
                string_t_maker.args.color.color_s.full = 0xFF1977FF;
                string_t_maker.args.color.color_e.full = 0xFF1977FF;
                string_t_maker.text = SCUI_MULTI_LANG_0X0140;
                
                scui_widget_create(&string_t_maker, &string_handle);
                
                string_t_maker.widget.clip.x = 36;
                string_t_maker.widget.clip.y = 77;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_world_time1_time = string_handle;
                
                string_t_maker.widget.clip.x = 410 / 2 + 36;
                string_t_maker.widget.clip.y = 77;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_t_maker.args.color.color_e.full = 0xFFFFFFFF;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_world_time2_time = string_handle;
                
                string_t_maker.widget.clip.x = 36;
                string_t_maker.widget.clip.y = 127;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_world_time1_unit = string_handle;
                
                string_t_maker.widget.clip.x = 410 / 2 + 36;
                string_t_maker.widget.clip.y = 127;
                string_t_maker.widget.clip.w = 410 / 2 - 36 * 2;
                string_t_maker.widget.clip.h = 36;
                string_t_maker.args.align_hor = 2;
                string_t_maker.args.color.color_s.full = 0xFF4D4D4D;
                string_t_maker.args.color.color_e.full = 0xFF4D4D4D;
                scui_widget_create(&string_t_maker, &string_handle);
                scui_string_update_str(string_handle, "");
                scui_ui_res_local->data_world_time2_unit = string_handle;
                
                break;
            }
            
            default:
                break;
            }
        }
        
        scui_ui_res_local->linear = event->object;
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_MINI_CARD_BAR_ARC;
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
void scui_ui_scene_mini_card_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 做个缓速 */
        static uint32_t span_fps = 7;
        static uint32_t tick_cnt = 0;
        
        tick_cnt += event->tick;
        if (tick_cnt < 1000 / span_fps) return;
        else tick_cnt -= 1000 / span_fps;
        
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        // 清空图像资源缓存
        scui_image_cache_clear();
        scui_ui_scene_mini_card_cfg();
        
        // 销毁该占用控件, 为list控件留出位置
        scui_linear_maker_t linear_maker = {0};
        scui_handle_t linear_handle = SCUI_HANDLE_INVALID;
        linear_maker.widget.type = scui_widget_type_linear;
        linear_maker.widget.style.fully_bg  = true;
        linear_maker.widget.style.indev_enc = true;
        linear_maker.widget.style.indev_key = true;
        linear_maker.widget.clip.w = SCUI_HOR_RES;
        linear_maker.widget.clip.h = SCUI_VER_RES;
        linear_maker.widget.parent = event->object;
        linear_maker.widget.event_cb   = scui_ui_scene_mini_card_scroll_event;
        linear_maker.widget.child_num  = 50;
        linear_maker.scroll.pos        = scui_opt_pos_c;
        linear_maker.scroll.dir        = scui_opt_dir_ver;
        linear_maker.scroll.space      = 3;
        linear_maker.scroll.route_enc  = 117;
        linear_maker.scroll.route_key  = 117;
        linear_maker.scroll.keyid_fdir = SCUI_WIDGET_SCROLL_KEY_FDIR;
        linear_maker.scroll.keyid_bdir = SCUI_WIDGET_SCROLL_KEY_BDIR;
        linear_maker.scroll.springback = 70;
        linear_maker.list_num = scui_ui_scene_mini_card_num;
        scui_widget_create(&linear_maker, &linear_handle);
        break;
    }
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
