/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 初始化scui
 */
void scui_ready(void)
{
    /* 配置屏参 */
    scui_disp_set_hor_res(SCUI_DRV_HOR_RES);
    scui_disp_set_ver_res(SCUI_DRV_VER_RES);
    
    /* 初始化scui */
    scui_engine_ready();
    
    /* frame buffer: */
    scui_area_t clip = {
        .w = SCUI_DRV_HOR_RES,
        .w = SCUI_DRV_VER_RES,
    };
    scui_surface_t *surface_fb = NULL;
    scui_coord_t surface_fb_byte   = 0;
    scui_coord_t surface_fb_remain = 0;
    scui_multi_t surface_fb_size   = 0;
    
    surface_fb = scui_frame_buffer_draw();
    surface_fb->format  = SCUI_PIXEL_CF_DEF;
    surface_fb_byte     = scui_pixel_bits(surface_fb->format) / 8;
    surface_fb_remain   = sizeof(scui_color_wt_t) - surface_fb_byte;
    surface_fb_size     = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * surface_fb_byte + surface_fb_remain;
    surface_fb->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    SCUI_ASSERT(surface_fb->pixel != NULL);
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_fill.dst_surface = surface_fb,
            .area_fill.dst_clip    = &clip,
            .area_fill.src_alpha   = surface_fb->alpha,
        };
        scui_draw_area_fill(&draw_dsc);
    };
    #if SCUI_FRAME_BUFFER_ASYNC
    surface_fb = scui_frame_buffer_refr();
    surface_fb->format  = SCUI_PIXEL_CF_DEF;
    surface_fb_byte     = scui_pixel_bits(surface_fb->format) / 8;
    surface_fb_remain   = sizeof(scui_color_wt_t) - surface_fb_byte;
    surface_fb_size     = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * surface_fb_byte + surface_fb_remain;
    surface_fb->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    SCUI_ASSERT(surface_fb->pixel != NULL);
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_fill.dst_surface = surface_fb,
            .area_fill.dst_clip    = &clip,
            .area_fill.src_alpha   = surface_fb->alpha,
        };
        scui_draw_area_fill(&draw_dsc);
    };
    #endif
    
    /* event register: */
    scui_event_register_type_stringify(scui_event_type_misc_stringify);
    
    /* event register: */
    scui_event_register_prepare(scui_event_custom_prepare);
    scui_event_register_finish(scui_event_custom_finish);
    scui_event_register_custom(scui_event_custom);
    
    scui_handle_table_t table = {0};
    /* 句柄表(widget) */
    static void *widget_remap[scui_arr_len(scui_widget_parser_table)] = {0};
    table.source_remap = widget_remap;
    table.offset = SCUI_HANDLE_OFFSET_WIDGET + 1;
    table.source = scui_widget_parser_table;
    table.number = scui_arr_len(scui_widget_parser_table);
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
    scui_font_name_set(SCUI_FONT_TYPE_ZH);
    
    /* 窗口交互风格 */
    scui_window_switch_type_t *cfg_type = NULL;
    scui_window_switch_cfg_type(&cfg_type);
    *cfg_type = SCUI_WINDOW_MGR_TYPE_CFG;
    
    scui_opt_dir_t *cfg_dir = NULL;
    scui_window_switch_cfg_dir(&cfg_dir);
    *cfg_dir  = SCUI_WINDOW_MGR_JUMP_DIR;
    
    /* 窗口交互参数 */
    scui_window_switch_args_t *cfg_args = NULL;
    scui_window_switch_cfg_args(&cfg_args);
    cfg_args->cube.shadow = scui_image_prj_image_src_00_3d_Trans_Lightpng;
    
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
}
