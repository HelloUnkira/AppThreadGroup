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
    
    uint8_t dtime_ms = tick_ms % 1000; tick_ms /= 1000;
    uint8_t dtime_s  = tick_ms % 60;   tick_ms /= 60;
    uint8_t dtime_m  = tick_ms % 60;   tick_ms /= 60;
    uint8_t dtime_h  = tick_ms % 60;   tick_ms /= 60;
    SCUI_ASSERT(tick_ms == 0 && dtime_h <= 24);
    
    dtime_ms = tick_mode ? dtime_ms : 0;
    
    scui_coord_t angle_h = dtime_h * (360 / 24) + dtime_m * (360 / 60) * 6 / 360;
    scui_coord_t angle_m = dtime_m * (360 / 60) + dtime_s * (360 / 60) * 6 / 360;
    scui_coord_t angle_s = dtime_s * (360 / 60) + dtime_ms * 6 / 1000;
    
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
    bool tag_draw = false;
    
    // 无效迭代,去掉它
    if (tick_passby <= 0)
        return;
    
    if (tick_last_s != tick_curr_s) {
        tick_last_s  = tick_curr_s;
        tag_draw = true;
    } else {
        if (tick_mode) {
            tick_curr_ms += tick_passby;
            if (tick_curr_ms >= 1000)
                tick_curr_ms -= 1000;
            
            /* 一度一跳时 */
            if (scui_dist(tick_last_ms, tick_curr_ms) >= (1000 / 6)) {
                tick_last_ms = tick_curr_ms;
                tag_draw = true;
            }
        } else {
            tick_curr_ms = 0;
            tick_last_ms = 0;
        }
    }
    
    draw_dsc->dial_ptr.tick_curr_s  = tick_curr_s;
    draw_dsc->dial_ptr.tick_last_s  = tick_last_s;
    draw_dsc->dial_ptr.tick_curr_ms = tick_curr_ms;
    draw_dsc->dial_ptr.tick_last_ms = tick_last_ms;
    
    if (tag_draw) {
        scui_widget_draw(event->object, NULL, false);
    }
}
