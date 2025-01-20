/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:表盘指针
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t  *event    = draw_dsc->event;
    scui_area_t   *clip     = draw_dsc->clip;
    scui_handle_t *image    = draw_dsc->dial_ptr.image;
    scui_point_t  *anchor   = draw_dsc->dial_ptr.anchor;
    scui_point_t  *center   = draw_dsc->dial_ptr.center;
    uint64_t tick_mode      = draw_dsc->dial_ptr.tick_mode;
    uint64_t tick_curr_s    = draw_dsc->dial_ptr.tick_curr_s;
    uint64_t tick_last_s    = draw_dsc->dial_ptr.tick_last_s;
    uint64_t tick_curr_ms   = draw_dsc->dial_ptr.tick_curr_ms;
    uint64_t tick_last_ms   = draw_dsc->dial_ptr.tick_last_ms;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    uint64_t tick_ms = tick_curr_s * 1000 + tick_curr_ms;
    uint64_t time_ms = tick_ms % 1000; tick_ms /= 1000;
    uint64_t dtime_s = tick_ms % 60;   tick_ms /= 60;
    uint64_t dtime_m = tick_ms % 60;   tick_ms /= 60;
    uint64_t dtime_h = tick_ms % 60;   tick_ms /= 60;
    SCUI_ASSERT(tick_ms == 0 && dtime_h <= 24);
    
    time_ms = tick_mode ? time_ms : 0;
    scui_coord_t angle_h = dtime_h * (360 / 24) + dtime_m * (360 / 60) * 6 / 360;
    scui_coord_t angle_m = dtime_m * (360 / 60) + dtime_s * (360 / 60) * 6 / 360;
    scui_coord_t angle_s = dtime_s * (360 / 60) + time_ms * 6 / 1000;
    
    if (image[0] != SCUI_HANDLE_INVALID)    // hour
    scui_widget_draw_image_rotate(event->object, NULL,
        image[0], NULL, &anchor[0], &center[0], angle_h);
    
    if (image[1] != SCUI_HANDLE_INVALID)    // minute
    scui_widget_draw_image_rotate(event->object, NULL,
        image[1], NULL, &anchor[1], &center[1], angle_m);
    
    if (image[2] != SCUI_HANDLE_INVALID)    // second
    scui_widget_draw_image_rotate(event->object, NULL,
        image[2], NULL, &anchor[2], &center[2], angle_s);
    
}

/*@brief 自定义控件:插件:表盘指针(动画)
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_anim_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t  *event    = draw_dsc->event;
    scui_area_t   *clip     = draw_dsc->clip;
    scui_handle_t *image    = draw_dsc->dial_ptr.image;
    scui_point_t  *anchor   = draw_dsc->dial_ptr.anchor;
    scui_point_t  *center   = draw_dsc->dial_ptr.center;
    uint64_t tick_mode      = draw_dsc->dial_ptr.tick_mode;
    uint64_t tick_curr_s    = draw_dsc->dial_ptr.tick_curr_s;
    uint64_t tick_last_s    = draw_dsc->dial_ptr.tick_last_s;
    uint64_t tick_curr_ms   = draw_dsc->dial_ptr.tick_curr_ms;
    uint64_t tick_last_ms   = draw_dsc->dial_ptr.tick_last_ms;
    uint64_t tick_passby    = draw_dsc->dial_ptr.tick_passby;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    bool tick_draw = false;
    
    if (tick_last_s != tick_curr_s) {
        tick_last_s  = tick_curr_s;
        tick_curr_ms = 0;
        tick_last_ms = 0;
        tick_draw = true;
    } else {
        if (tick_mode) {
            tick_curr_ms += tick_passby - 1;
            if (tick_curr_ms >= 1000)
                tick_curr_ms  = 1000;
            
            /* 一度一跳时 */
            if (scui_dist(tick_last_ms, tick_curr_ms) >= (1000 / 6 / 2)) {
                tick_last_ms = tick_curr_ms;
                tick_draw = true;
            }
        } else {
            tick_curr_ms = 0;
            tick_last_ms = 0;
        }
    }
    SCUI_LOG_INFO("c_s:%u, c_ms:%u", tick_curr_s, tick_curr_ms);
    
    draw_dsc->dial_ptr.tick_curr_s  = tick_curr_s;
    draw_dsc->dial_ptr.tick_last_s  = tick_last_s;
    draw_dsc->dial_ptr.tick_curr_ms = tick_curr_ms;
    draw_dsc->dial_ptr.tick_last_ms = tick_last_ms;
    
    if (tick_draw) {
        
        #if 0
        /* <curr> <last> */
        uint64_t tick_ms[2] = {0};
        tick_ms[0] = tick_curr_s * 1000 + tick_curr_ms;
        tick_ms[1] = tick_last_s * 1000 + tick_last_ms;
        
        for (uint8_t idx_tick_ms = 0; idx_tick_ms < scui_arr_len(tick_ms); idx_tick_ms++) {
            
            uint8_t time_ms = tick_ms[idx_tick_ms] % 1000; tick_ms[idx_tick_ms] /= 1000;
            uint8_t dtime_s = tick_ms[idx_tick_ms] % 60;   tick_ms[idx_tick_ms] /= 60;
            uint8_t dtime_m = tick_ms[idx_tick_ms] % 60;   tick_ms[idx_tick_ms] /= 60;
            uint8_t dtime_h = tick_ms[idx_tick_ms] % 60;   tick_ms[idx_tick_ms] /= 60;
            SCUI_ASSERT(tick_ms[idx_tick_ms] == 0 && dtime_h <= 24);
            
            time_ms = tick_mode ? time_ms : 0;
            scui_coord_t angle_h = dtime_h * (360 / 24) + dtime_m * (360 / 60) * 6 / 360;
            scui_coord_t angle_m = dtime_m * (360 / 60) + dtime_s * (360 / 60) * 6 / 360;
            scui_coord_t angle_s = dtime_s * (360 / 60) + time_ms * 6 / 1000;
            scui_coord_t angle[3] = {angle_h, angle_m, angle_s};
            
            for (uint8_t idx_angle = 0; idx_angle < scui_arr_len(angle); idx_angle++) {
                
                scui_point2_t anchor2 = {.x = +anchor[idx_angle].x, .y = +anchor[idx_angle].y,};
                scui_point2_t center2 = {.x = -center[idx_angle].x, .y = -center[idx_angle].y,};
                
                scui_matrix_t matrix = {0};
                scui_matrix_identity(&matrix);
                scui_matrix_translate(&matrix, &anchor2);
                scui_matrix_rotate(&matrix, (float)angle[idx_angle], 0x00);
                scui_matrix_translate(&matrix, &center2);
                
                scui_area_t image_clip = {
                    .w = scui_image_w(image[idx_angle]),
                    .h = scui_image_h(image[idx_angle]),
                };
                
                scui_face2_t image2_clip = {0};
                scui_face3_t image3_clip = {0};
                scui_area2_by_area(&image2_clip, &image_clip);
                scui_area3_by_area2(&image3_clip, &image2_clip);
                scui_area3_transform_by_matrix(&image3_clip, &matrix);
                scui_area3_to_area2(&image3_clip, &image2_clip);
                
                scui_area_t clip_widget = scui_widget_clip(event->object);
                scui_area_t  clip_frag = clip_widget;
                
                scui_coord_t vofs = 0;
                scui_coord_t vfrag = clip_widget.h / 10;
                scui_multi_t sumpox = 0;
                
                for (true; true; true) {
                    
                    clip_frag.y = clip_widget.y + vofs;
                    clip_frag.h = clip_widget.h - vofs < vfrag ? clip_widget.h - vofs : vfrag;
                    if (clip_frag.h <= 0)
                        break;
                    
                    scui_area_t draw_clip = {0};
                    bool calc = scui_widget_draw_frag(&draw_clip, &clip_frag, &image2_clip);
                    
                    if (calc) {
                        sumpox += draw_clip.w * draw_clip.h;
                        
                        SCUI_LOG_INFO("draw clip<%d,%d,%d,%d>",
                            draw_clip.x, draw_clip.y,
                            draw_clip.w, draw_clip.h);
                        
                        scui_widget_draw(event->object, &draw_clip, false);
                    }
                    
                    vofs += clip_frag.h;
                }
                SCUI_LOG_INFO("draw clip:%d", sumpox);
            }
        }
        #else
        scui_widget_draw(event->object, NULL, false);
        #endif
    }
}
