/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_xfloat_buffer_event(scui_event_t * event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_xfloat_event_proc(scui_event_t * event)
{
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
    case scui_event_focus_lost:
    case scui_event_scroll_layout:
    case scui_event_key_click: {
        /* 占位子控件滚动到中心 */
        scui_area_t clip_p = scui_widget_clip(event->object);
        for (scui_handle_t idx = 0; idx < scui_widget_child_now(event->object); idx++) {
            scui_handle_t child = scui_widget_child_by_index(event->object, idx);
            if (child == SCUI_HANDLE_INVALID)
                continue;
            
            scui_area_t clip_w = scui_widget_clip(child);
            /* 全屏子控件为占位(不可见), 用于边缘对齐 */
            if (clip_w.w == clip_p.w && clip_w.h == clip_p.h) {
                scui_point_t offset = {
                    .x = -(clip_w.x - clip_p.x),
                    .y = -(clip_w.y - clip_p.y),
                };
                bool anima = false;
                if (event->type == scui_event_key_click) anima = true;
                scui_scroll_offset(event->object, &offset, anima);
                
                if (event->type == scui_event_key_click)
                if (offset.x != 0 || offset.y != 0)
                    scui_event_mask_over(event);
                
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应构建
 *@param handle   父控件句柄
 *@param w_h      卡片宽或高
 *@param pos      位置(上/下/左/右)
 *@param event_cb 事件回调
 */
void scui_ui_scene_xfloat_create(scui_handle_t handle, scui_coord_t w_h,
    scui_opt_pos_t pos, scui_event_cb_t event_cb)
{
    scui_area_t clip_z = scui_widget_area(handle);
    
    /* 滚动容器(替代 linear, 边缘对齐侧拉半屏浮窗) */
    scui_scroll_maker_define(scroll_maker);
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    
    scroll_maker.widget.style.indev_ptr = true;
    scroll_maker.widget.style.indev_key = true;
    scroll_maker.widget.clip            = clip_z;
    scroll_maker.widget.parent          = handle;
    scroll_maker.widget.event_cb        = scui_ui_scene_xfloat_event_proc;
    scroll_maker.widget.child_num       = 2;
    scroll_maker.pos                    = pos;
    
    if (scui_opt_bits_check(pos, scui_opt_pos_hor)) scroll_maker.dir = scui_opt_dir_hor;
    if (scui_opt_bits_check(pos, scui_opt_pos_ver)) scroll_maker.dir = scui_opt_dir_ver;
    
    switch (pos)  {
    case scui_opt_pos_l: scroll_maker.skip = scui_opt_pos_r; break;
    case scui_opt_pos_r: scroll_maker.skip = scui_opt_pos_l; break;
    case scui_opt_pos_u: scroll_maker.skip = scui_opt_pos_d; break;
    case scui_opt_pos_d: scroll_maker.skip = scui_opt_pos_u; break;
    default: break;
    }
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    
    /* 创建顺序: u/l 卡片先(边缘), d/r 占位先(中部) */
    uint8_t wseq[2] = {0};
    if (scui_opt_bits_check(pos, scui_opt_pos_u | scui_opt_pos_l)) {wseq[0] = 1; wseq[1] = 0;}
    if (scui_opt_bits_check(pos, scui_opt_pos_d | scui_opt_pos_r)) {wseq[0] = 0; wseq[1] = 1;}
    
    for (uint8_t idx = 0; idx < 2; idx++) {
        scui_custom_maker_define(custom_maker);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        
        custom_maker.widget.parent   = scroll_handle;
        custom_maker.widget.event_cb = NULL;
        
        if (wseq[idx] == 0) {
            /* 中部留白占位(全屏, 不可见), 用于边缘对齐 */
            custom_maker.widget.clip = clip_z;
            scui_widget_create(&custom_maker, &custom_handle);
        }
        if (wseq[idx] == 1) {
            /* 侧边卡片外层(独立buffer子控件): 左/右宽=w_h高=全屏, 上/下宽=全屏高=w_h
             * draw_buffer 合成由本层处理, 画内容由内层子控件回调 */
            scui_area_t clip = clip_z;
            if (scui_opt_bits_check(pos, scui_opt_pos_hor)) clip.w = w_h;
            if (scui_opt_bits_check(pos, scui_opt_pos_ver)) clip.h = w_h;
            custom_maker.widget.style.buffer   = true;
            custom_maker.widget.style.buffer_d = true;
            custom_maker.widget.style.fully_bg = true;
            custom_maker.widget.event_cb       = scui_ui_scene_xfloat_buffer_event;
            custom_maker.widget.clip           = clip;
            custom_maker.widget.format         = SCUI_PIXEL_CF_DEF_A;
            custom_maker.widget.child_num      = 1;
            scui_widget_create(&custom_maker, &custom_handle);
            
            /* 侧边卡片内层(子控件, 大小一致): 画内容由调用方回调 */
            scui_custom_maker_define(inner_maker);
            scui_handle_t inner_handle = SCUI_HANDLE_INVALID;
            scui_area_t clip_i = {.w = clip.w, .h = clip.h};
            inner_maker.widget.parent   = custom_handle;
            inner_maker.widget.event_cb = event_cb;
            inner_maker.widget.clip     = clip_i;
            scui_widget_create(&inner_maker, &inner_handle);
        }
    }
    
    /* 初始停靠到中部占位(卡片隐藏)由 event_proc 布局事件处理, 侧拉出卡片 */
    scui_widget_draw(scroll_handle, NULL, false, 0);
}