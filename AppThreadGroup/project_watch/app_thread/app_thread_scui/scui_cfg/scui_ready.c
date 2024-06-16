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
    scui_color_t pixel = {0};
    scui_surface_t *surface_fb = NULL;
    scui_coord_t surface_fb_byte = 0;
    uint32_t surface_fb_size = 0;
    
    surface_fb = scui_surface_fb_draw();
    surface_fb->format  = scui_pixel_cf_bmp565;
    surface_fb_byte     = scui_pixel_bits(surface_fb->format) / 8;
    surface_fb_size     = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * surface_fb_byte;
    surface_fb->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(surface_fb, &clip, pixel, surface_fb->alpha);
    #if SCUI_SURFACE_FB_LIMIT == 2
    surface_fb = scui_surface_fb_refr();
    surface_fb->format  = scui_pixel_cf_bmp565;
    surface_fb_byte     = scui_pixel_bits(surface_fb->format) / 8;
    surface_fb_size     = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * surface_fb_byte;
    surface_fb->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    SCUI_ASSERT(surface_fb->pixel != NULL);
    scui_draw_area_fill(surface_fb, &clip, pixel, surface_fb->alpha);
    #endif
    
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
    
    /* 窗口交互风格 */
    scui_window_switch_dir_cfg(SCUI_WINDOW_JUMP_DIR);
    /* 窗口交互风格 */
    scui_window_switch_type_cfg(scui_window_switch_normal);
    scui_window_switch_type_cfg(scui_window_switch_zoom1);
    scui_window_switch_type_cfg(scui_window_switch_zoom2);
    
    scui_window_switch_type_cfg(scui_window_switch_center_in);
    scui_window_switch_type_cfg(scui_window_switch_center_out);
    scui_window_switch_type_cfg(scui_window_switch_circle);
    scui_window_switch_type_cfg(scui_window_switch_grid);
    
    scui_window_switch_type_cfg(scui_window_switch_flip);
    scui_window_switch_type_cfg(scui_window_switch_cube);
    
    /* 初始窗口 */
    scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
    
    #if 0
    scui_widget_show(SCUI_UI_SCENE_POPUP, false);
    scui_ui_scene_popup_text(SCUI_MULTI_LANG_0X002f, NULL);
    scui_ui_scene_popup_exec();
    scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, scui_alpha_pct0, true);
    #endif
    
    scui_engine_execute_status_set(true);
}
