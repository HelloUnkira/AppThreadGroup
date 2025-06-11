/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t string;
    scui_multi_t  way;
    scui_coord_t  angle;
    uint8_t       anima:1;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_thumbwheel_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_local_res:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        break;
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            
            scui_ui_scene_list_cfg(scui_ui_scene_list_type_themewheel);
            
            scui_area_t widget_clip = scui_widget_clip(SCUI_UI_SCENE_THUMBWHEEL);
            
            scui_coord_t image_w  = scui_image_w(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
            scui_coord_t image_h  = scui_image_h(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
            scui_coord_t img_dia  = scui_min(image_w, image_h);
            scui_coord_t img_dist = scui_min(widget_clip.w, widget_clip.h) / 2 - img_dia / 2;
            
            scui_area_t dst_clip = {0};
            scui_handle_t image_arrow = SCUI_UI_THEMEWHEEL_IMAGE_ARROW;
            dst_clip.w = scui_image_w(image_arrow);
            dst_clip.h = scui_image_h(image_arrow);
            dst_clip.x = widget_clip.w / 2 - img_dist + img_dia / 2 + dst_clip.w / 2;
            dst_clip.y = widget_clip.h / 2 - dst_clip.h / 2;
            dst_clip.x = dst_clip.x + dst_clip.w * 3 / 2;
            
            scui_string_maker_t string_maker = {0};
            string_maker.widget.type                = scui_widget_type_string;
            string_maker.widget.style.trans         = true;
            string_maker.widget.parent              = SCUI_UI_SCENE_THUMBWHEEL_CUSTOM;
            string_maker.widget.clip.x              = dst_clip.x;
            string_maker.widget.clip.w              = img_dist * 2 - img_dia * 2 - dst_clip.w * 2;
            string_maker.widget.clip.h              = 60;
            string_maker.widget.clip.y              = widget_clip.h / 2 - string_maker.widget.clip.h / 2;
            string_maker.args.align_hor             = 0;
            string_maker.args.align_ver             = 2;
            string_maker.args.mode_scroll           = 1;
            string_maker.args.color.color_s.full    = 0xFFFFFFFF;
            string_maker.args.color.color_e.full    = 0xFFFFFFFF;
            string_maker.args.color.filter          = true;
            string_maker.font_idx                   = SCUI_FONT_IDX_36;
            scui_widget_create(&string_maker, &scui_ui_res_local->string, false);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_key_click:
        break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_thumbwheel_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        if (scui_ui_res_local->anima) {
            scui_coord_t angle_ofs = scui_ui_res_local->way > 0 ? SCUI_UI_THEMEWHEEL_ANGLE_UNIT -
                scui_mabs(scui_ui_res_local->angle, (int64_t)SCUI_UI_THEMEWHEEL_ANGLE_UNIT) :
                scui_mabs(scui_ui_res_local->angle, (int64_t)SCUI_UI_THEMEWHEEL_ANGLE_UNIT);
            
            angle_ofs %= SCUI_UI_THEMEWHEEL_ANGLE_CELL;
            angle_ofs  = angle_ofs != 0 ? angle_ofs : SCUI_UI_THEMEWHEEL_ANGLE_CELL;
            
            if (scui_ui_res_local->angle %  SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0)
                scui_ui_res_local->angle += scui_ui_res_local->way * angle_ofs;
            
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    case scui_event_ptr_click:
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        /* 偷个懒,我们在绘制前检查动画是否需要关闭 */
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT == 0) {
            scui_ui_res_local->anima = false;
            scui_widget_draw(SCUI_UI_SCENE_THUMBWHEEL, NULL, false);
        }
        
        scui_area_t  widget_clip = scui_widget_clip(event->object);
        scui_coord_t widget_cx   = widget_clip.x + widget_clip.w / 2;
        scui_coord_t widget_cy   = widget_clip.y + widget_clip.h / 2;
        
        scui_coord_t image_w  = scui_image_w(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
        scui_coord_t image_h  = scui_image_h(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
        scui_coord_t img_dia  = scui_min(image_w, image_h);
        scui_coord_t img_dist = scui_min(widget_clip.w, widget_clip.h) / 2 - img_dia / 2;
        
        scui_coord_t angle_bse = scui_ui_res_local->angle / SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        scui_coord_t angle_ofs = scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        for (scui_coord_t angle = 0; angle < 360;  angle += SCUI_UI_THEMEWHEEL_ANGLE_UNIT) {
            scui_multi_t  img_f = (180);
            scui_multi_t  img_x = (scui_cos4096(angle + angle_ofs + img_f) * img_dist) >> 12;
            scui_multi_t  img_y = (scui_sin4096(angle + angle_ofs + img_f) * img_dist) >> 12;
            scui_coord_t  idx_ofs = angle / SCUI_UI_THEMEWHEEL_ANGLE_UNIT - angle_bse;
            scui_handle_t lst_ofs = scui_mabs(idx_ofs, (int64_t)scui_ui_scene_list_num);
            img_y = -img_y; // x轴翻转(顺时针与逆时针的切换)
            scui_area_t dst_clip = {0};
            dst_clip.x = widget_cx + img_x - img_dia / 2;
            dst_clip.y = widget_cy + img_y - img_dia / 2;
            dst_clip.w = image_w;
            dst_clip.h = image_h;
            
            #if 1
            scui_coord_t ofs_a = angle + angle_ofs;
            scui_handle_t lst_a = scui_map(ofs_a, 0, 360 - SCUI_UI_THEMEWHEEL_ANGLE_UNIT,
                SCUI_UI_THEMEWHEEL_OFS_MAX, SCUI_UI_THEMEWHEEL_OFS_MIN);
            #else
            scui_handle_t lst_a = SCUI_UI_THEMEWHEEL_OFS_MAX;
            #endif
            
            scui_handle_t image = scui_ui_scene_list_image[lst_ofs] + lst_a;
            dst_clip.x += (dst_clip.w - scui_image_w(image)) / 2;
            dst_clip.y += (dst_clip.h - scui_image_h(image)) / 2;
            dst_clip.w = scui_image_w(image);
            dst_clip.h = scui_image_h(image);
            
            if (event->type == scui_event_draw) {
                /* 画左边箭头以及文本: */
                if (angle == 0) {
                    scui_handle_t image_arrow = SCUI_UI_THEMEWHEEL_IMAGE_ARROW;
                    scui_area_t dst_clip_a = {0};
                    dst_clip_a.w = scui_image_w(image_arrow);
                    dst_clip_a.h = scui_image_h(image_arrow);
                    dst_clip_a.x = widget_cx - img_dist + img_dia / 2 + dst_clip_a.w / 2;
                    dst_clip_a.y = widget_cy - dst_clip_a.h / 2;
                    scui_widget_draw_image(event->object, &dst_clip_a, image_arrow, NULL, SCUI_COLOR_UNUSED);
                    
                    scui_handle_t text = scui_ui_scene_list_text[lst_ofs];
                    scui_string_update_text(scui_ui_res_local->string, text);
                }
                
                scui_widget_draw_image(event->object, &dst_clip, image, NULL, SCUI_COLOR_UNUSED);
            }
            if (event->type == scui_event_ptr_click) {
                if (scui_area_point(&dst_clip, &event->ptr_c)) {
                    SCUI_LOG_WARN("click idx:%d", lst_ofs);
                    break;
                }
            }
        }
        
        break;
    }
    case scui_event_ptr_move: {
        scui_event_mask_over(event);
        
        scui_area_t  widget_clip = scui_widget_clip(event->object);
        scui_coord_t widget_cx   = widget_clip.x + widget_clip.w / 2;
        scui_coord_t widget_cy   = widget_clip.y + widget_clip.h / 2;
        scui_coord_t widget_dist = scui_min(widget_clip.w, widget_clip.h);
        
        scui_coord_t image_w  = scui_image_w(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
        scui_coord_t image_h  = scui_image_h(scui_ui_scene_list_image[0] + SCUI_UI_THEMEWHEEL_OFS_MAX);
        scui_coord_t img_dia  = scui_min(image_w, image_h);
        scui_coord_t img_dist = scui_min(widget_clip.w, widget_clip.h) / 2 - img_dia / 2;
        
        scui_coord_t flag = 0;
        scui_coord_t pos_s_x = event->ptr_s.x;
        scui_coord_t pos_s_y = event->ptr_s.y;
        scui_coord_t pos_e_x = event->ptr_e.x;
        scui_coord_t pos_e_y = event->ptr_e.y;
        scui_coord_t del_x = scui_max(pos_s_x, pos_e_x) - scui_min(pos_s_x, pos_e_x);
        scui_coord_t del_y = scui_max(pos_s_y, pos_e_y) - scui_min(pos_s_y, pos_e_y);
        /* 先判断方向(以圆心为分割线四个象限的运动) */
        if ((pos_s_x > widget_cx && pos_e_y > pos_s_y && del_x <= del_y) ||
            (pos_s_x < widget_cx && pos_e_y < pos_s_y && del_x <= del_y) ||
            (pos_s_y < widget_cy && pos_e_x > pos_s_x && del_x >= del_y) ||
            (pos_s_y > widget_cy && pos_e_x < pos_s_x && del_x >= del_y)) {
             scui_ui_res_local->way = flag = +1;  // 顺时针
             scui_ui_res_local->way = flag = -1;  // 逆时针
        } else {
             scui_ui_res_local->way = flag = -1;  // 顺时针
             scui_ui_res_local->way = flag = +1;  // 逆时针
        }
        /* 计算增量比例,以最大值做贡献 */
        scui_coord_t del_p = scui_max(del_x, del_y);
        /* 位移向量映射到直径上,直径是180度,做出比值成偏转角 */
        scui_ui_res_local->angle += flag * del_p * 180 / widget_dist;
        
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_up:
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0)
            scui_ui_res_local->anima = true;
        break;
    case scui_event_enc_clockwise: {
        scui_event_mask_over(event);
        
        /* 补完上次没跑完的位移 */
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0) {
            
            if (scui_ui_res_local->way == +1)
                scui_ui_res_local->angle += SCUI_UI_THEMEWHEEL_ANGLE_UNIT -
                scui_ui_res_local->angle %  SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
            
            if (scui_ui_res_local->way == -1)
                scui_ui_res_local->angle -= SCUI_UI_THEMEWHEEL_ANGLE_UNIT -
                scui_ui_res_local->angle %  SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        }
        
        /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
        for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
            scui_ui_res_local->angle += SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        
        scui_ui_res_local->angle += 1;
        scui_ui_res_local->way   = +1;
        
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0)
            scui_ui_res_local->anima = true;
        break;
    }
    case scui_event_enc_clockwise_anti: {
        scui_event_mask_over(event);
        
        /* 补完上次没跑完的位移 */
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0) {
            
            if (scui_ui_res_local->way == +1)
                scui_ui_res_local->angle += SCUI_UI_THEMEWHEEL_ANGLE_UNIT -
                scui_ui_res_local->angle %  SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
            
            if (scui_ui_res_local->way == -1)
                scui_ui_res_local->angle -= SCUI_UI_THEMEWHEEL_ANGLE_UNIT -
                scui_ui_res_local->angle %  SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        }
        
        /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
        for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
            scui_ui_res_local->angle -= SCUI_UI_THEMEWHEEL_ANGLE_UNIT;
        
        scui_ui_res_local->angle -= 1;
        scui_ui_res_local->way   = -1;
        
        if (scui_ui_res_local->angle % SCUI_UI_THEMEWHEEL_ANGLE_UNIT != 0)
            scui_ui_res_local->anima = true;
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
