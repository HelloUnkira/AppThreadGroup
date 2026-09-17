/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string)
{
    #if 1   // 有俩种选择,一种是打断以前的,另一种是不打断
    if (scui_widget_is_show(SCUI_UI_SCENE_POPUP)) {
        scui_window_active(scui_window_active_last(1));
        scui_widget_hide(SCUI_UI_SCENE_POPUP, false);
    }
    #else
    if (scui_widget_is_show(SCUI_UI_SCENE_POPUP))
        return;
    #endif
    
    // 显示界面,重绘它
    scui_widget_show(SCUI_UI_SCENE_POPUP, false);
    scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false, 0);
    scui_window_active(SCUI_UI_SCENE_POPUP);
    
    if (text != SCUI_HANDLE_INVALID)
        scui_string_update_text(SCUI_UI_SCENE_POPUP_STRING, text);
    else if (string != NULL)
        scui_string_update_str(SCUI_UI_SCENE_POPUP_STRING, string);
    
    scui_string_scroll_abort(SCUI_UI_SCENE_POPUP_STRING, true);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw_graph: {
        
        #if SCUI_MEM_FEAT_MINI == 0
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_ZEROED);
        #endif
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_scale_event_proc(scui_event_t *event)
{
    /* 弹窗缩放/淡入淡出动画状态(局部静态) */
    static scui_coord_t  scale_pct    = 0;   /* 缩放百分比(0-100) */
    static scui_alpha_t  scale_alpha  = 0;   /* 内容透明度(0-255) */
    static scui_coord_t  scale_way    = +1;  /* (+1)==>(-1)==>(0) */
    static scui_coord_t  popup_anima  = 0;
    
    switch (event->type) {
    case scui_event_create: {
        // 启动工步1:
        scale_way   = +1;
        scale_pct   = 0;
        scale_alpha = scui_alpha_trans;
        popup_anima = 0;
        break;
    }
    case scui_event_anima_elapse: {
        // 工步1:重力回弹原点放大,透明度淡入
        // 工步2:悬停至少指定时间或等待结束
        // 工步3:淡出时候原点缩小,透明度淡出
        
        // 工步1,3:
        if (scale_way == +1 || scale_way == -1) {
            scui_coord_t  pct_s  = scale_way == +1 ? 0 : 100;
            scui_coord_t  pct_e  = scale_way == +1 ? 100 : 0;
            scui_map_cb_t map_cb = scale_way == +1 ? scui_map_bounce : scui_map_ease_in;
            
            if (popup_anima <= SCUI_UI_POPUP_ANIM_TIME) {
                scui_coord_t pct = scui_map(popup_anima, 0, SCUI_UI_POPUP_ANIM_TIME, pct_s, pct_e);
                scale_alpha = map_cb(pct, 0, 100, scui_alpha_pct0, scui_alpha_pct100);
                scale_pct = map_cb(pct, 0, 100, 0, 100);
                scale_pct = scui_clamp(scale_pct, 1, 100);
                
                SCUI_LOG_INFO("popup scale:alpha:%d, pct:%d", scale_alpha, pct);
                scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false, 0);
                
                popup_anima += event->tick;
            }
            
            // 工步1结束到达工步2:
            if (scale_way == +1 && popup_anima >= SCUI_UI_POPUP_ANIM_TIME) {
                scui_string_scroll_abort(SCUI_UI_SCENE_POPUP_STRING, false);
                popup_anima = 0;
                scale_way = 0;
            }
            // 工步3结束:
            if (scale_way == -1 && popup_anima >= SCUI_UI_POPUP_ANIM_TIME) {
                scui_window_active(scui_window_active_last(1));
                scui_widget_hide(SCUI_UI_SCENE_POPUP, true);
                scale_way = 0xFF;
            }
        }
        
        // 工步2:
        if (scale_way == 0) {
            if (popup_anima <= SCUI_UI_POPUP_WAIT_TIME)
                popup_anima += event->tick;
            
            // 工步2结束到达工步3:
            if (popup_anima >= SCUI_UI_POPUP_WAIT_TIME &&
                scui_string_scroll_over(SCUI_UI_SCENE_POPUP_STRING)) {
                SCUI_LOG_INFO("scroll over");
                popup_anima = 0;
                scale_way = -1;
            } else {
                SCUI_LOG_INFO("popup wait");
                scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false, 0);
            }
        }
        break;
    }
    case scui_event_draw_graph: {
        // 背景贴图(独立画布清透明由基础处理完成)
        scui_handle_t image = scui_image_prj_rpt_btn_01_card_mediun;
        scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_ZEROED);
        break;
    }
    case scui_event_draw_buffer: {
        /* 中心缩放合成到父控件画布(buffer内容) */
        scui_point_t img_scale = {
            .x = 1024 * scale_pct / 100,
            .y = 1024 * scale_pct / 100,
        };
        /* alpha只作用于draw步调且不可继承: 临时设置, 绘制后撤回 */
        scui_alpha_t alpha_old = scui_alpha_cover;
        scui_widget_alpha_get(event->object, &alpha_old);
        scui_widget_alpha_set(event->object, scale_alpha, false);
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image_scale(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED, img_scale, scui_opt_pos_c);
        scui_widget_alpha_set(event->object, alpha_old, false);
        break;
    }
    case scui_event_ptr_click:
    case scui_event_key_click:
        scui_event_mask_over(event);
        scui_window_active(scui_window_active_last(1));
        scui_widget_hide(SCUI_UI_SCENE_POPUP, true);
        break;
    default:
        break;
    }
}
