/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_xwatch_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_xwatch_t *xwatch = widget;
    scui_xwatch_maker_t *xwatch_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_xwatch));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    for (scui_coord_t idx = 0; idx < 3; idx++) {
        xwatch->image[idx]  = xwatch_maker->image[idx];
        xwatch->anchor[idx] = xwatch_maker->anchor[idx];
        xwatch->center[idx] = xwatch_maker->center[idx];
    }
    
    xwatch->tick_mode  = xwatch_maker->tick_mode;
    xwatch->use_always = xwatch_maker->use_always;
    
    xwatch->get_h  = xwatch_maker->get_h;
    xwatch->get_m  = xwatch_maker->get_m;
    xwatch->get_s  = xwatch_maker->get_s;
    xwatch->get_ms = xwatch_maker->get_ms;
    
    xwatch->tick_sync = true;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_xwatch_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xwatch));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xwatch_t *xwatch = (void *)widget;
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_xwatch_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_xwatch_t *xwatch = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 同步时间迭代数 */
        uint8_t  time_h  = xwatch->get_h();
        uint8_t  time_m  = xwatch->get_m();
        uint8_t  time_s  = xwatch->get_s();
        uint16_t time_ms = xwatch->get_ms();
        uint64_t tick_cc = (((time_h * 60) + time_m) * 60 + time_s) * 1000 + time_ms;
        bool sync_need_c = scui_dist(xwatch->tick_curr, tick_cc) > 1000 * 60;
        bool tick_draw = false;
        
        /* 外界时钟完全可信时, 完全使用外界时钟 */
        /* 外界时钟不完全可信时, 使用帧动画, 外界时钟充当校正 */
        if (xwatch->use_always || sync_need_c) xwatch->tick_curr = tick_cc;
        else xwatch->tick_curr += event->tick;
        
        /* 一秒一跳时 */
        if (xwatch->tick_mode == 0)
        if (xwatch->tick_last / 1000 != xwatch->tick_curr / 1000) {
            xwatch->tick_last = xwatch->tick_curr;
            tick_draw = true;
        }
        /* 一度一跳时 */
        if (xwatch->tick_mode == 1)
        if (scui_dist(xwatch->tick_last, xwatch->tick_curr) >= (1000 / 6)) {
            xwatch->tick_last = xwatch->tick_curr;
            tick_draw = true;
        }
        /* 一帧一跳时 */
        if (xwatch->tick_mode == 2) {
            xwatch->tick_last = xwatch->tick_curr;
            tick_draw = true;
        }
        
        SCUI_LOG_INFO("c_s:%u, c_ms:%u", xwatch->tick_curr, xwatch->tick_last);
        
        if (tick_draw) {
            
            #if 1
            /* <curr> <last> */
            float tick_ms[2] = {0};
            tick_ms[0] = xwatch->tick_curr;
            tick_ms[1] = xwatch->tick_last;
            
            SCUI_LOG_INFO("draw ready");
            for (scui_multi_t idx_tick_ms = 0; idx_tick_ms < scui_arr_len(tick_ms); idx_tick_ms++) {
                
                float time_ms = fmod(tick_ms[idx_tick_ms], 1000.0f); tick_ms[idx_tick_ms] /= 1000.0f;
                float dtime_s = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
                float dtime_m = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
                float dtime_h = fmod(tick_ms[idx_tick_ms], 60.0f);   tick_ms[idx_tick_ms] /= 60.0f;
                dtime_h = fmod(dtime_h, 12.0f);
                
                dtime_s = xwatch->tick_mode ? dtime_s : (int)dtime_s;
                scui_coord3_t angle_h = dtime_h * 30;
                scui_coord3_t angle_m = dtime_m * 6;
                scui_coord3_t angle_s = dtime_s * 6;
                scui_coord3_t angle[3] = {angle_h, angle_m, angle_s};
                SCUI_LOG_INFO("angle<h:%f;m:%f;s:%f;>", angle_h, angle_m, angle_s);
                
                for (scui_multi_t idx_angle = 0; idx_angle < scui_arr_len(angle); idx_angle++) {
                    
                    scui_point2_t anchor2 = {.x = +xwatch->anchor[idx_angle].x, .y = +xwatch->anchor[idx_angle].y,};
                    scui_point2_t center2 = {.x = -xwatch->center[idx_angle].x, .y = -xwatch->center[idx_angle].y,};
                    
                    scui_matrix_t matrix = {0};
                    scui_matrix_identity(&matrix);
                    scui_matrix_translate(&matrix, &anchor2);
                    scui_matrix_rotate_a(&matrix, angle[idx_angle], 0x00);
                    scui_matrix_translate(&matrix, &center2);
                    
                    scui_coord_t image_w = scui_image_w(xwatch->image[idx_angle]);
                    scui_coord_t image_h = scui_image_h(xwatch->image[idx_angle]);
                    scui_area_t  image_clip = {.w = image_w, .h = image_h,};
                    scui_area_t  image_clip_reb = image_clip;
                    
                    scui_face2_t image2_clip = {0};
                    scui_face3_t image3_clip = {0};
                    scui_area2_by_area(&image2_clip, &image_clip);
                    scui_area3_by_area2(&image3_clip, &image2_clip);
                    scui_area3_transform_by_matrix(&image3_clip, &matrix);
                    scui_area3_to_area2(&image3_clip, &image2_clip);
                    scui_area2_to_area(&image2_clip, &image_clip);
                    
                    scui_area_t clip_widget = {.x = 0, .y = 0, .w = widget->clip.w, .h = widget->clip.h};
                    if (!scui_area_inter2(&clip_widget, &image_clip))
                         break;
                    
                    /*FPB分段:比率超过阈值触发,沿Y轴切片
                     *接近0/90度时比率≈1.0不触发,绕过
                     *scui_widget_draw_frag水平不稳定
                     */
                    const scui_coord3_t fpb_limit_s = 2.0f;  /* 触发阈值 */
                    const scui_coord3_t fpb_limit_m = 3.2f;  /* 45°最大比率 */
                    const scui_coord_t  fpb_seg_min = 2;     /* 分段下限 */
                    const scui_coord_t  fpb_seg_max = 13;    /* 分段上限 */
                    
                    /* 计算是否需要分段 */
                    scui_coord3_t image_size  = scui_area_size(&image_clip_reb);
                    scui_coord3_t widget_size = scui_area_size(&clip_widget);
                    scui_coord3_t fpb_ratio_c = widget_size / image_size;
                    if (widget_size <= image_size * fpb_limit_s) {
                        scui_widget_draw(event->object, &clip_widget, false);
                        continue;
                    }
                    
                    /* 均匀线性映射 [limit_s, limit_m] → [seg_min, seg_max] */
                    scui_coord_t num_seg = scui_map(fpb_ratio_c,
                        fpb_limit_s, fpb_limit_m, fpb_seg_min, fpb_seg_max);
                    
                    /* 沿Y轴水平分段, 逐条带求交绘制 */
                    scui_coord_t vfrag = clip_widget.h / num_seg;
                    if (vfrag < 1) vfrag = 1;
                    
                    scui_multi_t sumpox = 0;
                    scui_area_t  clip_frag = {
                        .x = clip_widget.x,
                        .w = clip_widget.w,
                    };
                    
                    for (scui_coord_t seg = 0; seg < num_seg; seg++) {
                        clip_frag.y = clip_widget.y + (scui_coord_t)(seg * vfrag);
                        clip_frag.h = (seg == num_seg - 1) ? (clip_widget.y + clip_widget.h - clip_frag.y) : vfrag;
                        
                        scui_area_t draw_clip = clip_frag;
                        if (scui_widget_draw_frag(&draw_clip, &clip_frag, &image2_clip)) {
                            scui_widget_draw(event->object, &draw_clip, false);
                            sumpox += scui_area_size(&draw_clip);
                        }
                    }
                    
                    SCUI_LOG_INFO("draw idx:%d ratio:%.1f seg:%d sumpox:%d pct:%.1f%%", idx_angle,
                        fpb_ratio_c, num_seg, sumpox, sumpox / widget_size * 100.0);
                    
                    #if 0
                    /* 对比: 不分段直接整体绘制 */
                    scui_widget_draw(event->object, &image_clip, false);
                    #endif
                }
            }
            SCUI_LOG_INFO("draw over");
            #else
            scui_widget_draw(event->object, NULL, false);
            #endif
        }
        
        break;
    }
    case scui_event_focus_get:
    case scui_event_focus_lost: {
        xwatch->tick_sync = true;
        xwatch->tick_curr = 0;
        break;
    }
    case scui_event_draw_graph: {
        
        /* 同步时间迭代数 */
        if (xwatch->tick_sync) {
            xwatch->tick_sync = false;
            xwatch->tick_curr = 0;
            
            /* 临时转移到anima执行一次 */
            event->tick = 0;
            event->type = scui_event_anima_elapse;
            scui_xwatch_invoke(event);
            event->type = scui_event_draw;
        }
        
        float tick_ms = xwatch->tick_curr;
        float time_ms = fmod(tick_ms, 1000.0f); tick_ms /= 1000.0f;
        float dtime_s = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
        float dtime_m = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
        float dtime_h = fmod(tick_ms, 60.0f);   tick_ms /= 60.0f;
        dtime_h = fmod(dtime_h, 12.0f);
        
        dtime_s = xwatch->tick_mode ? dtime_s : (int)dtime_s;
        scui_coord3_t angle_h = dtime_h * 30;
        scui_coord3_t angle_m = dtime_m * 6;
        scui_coord3_t angle_s = dtime_s * 6;
        SCUI_LOG_INFO("angle<h:%f;m:%f;s:%f;>", angle_h, angle_m, angle_s);
        
        scui_point_t anchor_c = {0}, center_c = {0};
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        if (xwatch->image[0] != SCUI_HANDLE_INVALID) {  /* hour */
            anchor_c.x = widget_clip.x + xwatch->anchor[0].x;
            anchor_c.y = widget_clip.y + xwatch->anchor[0].y;
            scui_widget_draw_image_rotate(event->object, NULL, xwatch->image[0], NULL,
                anchor_c, xwatch->center[0], angle_h * SCUI_SCALE_COF);
        }
        if (xwatch->image[1] != SCUI_HANDLE_INVALID) {  /* minute */
            anchor_c.x = widget_clip.x + xwatch->anchor[1].x;
            anchor_c.y = widget_clip.y + xwatch->anchor[1].y;
            scui_widget_draw_image_rotate(event->object, NULL, xwatch->image[1], NULL,
                anchor_c, xwatch->center[1], angle_m * SCUI_SCALE_COF);
        }
        if (xwatch->image[2] != SCUI_HANDLE_INVALID) {  /* second */
            anchor_c.x = widget_clip.x + xwatch->anchor[2].x;
            anchor_c.y = widget_clip.y + xwatch->anchor[2].y;
            scui_widget_draw_image_rotate(event->object, NULL, xwatch->image[2], NULL,
                anchor_c, xwatch->center[2], angle_s * SCUI_SCALE_COF);
        }
        
        break;
    }
    default:
        break;
    }
}
