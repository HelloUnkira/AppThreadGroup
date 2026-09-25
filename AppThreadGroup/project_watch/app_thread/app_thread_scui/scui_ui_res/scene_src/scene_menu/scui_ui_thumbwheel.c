/*实现目标:
 *    主题:波轮
 *    一圈图标旋转选择, 选中在顶部最大, 背面最小(真实缩放, 无换挡组图)
 *    跟手转动 / 编码器换位 / 动画拨正
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t string;
    scui_multi_t  way;
    scui_coord_t  angle;
    uint8_t       anima:1;
    
    scui_coord_t  angle_unit;    /* 一圈数量: 360 / angle_unit */
    scui_coord_t  angle_cell;    /* 动画颗粒度, 一般不改 */
    scui_coord3_t scale_max;     /* 选中(顶部)缩放 */
    scui_coord3_t scale_min;     /* 背面(底部)缩放 */
    scui_handle_t image_arrow;   /* 指示箭头 */
    
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_thumbwheel_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        scui_ui_res_local->angle_unit  = 24;
        scui_ui_res_local->angle_cell  = 6;
        scui_ui_res_local->image_arrow = scui_image_prj_rpt_arr_05_back;

        scui_ui_scene_list_cfg(scui_ui_scene_list_type_themewheel);
        /* 统一单图: 复刻原换挡组(+0最小22px / +5最大72px), 按menu图实际尺寸反算scale */
        scui_coord_t menu_w = scui_image_w(scui_ui_scene_list[0].image);
        scui_ui_res_local->scale_max   = 72.0f / menu_w;
        scui_ui_res_local->scale_min   = 22.0f / menu_w;
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click:
        break;
    default:
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
            scui_coord_t angle_ofs = scui_ui_res_local->way > 0 ? scui_ui_res_local->angle_unit -
                scui_mabs(scui_ui_res_local->angle, (int64_t)scui_ui_res_local->angle_unit) :
                scui_mabs(scui_ui_res_local->angle, (int64_t)scui_ui_res_local->angle_unit);
            
            angle_ofs %= scui_ui_res_local->angle_cell;
            angle_ofs  = angle_ofs != 0 ? angle_ofs : scui_ui_res_local->angle_cell;
            
            if (scui_ui_res_local->angle % scui_ui_res_local->angle_unit != 0)
                scui_ui_res_local->angle += scui_ui_res_local->way * angle_ofs;
            
            scui_widget_draw(event->object, NULL, false, 0);
        }
        break;
    case scui_event_create: {
        
        scui_area_t clip_w = scui_widget_clip(SCUI_UI_SCENE_THUMBWHEEL);
        
        scui_handle_t image = scui_ui_scene_list[0].image;
        scui_coord_t  image_w = scui_image_w(image);
        scui_coord_t  image_h = scui_image_h(image);
        scui_coord_t  img_dia  = scui_min(image_w, image_h);
        scui_coord_t  img_dist = scui_min(clip_w.w, clip_w.h) / 2 - img_dia / 2;
        
        scui_area_t dst_clip = {0};
        scui_handle_t image_arrow = scui_ui_res_local->image_arrow;
        dst_clip.w = scui_image_w(image_arrow);
        dst_clip.h = scui_image_h(image_arrow);
        dst_clip.x = clip_w.w / 2 - img_dist + img_dia / 2 + dst_clip.w / 2;
        dst_clip.y = clip_w.h / 2 - dst_clip.h / 2;
        dst_clip.x = dst_clip.x + dst_clip.w * 3 / 2;
        
        scui_string_maker_define(string_maker);
        
        string_maker.widget.parent              = event->object;
        string_maker.widget.clip.x              = dst_clip.x;
        string_maker.widget.clip.w              = img_dist * 2 - img_dia * 2 - dst_clip.w * 2;
        string_maker.widget.clip.h              = 60;
        string_maker.widget.clip.y              = clip_w.h / 2 - string_maker.widget.clip.h / 2;
        string_maker.args.align_hor             = 0;
        string_maker.args.align_ver             = 2;
        string_maker.args.mode_scroll           = 1;
        string_maker.args.color     = SCUI_COLOR_MAKE32_SE(true, 0, 0xFFFFFFFF, 0xFFFFFFFF);
        string_maker.font_idx                   = SCUI_FONT_IDX_36;
        scui_widget_create(&string_maker, &scui_ui_res_local->string);
        
        break;
    }
    case scui_event_ptr_click:
    case scui_event_draw_graph: {
        
        /* 偷个懒,我们在绘制前检查动画是否需要关闭 */
        if (scui_ui_res_local->anima)
        if (scui_ui_res_local->angle % scui_ui_res_local->angle_unit == 0) {
            scui_ui_res_local->anima = false;
            scui_widget_draw(SCUI_UI_SCENE_THUMBWHEEL, NULL, false, 0);
        }
        
        scui_area_t  clip_w = scui_widget_area(event->object);
        scui_coord_t widget_cx = clip_w.x + clip_w.w / 2;
        scui_coord_t widget_cy = clip_w.y + clip_w.h / 2;
        
        scui_handle_t image_base = scui_ui_scene_list[0].image;
        scui_coord_t  image_w = scui_image_w(image_base);
        scui_coord_t  image_h = scui_image_h(image_base);
        scui_coord_t  img_dia  = scui_min(image_w, image_h);
        scui_coord_t  img_dist = scui_min(clip_w.w, clip_w.h) / 2 - img_dia / 2;
        
        scui_coord_t angle_bse = scui_ui_res_local->angle / scui_ui_res_local->angle_unit;
        scui_coord_t angle_ofs = scui_ui_res_local->angle % scui_ui_res_local->angle_unit;
        for (scui_coord_t angle = 0; angle < 360; angle += scui_ui_res_local->angle_unit) {
            scui_multi_t  img_f = (180);
            scui_multi_t  img_x = (scui_cos4096(angle + angle_ofs + img_f) * img_dist) >> 12;
            scui_multi_t  img_y = (scui_sin4096(angle + angle_ofs + img_f) * img_dist) >> 12;
            scui_coord_t  idx_ofs = angle / scui_ui_res_local->angle_unit - angle_bse;
            scui_handle_t lst_ofs = scui_mabs(idx_ofs, (int64_t)scui_ui_scene_list_num);
            img_y = -img_y; /* x轴翻转(顺时针与逆时针的切换) */
            
            /* 真实缩放: 复刻原换挡组 scui_map(ofs,0,330,MAX,MIN): 顶部(ofs=0)最大72,
               顺时针一圈连续缩到最小22(ofs=330), 整环均匀过渡 */
            scui_coord3_t t = (scui_coord3_t)(angle + angle_ofs) /
                (360.0f - scui_ui_res_local->angle_unit);
            if (t > 1.0f) t = 1.0f;
            scui_coord3_t scale_s = scui_ui_res_local->scale_max +
                (scui_ui_res_local->scale_min - scui_ui_res_local->scale_max) * t;
            scui_coord_t icon_w = image_w * scale_s;
            scui_coord_t icon_h = image_h * scale_s;
            
            scui_area_t dst_clip = {0};
            dst_clip.x = widget_cx + img_x - icon_w / 2;
            dst_clip.y = widget_cy + img_y - icon_h / 2;
            dst_clip.w = icon_w;
            dst_clip.h = icon_h;
            
            scui_handle_t image = scui_ui_scene_list[lst_ofs].image;
            
            if (event->type == scui_event_draw_graph) {
                /* 绘制目标:从滚动空间坐标转换为控件局部坐标 */
                scui_area_t draw_clip = dst_clip;
                draw_clip.x -= clip_w.x;
                draw_clip.y -= clip_w.y;
                
                /* 画左边箭头以及文本: */
                if (angle == 0) {
                    scui_handle_t image_arrow = scui_ui_res_local->image_arrow;
                    scui_area_t dst_clip_a = {0};
                    dst_clip_a.w = scui_image_w(image_arrow);
                    dst_clip_a.h = scui_image_h(image_arrow);
                    dst_clip_a.x = widget_cx - img_dist + img_dia / 2 + dst_clip_a.w / 2;
                    dst_clip_a.y = widget_cy - dst_clip_a.h / 2;
                    /* 箭头绘制目标:转换为控件局部坐标 */
                    dst_clip_a.x -= clip_w.x;
                    dst_clip_a.y -= clip_w.y;
                    scui_widget_draw_image(event->object, &dst_clip_a, image_arrow, NULL, SCUI_COLOR_UNUSED);
                    
                    scui_handle_t text = scui_ui_scene_list[lst_ofs].text;
                    scui_string_update_text(scui_ui_res_local->string, text);
                }
                
                scui_point_t scale = {
                    .x = icon_w * SCUI_SCALE_COF / image_w,
                    .y = icon_h * SCUI_SCALE_COF / image_h,
                };
                scui_area_t  area_i  = scui_image_area(image);
                scui_area_t  area_w  = draw_clip;
                scui_point_t anchor = scui_area_center(&area_w);
                scui_point_t center = scui_area_center(&area_i);
                scui_widget_draw_image_scale(event->object, &draw_clip, image, NULL, SCUI_COLOR_UNUSED, anchor, center, scale);
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
        
        scui_area_t  clip_w = scui_widget_clip(event->object);
        scui_coord_t widget_cx   = clip_w.x + clip_w.w / 2;
        scui_coord_t widget_cy   = clip_w.y + clip_w.h / 2;
        scui_coord_t widget_dist = scui_min(clip_w.w, clip_w.h);
        
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
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_up:
        if (scui_ui_res_local->angle % scui_ui_res_local->angle_unit != 0)
            scui_ui_res_local->anima = true;
        break;
    case scui_event_enc_tick: {
        scui_event_mask_over(event);
        
        /* 补完上次没跑完的位移 */
        if (scui_ui_res_local->angle % scui_ui_res_local->angle_unit != 0) {
            
            if (scui_ui_res_local->way == +1)
                scui_ui_res_local->angle += scui_ui_res_local->angle_unit -
                scui_ui_res_local->angle %  scui_ui_res_local->angle_unit;
            
            if (scui_ui_res_local->way == -1)
                scui_ui_res_local->angle -= scui_ui_res_local->angle_unit -
                scui_ui_res_local->angle %  scui_ui_res_local->angle_unit;
        }
        
        if (event->enc_way == 0) {
            /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
            for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
                scui_ui_res_local->angle += scui_ui_res_local->angle_unit;
            
            scui_ui_res_local->angle += 1;
            scui_ui_res_local->way   = +1;
        }
        if (event->enc_way == 1) {
            /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
            for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
                scui_ui_res_local->angle -= scui_ui_res_local->angle_unit;
            
            scui_ui_res_local->angle -= 1;
            scui_ui_res_local->way   = -1;
        }
        
        if (scui_ui_res_local->angle % scui_ui_res_local->angle_unit != 0)
            scui_ui_res_local->anima = true;
        break;
    }
    default:
        break;
    }
}
