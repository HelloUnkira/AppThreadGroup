/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static bool scui_engine_execute_status = false;

/*@brief 引擎就绪
 */
void scui_engine_ready(void)
{
    SCUI_ASSERT(SCUI_PIXEL_COND);
    SCUI_ASSERT(SCUI_DRV_PIXEL_DEPTH / 8 == SCUI_PIXEL_SIZE);
    
    scui_mem_ready();
    scui_disp_set_hor_res(SCUI_DRV_HOR_RES);
    scui_disp_set_ver_res(SCUI_DRV_VER_RES);
    scui_disp_ready();
    scui_indev_ready();
    scui_event_ready();
    scui_image_cache_ready();
    
    scui_surface_fb_ready();
    /* frame buffer: */
    SCUI_PIXEL_TYPE pixel = {0};
    scui_surface_t *surface_fb = NULL;
    const uint32_t  surface_fb_size = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * SCUI_PIXEL_SIZE;
    surface_fb = scui_surface_fb_draw();
    surface_fb->pixel  = SCUI_MEM_ALLOC(scui_mem_is_image, surface_fb_size);
    surface_fb->alpha  = scui_alpha_cover;
    surface_fb->clip.x = 0;
    surface_fb->clip.y = 0;
    surface_fb->clip.w = SCUI_DRV_HOR_RES;
    surface_fb->clip.h = SCUI_DRV_VER_RES;
    scui_draw_area_fill(surface_fb, &surface_fb->clip, &pixel, surface_fb->alpha);
    #if SCUI_SURFACE_FB_LIMIT == 2
    surface_fb = scui_surface_fb_refr();
    surface_fb->pixel  = SCUI_MEM_ALLOC(scui_mem_is_image, surface_fb_size);
    surface_fb->alpha  = scui_alpha_cover;
    surface_fb->clip.x = 0;
    surface_fb->clip.y = 0;
    surface_fb->clip.w = SCUI_DRV_HOR_RES;
    surface_fb->clip.h = SCUI_DRV_VER_RES;
    scui_draw_area_fill(surface_fb, &surface_fb->clip, &pixel, surface_fb->alpha);
    #endif
    
    /* event register: */
    scui_event_register_custom(scui_event_custom);
    scui_event_register_before(scui_event_before);
    scui_event_register_after(scui_event_after);
    
    scui_handle_table_t table = {0};
    table.source_remap = NULL;
    /* 句柄表(widget) */
    static void *widget_remap[scui_arr_len(scui_widget_parser_table)] = {0};
    table.offset = SCUI_HANDLE_OFFSET_WIDGET + 1;
    table.source = scui_widget_parser_table;
    table.number = scui_arr_len(scui_widget_parser_table);
    table.source_remap = widget_remap;
    scui_handle_table_register(&table);
    /* 句柄表(image) */
    table.offset = SCUI_HANDLE_OFFSET_IMAGE + 1;
    table.source = scui_image_combine_table;
    table.number = scui_arr_len(scui_image_combine_table);
    scui_handle_table_register(&table);
    table.source_remap = NULL;
    
}

/*@brief 引擎执行
 */
void scui_engine_execute(void)
{
    scui_event_wait();
    scui_event_dispatch();
}

/*@biref 设置引擎执行状态
 *@param 引擎是否执行
 */
void scui_engine_execute_status_set(bool execute)
{
    scui_engine_execute_status = execute;
}

/*@biref 获得引擎执行状态
 *@retval 引擎是否执行
 */
bool scui_engine_execute_status_get(void)
{
    return scui_engine_execute_status;
}