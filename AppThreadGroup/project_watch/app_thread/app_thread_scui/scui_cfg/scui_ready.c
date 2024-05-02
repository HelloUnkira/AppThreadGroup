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
    SCUI_ASSERT(SCUI_PIXEL_COND);
    SCUI_ASSERT(SCUI_DRV_PIXEL_DEPTH / 8 == SCUI_PIXEL_SIZE);
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
    SCUI_PIXEL_TYPE pixel = {0};
    scui_surface_t *surface_fb = NULL;
    const uint32_t  surface_fb_size = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * SCUI_PIXEL_SIZE * 10;
    surface_fb = scui_surface_fb_draw();
    surface_fb->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->format  = SCUI_PIXEL_FORMAT;
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    scui_draw_area_fill(surface_fb, &clip, &pixel, surface_fb->alpha);
    #if SCUI_SURFACE_FB_LIMIT == 2
    surface_fb = scui_surface_fb_refr();
    surface_fb->pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_fb_size);
    surface_fb->format  = SCUI_PIXEL_FORMAT;
    surface_fb->hor_res = SCUI_DRV_HOR_RES;
    surface_fb->ver_res = SCUI_DRV_VER_RES;
    surface_fb->alpha   = scui_alpha_cover;
    scui_draw_area_fill(surface_fb, &clip, &pixel, surface_fb->alpha);
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
    table.source = scui_image_combine_table;
    table.number = scui_arr_len(scui_image_combine_table);
    scui_handle_table_register(&table);
    /* 句柄表(multi language) */
    table.offset = SCUI_HANDLE_OFFSET_LANG + 1;
    table.source = scui_multi_language_table;
    table.number = SCUI_MULTI_LANGUAGE_NUM_STR * SCUI_MULTI_LANGUAGE_NUM_TYPE;
    scui_handle_table_register(&table);
    
    /* 窗口交互风格 */
    scui_window_switch_type_cfg(scui_window_switch_normal);
    scui_window_switch_type_cfg(scui_window_switch_zoom2);
    /* 初始窗口 */
    scui_handle_t handle = SCUI_UI_SCENE_TEST;
    scui_widget_show(handle, false);
    scui_window_active(handle);
    // scui_widget_hide(handle, false);
    
    scui_engine_execute_status_set(true);
}
