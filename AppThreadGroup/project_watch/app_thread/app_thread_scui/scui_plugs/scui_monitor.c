/*实现目标:
 *    监视器:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t window;
    scui_handle_t srting_refr;
    scui_handle_t srting_mem1;
    scui_handle_t srting_mem2;
    scui_handle_t srting_cache;
} scui_monitor = {0};

#define SCUI_MEM_TYPE_NUM     4
#define SCUI_CACHE_TYPE_NUM   3

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_monitor_event_cb(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        bool redraw_tag = false;
        
        #if 1 // srting_refr
        scui_tick_stat_t tick_stat = {0};
        scui_tick_stat_rcd(&tick_stat);
        
        if (tick_stat.sched_sum >  tick_stat.draw_sum)
            tick_stat.sched_sum -= tick_stat.draw_sum;
        else
            tick_stat.sched_sum = 0;
        
        static uint64_t refr_us_last = 0;
        uint64_t elapse_us = scui_tick_us() - refr_us_last;
        if (elapse_us > 1000 * 1000) {
            
            uint8_t  str_refr[100] = {0};
            uint32_t val_args[5] = {
                100 * tick_stat.sched_sum   / elapse_us,
                100 * tick_stat.draw_sum    / elapse_us,
                100 * tick_stat.draw_sum_sw / elapse_us,
                100 * tick_stat.draw_sum_hw / elapse_us,
                tick_stat.refr_fps,
            };
            snprintf(str_refr, sizeof(str_refr) - 1,
                    "Sched:#-%d%%-#, Draw:(#-%d%%-#,#-%d%%-#,#-%d%%-#), Fps:#-%d-#",
                     val_args[0], val_args[1], val_args[2], val_args[3], val_args[4]);
            
            uint32_t color_g = 0xFF008000;
            uint32_t color_y = 0xFF808000;
            uint32_t color_r = 0xFF800000;
            
            scui_color_t recolors[] = {
                {   .filter = true,
                    .color_s.full = val_args[0] > 80 ? color_r : val_args[0] > 60 ? color_y : color_g,
                    .color_e.full = val_args[0] > 80 ? color_r : val_args[0] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = val_args[1] > 80 ? color_r : val_args[1] > 60 ? color_y : color_g,
                    .color_e.full = val_args[1] > 80 ? color_r : val_args[1] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = val_args[2] > 80 ? color_r : val_args[2] > 60 ? color_y : color_g,
                    .color_e.full = val_args[2] > 80 ? color_r : val_args[2] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = val_args[3] > 80 ? color_r : val_args[3] > 60 ? color_y : color_g,
                    .color_e.full = val_args[3] > 80 ? color_r : val_args[3] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = val_args[4] > 40 ? color_g : val_args[4] > 30 ? color_y : color_r,
                    .color_e.full = val_args[4] > 40 ? color_g : val_args[4] > 30 ? color_y : color_r,},
            };
            scui_string_update_str_rec(scui_monitor.srting_refr, str_refr, scui_arr_len(recolors), recolors);
            redraw_tag = true;
            
            refr_us_last = scui_tick_us();
            scui_tick_stat(scui_tick_stat_reset);
        }
        #endif
        
        #if 1 // srting_mem
        uintptr_t mem_total[SCUI_MEM_TYPE_NUM] = {
            scui_mem_size_total(scui_mem_type_mix),
            scui_mem_size_total(scui_mem_type_font),
            scui_mem_size_total(scui_mem_type_graph),
            scui_mem_size_total(scui_mem_type_user),
        };
        uintptr_t mem_used[SCUI_MEM_TYPE_NUM] = {
            scui_mem_size_used(scui_mem_type_mix),
            scui_mem_size_used(scui_mem_type_font),
            scui_mem_size_used(scui_mem_type_graph),
            scui_mem_size_used(scui_mem_type_user),
        };
        uintptr_t mem_frag[SCUI_MEM_TYPE_NUM] = {
            0, // scui_mem_size_frag(scui_mem_type_mix),
            0, // scui_mem_size_frag(scui_mem_type_font),
            scui_mem_size_frag(scui_mem_type_graph),
            0, // scui_mem_size_frag(scui_mem_type_user),
        };
        
        static uintptr_t mem_used_rcd[SCUI_MEM_TYPE_NUM] = {0};
        if (scui_dist(mem_used[0], mem_used_rcd[0]) > 1024 * 2  ||
            scui_dist(mem_used[1], mem_used_rcd[1]) > 1024 * 8  ||
            scui_dist(mem_used[2], mem_used_rcd[2]) > 1024 * 16 ||
            scui_dist(mem_used[3], mem_used_rcd[3]) > 1024 * 2) {
            
            for (uint8_t idx = 0; idx < scui_arr_len(mem_used); idx++)
                mem_used_rcd[idx] = mem_used[idx];
            
            float mem_total_f[SCUI_MEM_TYPE_NUM] = {0};
            mem_total_f[0] = (float)mem_total[0] / 1024 / 1024;
            mem_total_f[1] = (float)mem_total[1] / 1024 / 1024;
            mem_total_f[2] = (float)mem_total[2] / 1024 / 1024;
            mem_total_f[3] = (float)mem_total[3] / 1024 / 1024;
            
            float mem_used_f[SCUI_MEM_TYPE_NUM] = {0};
            mem_used_f[0] = (float)mem_used[0] / 1024;
            mem_used_f[1] = (float)mem_used[1] / 1024;
            mem_used_f[2] = (float)mem_used[2] / 1024 / 1024;
            mem_used_f[3] = (float)mem_used[3] / 1024;
            
            float mem_frag_f[SCUI_MEM_TYPE_NUM] = {0};
            mem_frag_f[0] = (float)mem_frag[0] / 1024;
            mem_frag_f[1] = (float)mem_frag[1] / 1024;
            mem_frag_f[2] = (float)mem_frag[2] / 1024 / 1024;
            mem_frag_f[3] = (float)mem_frag[3] / 1024;
            
            uint32_t pct_args[SCUI_MEM_TYPE_NUM] = {0};
            for (uint8_t idx = 0; idx < scui_arr_len(mem_used); idx++)
                pct_args[idx] = mem_total[idx] == 0 ? 0 : mem_used[idx] * 100 / mem_total[idx];
            
            uint8_t str_mem1[100] = {0};
            uint8_t str_mem2[100] = {0};
            snprintf(str_mem1, sizeof(str_mem1) - 1,
                "Mix:#-%d%%:%.02fK-# User:#-%d%%:%.02fK-#",
                     pct_args[0], mem_used_f[0], pct_args[3], mem_used_f[3]);
            snprintf(str_mem2, sizeof(str_mem2) - 1,
                "Font:#-%d%%:%.02fK-# Graph:#-%d%%:%.02fM-# Frag:#-%.03fM-#",
                     pct_args[1], mem_used_f[1], pct_args[2], mem_used_f[2], mem_frag_f[2]);
            
            uint32_t color_g = 0xFF008000;
            uint32_t color_y = 0xFF808000;
            uint32_t color_r = 0xFF800000;
            
            scui_color_t recolors1[2] = {
                {   .filter = true,
                    .color_s.full = pct_args[0] > 80 ? color_r : pct_args[0] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[0] > 80 ? color_r : pct_args[0] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = pct_args[3] > 80 ? color_r : pct_args[3] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[3] > 80 ? color_r : pct_args[3] > 60 ? color_y : color_g,},
            };
            scui_color_t recolors2[3] = {
                {   .filter = true,
                    .color_s.full = pct_args[1] > 80 ? color_r : pct_args[1] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[1] > 80 ? color_r : pct_args[1] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,},
            };
            scui_string_update_str_rec(scui_monitor.srting_mem1, str_mem1, scui_arr_len(recolors1), recolors1);
            scui_string_update_str_rec(scui_monitor.srting_mem2, str_mem2, scui_arr_len(recolors2), recolors2);
            redraw_tag = true;
        }
        #endif
        
        #if 1 // srting_cache
        if (redraw_tag) {
            uint32_t cache_rcd[SCUI_CACHE_TYPE_NUM * 2] = {0};
            scui_cache_font_usage(&cache_rcd[0]);
            scui_cache_font_nodes(&cache_rcd[1]);
            scui_cache_glyph_usage(&cache_rcd[2]);
            scui_cache_glyph_nodes(&cache_rcd[3]);
            scui_cache_image_usage(&cache_rcd[4]);
            scui_cache_image_nodes(&cache_rcd[5]);
            
            uintptr_t mem_total[SCUI_CACHE_TYPE_NUM] = {
                SCUI_CACHE_TOTAL_FONT,
                SCUI_CACHE_TOTAL_GLYPH,
                SCUI_CACHE_TOTAL_IMAGE,
            };
            uintptr_t mem_used[SCUI_CACHE_TYPE_NUM] = {
                cache_rcd[0], cache_rcd[2], cache_rcd[4],
            };
            
            float mem_total_f[SCUI_CACHE_TYPE_NUM] = {0};
            mem_total_f[0] = (float)mem_total[0] / 1024 / 1024;
            mem_total_f[1] = (float)mem_total[1] / 1024 / 1024;
            mem_total_f[2] = (float)mem_total[2] / 1024 / 1024;
            
            float mem_used_f[SCUI_CACHE_TYPE_NUM] = {0};
            mem_used_f[0] = (float)mem_used[0] / 1024;
            mem_used_f[1] = (float)mem_used[1] / 1024;
            mem_used_f[2] = (float)mem_used[2] / 1024 / 1024;
            
            uint32_t pct_args[SCUI_CACHE_TYPE_NUM] = {0};
            for (uint8_t idx = 0; idx < scui_arr_len(mem_used); idx++)
                pct_args[idx] = mem_total[idx] == 0 ? 0 : mem_used[idx] * 100 / mem_total[idx];
            
            #if 0
            uint8_t str_cache[100] = {0};
            snprintf(str_cache, sizeof(str_cache) - 1,
                "Font:#-%d,%d%%:%.02fK-# Glyph:#-%d,%d%%:%.02fK-# Graph:#-%d,%d%%:%.02fM-#",
                     cache_rcd[1], pct_args[0], mem_used_f[0],
                     cache_rcd[3], pct_args[1], mem_used_f[1],
                     cache_rcd[5], pct_args[2], mem_used_f[2]);
            #else
            uint8_t str_cache[100] = {0};
            snprintf(str_cache, sizeof(str_cache) - 1,
                "Font:#-%u,%.02fK-# Glyph:#-%d,%.02fK-# Graph:#-%d,%.02fM-#",
                     cache_rcd[1], mem_used_f[0],
                     cache_rcd[3], mem_used_f[1],
                     cache_rcd[5], mem_used_f[2]);
            #endif
            
            uint32_t color_g = 0xFF008000;
            uint32_t color_y = 0xFF808000;
            uint32_t color_r = 0xFF800000;
            
            scui_color_t recolors[3] = {
                {   .filter = true,
                    .color_s.full = pct_args[0] > 80 ? color_r : pct_args[0] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[0] > 80 ? color_r : pct_args[0] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = pct_args[1] > 80 ? color_r : pct_args[1] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[1] > 80 ? color_r : pct_args[1] > 60 ? color_y : color_g,},
                {   .filter = true,
                    .color_s.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,
                    .color_e.full = pct_args[2] > 80 ? color_r : pct_args[2] > 60 ? color_y : color_g,},
            };
            
            scui_string_update_str_rec(scui_monitor.srting_cache, str_cache, scui_arr_len(recolors), recolors);
        }
        #endif
        
        // 怪事???
        if (redraw_tag) {
            scui_widget_draw(scui_monitor.srting_refr,  NULL, false);
            scui_widget_draw(scui_monitor.srting_mem1,  NULL, false);
            scui_widget_draw(scui_monitor.srting_mem2,  NULL, false);
            scui_widget_draw(scui_monitor.srting_cache, NULL, false);
        }
        
        break;
    }
    case scui_event_create: {
        
        /* 创建显示信息文本,都是英文 */
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        string_maker.widget.type                = scui_widget_type_string;
        string_maker.widget.parent              = event->object;
        string_maker.args.recolor               = true;
        string_maker.args.align_hor             = 0;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFF404040;
        string_maker.args.color.color_e.full    = 0xFF404040;
        string_maker.args.color.filter          = true;
        string_maker.args.line_width            = 1;
        string_maker.args.line_under            = 1;
        string_maker.args.line_delete           = 1;
        string_maker.args.lang                  = scui_lang_type_ascii;
        string_maker.font_idx                   = SCUI_FONT_IDX_A16;
        
        string_maker.widget.clip.x              = 10;
        string_maker.widget.clip.y              = 30;
        string_maker.widget.clip.w              = SCUI_HOR_RES;
        string_maker.widget.clip.h              = 20;
        scui_widget_create(&string_maker, &string_handle);
        scui_monitor.srting_refr = string_handle;
        string_maker.widget.clip.x              = 10;
        string_maker.widget.clip.y              = 50;
        string_maker.widget.clip.w              = SCUI_HOR_RES;
        string_maker.widget.clip.h              = 20;
        scui_widget_create(&string_maker, &string_handle);
        scui_monitor.srting_mem1 = string_handle;
        string_maker.widget.clip.x              = 10;
        string_maker.widget.clip.y              = 70;
        string_maker.widget.clip.w              = SCUI_HOR_RES;
        string_maker.widget.clip.h              = 20;
        scui_widget_create(&string_maker, &string_handle);
        scui_monitor.srting_mem2 = string_handle;
        string_maker.widget.clip.x              = 10;
        string_maker.widget.clip.y              = 90;
        string_maker.widget.clip.w              = SCUI_HOR_RES;
        string_maker.widget.clip.h              = 20;
        scui_widget_create(&string_maker, &string_handle);
        scui_monitor.srting_cache = string_handle;
        
        scui_string_update_str(scui_monitor.srting_refr,  "srting_refr");
        scui_string_update_str(scui_monitor.srting_mem1,  "mem");
        scui_string_update_str(scui_monitor.srting_mem2,  "mem");
        scui_string_update_str(scui_monitor.srting_cache, "srting_cache");
        break;
    }
    case scui_event_draw:
        #if SCUI_MEM_FEAT_MINI == 0
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_ZEROED);
        #endif
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}

/*@brief 显示监视器
 *@param buffer 开辟buffer缓存
 */
void scui_monitor_show(bool buffer)
{
    if (scui_monitor.window == SCUI_HANDLE_INVALID) {
        
        scui_window_maker_t window_maker = {0};
        scui_handle_t window_handle = SCUI_HANDLE_INVALID;
        window_maker.widget.type = scui_widget_type_window;
        window_maker.widget.style.sched_anima = true;
        window_maker.widget.clip.x = 0;
        window_maker.widget.clip.y = SCUI_VER_RES - 120;
        window_maker.widget.clip.w = SCUI_HOR_RES;
        window_maker.widget.clip.h = 120;
        window_maker.widget.event_cb  = scui_monitor_event_cb;
        window_maker.widget.child_num = 10;
        window_maker.level    = 10;
        window_maker.buffer   = buffer;
        window_maker.resident = true;
        window_maker.format   =  SCUI_PIXEL_CF_DEF_A;
        scui_widget_create(&window_maker, &window_handle);
        scui_widget_show(window_handle, true);
        scui_monitor.window = window_handle;
    }
}

/*@brief 隐藏监视器
 */
void scui_monitor_hide(void)
{
    if (scui_monitor.window != SCUI_HANDLE_INVALID) {
        
        scui_widget_hide(scui_monitor.window, true);
        scui_monitor.window = SCUI_HANDLE_INVALID;
    }
}
