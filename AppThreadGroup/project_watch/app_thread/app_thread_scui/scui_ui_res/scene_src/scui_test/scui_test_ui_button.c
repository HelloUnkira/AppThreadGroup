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
            scui_custom_maker_define(custom_maker);
            scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
            
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
        scui_menial_maker_define(menial_maker);
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
        menial_maker.widget.parent   = event->object;
        menial_maker.widget.event_cb = scui_test_ui_button_item_event_proc;
        menial_maker.widget.clip.w   = btn_w;
        menial_maker.widget.clip.h   = 100;
        menial_maker.widget.clip.x   = SCUI_HOR_RES / 2 - btn_w / 2;
        menial_maker.widget.clip.y   = cur_y;
        menial_maker.type = scui_menial_type_btn;
        menial_maker.data.btn.check = 1;   /* 运行初值: 点击切换 */
        scui_widget_create(&menial_maker, &menial_handle);
        
        /* 创建后应用样式(用户自定义) */
        scui_menial_btn_res_t res = {0};
        res.color[0].color_s.full = 0xFF00FF00;  /* def */
        res.color[1].color_s.full = 0xFF008000;  /* pre */
        res.color[2].color_s.full = 0xFF000080;  /* chk */
        res.color[3].color_s.full = 0xFF000080;  /* chk pre */
        res.width  = 3;
        res.radius = 20;
        res.time   = 200; /* 动画时间 */
        res.lim    = 80;  /* 缩小限制 */
        res.part = scui_object_part_rect_bg;
        scui_menial_btn_style(menial_handle, &res);
        
        cur_y += 100 + btn_gap;
        #endif
        
        #if 1
        /* menial btn 倒数1: 四类 part 同显(阴影/外部线/边框/背景) */
        /* btn 默认循环绘制 sha->box->edge->bg, 此处外部覆盖 sha/box/edge 的 prop/tran */
        scui_menial_maker_define(menial2_maker);
        scui_handle_t menial2_handle = SCUI_HANDLE_INVALID;
        
        menial2_maker.widget.parent   = event->object;
        menial2_maker.widget.event_cb = scui_test_ui_button_item_event_proc;
        menial2_maker.widget.clip.w   = btn_w;
        menial2_maker.widget.clip.h   = 100;
        menial2_maker.widget.clip.x   = SCUI_HOR_RES / 2 - btn_w / 2;
        menial2_maker.widget.clip.y   = cur_y;
        menial2_maker.type = scui_menial_type_btn;
        menial2_maker.data.btn.check = 1;   /* 运行初值: 点击切换 */
        scui_widget_create(&menial2_maker, &menial2_handle);
        
        /* 创建后应用样式 */
        scui_menial_btn_res_t bg_res = {0};
        bg_res.part = scui_object_part_rect_bg;
        bg_res.color[0].color_s.full = 0xFF87CEFA;  /* def */
        bg_res.color[1].color_s.full = 0xFF4682B4;  /* pre */
        bg_res.color[2].color_s.full = 0xFF87CEFA;  /* chk */
        bg_res.color[3].color_s.full = 0xFF4682B4;  /* chk pre */
        bg_res.area.w = btn_w - (12 + 4 + 4) * 2;
        bg_res.area.h = 100 - (12 + 4 + 4) * 2;
        bg_res.width  = 0;
        bg_res.radius = 44 - (12 + 4 + 4);
        
        /* edge/box/sha 各部件 */
        scui_menial_btn_res_t edge_res = {0};
        edge_res.part = scui_object_part_rect_edge;
        edge_res.color[0].color_s.full = 0xFFFFFFFF;  /* def */
        edge_res.color[1].color_s.full = 0xFFFFFFFF;  /* pre */
        edge_res.color[2].color_s.full = 0xFFFFFFFF;  /* chk */
        edge_res.color[3].color_s.full = 0xFFFFFFFF;  /* chk pre */
        edge_res.area.w = btn_w - (12 + 4) * 2;
        edge_res.area.h = 100 - (12 + 4) * 2;
        edge_res.width  = 4;
        edge_res.radius = 44 - (12 + 4);
        
        scui_menial_btn_res_t box_res = {0};
        box_res.part  = scui_object_part_rect_box;
        box_res.color[0].color_s.full = 0xFFFF0000;  /* def */
        box_res.color[1].color_s.full = 0xFFFF0000;  /* pre */
        box_res.color[2].color_s.full = 0xFFFF0000;  /* chk */
        box_res.color[3].color_s.full = 0xFFFF0000;  /* chk pre */
        box_res.area.w = btn_w - (12) * 2;
        box_res.area.h = 100 - (12) * 2;
        box_res.width  = 4;
        box_res.radius = 44 - (12);
        
        scui_menial_btn_res_t sha_res = {0};
        sha_res.part  = scui_object_part_rect_sha;
        sha_res.color[0].color_s.full = 0xFF00FF00;  /* def */
        sha_res.color[1].color_s.full = 0xFF00FF00;  /* pre */
        sha_res.color[2].color_s.full = 0xFF00FF00;  /* chk */
        sha_res.color[3].color_s.full = 0xFF00FF00;  /* chk pre */
        sha_res.area.w = btn_w - (0) * 2;
        sha_res.area.h = 100 - (0) * 2;
        sha_res.width  = 12;
        sha_res.radius = 44 - (0);
        sha_res.shadow = 1;
        
        scui_menial_btn_style(menial2_handle, &bg_res);
        scui_menial_btn_style(menial2_handle, &edge_res);
        scui_menial_btn_style(menial2_handle, &box_res);
        scui_menial_btn_style(menial2_handle, &sha_res);
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    default:
        break;
    }
}
