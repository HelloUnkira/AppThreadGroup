/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_handle_t monitor_anima = SCUI_HANDLE_INVALID;
scui_handle_t monitor_refr  = SCUI_HANDLE_INVALID;
scui_handle_t monitor_mem   = SCUI_HANDLE_INVALID;

/*@brief scui 监控器常驻动画响应回调
 */
void scui_ui_scene_monitor_anima_expired(void *instance)
{
    #if 1 // refr
    uint32_t tick_fps = 0;
    uint32_t sched_us = 0;
    uint32_t draw_us  = 0;
    scui_tick_calc(0xAA, &tick_fps, &sched_us, &draw_us);
    
    static uint64_t refr_us_last = 0;
    uint64_t elapse_us = scui_tick_us() - refr_us_last;
    if (elapse_us > 1000 * 1000) {
        
        uint8_t  str_refr[50] = {0};
        uint32_t val_args[3] = { 100 * sched_us / elapse_us, 100 * draw_us / elapse_us, tick_fps, };
        snprintf(str_refr, sizeof(str_refr) - 1, "Sched:#-%d%%-#, Draw:#-%d%%-#, Fps:#-%d-#",
                 val_args[0], val_args[1], val_args[2]);
        
        uint32_t color_g = 0xFF00FF00;
        uint32_t color_y = 0xFFFFFF00;
        uint32_t color_r = 0xFFFF0000;
        
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
        
        refr_us_last = scui_tick_us();
        scui_tick_calc(0xFF, NULL, NULL, NULL);
    }
    #endif
    
    #if 1 // mem
    uintptr_t mem_total[3] = {0};
    uintptr_t mem_used[3] = {0};
    
    mem_total[0] = scui_mem_size_total(scui_mem_type_mix);
    mem_total[1] = scui_mem_size_total(scui_mem_type_font);
    mem_total[2] = scui_mem_size_total(scui_mem_type_graph);
    mem_used[0]  = scui_mem_size_used(scui_mem_type_mix);
    mem_used[1]  = scui_mem_size_used(scui_mem_type_font);
    mem_used[2]  = scui_mem_size_used(scui_mem_type_graph);
    
    static uintptr_t mem_used_rcd[3] = {0};
    if (scui_dist(mem_used[0], mem_used_rcd[0]) > 1024 * 2 ||
        scui_dist(mem_used[1], mem_used_rcd[1]) > 1024 * 8 ||
        scui_dist(mem_used[2], mem_used_rcd[2]) > 1024 * 16) {
        mem_used_rcd[0] = mem_used[0];
        mem_used_rcd[1] = mem_used[1];
        mem_used_rcd[2] = mem_used[2];
        
        float mem_total_f[3] = {0};
        float mem_used_f[3] = {0};
        
        mem_total_f[0] = (float)mem_total[0] / 1024 / 1024;
        mem_total_f[1] = (float)mem_total[1] / 1024 / 1024;
        mem_total_f[2] = (float)mem_total[2] / 1024 / 1024;
        
        mem_used_f[0] = (float)mem_used[0] / 1024;
        mem_used_f[1] = (float)mem_used[1] / 1024;
        mem_used_f[2] = (float)mem_used[2] / 1024 / 1024;
        
        uint32_t pct_args[3] = {
            mem_used[0] * 100 / mem_total[0],
            mem_used[1] * 100 / mem_total[1],
            mem_used[2] * 100 / mem_total[2],
        };
        
        uint8_t str_mem[100] = {0};
        snprintf(str_mem, sizeof(str_mem) - 1, "Mix:#-%d%%:%.02fK-# Font:#-%d%%:%.02fK-# Graph:#-%d%%:%.02fM-#",
                 pct_args[0], mem_used_f[0], pct_args[1], mem_used_f[1], pct_args[2], mem_used_f[2]);
        
        uint32_t color_g = 0xFF00FF00;
        uint32_t color_y = 0xFFFFFF00;
        uint32_t color_r = 0xFFFF0000;
        
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
        scui_string_update_str_rec(monitor_mem, str_mem, scui_arr_len(recolors), recolors);
    }
    #endif
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_monitor_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        if (scui_widget_event_check_prepare(event)) {
            
            if (monitor_anima == SCUI_HANDLE_INVALID) {
                scui_anima_t anima = {0};
                anima.expired = scui_ui_scene_monitor_anima_expired;
                anima.peroid  = SCUI_ANIMA_TICK * 5;
                anima.reload  = SCUI_ANIMA_INFINITE;
                scui_anima_create(&anima, &monitor_anima);
                scui_anima_start(monitor_anima);
                
                /* 创建显示信息文本,都是英文 */
                scui_string_maker_t string_maker = {0};
                string_maker.widget.type                = scui_widget_type_string;
                string_maker.widget.style.trans         = true;
                string_maker.widget.parent              = SCUI_UI_SCENE_MONITOR;
                string_maker.font_idx                   = 0;
                string_maker.args.recolor               = true;
                string_maker.args.align_hor             = 0;
                string_maker.args.align_ver             = 2;
                string_maker.args.color.color_s.full    = 0xFF808080;
                string_maker.args.color.color_e.full    = 0xFF808080;
                string_maker.args.color.filter          = true;
                string_maker.args.name                  = SCUI_FONT_TYPE_16_ASCII;
                
                string_maker.widget.clip.x              = 10;
                string_maker.widget.clip.y              = 50;
                string_maker.widget.clip.w              = SCUI_DRV_HOR_RES;
                string_maker.widget.clip.h              = 20;
                scui_string_create(&string_maker, &monitor_refr, false);
                string_maker.widget.clip.x              = 10;
                string_maker.widget.clip.y              = 70;
                string_maker.widget.clip.w              = SCUI_DRV_HOR_RES;
                string_maker.widget.clip.h              = 20;
                scui_string_create(&string_maker, &monitor_mem, false);
                
                scui_string_update_str(monitor_refr, "refr");
                scui_string_update_str(monitor_mem,  "mem");
            }
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        if (scui_widget_event_check_finish(event)) {
            
            if (monitor_anima != SCUI_HANDLE_INVALID) {
                scui_anima_stop(monitor_anima);
                scui_anima_destroy(monitor_anima);
                monitor_anima = SCUI_HANDLE_INVALID;
            }
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
