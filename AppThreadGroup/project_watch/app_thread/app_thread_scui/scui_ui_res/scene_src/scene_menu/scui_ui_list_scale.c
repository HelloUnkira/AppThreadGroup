/*实现目标:
 *    可缩放列表界面: buffer_d动态画布 + draw_finish回收
 *    draw_graph: 子控件树绘制(背景+卡片+icon+箭头, 100%) → 独立画布内部
 *    draw_buffer: 主控件绘制(画布信息缩放blit) → 父画布
 *    旧 linear_m.draw → draw_buffer(仅buffer_2d), 旧 linear_s.draw → draw_graph(含背景)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t     item_m[50];         /* 列表项句柄 */
    scui_coord_t      scroll_pct;         /* 滚动百分比(用于缩放计算) */
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 列表项控件事件响应回调
 *       draw_graph:旧linear_s(子控件树) → 绘制到独立画布
 *       draw_buffer:旧linear_m(主控件) → 重定向绘制到父画布
 *@param event 事件
 */
static void scui_ui_scene_scale_item_event_proc(scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_source_check(event->object);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw_graph: {
        /* 旧 linear_s.draw 职责:
         *   画背景 + 画icon + 画箭头(均未缩放,100%)
         *   绘制目标: 自身独立画布(widget->surface_c)
         *   背景和内容一起绘制, 在draw_buffer中由buffer_2d统一缩放blit
         */
        
        /* 匹配当前项目索引 */
        scui_handle_t match_idx = -1;
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++)
            if (scui_ui_res_local->item_m[idx] == event->object) {
                match_idx = idx;
                continue;
            }
        SCUI_ASSERT(match_idx != (scui_handle_t)-1);
        
        /* 绘制坐标相对于独立画布原点(0,0) */
        scui_area_t clip_w = {.x = 0, .y = 0,
                              .w = widget->clip.w, .h = widget->clip.h,};
        
        /* 缩放背景(100%, 实际缩放在blit时由buffer_2d完成) */
        scui_color_t bg_color = {.color.full = 0xFF282828,};
        scui_widget_draw_color(event->object, NULL, bg_color);
        
        /* 9-patch卡片背景(100%,全填充, 会在blit时统一缩放) */
        scui_area_t  btn_clip = clip_w;
        scui_color_t btn_color_full = {.color.full = 0xFF282828,};
        scui_handle_t btn_image_full[4] = {
            scui_image_prj_image_src_repeat_card_04_r36_1bmp,
            scui_image_prj_image_src_repeat_card_05_r36_2bmp,
            scui_image_prj_image_src_repeat_card_06_r36_3bmp,
            scui_image_prj_image_src_repeat_card_07_r36_4bmp,
        };
        scui_custom_data_t data = {0};
        scui_custom_data_config_image_crect4(&data,
            btn_image_full, btn_color_full, -1);
        scui_custom_draw_image_crect4(event->object, &btn_clip, &data);
        
        /* 绘制图标(100%, 居中) */
        scui_opt_pos_t img_pos  = scui_opt_pos_c;
        scui_handle_t  image_icon = scui_ui_scene_list_image[match_idx] + 3;
        scui_point_t   img_scale = {.x = SCUI_SCALE_COF, .y = SCUI_SCALE_COF,};
        scui_widget_draw_image_scale(event->object, NULL,
            image_icon,
            NULL,
            img_scale,
            img_pos);
        
        /* 绘制箭头指示器(正常尺寸,右侧居中) */
        {
            scui_area_t arrow_clip  = clip_w;
            scui_handle_t image_arrow = scui_image_prj_image_src_repeat_arrow_06_backbmp;
            arrow_clip.x = arrow_clip.w - scui_image_w(image_arrow);
            arrow_clip.w = scui_image_w(image_arrow);
            arrow_clip.y += (arrow_clip.h - scui_image_h(image_arrow)) / 2;
            arrow_clip.h -= (arrow_clip.h - scui_image_h(image_arrow));
            scui_color_t color_white = {.filter = true,.color.full = 0xFFFFFFFF,};
            scui_widget_draw_image(event->object, &arrow_clip,
                image_arrow,
                NULL,
                color_white);
        }
        
        break;
    }
    case scui_event_draw_buffer: {
        /* 旧 linear_m.draw 职责:
         *   画布信息缩放blit(背景已在draw_graph绘制在独立画布上)
         *   绘制目标: 父控件画布(scroll的surface_c)
         */
        
        /* 匹配当前项目索引 */
        scui_handle_t match_idx = -1;
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++)
            if (scui_ui_res_local->item_m[idx] == event->object) {
                match_idx = idx;
                continue;
            }
        SCUI_ASSERT(match_idx != (scui_handle_t)-1);
        
        /* 计算缩放百分比(基于父控件中心与当前控件中心的距离) */
        scui_multi_t percent = 100;
        {
            scui_area_t clip_p = scui_widget_clip(scui_widget_parent(event->object));
            scui_area_t clip_a = scui_widget_clip(event->object);
            scui_point_t offset = {0};
            offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_a.x + clip_a.w / 2);
            offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_a.y + clip_a.h / 2);
            
            scui_multi_t rad_rr = clip_p.h / 2 - widget->clip.h / 2;
            scui_multi_t dist_y = (1024 * scui_min(rad_rr, offset.y) / rad_rr);
            scui_multi_t cos_a2 = (1024 * 1024) - dist_y * dist_y;
            scui_multi_t cos_ia = 0, cos_fa = 0;
            scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
            scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
            
            dist_x  = scui_min(dist_x, clip_p.w / 2);
            percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
            percent = scui_min(scui_max(percent, 30), 100);
        }
        
        /* 渐隐: 保存原始alpha并设置渐隐alpha */
        scui_alpha_t alpha_raw = scui_alpha_cover;
        scui_widget_alpha_get(event->object, &alpha_raw);
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, alpha, true);
        
        /* 画布信息缩放blit: 将独立画布(含背景+卡片+icon+箭头,均为100%)缩放到父画布 */
        /*    anchor: 源图中心(本地坐标); center: 目标中心(父控件坐标) */
        scui_point_t anchor = {
            .x = widget->clip.w / 2,
            .y = widget->clip.h / 2,
        };
        scui_point_t center = {
            .x = widget->clip.x + widget->clip.w / 2,
            .y = widget->clip.y + widget->clip.h / 2,
        };
        scui_point_t scale = {
            .x = SCUI_SCALE_COF * (scui_multi_t)percent / 100,
            .y = SCUI_SCALE_COF * (scui_multi_t)percent / 100,
        };
        scui_widget_draw_buffer_2d(event->object, NULL,
            NULL, anchor, center, scale, 0, SCUI_COLOR_UNUSED);
        
        /* 恢复原始alpha */
        scui_widget_alpha_set(event->object, alpha_raw, true);
        break;
    }
    case scui_event_ptr_click: {
        
        /* 半透明项忽略点击 */
        scui_alpha_t alpha = scui_alpha_trans;
        scui_widget_alpha_get(event->object, &alpha);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_event_mask_over(event);
        scui_handle_t index = scui_widget_child_to_index(event->object) - 1;
        SCUI_LOG_WARN("click idx:%d", index);
        break;
    }
    default:
        break;
    }
}

/*@brief 弧形进度条事件回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 滚动控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        /* 构造器:列表项 */
        scui_custom_maker_t item_maker = {0};
        scui_widget_maker_def_cfg(&item_maker, scui_widget_type_custom);
        
        /* 独立画布 + 动态构建(延迟到draw_ready分配pixel) */
        item_maker.widget.style.buffer    = true;
        item_maker.widget.style.buffer_d  = true;
        item_maker.widget.style.indev_ptr = true;
        item_maker.widget.parent          = event->object;
        item_maker.widget.clip.w          = SCUI_HOR_RES;
        item_maker.widget.clip.h          = 72;
        item_maker.widget.event_cb        = scui_ui_scene_scale_item_event_proc;
        
        /* 创建各列表项(起始留白 + 列表项 + 结尾留白) */
        {
            /* 上半部分留白 */
            scui_custom_maker_t spacer_maker = {0};
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 72 / 2;
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        /* 列表项 */
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            scui_handle_t item_handle = SCUI_HANDLE_INVALID;
            scui_widget_create(&item_maker, &item_handle);
            scui_ui_res_local->item_m[idx] = item_handle;
        }
        
        /* 下半部分留白 */
        {
            scui_custom_maker_t spacer_maker = {0};
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 72 / 2;
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        /* 初始化arc指示器 */
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC;
        break;
    }
    case scui_event_scroll_layout:
    case scui_event_scroll_start:
    case scui_event_scroll_keep:
    case scui_event_scroll_over: {
        
        scui_coord_t scroll_pct = 0;
        scui_scroll_percent_get(event->object, &scroll_pct);
        scui_ui_res_local->scroll_pct = scroll_pct;
        scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
        scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        break;
    }
    case scui_event_draw_finish: {
        /* 回收离屏列表项的画布内存 */
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            scui_handle_t handle_m = scui_ui_res_local->item_m[idx];
            if (scui_widget_draw_empty(handle_m))
                scui_widget_surface_recycle(handle_m);
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 清空图像资源缓存并加载该场景资源 */
        scui_ui_scene_list_cfg(scui_ui_scene_list_type_list_scale);
        
        /* 创建滚动控件(替代旧的 linear) */
        scui_scroll_maker_t scroll_maker = {0};
        scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&scroll_maker, scui_widget_type_scroll);
        scroll_maker.widget.style.fully_bg  = true;
        scroll_maker.widget.style.indev_enc = true;
        scroll_maker.widget.style.indev_key = true;
        scroll_maker.widget.clip.w          = SCUI_HOR_RES;
        scroll_maker.widget.clip.h          = SCUI_VER_RES;
        scroll_maker.widget.parent          = event->object;
        scroll_maker.widget.event_cb   = scui_ui_scene_list_scale_scroll_event;
        scroll_maker.widget.child_num  = 50;
        scroll_maker.pos               = scui_opt_pos_c;
        scroll_maker.dir               = scui_opt_dir_ver;
        scroll_maker.space             = 10;
        scroll_maker.route_enc         = 117;
        scroll_maker.route_key         = 117;
        scroll_maker.keyid_fdir        = SCUI_WIDGET_SCROLL_KEY_FDIR;
        scroll_maker.keyid_bdir        = SCUI_WIDGET_SCROLL_KEY_BDIR;
        scroll_maker.springback        = 70;
        scui_widget_create(&scroll_maker, &scroll_handle);
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click:
        
        #if 0
        scui_plug_coupler_recycle(scui_ui_res_local->coupler, true);
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del(SCUI_UI_SCENE_LIST);
        scui_window_switch_type_cfg_set(type);
        scui_event_mask_over(event);
        #endif
        break;
    default:
        break;
    }
}
