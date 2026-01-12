/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应构建
 *@param event    事件
 *@param pos      位置
 *@param event_cb 事件回调(主,子主,子从)
 */
void scui_ui_scene_float_event(scui_event_t *event, scui_opt_pos_t pos, scui_event_cb_t event_cb[3])
{
    scui_linear_maker_t linear_maker = {0};
    scui_handle_t linear_handle = SCUI_HANDLE_INVALID;
    linear_maker.widget.type = scui_widget_type_linear;
    linear_maker.widget.style.indev_ptr = true;
    linear_maker.widget.style.indev_enc = true;
    linear_maker.widget.style.indev_key = true;
    linear_maker.widget.clip.w = SCUI_HOR_RES;
    linear_maker.widget.clip.h = SCUI_VER_RES;
    linear_maker.widget.parent = event->object;
    linear_maker.widget.event_cb    = event_cb[0];
    linear_maker.widget.child_num   = 5;
    linear_maker.scroll.space       = 0;
    linear_maker.scroll.springback  = 0;
    
    if (scui_opt_bits_check(pos, scui_opt_pos_hor)) {
        linear_maker.scroll.pos  = pos;
        linear_maker.scroll.dir  = scui_opt_dir_hor;
        linear_maker.scroll.skip = scui_opt_dir_hor;
    }
    if (scui_opt_bits_check(pos, scui_opt_pos_ver)) {
        linear_maker.scroll.pos  = pos;
        linear_maker.scroll.dir  = scui_opt_dir_ver;
        linear_maker.scroll.skip = scui_opt_dir_ver;
    }
    
    scui_widget_create(&linear_maker, &linear_handle);
    
    // 通用属性配置
    scui_linear_m_maker_t linear_m_maker = {0};
    scui_linear_s_maker_t linear_s_maker = {0};
    scui_handle_t linear_m_handle = SCUI_HANDLE_INVALID;
    scui_handle_t linear_s_handle = SCUI_HANDLE_INVALID;
    linear_m_maker.widget.type      = scui_widget_type_linear_m;
    linear_m_maker.widget.clip.w    = SCUI_HOR_RES;
    linear_m_maker.widget.clip.h    = SCUI_VER_RES;
    linear_m_maker.widget.parent    = linear_handle;
    linear_s_maker.widget.type      = scui_widget_type_linear_s;
    linear_s_maker.widget.clip.w    = SCUI_HOR_RES;
    linear_s_maker.widget.clip.h    = SCUI_VER_RES;
    linear_s_maker.widget.child_num = 10;
    linear_s_maker.widget.event_cb  = event_cb[2];
    scui_handle_t linear_mx_handle  = SCUI_HANDLE_INVALID;
    
    if (scui_opt_bits_check(pos, scui_opt_pos_u | scui_opt_pos_l)) {
        
        if (scui_opt_bits_check(pos, scui_opt_pos_hor)) {
            linear_m_maker.widget.clip.w = SCUI_HOR_RES / 2;
            linear_s_maker.widget.clip.w = SCUI_HOR_RES / 2;
        }
        if (scui_opt_bits_check(pos, scui_opt_pos_ver)) {
            linear_m_maker.widget.clip.h = SCUI_VER_RES / 2;
            linear_s_maker.widget.clip.h = SCUI_VER_RES / 2;
        }
        // 创建并绑定子控件(主)(从)
        linear_m_maker.widget.event_cb = event_cb[1];
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        scui_widget_create(&linear_s_maker, &linear_s_handle);
        scui_linear_m_set(linear_m_handle, &linear_s_handle);
        scui_linear_s_set(linear_s_handle, &linear_m_handle);
        // 中部留白备用
        linear_m_maker.widget.clip.w   = SCUI_HOR_RES;
        linear_m_maker.widget.clip.h   = SCUI_VER_RES;
        linear_s_maker.widget.clip.w   = SCUI_HOR_RES;
        linear_s_maker.widget.clip.h   = SCUI_VER_RES;
        linear_m_maker.widget.event_cb = NULL;
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        linear_mx_handle = linear_m_handle;
    }
    if (scui_opt_bits_check(pos, scui_opt_pos_d | scui_opt_pos_r)) {
        // 中部留白备用
        linear_m_maker.widget.clip.w   = SCUI_HOR_RES;
        linear_m_maker.widget.clip.h   = SCUI_VER_RES;
        linear_s_maker.widget.clip.w   = SCUI_HOR_RES;
        linear_s_maker.widget.clip.h   = SCUI_VER_RES;
        linear_m_maker.widget.event_cb = NULL;
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        linear_mx_handle = linear_m_handle;
        
        if (scui_opt_bits_check(pos, scui_opt_pos_hor)) {
            linear_m_maker.widget.clip.w = SCUI_HOR_RES / 2;
            linear_s_maker.widget.clip.w = SCUI_HOR_RES / 2;
        }
        if (scui_opt_bits_check(pos, scui_opt_pos_ver)) {
            linear_m_maker.widget.clip.h = SCUI_VER_RES / 2;
            linear_s_maker.widget.clip.h = SCUI_VER_RES / 2;
        }
        // 创建并绑定子控件(主)(从)
        linear_m_maker.widget.event_cb = event_cb[1];
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        scui_widget_create(&linear_s_maker, &linear_s_handle);
        scui_linear_m_set(linear_m_handle, &linear_s_handle);
        scui_linear_s_set(linear_s_handle, &linear_m_handle);
    }
    
    
    
    scui_scroll_center(linear_handle, linear_mx_handle, false);
}
