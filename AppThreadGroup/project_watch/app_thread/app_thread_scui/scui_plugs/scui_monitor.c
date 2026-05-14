/*实现目标:
 *    监视器:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t window;
    scui_handle_t srting_refr;
    scui_handle_t srting_mem;
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
        
        bool update_refr = false;
        bool update_mem  = false;
        uint32_t color_g = 0xFF008000;
        uint32_t color_y = 0xFF808000;
        uint32_t color_r = 0xFF800000;
        
        float mem_used[SCUI_MEM_TYPE_NUM][2] = {
            {(float)scui_mem_size_used(scui_mem_type_mix)   / 1024,          2.0f * 1024 / 1024,},
            {(float)scui_mem_size_used(scui_mem_type_font)  / 1024,          8.0f * 1024 / 1024,},
            {(float)scui_mem_size_used(scui_mem_type_graph) / 1024 / 1024,  16.0f * 1024 / 1024 / 1024,},
            {(float)scui_mem_size_used(scui_mem_type_user)  / 1024,          2.0f * 1024 / 1024,},
        };
        float mem_frag[SCUI_MEM_TYPE_NUM] = {
            0, // (float)scui_mem_size_frag(scui_mem_type_mix)  / 1024,
            0, // (float)scui_mem_size_frag(scui_mem_type_font) / 1024,
            (float)scui_mem_size_frag(scui_mem_type_graph) / 1024 / 1024,
            0, // (float)scui_mem_size_frag(scui_mem_type_user) / 1024,
        };
        uint32_t mem_pct[SCUI_MEM_TYPE_NUM] = {
            scui_mem_size_pct(scui_mem_type_mix),
            scui_mem_size_pct(scui_mem_type_font),
            scui_mem_size_pct(scui_mem_type_graph),
            scui_mem_size_pct(scui_mem_type_user),
        };
        
        static float mem_used_rcd[SCUI_MEM_TYPE_NUM] = {0};
        if (scui_dist(mem_used[0][0], mem_used_rcd[0]) > mem_used[0][1] ||
            scui_dist(mem_used[1][0], mem_used_rcd[1]) > mem_used[1][1] ||
            scui_dist(mem_used[2][0], mem_used_rcd[2]) > mem_used[2][1] ||
            scui_dist(mem_used[3][0], mem_used_rcd[3]) > mem_used[3][1]) {
            for (uint8_t idx = 0; idx < scui_arr_len(mem_used_rcd); idx++)
                mem_used_rcd[idx] = mem_used[idx][0];
            update_mem = true;
        }
        
        static uint64_t elapse_last = 0;
        uint64_t elapse_curr = scui_tick_us() - elapse_last;
        if (elapse_curr > 1000 * 1000) {
            elapse_last = scui_tick_us();
            update_refr = true;
        }
        
        
        
        #if 1 // srting_refr
        if (update_refr) {
            scui_tick_stat_t tick_stat = {0};
            scui_tick_stat_rcd(&tick_stat);
            scui_tick_stat(scui_tick_stat_reset);
            tick_stat.sched_sum -= scui_min(tick_stat.sched_sum, tick_stat.draw_sum);
            
            uint32_t val_args[4] = {
                100 * tick_stat.sched_sum   / elapse_curr,
                100 * tick_stat.draw_sum    / elapse_curr,
                100 * tick_stat.draw_sum_hw / elapse_curr,
                tick_stat.refr_fps,
            };
            
            uint32_t color[] = {
                val_args[0] > 80 ? color_r : val_args[0] > 60 ? color_y : color_g,
                val_args[1] > 80 ? color_r : val_args[1] > 60 ? color_y : color_g,
                val_args[2] > 80 ? color_r : val_args[2] > 60 ? color_y : color_g,
                val_args[3] > 40 ? color_g : val_args[3] > 30 ? color_y : color_r,
                 mem_pct[2] > 80 ? color_r :  mem_pct[2] > 60 ? color_y : color_g,
            };
            scui_color_t recolors[] = {
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[0], color[0]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[1], color[1]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[2], color[2]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[3], color[3]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[4], color[4]),
            };
            
            uint8_t  str_refr[100] = {0};
            snprintf(str_refr, sizeof(str_refr) - 1,
                    "Time:(#-%d%%-#,#-%d%%-#,#-%d%%-#), Fps:#-%d-#, G-Frag:#-%.03fM-#",
                     val_args[0], val_args[1], val_args[2], val_args[3], mem_frag[2]);
            
            scui_string_update_str_rec(scui_monitor.srting_refr,
                str_refr, scui_arr_len(recolors), recolors);
        }
        #endif
        
        #if 1 // srting_mem
        if (update_mem) {
            
            uint32_t color[] = {
                mem_pct[0] > 80 ? color_r : mem_pct[0] > 60 ? color_y : color_g,
                mem_pct[1] > 80 ? color_r : mem_pct[1] > 60 ? color_y : color_g,
                mem_pct[2] > 80 ? color_r : mem_pct[2] > 60 ? color_y : color_g,
                mem_pct[3] > 80 ? color_r : mem_pct[3] > 60 ? color_y : color_g,
            };
            scui_color_t recolors[] = {
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[0], color[0]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[1], color[1]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[2], color[2]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[3], color[3]),
            };
            
            uint8_t str_mem[100] = {0};
            snprintf(str_mem, sizeof(str_mem) - 1,
                "Mem:#-%d%%:%.02fK-#;#-%d%%:%.02fK-#;#-%d%%:%.02fM-#;#-%d%%:%.02fK-#;",
                     mem_pct[0], mem_used[0][0], mem_pct[1], mem_used[1][0],
                     mem_pct[2], mem_used[2][0], mem_pct[3], mem_used[3][0]);
            
            scui_string_update_str_rec(scui_monitor.srting_mem,
                str_mem, scui_arr_len(recolors), recolors);
        }
        #endif
        
        #if 1 // srting_cache
        if (update_mem) {
            uint32_t cache_rcd[SCUI_CACHE_TYPE_NUM * 2] = {0};
            scui_cache_font_usage(&cache_rcd[0]);
            scui_cache_font_nodes(&cache_rcd[1]);
            scui_cache_glyph_usage(&cache_rcd[2]);
            scui_cache_glyph_nodes(&cache_rcd[3]);
            scui_cache_image_usage(&cache_rcd[4]);
            scui_cache_image_nodes(&cache_rcd[5]);
            
            uint32_t mem_pct[SCUI_CACHE_TYPE_NUM] = {
                cache_rcd[0] * 100 / SCUI_CACHE_TOTAL_FONT,
                cache_rcd[2] * 100 / SCUI_CACHE_TOTAL_GLYPH,
                cache_rcd[4] * 100 / SCUI_CACHE_TOTAL_IMAGE,
            };
            uint32_t color[] = {
                mem_pct[0] > 80 ? color_r : mem_pct[0] > 60 ? color_y : color_g,
                mem_pct[1] > 80 ? color_r : mem_pct[1] > 60 ? color_y : color_g,
                mem_pct[2] > 80 ? color_r : mem_pct[2] > 60 ? color_y : color_g,
            };
            scui_color_t recolors[] = {
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[0], color[0]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[1], color[1]),
                SCUI_COLOR_MAKE32_SE(true, 0x0, color[2], color[2]),
            };
            
            uint8_t str_cache[100] = {0};
            snprintf(str_cache, sizeof(str_cache) - 1,
                "Font:#-%u,%.02fK-# Glyph:#-%d,%.02fK-# Graph:#-%d,%.02fM-#",
                cache_rcd[1], (float)cache_rcd[0] / 1024,
                cache_rcd[3], (float)cache_rcd[2] / 1024,
                cache_rcd[5], (float)cache_rcd[4] / 1024 / 1024);
            
            scui_string_update_str_rec(scui_monitor.srting_cache,
                str_cache, scui_arr_len(recolors), recolors);
        }
        #endif
        
        break;
    }
    case scui_event_create: {
        
        /* 创建显示信息文本,都是英文 */
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        string_maker.widget.type                = scui_widget_type_string;
        string_maker.widget.parent              = event->object;
        string_maker.widget.clip.x              = 0;
        string_maker.widget.clip.y              = 0;
        string_maker.widget.clip.w              = SCUI_HOR_RES - 10 * 2;
        string_maker.widget.clip.h              = 20;
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
        string_maker.font_idx                   = SCUI_FONT_IDX_X16;
        
        
        
        string_maker.widget.clip.y = 70 - 60;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, "refr");
        scui_monitor.srting_refr = string_handle;
        
        string_maker.widget.clip.y = 70 - 40;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, "cache");
        scui_monitor.srting_cache = string_handle;
        
        string_maker.widget.clip.y = 70 - 20;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, "mem");
        scui_monitor.srting_mem = string_handle;
        
        break;
    }
    case scui_event_destroy: {
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
        window_maker.widget.clip.x = 10;
        window_maker.widget.clip.y = SCUI_VER_RES - 70;
        window_maker.widget.clip.w = SCUI_HOR_RES - 10 * 2;
        window_maker.widget.clip.h = 70;
        window_maker.widget.event_cb  = scui_monitor_event_cb;
        window_maker.widget.child_num = 5;
        window_maker.level    = 10;
        window_maker.buffer   = buffer;
        window_maker.resident = true;
        window_maker.format   = SCUI_PIXEL_CF_DEF_A;
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
