/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static void scui_evaluate(void)
{
    SCUI_LOG_INFO("");
    float graph_occupy = scui_mem_size_used(scui_mem_type_graph) / 1024.0f / 1024.0f;
    SCUI_LOG_INFO("scui graph occupy: %.2fM", graph_occupy);
    
    SCUI_LOG_INFO("");
    SCUI_LOG_INFO("scui widget evaluate:");
    SCUI_LOG_INFO("scui_widget_t:%d, scui_widget_maker_t:%d", sizeof(scui_widget_t), sizeof(scui_widget_maker_t));
    SCUI_LOG_INFO("scui_window_t:%d, scui_window_maker_t:%d", sizeof(scui_window_t), sizeof(scui_window_maker_t));
    SCUI_LOG_INFO("scui_custom_t:%d, scui_custom_maker_t:%d", sizeof(scui_custom_t), sizeof(scui_custom_maker_t));
    SCUI_LOG_INFO("scui_scroll_t:%d, scui_scroll_maker_t:%d", sizeof(scui_scroll_t), sizeof(scui_scroll_maker_t));
    SCUI_LOG_INFO("scui_string_t:%d, scui_string_maker_t:%d", sizeof(scui_string_t), sizeof(scui_string_maker_t));
    SCUI_LOG_INFO("scui_linear_t:%d, scui_linear_maker_t:%d", sizeof(scui_linear_t), sizeof(scui_linear_maker_t));
    SCUI_LOG_INFO("scui_roller_t:%d, scui_roller_maker_t:%d", sizeof(scui_roller_t), sizeof(scui_roller_maker_t));
    
    SCUI_LOG_INFO("scui_objbtn_t:%d, scui_objbtn_maker_t:%d", sizeof(scui_objbtn_t), sizeof(scui_objbtn_maker_t));
    SCUI_LOG_INFO("scui_button_t:%d, scui_button_maker_t:%d", sizeof(scui_button_t), sizeof(scui_button_maker_t));
    SCUI_LOG_INFO("scui_chart_t:%d,  scui_chart_maker_t:%d",  sizeof(scui_chart_t),  sizeof(scui_chart_maker_t));
    
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
    surface.hor_res = SCUI_HOR_RES;
    surface.ver_res = SCUI_VER_RES;
    surface.alpha   = scui_alpha_cover;
    scui_coord_t surface_byte = scui_pixel_bits(surface.format) / 8;
    scui_coord_t surface_rem  = sizeof(scui_color_wt_t) - surface_byte;
    scui_coord_t surface_line = surface.hor_res * surface_byte;
    scui_multi_t surface_size = surface.ver_res * surface_line + surface_rem;
    
    #if SCUI_MEM_FEAT_MINI
    clip.h = SCUI_FRAME_BUFFER_SEG;
    surface.ver_res = SCUI_FRAME_BUFFER_SEG;
    surface_size = surface.ver_res * surface_line + surface_rem;
    #endif
    
    // 为绘制画布配置参数信息并且开辟资源
    surface_fb = scui_frame_buffer_draw(); *surface_fb = surface;
    surface_fb->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_size);
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(surface_fb, &clip, surface_fb->alpha, SCUI_COLOR_ZEROED);
    
    #if SCUI_FRAME_BUFFER_ASYNC
    // 为送显画布配置参数信息并且开辟资源
    surface_fb = scui_frame_buffer_refr(); *surface_fb = surface;
    surface_fb->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_size);
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(surface_fb, &clip, surface_fb->alpha, SCUI_COLOR_ZEROED);
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
    table.source_remap = SCUI_MEM_ALLOC(scui_mem_type_mix, remap_size);
    memset(table.source_remap, 0, remap_size);
    scui_handle_table_register(&table);
    table.source_remap = NULL;
    /* 句柄表(image) */
    table.offset = SCUI_HANDLE_OFFSET_IMAGE + 1;
    table.source = scui_image_parser_table;
    table.number = scui_arr_len(scui_image_parser_table);
    scui_handle_table_register(&table);
    /* 句柄表(font) */
    table.offset = SCUI_HANDLE_OFFSET_FONT + 1;
    table.source = scui_font_type_table;
    table.number = scui_arr_len(scui_font_type_table);
    scui_handle_table_register(&table);
    /* 句柄表(multi language) */
    table.offset = SCUI_HANDLE_OFFSET_LANG + 1;
    table.source = scui_multi_lang_table;
    table.number = SCUI_MULTI_LANG_NUM_STR * SCUI_MULTI_LANG_NUM_TYPE;
    scui_handle_table_register(&table);
    
    /* 字库配置 */
    scui_multi_lang_type_t lang_type = scui_multi_lang_type_en;
    scui_multi_lang_set(&lang_type);
    
    /* 窗口交互风格 */
    scui_opt_dir_t *cfg_dir = NULL;
    scui_window_switch_type_t *cfg_type = NULL;
    scui_window_switch_args_t *cfg_args = NULL;
    
    scui_window_switch_cfg_dir(&cfg_dir);
    scui_window_switch_cfg_type(&cfg_type);
    scui_window_switch_cfg_args(&cfg_args);
    *cfg_dir  = SCUI_WINDOW_SWITCH_JUMP_DIR;
    *cfg_type = SCUI_WINDOW_SWITCH_TYPE;
    
    /* 窗口交互参数 */
    cfg_args->shadow = scui_image_prj_image_src_00_3d_Trans_Lightpng;
    
    /* 初始窗口 */
    scui_window_stack_reset(SCUI_UI_SCENE_NONE, false);
    scui_engine_execute_status_set(true);
    
    /* 初始化scui性能监控 */
    #if SCUI_UI_MONITOR_USE
    scui_widget_show(SCUI_UI_SCENE_MONITOR, false);
    #endif
    
    #if SCUI_MONKEY_TEST
    scui_monkey_test();
    #endif
    
    // 输出一些基础信息
    scui_evaluate();
}
