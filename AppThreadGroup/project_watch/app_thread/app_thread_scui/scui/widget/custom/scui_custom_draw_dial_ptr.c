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
    uintptr_t tick_mode     = draw_dsc->dial_ptr.tick_mode;
    uintptr_t tick_curr     = draw_dsc->dial_ptr.tick_curr;
    uintptr_t tick_last     = draw_dsc->dial_ptr.tick_last;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    float tick_ms = tick_curr;
    float time_ms = fmod(tick_ms, 1000.0f); tick_ms /= 1000.0f;
    float dtime_s = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
    float dtime_m = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
    float dtime_h = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
    dtime_h = fmod(dtime_h, 12.0f);
    
    dtime_s = tick_mode ? dtime_s : (int)dtime_s;
    scui_coord3_t angle_h = dtime_h * 30;
    scui_coord3_t angle_m = dtime_m * 6;
    scui_coord3_t angle_s = dtime_s * 6;
    SCUI_LOG_INFO("angle<h:%f;m:%f;s:%f;>", angle_h, angle_m, angle_s);
    
    scui_point_t anchor_c = {0}, center_c = {0};
    scui_area_t widget_clip = scui_widget_clip(event->object);
    
    if (image[0] != SCUI_HANDLE_INVALID) {  // hour
        anchor_c.x = widget_clip.x + anchor[0].x;
        anchor_c.y = widget_clip.y + anchor[0].y;
        scui_widget_draw_image_rotate(event->object, NULL,
            image[0], NULL, anchor_c, center[0], angle_h * SCUI_SCALE_COF);
    }
    if (image[1] != SCUI_HANDLE_INVALID) {  // minute
        anchor_c.x = widget_clip.x + anchor[1].x;
        anchor_c.y = widget_clip.y + anchor[1].y;
        scui_widget_draw_image_rotate(event->object, NULL,
            image[1], NULL, anchor_c, center[1], angle_m * SCUI_SCALE_COF);
    }
    if (image[2] != SCUI_HANDLE_INVALID) {  // second
        anchor_c.x = widget_clip.x + anchor[2].x;
        anchor_c.y = widget_clip.y + anchor[2].y;
        scui_widget_draw_image_rotate(event->object, NULL,
            image[2], NULL, anchor_c, center[2], angle_s * SCUI_SCALE_COF);
    }
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
    uint64_t tick_curr      = draw_dsc->dial_ptr.tick_curr;
    uint64_t tick_last      = draw_dsc->dial_ptr.tick_last;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    bool tick_draw = false;
    
    /* 一帧一跳时 */
    if (tick_mode == 2) {
        tick_last = tick_curr;
        tick_draw = true;
    }
    /* 一度一跳时 */
    if (tick_mode == 1 && scui_dist(tick_last, tick_curr) >= (1000 / 6)) {
        tick_last = tick_curr;
        tick_draw = true;
    }
    /* 一秒一跳时 */
    if (tick_mode == 0 && tick_last / 1000 != tick_curr / 1000) {
        tick_last = tick_curr;
        tick_draw = true;
    }
    SCUI_LOG_INFO("c_s:%u, c_ms:%u", tick_curr, tick_last);
    
    // 同步时间迭代数
    draw_dsc->dial_ptr.tick_last = tick_last;
    draw_dsc->dial_ptr.tick_curr = tick_curr;
    
    if (tick_draw) {
        
        #if 1
        /* <curr> <last> */
        float tick_ms[2] = {0};
        tick_ms[0] = tick_curr;
        tick_ms[1] = tick_last;
        
        SCUI_LOG_INFO("draw ready");
        for (scui_multi_t idx_tick_ms = 0; idx_tick_ms < scui_arr_len(tick_ms); idx_tick_ms++) {
            
            float time_ms = fmod(tick_ms[idx_tick_ms], 1000.0f); tick_ms[idx_tick_ms] /= 1000.0f;
            float dtime_s = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
            float dtime_m = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
            float dtime_h = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
            dtime_h = fmod(dtime_h, 12.0f);
            
            dtime_s = tick_mode ? dtime_s : (int)dtime_s;
            scui_coord3_t angle_h = dtime_h * 30;
            scui_coord3_t angle_m = dtime_m * 6;
            scui_coord3_t angle_s = dtime_s * 6;
            scui_coord3_t angle[3] = {angle_h, angle_m, angle_s};
            SCUI_LOG_INFO("angle<h:%f;m:%f;s:%f;>", angle_h, angle_m, angle_s);
            
            for (scui_multi_t idx_angle = 0; idx_angle < scui_arr_len(angle); idx_angle++) {
                
                scui_point2_t anchor2 = {.x = +anchor[idx_angle].x, .y = +anchor[idx_angle].y,};
                scui_point2_t center2 = {.x = -center[idx_angle].x, .y = -center[idx_angle].y,};
                
                scui_matrix_t matrix = {0};
                scui_matrix_identity(&matrix);
                scui_matrix_translate(&matrix, &anchor2);
                scui_matrix_rotate_a(&matrix, angle[idx_angle], 0x00);
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
                scui_area2_to_area(&image2_clip, &image_clip);
                
                #if 0
                // 是整体区域剪除
                // 从控件区域缩小到图片完全绘制区域
                scui_area_t draw_clip = image_clip;
                scui_widget_draw(event->object, &draw_clip, false);
                #else
                scui_area_t clip_widget = scui_widget_clip(event->object);
                // if (!scui_area_inter2(&clip_widget, &image_clip))
                //      break;
                
                scui_multi_t sumpox = 0, vofs = 0;
                scui_multi_t vfrag = clip_widget.h / 10;
                scui_area_t  clip_frag = clip_widget;
                
                while (true) {
                    
                    clip_frag.y = clip_widget.y + vofs;
                    clip_frag.h = clip_widget.h - vofs < vfrag ? clip_widget.h - vofs : vfrag;
                    if (clip_frag.h <= 0)
                        break;
                    
                    scui_area_t draw_clip = clip_frag;
                    // 这里脏矩阵计算重绘区域还有问题(?)
                    if (scui_widget_draw_frag(&draw_clip, &clip_frag, &image2_clip)) {
                        
                        #if 0
                        SCUI_LOG_INFO("draw_clip<%3d,%3d,%3d,%3d>",
                            draw_clip.x, draw_clip.y,
                            draw_clip.w, draw_clip.h);
                        
                        SCUI_LOG_INFO("draw_clip<%3d,%3d,%3d,%3d>"
                                      "clip_frag<%3d,%3d,%3d,%3d>"
                                      "image2_clip<%3.2f,%3.2f><%3.2f,%3.2f><%3.2f,%3.2f><%3.2f,%3.2f>",
                            
                            draw_clip.x, draw_clip.y,
                            draw_clip.w, draw_clip.h,
                            
                            clip_frag.x, clip_frag.y,
                            clip_frag.w, clip_frag.h,
                            
                            image2_clip.point2[0].x, image2_clip.point2[0].y,
                            image2_clip.point2[1].x, image2_clip.point2[1].y,
                            image2_clip.point2[2].x, image2_clip.point2[2].y,
                            image2_clip.point2[3].x, image2_clip.point2[3].y);
                        #endif
                        
                        scui_widget_draw(event->object, &draw_clip, false);
                        sumpox += draw_clip.w * draw_clip.h;
                    }
                    vofs += clip_frag.h;
                }
                SCUI_LOG_INFO("draw clip:%d", sumpox);
                #endif
            }
        }
        SCUI_LOG_INFO("draw over");
        #else
        scui_widget_draw(event->object, NULL, false);
        #endif
    }
}
