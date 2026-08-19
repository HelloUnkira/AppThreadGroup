/*实现目标:
 *    窗口:scui_test_ui_button
 *    用途: 测试按钮(驱逐 button 控件后改为 custom image_crect4 + menial btn)
 *    构件: custom image_crect4 x2 + menial btn x2
 *    custom: 静态四角图按钮(点击发 button_click)
 *    menial 倒数2: 点击缩放动画(动画用 menial 配置)
 *    menial 倒数1: 四类 part 同显(阴影/外部线/边框/背景)
 *    交互: 点击打印
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 按钮点击事件响应回调
 *@param event 事件
 */
void scui_test_ui_button_item_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click:
        SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
        break;
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_button_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        /* 整体垂直居中: 2个custom按钮(72) + 2个menial按钮(100) */
        scui_coord_t btn_w = 72 * 2;
        scui_coord_t btn_h = 72;
        scui_coord_t btn_gap = 15;
        scui_coord_t total_h = btn_h * 2 + 100 * 2 + btn_gap * 3;
        scui_coord_t cur_y = (SCUI_VER_RES - total_h) / 2;
        
        #if 1
        /* custom image_crect4 x2 (替代 button image 类型, 静态) */
        static const scui_handle_t btn_images[2][4] = {
            {
                scui_image_prj_repeat_card_04_r36_1jpg,
                scui_image_prj_repeat_card_05_r36_2jpg,
                scui_image_prj_repeat_card_06_r36_3jpg,
                scui_image_prj_repeat_card_07_r36_4jpg,
            },
            {
                scui_image_prj_repeat_box_sleep_breathe_02_left_upjpg,
                scui_image_prj_repeat_box_sleep_breathe_04_right_upjpg,
                scui_image_prj_repeat_box_sleep_breathe_01_left_downjpg,
                scui_image_prj_repeat_box_sleep_breathe_03_right_downjpg,
            },
        };
        static const scui_coord_t btn_hgt[2] = {72, 92};
        static const scui_coord_t btn_delta[2] = {-1, 4};
        
        for (uint8_t idx = 0; idx < 2; idx++) {
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t       custom_handle = SCUI_HANDLE_INVALID;
            
            scui_widget_maker_def_cfg(&custom_maker, scui_widget_type_custom);
            custom_maker.widget.style.indev_ptr = true;
            custom_maker.widget.parent   = event->object;
            custom_maker.widget.event_cb = scui_test_ui_button_item_event_proc;
            custom_maker.widget.clip.w   = btn_w + (idx == 1 ? 20 : 0);
            custom_maker.widget.clip.h   = btn_hgt[idx];
            custom_maker.widget.clip.x   = SCUI_HOR_RES / 2 - custom_maker.widget.clip.w / 2;
            custom_maker.widget.clip.y   = cur_y;
            custom_maker.type = scui_custom_type_image_crect4;
            custom_maker.data.image_crect4.image[0] = btn_images[idx][0];
            custom_maker.data.image_crect4.image[1] = btn_images[idx][1];
            custom_maker.data.image_crect4.image[2] = btn_images[idx][2];
            custom_maker.data.image_crect4.image[3] = btn_images[idx][3];
            custom_maker.data.image_crect4.color = SCUI_COLOR_MAKE32(false, 0x0, 0xFF282828);
            custom_maker.data.image_crect4.delta = btn_delta[idx];
            scui_widget_create(&custom_maker, &custom_handle);
            
            cur_y += custom_maker.widget.clip.h + btn_gap;
        }
        #endif
        
        #if 1
        /* menial btn 倒数2: 点击缩放动画(动画用 menial 配置) */
        scui_menial_maker_t menial_maker = {0};
        scui_handle_t       menial_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&menial_maker, scui_widget_type_menial);
        menial_maker.widget.parent   = event->object;
        menial_maker.widget.event_cb = scui_test_ui_button_item_event_proc;
        menial_maker.widget.clip.w   = btn_w;
        menial_maker.widget.clip.h   = 100;
        menial_maker.widget.clip.x   = SCUI_HOR_RES / 2 - btn_w / 2;
        menial_maker.widget.clip.y   = cur_y;
        menial_maker.type = scui_menial_type_btn;
        menial_maker.data.btn.color[0].color_l.full = 0xFF00FF00;
        menial_maker.data.btn.color[0].color_d.full = 0xFF008000;
        menial_maker.data.btn.color[1].color_l.full = 0xFF000080;
        menial_maker.data.btn.color[1].color_d.full = 0xFF000080;
        menial_maker.data.btn.fixed  = 0;   /* 开启动画 */
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 3;
        menial_maker.data.btn.radius = 20;
        menial_maker.data.btn.time   = 200; /* 动画时间 */
        menial_maker.data.btn.lim    = 80;  /* 缩小限制 */
        scui_widget_create(&menial_maker, &menial_handle);
        
        cur_y += 100 + btn_gap;
        #endif
        
        #if 1
        /* menial btn 倒数1: 四类 part 同显(阴影/外部线/边框/背景) */
        /* btn 默认循环绘制 sha->box->edge->bg, 此处外部覆盖 sha/box/edge 的 prop/tran */
        scui_menial_maker_t menial2_maker = {0};
        scui_handle_t       menial2_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&menial2_maker, scui_widget_type_menial);
        menial2_maker.widget.parent   = event->object;
        menial2_maker.widget.event_cb = scui_test_ui_button_item_event_proc;
        menial2_maker.widget.clip.w   = btn_w;
        menial2_maker.widget.clip.h   = 100;
        menial2_maker.widget.clip.x   = SCUI_HOR_RES / 2 - btn_w / 2;
        menial2_maker.widget.clip.y   = cur_y;
        menial2_maker.type = scui_menial_type_btn;
        menial2_maker.data.btn.color[0].color_l.full = 0xFF87CEFA;
        menial2_maker.data.btn.color[0].color_d.full = 0xFF4682B4;
        menial2_maker.data.btn.color[1].color_l.full = 0xFF87CEFA;
        menial2_maker.data.btn.color[1].color_d.full = 0xFF4682B4;
        menial2_maker.data.btn.fixed  = 0;
        menial2_maker.data.btn.check  = 1;
        menial2_maker.data.btn.width  = 3;
        menial2_maker.data.btn.radius = 20;
        menial2_maker.data.btn.time   = 200;
        menial2_maker.data.btn.lim    = 90;
        scui_widget_create(&menial2_maker, &menial2_handle);
        
        /* 外部覆盖: 参考 btn create 的 bg 完整配置模式, 复制到 edge/box/sha */
        /* 各 part 均含: 几何 prop_rect + 状态色 prop_new + 缩放 prop_new + tran_new2 动画 */
        scui_object_sub_t sub = {0};
        scui_area_t widget_clip = {
            .w = btn_w,
            .h = 100,
        };
        scui_coord_t radius = 20;
        scui_coord_t width  = 3;
        scui_coord_t main_time = 200;
        
        scui_multi_t scale_w = (scui_multi_t)widget_clip.w * 90 / 100;
        scui_multi_t scale_h = (scui_multi_t)widget_clip.h * 90 / 100;
        scui_multi_t default_w = widget_clip.w;
        scui_multi_t default_h = widget_clip.h;
        
        /* 边框 edge */
        {
            sub.rect.alpha.alpha       = scui_alpha_cover;
            sub.rect.align.align       = scui_opt_pos_c;
            sub.rect.width.number      = widget_clip.w + width * 1;
            sub.rect.height.number     = widget_clip.h + width * 1;
            sub.rect.radius.number     = radius + width * 1;
            sub.rect.side_width.number = width;
            sub.part  = scui_object_part_rect_edge;
            sub.state = scui_object_state_def;
            scui_object_prop_rect(menial2_handle, &sub);
            sub.state = scui_object_state_pre;
            scui_object_prop_rect(menial2_handle, &sub);
            
            scui_object_prop_new(menial2_handle, rect_edge, rect_color, def, scui_object_data_color32(0xFFFFFFFF));
            scui_object_prop_new(menial2_handle, rect_edge, rect_color, pre, scui_object_data_color32(0xFFFFFFFF));
            scui_object_tran_new2(menial2_handle, rect_edge, rect_color, def, pre,
                scui_object_data_color32(0xFFFFFFFF), scui_object_data_color32(0xFFFFFFFF), NULL, main_time, 0);
            
            scui_object_prop_new(menial2_handle, rect_edge, rect_width,  def, scui_object_data_number(scale_w));
            scui_object_prop_new(menial2_handle, rect_edge, rect_width,  pre, scui_object_data_number(default_w));
            scui_object_prop_new(menial2_handle, rect_edge, rect_height, def, scui_object_data_number(scale_h));
            scui_object_prop_new(menial2_handle, rect_edge, rect_height, pre, scui_object_data_number(default_h));
            scui_object_tran_new2(menial2_handle, rect_edge, rect_width,  def, pre,
                scui_object_data_number(scale_w), scui_object_data_number(default_w), NULL, main_time, 0);
            scui_object_tran_new2(menial2_handle, rect_edge, rect_height, def, pre,
                scui_object_data_number(scale_h), scui_object_data_number(default_h), NULL, main_time, 0);
        }
        
        /* 外部线 box */
        {
            sub.rect.width.number      = widget_clip.w + width * 2;
            sub.rect.height.number     = widget_clip.h + width * 2;
            sub.rect.radius.number     = radius + width * 2;
            sub.part  = scui_object_part_rect_box;
            sub.state = scui_object_state_def;
            scui_object_prop_rect(menial2_handle, &sub);
            sub.state = scui_object_state_pre;
            scui_object_prop_rect(menial2_handle, &sub);
            
            scui_object_prop_new(menial2_handle, rect_box, rect_color, def, scui_object_data_color32(0xFFFF0000));
            scui_object_prop_new(menial2_handle, rect_box, rect_color, pre, scui_object_data_color32(0xFFFF0000));
            scui_object_tran_new2(menial2_handle, rect_box, rect_color, def, pre,
                scui_object_data_color32(0xFFFF0000), scui_object_data_color32(0xFFFF0000), NULL, main_time, 0);
            
            scui_object_prop_new(menial2_handle, rect_box, rect_width,  def, scui_object_data_number(scale_w));
            scui_object_prop_new(menial2_handle, rect_box, rect_width,  pre, scui_object_data_number(default_w));
            scui_object_prop_new(menial2_handle, rect_box, rect_height, def, scui_object_data_number(scale_h));
            scui_object_prop_new(menial2_handle, rect_box, rect_height, pre, scui_object_data_number(default_h));
            scui_object_tran_new2(menial2_handle, rect_box, rect_width,  def, pre,
                scui_object_data_number(scale_w), scui_object_data_number(default_w), NULL, main_time, 0);
            scui_object_tran_new2(menial2_handle, rect_box, rect_height, def, pre,
                scui_object_data_number(scale_h), scui_object_data_number(default_h), NULL, main_time, 0);
        }
        
        /* 阴影 sha */
        {
            sub.rect.width.number      = widget_clip.w + width * 3;
            sub.rect.height.number     = widget_clip.h + width * 3;
            sub.rect.radius.number     = radius + width * 3;
            sub.rect.alpha.alpha       = scui_alpha_pct50;
            sub.part  = scui_object_part_rect_sha;
            sub.state = scui_object_state_def;
            scui_object_prop_rect(menial2_handle, &sub);
            sub.state = scui_object_state_pre;
            scui_object_prop_rect(menial2_handle, &sub);
            
            scui_object_prop_new(menial2_handle, rect_sha, rect_color, def, scui_object_data_color32(0xFF00FF00));
            scui_object_prop_new(menial2_handle, rect_sha, rect_color, pre, scui_object_data_color32(0xFF00FF00));
            scui_object_tran_new2(menial2_handle, rect_sha, rect_color, def, pre,
                scui_object_data_color32(0xFF00FF00), scui_object_data_color32(0xFF00FF00), NULL, main_time, 0);
            
            scui_object_prop_new(menial2_handle, rect_sha, rect_width,  def, scui_object_data_number(scale_w));
            scui_object_prop_new(menial2_handle, rect_sha, rect_width,  pre, scui_object_data_number(default_w));
            scui_object_prop_new(menial2_handle, rect_sha, rect_height, def, scui_object_data_number(scale_h));
            scui_object_prop_new(menial2_handle, rect_sha, rect_height, pre, scui_object_data_number(default_h));
            scui_object_tran_new2(menial2_handle, rect_sha, rect_width,  def, pre,
                scui_object_data_number(scale_w), scui_object_data_number(default_w), NULL, main_time, 0);
            scui_object_tran_new2(menial2_handle, rect_sha, rect_height, def, pre,
                scui_object_data_number(scale_h), scui_object_data_number(default_h), NULL, main_time, 0);
        }
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    default:
        break;
    }
}
