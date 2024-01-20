/*实现目标:
 *    引擎
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static bool scui_engine_execute_status = false;

/*@brief 引擎就绪
 */
void scui_engine_ready(void)
{
    scui_disp_set_hor_res(SCUI_DRV_HOR_RES);
    scui_disp_set_ver_res(SCUI_DRV_VER_RES);
    scui_disp_ready();
    scui_indev_ready();
    scui_mem_ready();
    
    scui_event_ready();
    scui_image_cache_ready();
    
    scui_surface_fb_ready();
    
    APP_SYS_ASSERT(SCUI_PIXEL_COND);
    APP_SYS_ASSERT(SCUI_DRV_PIXEL_DEPTH / 8 == SCUI_PIXEL_SIZE);
    /* frame buffer: */
    SCUI_PIXEL_TYPE pixel = {0};
    scui_surface_t *surface_fb = NULL;
    const uint32_t  surface_fb_size = SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES * SCUI_PIXEL_SIZE;
    surface_fb = scui_surface_fb_draw();
    surface_fb->pixel  = SCUI_MEM_ALLOC(scui_mem_is_image, surface_fb_size);
    surface_fb->alpha  = scui_alpha_by_percent(100);
    surface_fb->clip.x = 0;
    surface_fb->clip.y = 0;
    surface_fb->clip.w = SCUI_DRV_HOR_RES;
    surface_fb->clip.h = SCUI_DRV_VER_RES;
    scui_draw_area_fill(surface_fb, &surface_fb->clip, &pixel, surface_fb->alpha);
    #if SCUI_SURFACE_FB_LIMIT == 2
    surface_fb = scui_surface_fb_refr();
    surface_fb->pixel  = SCUI_MEM_ALLOC(scui_mem_is_image, surface_fb_size);
    surface_fb->alpha  = scui_alpha_by_percent(100);
    surface_fb->clip.x = 0;
    surface_fb->clip.y = 0;
    surface_fb->clip.w = SCUI_DRV_HOR_RES;
    surface_fb->clip.h = SCUI_DRV_VER_RES;
    scui_draw_area_fill(surface_fb, &surface_fb->clip, &pixel, surface_fb->alpha);
    #endif
    
    /* event register: */
    scui_event_register_before(scui_event_before);
    scui_event_register_after(scui_event_after);
    scui_event_register_custom(scui_event_custom);
    
    scui_handle_table_t table = {0};
    table.source_remap = NULL;
    /* 句柄表(image) */
    table.offset = SCUI_HANDLE_OFFSET_IMAGE;
    table.source = scui_image_combine_table;
    table.number = app_sys_arr_len(scui_image_combine_table);
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
