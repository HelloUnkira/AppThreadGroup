/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_multi_t way;
    scui_coord_t skew;
    scui_coord_t route;
    uint8_t      anima:1;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_spread_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
            
            scui_ui_scene_list_cfg(scui_ui_scene_list_type_spread);
        }
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            // scui_ui_res_local->route = SCUI_UI_SPREAD_RAD_MIN + SCUI_UI_SPREAD_DIS_MAX;
            scui_ui_res_local->route = SCUI_UI_SPREAD_DIS_MAX * 2;
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
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
void scui_ui_scene_spread_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (scui_ui_res_local->anima) {
            scui_coord_t angle_ofs = scui_ui_res_local->way > 0 ? scui_ui_res_local->route -
                scui_mabs(scui_ui_res_local->skew, scui_ui_res_local->route) :
                scui_mabs(scui_ui_res_local->skew, scui_ui_res_local->route);
            
            angle_ofs %= SCUI_UI_THEMEWHEEL_ANGLE_CELL;
            angle_ofs  = angle_ofs != 0 ? angle_ofs : SCUI_UI_THEMEWHEEL_ANGLE_CELL;
            
            if (scui_ui_res_local->skew %  scui_ui_res_local->route != 0)
                scui_ui_res_local->skew += scui_ui_res_local->way * angle_ofs;
            
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    case scui_event_ptr_click:
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  widget_clip = scui_widget_clip(event->object);
        scui_coord_t widget_cx   = widget_clip.x + widget_clip.w / 2;
        scui_coord_t widget_cy   = widget_clip.y + widget_clip.h / 2;
        
        /* 大圈小圈限制半径, 环图标数量 */
        scui_coord_t image_min_w = scui_image_w(scui_ui_scene_list_image[0] + SCUI_UI_SPREAD_OFS_MIN);
        scui_coord_t image_min_h = scui_image_h(scui_ui_scene_list_image[0] + SCUI_UI_SPREAD_OFS_MIN);
        scui_coord_t image_max_w = scui_image_w(scui_ui_scene_list_image[0] + SCUI_UI_SPREAD_OFS_MAX);
        scui_coord_t image_max_h = scui_image_h(scui_ui_scene_list_image[0] + SCUI_UI_SPREAD_OFS_MAX);
        scui_multi_t img_min_rad = scui_min(image_min_w, image_min_h) / 2;
        scui_multi_t img_max_rad = scui_min(image_max_w, image_max_h) / 2;
        scui_coord_t img_num_rad = 360 / SCUI_UI_SPREAD_ANGLE_UNIT;
        /* 转化到图标边界 */
        img_max_rad = scui_min(widget_clip.w, widget_clip.h) / 2 - img_max_rad;
        img_min_rad = SCUI_UI_SPREAD_RAD_FADE + img_min_rad;
        /* 基准偏移校准,将偏移值校准到起始点 */
        scui_multi_t img_ofs_dis = SCUI_UI_SPREAD_OFS_MAX - SCUI_UI_SPREAD_OFS_MIN;
        scui_multi_t skew_bse = scui_ui_res_local->skew / SCUI_UI_SPREAD_DIS_MAX;
        scui_multi_t skew_ofs = scui_mabs(scui_abs(scui_ui_res_local->skew), SCUI_UI_SPREAD_DIS_MAX);
        
        
        
        #if 1   // patch
        /* 逆向标记(算是补丁) */
        scui_multi_t skew_rev = (scui_abs(skew_bse) > 1 && skew_bse * scui_ui_res_local->way < 0) ? -1 : 1;
        skew_rev = scui_abs(skew_bse) < 2 ? -1 : skew_rev;
        /* 0到+-route额外再逆向一下 */
        if (scui_abs(scui_ui_res_local->skew) < scui_ui_res_local->route) {
        
            if ((scui_ui_res_local->skew <= 0 && scui_ui_res_local->way < 0) ||
                (scui_ui_res_local->skew >= 0 && scui_ui_res_local->way > 0))
                 skew_rev = -skew_rev;
        }
        /*  */
        skew_ofs = SCUI_UI_SPREAD_RAD_MIN + skew_ofs * scui_ui_res_local->way * skew_rev;
        /* 逆向迭代到最小边界之外 */
        scui_multi_t skew_idx = 0;
        for (skew_idx = 0; true; skew_idx--) {
        
            if (skew_ofs < img_min_rad)
                break;
            skew_ofs -= SCUI_UI_SPREAD_DIS_MAX;
        }
        
        /* 临时插个眼:还有一个逻辑没调试完毕 */
        /* way等于-1时, skew从0到-route时的迭代有错误  */
        if (-scui_ui_res_local->route < scui_ui_res_local->skew && scui_ui_res_local->skew <= 0 && scui_ui_res_local->way < 0) {
        }
        
        #endif
        
        
        
        /* 多个圈,从最内圈画到最外圈直到画不出为止 */
        for (scui_coord_t idx = skew_idx; true; skew_ofs += SCUI_UI_SPREAD_DIS_MAX, idx++) {
            
            /* 当前值小于最小边界,跳过 */
            if (skew_ofs < img_min_rad)
                continue;
            /* 当前值大于最大边界,退出 */
            if (skew_ofs > img_max_rad)
                break;
            
            /* 第多少圈取决于基础轮转与基准点加尾偏移 */
            scui_coord_t lst_idx = -skew_bse * scui_ui_res_local->way * skew_rev + idx;
            
            #if 0
            SCUI_LOG_WARN("skew_ofs:%d\n", skew_ofs);
            SCUI_LOG_WARN("arr_off:%d \n", lst_idx);
            SCUI_LOG_WARN("cir_ofs:%d \n", lst_idx % 2 != 0);
            for (scui_coord_t ang = 0;  ang < 360; ang += SCUI_UI_SPREAD_ANGLE_UNIT) {
                 scui_coord_t idx_off = ang / SCUI_UI_SPREAD_ANGLE_UNIT + lst_idx * img_num_rad;
                 SCUI_LOG_WARN_RAW("%d", scui_mabs(idx_off, (scui_coord_t)scui_ui_scene_list_num));
            }
            SCUI_LOG_WARN("\n");
            #endif
            
            /* 通过行程映射出当前点偏移 */
            scui_handle_t img_map = scui_map(skew_ofs, img_min_rad, img_max_rad, 0, img_ofs_dis);
            
            /* 画这一圈的点,它跟随偏移量向内部收缩 */
            for (scui_coord_t angle = 0; angle < 360; angle += SCUI_UI_SPREAD_ANGLE_UNIT) {
                scui_handle_t image = scui_ui_scene_list_image[0] + img_map;
                scui_coord_t  image_w = scui_image_w(image);
                scui_coord_t  image_h = scui_image_h(image);
                /* 是否为偏转圆 */
                scui_coord_t angle_ofs = lst_idx % 2 != 0 ? SCUI_UI_SPREAD_ANGLE_UNIT / 2 : 0;
                /* 偏转圆比平均偏移一半 */
                scui_multi_t  img_x    = (scui_cos4096(angle + angle_ofs) * skew_ofs) >> 12;
                scui_multi_t  img_y    = (scui_sin4096(angle + angle_ofs) * skew_ofs) >> 12;
                scui_coord_t  idx_ofs  = angle / SCUI_UI_SPREAD_ANGLE_UNIT - lst_idx * img_num_rad;
                scui_handle_t lst_ofs  = scui_mabs(idx_ofs, (int64_t)scui_ui_scene_list_num);
                scui_area_t   dst_clip = {0};
                dst_clip.x = widget_cx + img_x - image_w / 2;
                dst_clip.y = widget_cy + img_y - image_h / 2;
                dst_clip.w = image_w;
                dst_clip.h = image_h;
                
                if (event->type == scui_event_draw) {
                    scui_handle_t image = scui_ui_scene_list_image[lst_ofs] + img_map;
                    scui_widget_draw_image(event->object, &dst_clip, image, NULL, SCUI_COLOR_UNUSED);
                }
                if (event->type == scui_event_ptr_click) {
                    if (scui_area_point(&dst_clip, &event->ptr_c)) {
                        SCUI_LOG_WARN("click idx:%d", lst_ofs);
                        break;
                    }
                }
            }
        }
        break;
    }
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_up:
        if (scui_ui_res_local->skew  % scui_ui_res_local->route != 0)
            scui_ui_res_local->anima = true;
        break;
    case scui_event_ptr_fling: {
        scui_widget_event_mask_over(event);
        
        scui_area_t  widget_clip = scui_widget_clip(event->object);
        scui_coord_t widget_cx   = widget_clip.x + widget_clip.w / 2;
        scui_coord_t widget_cy   = widget_clip.y + widget_clip.h / 2;
        
        #if 0
        #elif SCUI_UI_SPREAD_FLING_MODE == 1
        scui_point_t pos_s = event->ptr_s;
        scui_point_t pos_e = event->ptr_e;
        /* 规则:计算起始点和结束点相对中心的坐标形成的圆的半径即可 */
        pos_s.x = scui_max(pos_s.x, widget_cx) - scui_min(pos_s.x, widget_cx);
        pos_s.y = scui_max(pos_s.y, widget_cy) - scui_min(pos_s.y, widget_cy);
        pos_e.x = scui_max(pos_e.x, widget_cx) - scui_min(pos_e.x, widget_cx);
        pos_e.y = scui_max(pos_e.y, widget_cy) - scui_min(pos_e.y, widget_cy);
        
        /* 无需开方了 */
        if (pos_s.x * pos_s.x + pos_s.y * pos_s.y >
            pos_e.x * pos_e.x + pos_e.y * pos_e.y) {
            /* 动画正在运行中不响应事件 */
            if (!scui_ui_res_local->anima) {
                /* 一点点小小的拨动,让动画跑起来 */
                scui_ui_res_local->anima = true;
                scui_ui_res_local->skew -= 1;
                scui_ui_res_local->way  = -1;
            }
        } else {
            /* 动画正在运行中不响应事件 */
            if (!scui_ui_res_local->anima) {
                /* 一点点小小的拨动,让动画跑起来 */
                scui_ui_res_local->anima = true;
                scui_ui_res_local->skew += 1;
                scui_ui_res_local->way  = +1;
            }
        }
        #elif SCUI_UI_SPREAD_FLING_MODE == 0
        
        #if 0
        #elif SCUI_UI_SPREAD_FLING_WAY == 0
        if (true)
        #elif SCUI_UI_SPREAD_FLING_WAY == 1
        if ((scui_indev_ptr_dir(event) & scui_opt_dir_hor) != 0)
        #elif SCUI_UI_SPREAD_FLING_WAY == 2
        if ((scui_indev_ptr_dir(event) & scui_opt_dir_ver) != 0)
        #else
        #error "unsupport"
        if (false)
        #endif
        /*   */ {
            /* 动画正在运行中不响应事件 */
            if (!scui_ui_res_local->anima) {
                /* 一点点小小的拨动,让动画跑起来 */
                scui_ui_res_local->anima = true;
                scui_ui_res_local->skew += 1;
                scui_ui_res_local->way  = +1;
            }
        }
        
        #else
        #error "unsupport"
        #endif
        
        break;
    }
    case scui_event_enc_clockwise: {
        scui_widget_event_mask_over(event);
        
        /* 补完上次没跑完的位移 */
        if (scui_ui_res_local->skew % scui_ui_res_local->route != 0) {
            
            if (scui_ui_res_local->way == +1)
                scui_ui_res_local->skew += scui_ui_res_local->route -
                scui_ui_res_local->skew %  scui_ui_res_local->route;
            
            if (scui_ui_res_local->way == -1)
                scui_ui_res_local->skew -= scui_ui_res_local->route -
                scui_ui_res_local->skew %  scui_ui_res_local->route;
        }
        
        /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
        for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
            scui_ui_res_local->skew += scui_ui_res_local->route;
        
        scui_ui_res_local->skew += 1;
        scui_ui_res_local->way  = +1;
        
        if (scui_ui_res_local->skew  % scui_ui_res_local->route != 0)
            scui_ui_res_local->anima = true;
        break;
    }
    case scui_event_enc_clockwise_anti: {
        scui_widget_event_mask_over(event);
        
        /* 补完上次没跑完的位移 */
        if (scui_ui_res_local->skew % scui_ui_res_local->route != 0) {
            
            if (scui_ui_res_local->way == +1)
                scui_ui_res_local->skew += scui_ui_res_local->route -
                scui_ui_res_local->skew %  scui_ui_res_local->route;
            
            if (scui_ui_res_local->way == -1)
                scui_ui_res_local->skew -= scui_ui_res_local->route -
                scui_ui_res_local->skew %  scui_ui_res_local->route;
        }
        
        /* 多次旋转,跳过前面的动画直接加进去,动画拨正只跑最后一帧 */
        for (scui_coord_t idx = 0; idx < event->enc_diff - 1; idx++)
            scui_ui_res_local->skew -= scui_ui_res_local->route;
        
        scui_ui_res_local->skew -= 1;
        scui_ui_res_local->way  = -1;
        
        if (scui_ui_res_local->skew  % scui_ui_res_local->route != 0)
            scui_ui_res_local->anima = true;
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
