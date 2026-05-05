/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 统计基础信息
 */
static void scui_stat_info(void)
{
    SCUI_LOG_INFO("scui evaluate:");
    
    SCUI_LOG_INFO("");
    float graph_used = scui_mem_size_used(scui_mem_type_graph);
    SCUI_LOG_INFO("scui graph occupy: %.2fM", graph_used / 1024.0f / 1024.0f);
    
    /* 矢量绘图引擎资源 */
    #if SCUI_DRAW_USE_THORVG
    scui_multi_t tvg_pixel_num  = SCUI_DRAW_CACHE_THORVG;
    scui_coord_t tvg_pixel_byte = scui_pixel_byte(scui_pixel_cf_bmp8888);
    scui_coord_t tvg_pixel_rem  = sizeof(scui_color_wt_t) - tvg_pixel_byte;
    scui_multi_t tvg_pixel_size = tvg_pixel_byte * tvg_pixel_num + tvg_pixel_rem;
    SCUI_LOG_INFO("scui graph thorvg occupy: %.2fK", (float)tvg_pixel_size / 1024.0f);
    #endif
    
    SCUI_LOG_INFO("");
    SCUI_LOG_INFO("scui draw dsc size:%4d",   sizeof(scui_draw_dsc_t));
    SCUI_LOG_INFO("scui draw dsc occupy:%4d", sizeof(scui_draw_dsc_t) * SCUI_DRAW_TASK_DSC_NUM);
    
    SCUI_LOG_INFO("");
    SCUI_LOG_INFO("scui widget occupy:");
    for (scui_widget_type_t type = 1; type < scui_widget_type_num; type++) {
        
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(type, &widget_map);
        SCUI_LOG_INFO("widget[%s]\t size(inst):%4d, size(maker):%4d",
            widget_map->name, widget_map->size, widget_map->maker);
    }
    SCUI_LOG_INFO("");
    SCUI_LOG_INFO("scui string args size:%4d", sizeof(scui_string_args_t));
    
    SCUI_LOG_INFO("");
}

/*@brief 初始化scui
 */
void scui_ready(void)
{
    /* 初始化scui */
    scui_engine_ready();
    
    /* frame buffer: */
    scui_area_t clip = {0};
    clip.w = SCUI_HOR_RES;
    clip.h = SCUI_VER_RES;
    scui_surface_t  surface = {0};
    scui_surface_t *surface_fb = NULL;
    surface.format  = SCUI_PIXEL_CF_DEF;
    surface.pbyte   = scui_pixel_byte(surface.format);
    surface.hor_res = SCUI_HOR_RES;
    surface.ver_res = SCUI_VER_RES;
    surface.stride  = surface.hor_res * surface.pbyte;
    surface.alpha   = scui_alpha_cover;
    scui_coord_t surface_rem  = sizeof(scui_color_wt_t) - surface.pbyte;
    scui_multi_t surface_size = surface.ver_res * surface.stride + surface_rem;
    
    #if SCUI_MEM_FEAT_MINI
    clip.h = SCUI_FRAME_BUFFER_SEG;
    surface.ver_res = SCUI_FRAME_BUFFER_SEG;
    surface_size = surface.ver_res * surface.stride + surface_rem;
    #endif
    
    /* 为绘制画布配置参数信息并且开辟资源 */
    surface_fb = scui_frame_buffer_draw(); *surface_fb = surface;
    surface_fb->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_size);
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(true, surface_fb, clip, surface_fb->alpha, SCUI_COLOR_ZEROED);
    
    #if SCUI_FRAME_BUFFER_ASYNC
    /* 为送显画布配置参数信息并且开辟资源 */
    surface_fb = scui_frame_buffer_refr(); *surface_fb = surface;
    surface_fb->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_size);
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(true, surface_fb, clip, surface_fb->alpha, SCUI_COLOR_ZEROED);
    #endif
    
    /* event register: */
    scui_event_register_type_stringify(scui_event_type_misc_stringify);
    
    /* event register: */
    scui_event_register_access(scui_event_custom_access);
    scui_event_register_custom(scui_event_custom_myself);
    scui_event_register_finish(scui_event_custom_finish);
    
    scui_handle_table_t table = {0};
    /* 句柄表(widget) */
    table.offset = SCUI_HANDLE_OFFSET_WIDGET + 1;
    table.source = scui_widget_parser_table;
    table.number = scui_arr_len(scui_widget_parser_table);
    scui_multi_t remap_size = table.number * sizeof(void *);
    table.source_remap = SCUI_MEM_ZALLOC(scui_mem_type_mix, remap_size);
    scui_handle_table_register(&table);
    table.source_remap = NULL;
    /* 句柄表(image) */
    table.offset = SCUI_HANDLE_OFFSET_IMAGE + 1;
    table.source = scui_image_parser_table;
    table.number = scui_arr_len(scui_image_parser_table);
    scui_handle_table_register(&table);
    /* 句柄表(font) */
    table.offset = SCUI_HANDLE_OFFSET_FONT + 1;
    table.source = scui_font_package_table;
    table.number = scui_arr_len(scui_font_package_table);
    scui_handle_table_register(&table);
    /* 句柄表(multi language) */
    table.offset = SCUI_HANDLE_OFFSET_LANG + 1;
    table.source = scui_lang_parser_table;
    table.number = scui_lang_type_num * scui_lang_str_num;
    scui_handle_table_register(&table);
    
    /* 启用同步自定义信息 */ {
    scui_event_define(event, SCUI_HANDLE_SYSTEM, true, scui_event_engine_ready, NULL);
    scui_event_notify(&event);
    }
    
    /* 统计基础信息 */
    scui_stat_info();
    /* 激活脉冲器 */
    scui_tick_work(true);
    
    /* 启用同步自定义显示 */ {
    scui_event_define(event, SCUI_HANDLE_SYSTEM, false, scui_event_engine_show, NULL);
    scui_event_notify(&event);
    }
}

/*@brief 初始化scui自定义配置
 */
void scui_custom_ready(void)
{
    /* 字库配置 */
    scui_lang_type_t lang_type = scui_lang_type_en;
    scui_lang_set(&lang_type);
    
    /* 窗口交互风格 */
    scui_window_switch_set_cfg_dir(scui_opt_dir_to_l);
    scui_window_switch_set_cfg_type(scui_window_switch_move);
    
    /* 窗口交互参数 */
    scui_window_switch_args_t cfg_args = {0};
    scui_window_switch_get_cfg_args(&cfg_args);
    cfg_args.shadow = scui_image_prj_image_src_00_3d_Trans_Lightpng;
    scui_window_switch_set_cfg_args(&cfg_args);
}

/*@brief 初始化scui自定义显示
 */
void scui_custom_show(void)
{
    /* 初始窗口 */
    scui_window_stack_reset(SCUI_UI_SCENE_NONE, false);
    /* 启用scui性能监控 */
    #if SCUI_MONITOR_USE
    scui_monitor_show(false || !SCUI_MEM_FEAT_MINI);
    #endif
    /* 启用monkey test */
    #if SCUI_MONKEY_TEST
    scui_monkey_test();
    #endif
}
