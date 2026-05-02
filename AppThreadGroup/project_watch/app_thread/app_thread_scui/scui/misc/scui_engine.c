/*实现目标:
 *    引擎
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 系统OOM命中
 *@param type    内存类型
 *@param invalid 该OOM无法处理, 在崩溃前做最后日志输出
 */
static void scui_engine_oom_hit(scui_mem_type_t type, bool invalid)
{
    #if SCUI_MEM_FEAT_MINI
    if (invalid) {
        scui_cache_font_visit();
        scui_cache_glyph_visit();
        scui_cache_image_visit();
    } else {
        /* 图片缓存: */
        /* scui_cache_image_visit(); */
        scui_cache_image_clear();
        /* scui_cache_image_visit(); */
        
        /* 字库缓存: */
        /* scui_cache_font_visit(); */
        scui_cache_font_clear();
        /* scui_cache_font_visit(); */
        
        /* 文字缓存: */
        /* scui_cache_glyph_visit(); */
        scui_cache_glyph_clear();
        /* scui_cache_glyph_visit(); */
    }
    #else
    if (invalid) {
        if (type == scui_mem_type_graph)
            scui_cache_image_visit();
        if (type == scui_mem_type_font) {
            scui_cache_font_visit();
            scui_cache_glyph_visit();
        }
    } else {
        if (type == scui_mem_type_graph) {
            /* scui_cache_image_visit(); */
            scui_cache_image_clear();
            /* scui_cache_image_visit(); */
        }
        if (type == scui_mem_type_font) {
            /* 字库文字缓存(不回收字库): */
            /* scui_cache_font_visit(); */
            /* scui_cache_font_clear(); */
            /* scui_cache_font_visit(); */
            
            /* 文字缓存: */
            /* scui_cache_glyph_visit(); */
            scui_cache_glyph_clear();
            /* scui_cache_glyph_visit(); */
        }
    }
    #endif
}

/*@brief 引擎就绪
 */
void scui_engine_ready(void)
{
    scui_mem_ready(scui_engine_oom_hit);
    scui_draw_task_ready();
    scui_draw_graph_ready();
    
    scui_disp_ready();
    scui_indev_ready();
    scui_event_ready();
    
    scui_cache_font_ready();
    scui_cache_glyph_ready();
    scui_cache_image_ready();
    scui_frame_buffer_ready();
}

/*@brief 引擎等待
 */
void scui_engine_wait(void)
{
    scui_event_wait();
}

/*@brief 引擎执行
 */
void scui_engine_execute(void)
{
    scui_event_dispatch();
}
