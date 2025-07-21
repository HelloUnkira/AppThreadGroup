/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_handle_t monitor_anima = SCUI_HANDLE_INVALID;
scui_handle_t monitor_refr  = SCUI_HANDLE_INVALID;
scui_handle_t monitor_mem1  = SCUI_HANDLE_INVALID;
scui_handle_t monitor_mem2  = SCUI_HANDLE_INVALID;
scui_handle_t monitor_cache = SCUI_HANDLE_INVALID;

/*@brief scui 监控器常驻动画响应回调
 */
void scui_ui_scene_monitor_anima_expire(void *instance)
{
    bool redraw_tag = false;
    
    #if 1 // refr
    uint32_t tick_fps = 0;
    uint32_t sched_us = 0;
    uint32_t draw_us  = 0;
    scui_tick_calc(0xAA, &tick_fps, &sched_us, &draw_us);
    
    if (sched_us >  draw_us)
        sched_us -= draw_us;
    else
        sched_us = 0;
    
    static uint64_t refr_us_last = 0;
    uint64_t elapse_us = scui_tick_us() - refr_us_last;
    if (elapse_us > 1000 * 1000) {
        
        uint8_t  str_refr[50] = {0};
        uint32_t val_args[3] = { 100 * sched_us / elapse_us, 100 * draw_us / elapse_us, tick_fps, };
        snprintf(str_refr, sizeof(str_refr) - 1, "Sched:#-%d%%-#, Draw:#-%d%%-#, Fps:#-%d-#",
                 val_args[0], val_args[1], val_args[2]);
        
        uint32_t color_g = 0xFF008000;
        uint32_t color_y = 0xFF808000;
        uint32_t color_r = 0xFF800000;
        
        scui_color_t recolors[3] = {
            {   .filter = true,
                .color_s.full = val_args[0] > 80 ? color_r : val_args[0] > 60 ? color_y : color_g,
                .color_e.full = val_args[0] > 80 ? color_r : val_args[0] > 60 ? color_y : color_g,},
            {   .filter = true,
                .color_s.full = val_args[1] > 80 ? color_r : val_args[1] > 60 ? color_y : color_g,
                .color_e.full = val_args[1] > 80 ? color_r : val_args[1] > 60 ? color_y : color_g,},
            {   .filter = true,
                .color_s.full = val_args[2] > 40 ? color_g : val_args[2] > 30 ? color_y : color_r,
                .color_e.full = val_args[2] > 40 ? color_g : val_args[2] > 30 ? color_y : color_r,},
        };
        scui_string_update_str_rec(monitor_refr, str_refr, scui_arr_len(recolors), recolors);
        redraw_tag = true;
        
        refr_us_last = scui_tick_us();
        scui_tick_calc(0xFF, NULL, NULL, NULL);
    }
    #endif
    
    #if 1 // mem
    #define MEM_TYPE    4
    
    uintptr_t mem_total[MEM_TYPE] = {
        scui_mem_size_total(scui_mem_type_mix),
        scui_mem_size_total(scui_mem_type_font),
        scui_mem_size_total(scui_mem_type_graph),
        scui_mem_size_total(scui_mem_type_user),
    };
    uintptr_t mem_used[MEM_TYPE] = {
        scui_mem_size_used(scui_mem_type_mix),
        scui_mem_size_used(scui_mem_type_font),
        scui_mem_size_used(scui_mem_type_graph),
        scui_mem_size_used(scui_mem_type_user),
    };
    uintptr_t mem_frag[MEM_TYPE] = {
        0, // scui_mem_size_frag(scui_mem_type_mix),
        0, // scui_mem_size_frag(scui_mem_type_font),
        scui_mem_size_frag(scui_mem_type_graph),
        0, // scui_mem_size_frag(scui_mem_type_user),
    };
    
    static uintptr_t mem_used_rcd[MEM_TYPE] = {0};
    if (scui_dist(mem_used[0], mem_used_rcd[0]) > 1024 * 2  ||
        scui_dist(mem_used[1], mem_used_rcd[1]) > 1024 * 8  ||
        scui_dist(mem_used[2], mem_used_rcd[2]) > 1024 * 16 ||
        scui_dist(mem_used[3], mem_used_rcd[3]) > 1024 * 2) {
        
        for (uint8_t idx = 0; idx < scui_arr_len(mem_used); idx++)
            mem_used_rcd[idx] = mem_used[idx];
        
        float mem_total_f[MEM_TYPE] = {0};
        mem_total_f[0] = (float)mem_total[0] / 1024 / 1024;
        mem_total_f[1] = (float)mem_total[1] / 1024 / 1024;
        mem_total_f[2] = (float)mem_total[2] / 1024 / 1024;
        mem_total_f[3] = (float)mem_total[3] / 1024 / 1024;
        
        float mem_used_f[MEM_TYPE] = {0};
        mem_used_f[0] = (float)mem_used[0] / 1024;
        mem_used_f[1] = (float)mem_used[1] / 1024;
        mem_used_f[2] = (float)mem_used[2] / 1024 / 1024;
        mem_used_f[3] = (float)mem_used[3] / 1024;
        
        float mem_frag_f[MEM_TYPE] = {0};
        mem_frag_f[0] = (float)mem_frag[0] / 1024;
        mem_frag_f[1] = (float)mem_frag[1] / 1024;
        mem_frag_f[2] = (float)mem_frag[2] / 1024 / 1024;
        mem_frag_f[3] = (float)mem_frag[3] / 1024;
        
        uint32_t pct_args[MEM_TYPE] = {0};
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
        scui_string_update_str_rec(monitor_mem1, str_mem1, scui_arr_len(recolors1), recolors1);
        scui_string_update_str_rec(monitor_mem2, str_mem2, scui_arr_len(recolors2), recolors2);
        redraw_tag = true;
    }
    #endif
    
    #if 1 // cache
    #define CACHE_NUM   3
    if (redraw_tag) {
        uint32_t cache_rcd[CACHE_NUM * 2] = {0};
        scui_font_cache_usage(&cache_rcd[0]);
        scui_font_cache_nodes(&cache_rcd[1]);
        scui_font_glyph_cache_usage(&cache_rcd[2]);
        scui_font_glyph_cache_nodes(&cache_rcd[3]);
        scui_image_cache_usage(&cache_rcd[4]);
        scui_image_cache_nodes(&cache_rcd[5]);
        
        uintptr_t mem_total[CACHE_NUM] = {
            SCUI_CACHE_TOTAL_FONT,
            SCUI_CACHE_TOTAL_FONT_GLYPH,
            SCUI_CACHE_TOTAL_IMAGE,
        };
        uintptr_t mem_used[CACHE_NUM] = {
            cache_rcd[0], cache_rcd[2], cache_rcd[4],
        };
        
        float mem_total_f[CACHE_NUM] = {0};
        mem_total_f[0] = (float)mem_total[0] / 1024 / 1024;
        mem_total_f[1] = (float)mem_total[1] / 1024 / 1024;
        mem_total_f[2] = (float)mem_total[2] / 1024 / 1024;
        
        float mem_used_f[CACHE_NUM] = {0};
        mem_used_f[0] = (float)mem_used[0] / 1024;
        mem_used_f[1] = (float)mem_used[1] / 1024;
        mem_used_f[2] = (float)mem_used[2] / 1024 / 1024;
        
        uint32_t pct_args[CACHE_NUM] = {0};
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
        
        scui_string_update_str_rec(monitor_cache, str_cache, scui_arr_len(recolors), recolors);
    }
    #endif
    
    // 怪事???
    if (redraw_tag) {
        scui_widget_draw(monitor_refr, NULL, false);
        scui_widget_draw(monitor_mem1, NULL, false);
        scui_widget_draw(monitor_mem2, NULL, false);
        scui_widget_draw(monitor_cache, NULL, false);
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_monitor_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        
        if (monitor_anima == SCUI_HANDLE_INVALID) {
            scui_anima_t anima = {0};
            anima.expire = scui_ui_scene_monitor_anima_expire;
            anima.period = SCUI_ANIMA_TICK * 5;
            anima.reload = SCUI_ANIMA_INFINITE;
            scui_anima_create(&anima, &monitor_anima);
            scui_anima_start(monitor_anima);
            
            /* 创建显示信息文本,都是英文 */
            scui_string_maker_t string_maker = {0};
            string_maker.widget.type                = scui_widget_type_string;
            string_maker.widget.parent              = SCUI_UI_SCENE_MONITOR;
            string_maker.args.recolor               = true;
            string_maker.args.align_hor             = 0;
            string_maker.args.align_ver             = 2;
            string_maker.args.color.color_s.full    = 0xFF404040;
            string_maker.args.color.color_e.full    = 0xFF404040;
            string_maker.args.color.filter          = true;
            string_maker.args.line_width            = 1;
            string_maker.args.line_under            = 1;
            string_maker.args.line_delete           = 1;
            string_maker.args.lang                  = SCUI_MULTI_LANG_TYPE_A;
            string_maker.font_idx                   = SCUI_FONT_IDX_A16;
            
            string_maker.widget.clip.x              = 10;
            string_maker.widget.clip.y              = 30;
            string_maker.widget.clip.w              = SCUI_HOR_RES;
            string_maker.widget.clip.h              = 20;
            scui_widget_create(&string_maker, &monitor_refr);
            string_maker.widget.clip.x              = 10;
            string_maker.widget.clip.y              = 50;
            string_maker.widget.clip.w              = SCUI_HOR_RES;
            string_maker.widget.clip.h              = 20;
            scui_widget_create(&string_maker, &monitor_mem1);
            string_maker.widget.clip.x              = 10;
            string_maker.widget.clip.y              = 70;
            string_maker.widget.clip.w              = SCUI_HOR_RES;
            string_maker.widget.clip.h              = 20;
            scui_widget_create(&string_maker, &monitor_mem2);
            string_maker.widget.clip.x              = 10;
            string_maker.widget.clip.y              = 90;
            string_maker.widget.clip.w              = SCUI_HOR_RES;
            string_maker.widget.clip.h              = 20;
            scui_widget_create(&string_maker, &monitor_cache);
            
            scui_string_update_str(monitor_refr,  "refr");
            scui_string_update_str(monitor_mem1,  "mem");
            scui_string_update_str(monitor_mem2,  "mem");
            scui_string_update_str(monitor_cache, "cache");
        }
        break;
    }
    case scui_event_destroy: {
        
        if (monitor_anima != SCUI_HANDLE_INVALID) {
            scui_anima_stop(monitor_anima);
            scui_anima_destroy(monitor_anima);
            monitor_anima = SCUI_HANDLE_INVALID;
        }
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
