/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// 可以开字画布做控件树真缩放的
// 这里偷懒懒得折腾了,搞个差不齐的样子完事了



// 工步1:重力回弹原点放大,透明度淡入
// 工步2:悬停至少指定时间或等待结束
// 工步3:淡出时候原点缩小,透明度淡出
static scui_coord_t  scale_tar_w  = 0;
static scui_coord_t  scale_tar_h  = 0;
static scui_coord_t  scale_way    = +1;  /* (+1)==>(-1)==>(0) */
static scui_coord_t  popup_anima  = 0;
static scui_handle_t popup_string = SCUI_HANDLE_INVALID;

void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string)
{
    #if 1   // 有俩种选择,一种是打断以前的,另一种是不打断
    if (scui_widget_is_show(SCUI_UI_SCENE_POPUP))
        scui_widget_hide(SCUI_UI_SCENE_POPUP, false);
    #else
    if (scui_widget_is_show(SCUI_UI_SCENE_POPUP))
        return;
    #endif
    
    // 显示界面,重绘它
    scui_widget_show(SCUI_UI_SCENE_POPUP, false);
    scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
    
    if (text != SCUI_HANDLE_INVALID)
        scui_string_update_text(popup_string, text);
    else if (string != NULL)
        scui_string_update_str(popup_string, string);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_popup_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        SCUI_ASSERT(scale_tar_w != 0);
        SCUI_ASSERT(scale_tar_h != 0);
        
        // 工步1,3:
        if (scale_way == +1 || scale_way == -1) {
            scui_coord_t  pct_s  = scale_way == +1 ? 0 : 100;
            scui_coord_t  pct_e  = scale_way == +1 ? 100 : 0;
            scui_map_cb_t map_cb = scale_way == +1 ? scui_map_bounce : scui_map_ease_in;
            
            if (popup_anima <= SCUI_UI_POPUP_ANIM_TIME) {
                scui_coord_t pct = scui_map(popup_anima, 0, SCUI_UI_POPUP_ANIM_TIME, pct_s, pct_e);
                scui_area_t clip = scui_widget_clip(event->object);
                
                #if SCUI_MEM_FEAT_MINI == 0
                // 备注:如果是独立画布,此处的clip为<0,0>
                clip.x = clip.y = 0;
                #endif
                
                scui_alpha_t scale_alpha = map_cb(pct, 0, 100, scui_alpha_pct0, scui_alpha_pct100);
                scui_coord_t scale_cur_w = map_cb(pct, 0, 100, 0, scale_tar_w);
                scui_coord_t scale_cur_h = map_cb(pct, 0, 100, 0, scale_tar_h);
                scale_cur_w = scui_clamp(scale_cur_w, 10, scale_tar_w);
                scale_cur_h = scui_clamp(scale_cur_h, 10, scale_tar_h);
                scui_coord_t scale_cur_x = clip.x + (scale_tar_w - scale_cur_w) / 2;
                scui_coord_t scale_cur_y = clip.y + (scale_tar_h - scale_cur_h) / 2;
                
                SCUI_LOG_INFO("popup scale:alpha:%d, pct:%d", scale_alpha, pct);
                scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, scale_alpha, true);
                scui_widget_adjust_size(SCUI_UI_SCENE_POPUP_SCALE, scale_cur_w, scale_cur_h);
                scui_widget_move_pos(SCUI_UI_SCENE_POPUP_SCALE, &(scui_point_t){.x = scale_cur_x, .y = scale_cur_y});
                scui_widget_move_pos(SCUI_UI_SCENE_POPUP_BG, &(scui_point_t){.x = clip.x, .y = clip.y});
                
                scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
                
                popup_anima += SCUI_ANIMA_TICK;
            }
            
            // 工步1结束到达工步2:
            if (scale_way == +1 && popup_anima >= SCUI_UI_POPUP_ANIM_TIME) {
                scui_string_scroll_abort(popup_string, false);
                popup_anima = 0;
                scale_way = 0;
            }
            // 工步3结束:
            if (scale_way == -1 && popup_anima >= SCUI_UI_POPUP_ANIM_TIME) {
                scui_widget_hide(SCUI_UI_SCENE_POPUP, true);
                scale_way = 0xFF;
            }
        }
        
        // 工步2:
        if (scale_way == 0) {
            if (popup_anima <= SCUI_UI_POPUP_WAIT_TIME)
                popup_anima += SCUI_ANIMA_TICK;
            
            // 工步2结束到达工步3:
            if (popup_anima >= SCUI_UI_POPUP_WAIT_TIME &&
                scui_string_scroll_over(popup_string)) {
                popup_anima = 0;
                scale_way = -1;
            } else {
                SCUI_LOG_INFO("popup wait");
                scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, scui_alpha_cover, true);
                scui_widget_draw(SCUI_UI_SCENE_POPUP, NULL, false);
            }
        }
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
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
void scui_ui_scene_popup_bg_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_string_maker_t string_maker = {0};
        string_maker.widget.type                = scui_widget_type_string;
        string_maker.widget.style.trans         = true;
        string_maker.widget.parent              = SCUI_UI_SCENE_POPUP_BG;
        string_maker.widget.clip.x              = 20;
        string_maker.widget.clip.y              = 30;
        string_maker.widget.clip.w              = 350 - 20 * 2;
        string_maker.widget.clip.h              = 40;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFFFFFFFF;
        string_maker.args.color.color_e.full    = 0xFFFFFFFF;
        string_maker.unit_s                     = true;     //单次滚动,结束标记
        string_maker.font_idx                   = SCUI_FONT_IDX_32;
        scui_widget_create(&string_maker, &popup_string);
        scui_string_scroll_abort(popup_string, true);
        
        scui_widget_alpha_set(SCUI_UI_SCENE_POPUP, scui_alpha_trans, true);
        scale_tar_w = scui_widget_clip(SCUI_UI_SCENE_POPUP_SCALE).w;
        scale_tar_h = scui_widget_clip(SCUI_UI_SCENE_POPUP_SCALE).h;
        
        // 启动工步1:
        scale_way   = +1;
        popup_anima = 0;
        
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        // 这里需要填完全覆盖的纯透明色调
        #if SCUI_MEM_FEAT_MINI == 0
        scui_alpha_t alpha = scui_alpha_trans;
        scui_widget_alpha_get(event->object, &alpha);
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_ZEROED);
        scui_widget_alpha_set(event->object, alpha, false);
        #endif
        
        scui_handle_t image = scui_image_prj_image_src_repeat_btn_01_card_mediunpng;
        scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_ZEROED);
        
        break;
    }
    break;
    default:
        break;
    }
}
