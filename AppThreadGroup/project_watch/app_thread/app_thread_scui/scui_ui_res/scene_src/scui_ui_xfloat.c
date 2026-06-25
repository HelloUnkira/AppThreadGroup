/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_xfloat_item_event(scui_event_t * event)
{
    #if 0
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw_graph: {
        
        scui_handle_t handle_s = SCUI_HANDLE_INVALID;
        scui_custom_get(event->object, &handle_s);
        if (handle_s == SCUI_HANDLE_INVALID)
            break;
        
        scui_handle_t  custom = handle_s;
        scui_area_t  src_clip = scui_widget_clip(custom);
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_UNUSED);
        
        scui_handle_clear(image);
        break;
    }
    case scui_event_ptr_hold:
    case scui_event_key_hold: {
        
        scui_handle_t handle_s = SCUI_HANDLE_INVALID;
        scui_custom_get(event->object, &handle_s);
        if (handle_s == SCUI_HANDLE_INVALID)
            break;
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
    #endif
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_xfloat_event_proc(scui_event_t * event)
{
    #if 0
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_scroll_layout:
    case scui_event_key_click: {
        // 布局结束后将目标控件移动到中心
        for(scui_handle_t idx = 0;  idx < scui_widget_child_now(event->object); idx++) {
            scui_handle_t handle_m = scui_widget_child_by_index(event->object, idx);
            scui_handle_t handle_s = SCUI_HANDLE_INVALID;
            if (handle_m == SCUI_HANDLE_INVALID)
                continue;
            
            scui_custom_get(handle_m, &handle_s);
            if (handle_s == SCUI_HANDLE_INVALID) {
                scui_area_t clip_p = scui_widget_clip(event->object);
                scui_area_t clip_w = scui_widget_clip(handle_m);
                
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
    #endif
}

/*@brief 控件事件响应构建
 *@param event      事件
 *@param pos        位置
 *@param event_cb_s 事件回调(子控件树)
 */
void scui_ui_scene_xfloat_create(scui_event_t *event, scui_opt_pos_t pos, scui_event_cb_t event_cb)
{
    #if 0
    scui_area_t clip_z = scui_widget_area(event->object);
    scui_area_t clip_p = scui_widget_clip(event->object);
    
    scui_scroll_maker_t scroll_maker = {0};
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    
    scui_widget_maker_def_cfg(&scroll_maker, scui_widget_type_scroll);
    scroll_maker.widget.style.indev_key = true;
    scroll_maker.widget.clip            = clip_z;
    scroll_maker.widget.parent          = event->object;
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
    default: SCUI_HANDLE_INVALID;
    }
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    
    // 通用属性配置
    uint8_t wseq[2] = {0};
    if (scui_opt_bits_check(pos, scui_opt_pos_u | scui_opt_pos_l)) {wseq[0] = 1; wseq[1] = 0;}
    if (scui_opt_bits_check(pos, scui_opt_pos_d | scui_opt_pos_r)) {wseq[0] = 0; wseq[1] = 1;}
    for (uint8_t idx = 0; idx < 2; idx++) {
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&custom_maker, scui_widget_type_custom);
        custom_maker.widget.parent   = scroll_handle;
        custom_maker.widget.event_cb = scui_ui_scene_xfloat_item_event;
        
        if (wseq[idx] == 0) {
            // 中部留白备用
            custom_maker.widget.clip            = clip_z;
            scui_widget_create(&custom_maker, &custom_handle);
        }
        if (wseq[idx] == 1) {
            // 创建并绑定子控件(主)(从)
            scui_area_t clip = clip_z;
            if (scui_opt_bits_check(pos, scui_opt_pos_hor)) clip.w = SCUI_HOR_RES / 2;
            if (scui_opt_bits_check(pos, scui_opt_pos_ver)) clip.h = SCUI_VER_RES / 2;
            custom_maker.widget.style.buffer    = true;
            custom_maker.widget.style.buffer_d  = true;
            custom_maker.widget.clip            = clip;
            scui_widget_create(&custom_maker, &custom_handle);
        }
    }
    #endif
}