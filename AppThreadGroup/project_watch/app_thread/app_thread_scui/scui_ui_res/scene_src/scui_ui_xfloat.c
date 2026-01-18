/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_xfloat_m_event_proc(scui_event_t * event)
{
    // 特殊的固定调用
    scui_widget_event_shift(event);
    
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_handle_t handle_s = SCUI_HANDLE_INVALID;
        scui_linear_m_get(event->object, &handle_s);
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
        scui_linear_m_get(event->object, &handle_s);
        if (handle_s == SCUI_HANDLE_INVALID)
            break;
        
        scui_event_mask_over(event);
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
    case scui_event_widget_scroll_layout:
    case scui_event_key_click: {
        // 布局结束后将目标控件移动到中心
        for(scui_handle_t idx = 0;  idx < scui_widget_child_now(event->object); idx++) {
            scui_handle_t handle_m = scui_widget_child_by_index(event->object, idx);
            scui_handle_t handle_s = SCUI_HANDLE_INVALID;
            if (handle_m == SCUI_HANDLE_INVALID)
                continue;
            
            scui_linear_m_get(handle_m, &handle_s);
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
    
    // 转移至控件调度
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应构建
 *@param event      事件
 *@param pos        位置
 *@param event_cb_s 事件回调(子控件树)
 */
void scui_ui_scene_xfloat_create(scui_event_t *event, scui_opt_pos_t pos, scui_event_cb_t event_cb_s)
{
    scui_area_t clip_p = scui_widget_clip(event->object);
    scui_area_t clip_z = clip_p; clip_z.x = clip_z.y = 0;
    
    scui_linear_maker_t linear_maker = {0};
    scui_handle_t linear_handle = SCUI_HANDLE_INVALID;
    linear_maker.widget.type            = scui_widget_type_linear;
    linear_maker.widget.style.indev_key = true;
    linear_maker.widget.clip            = clip_z;
    linear_maker.widget.parent          = event->object;
    linear_maker.widget.event_cb        = scui_ui_scene_xfloat_event_proc;
    linear_maker.widget.child_num       = 2;
    linear_maker.scroll.pos             = pos;
    
    if (scui_opt_bits_check(pos, scui_opt_pos_hor)) linear_maker.scroll.dir = scui_opt_dir_hor;
    if (scui_opt_bits_check(pos, scui_opt_pos_ver)) linear_maker.scroll.dir = scui_opt_dir_ver;
    
    switch (pos)  {
    case scui_opt_pos_l: linear_maker.scroll.skip = scui_opt_pos_r; break;
    case scui_opt_pos_r: linear_maker.scroll.skip = scui_opt_pos_l; break;
    case scui_opt_pos_u: linear_maker.scroll.skip = scui_opt_pos_d; break;
    case scui_opt_pos_d: linear_maker.scroll.skip = scui_opt_pos_u; break;
    default: SCUI_HANDLE_INVALID;
    }
    
    scui_widget_create(&linear_maker, &linear_handle);
    
    // 通用属性配置
    scui_linear_m_maker_t linear_m_maker = {0};
    scui_linear_s_maker_t linear_s_maker = {0};
    scui_handle_t linear_m_handle = SCUI_HANDLE_INVALID;
    scui_handle_t linear_s_handle = SCUI_HANDLE_INVALID;
    linear_m_maker.widget.type            = scui_widget_type_linear_m;
    linear_m_maker.widget.style.indev_ptr = true;
    linear_m_maker.widget.style.indev_enc = true;
    linear_m_maker.widget.style.indev_key = true;
    linear_m_maker.widget.parent          = linear_handle;
    linear_m_maker.widget.event_cb        = scui_ui_scene_xfloat_m_event_proc;
    linear_s_maker.widget.type            = scui_widget_type_linear_s;
    linear_s_maker.widget.child_num       = 10;
    linear_s_maker.widget.event_cb        = event_cb_s;
    
    uint8_t wseq[2] = {0};
    if (scui_opt_bits_check(pos, scui_opt_pos_u | scui_opt_pos_l)) {wseq[0] = 1; wseq[1] = 0;}
    if (scui_opt_bits_check(pos, scui_opt_pos_d | scui_opt_pos_r)) {wseq[0] = 0; wseq[1] = 1;}
    for (uint8_t idx = 0; idx < 2; idx++) {
        
        if (wseq[idx] == 0) {
            // 中部留白备用
            linear_m_maker.widget.clip = clip_z;
            scui_widget_create(&linear_m_maker, &linear_m_handle);
        }
        if (wseq[idx] == 1) {
            // 创建并绑定子控件(主)(从)
            scui_area_t clip = clip_z;
            if (scui_opt_bits_check(pos, scui_opt_pos_hor)) clip.w = SCUI_HOR_RES / 2;
            if (scui_opt_bits_check(pos, scui_opt_pos_ver)) clip.h = SCUI_HOR_RES / 2;
            linear_m_maker.widget.clip = clip;
            linear_s_maker.widget.clip = clip;
            scui_widget_create(&linear_m_maker, &linear_m_handle);
            scui_widget_create(&linear_s_maker, &linear_s_handle);
            scui_linear_m_set(linear_m_handle, &linear_s_handle);
            scui_linear_s_set(linear_s_handle, &linear_m_handle);
        }
    }
}