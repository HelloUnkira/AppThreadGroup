/*实现目标:
 *    窗口:scui_test_ui_roller
 *    用途: 测试滚轮(从 scui_ui_float_4 挖取)
 *    构件: roller spin + scale
 *    交互: 动画打印中心子控件文本
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t roller_1;     // 滚轮控件
    scui_handle_t roller_2;     // 滚轮控件
} * scui_ui_res_local = NULL;

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_roller_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        #if SCUI_MEM_FEAT_MINI == 0
        /* 滚动中心子控件 */
        scui_handle_t handle_scroll_1 = SCUI_HANDLE_INVALID;
        static scui_handle_t handle_scroll_1_rcd = SCUI_HANDLE_INVALID;
        scui_roller_center_get(scui_ui_res_local->roller_1, &handle_scroll_1);
        if (handle_scroll_1_rcd != handle_scroll_1) {
            handle_scroll_1_rcd  = handle_scroll_1;
            if (handle_scroll_1 != SCUI_HANDLE_INVALID) {
                uint8_t *str_utf8 = NULL;
                scui_string_str_utf8(handle_scroll_1, &str_utf8);
                SCUI_LOG_WARN("roller 1 center:%s", str_utf8);
            }
        }
        
        /* 滚动中心子控件 */
        scui_handle_t handle_scroll_2 = SCUI_HANDLE_INVALID;
        static scui_handle_t handle_scroll_2_rcd = SCUI_HANDLE_INVALID;
        scui_roller_center_get(scui_ui_res_local->roller_2, &handle_scroll_2);
        if (handle_scroll_2_rcd != handle_scroll_2) {
            handle_scroll_2_rcd  = handle_scroll_2;
            if (handle_scroll_2 != SCUI_HANDLE_INVALID) {
                uint8_t *str_utf8 = NULL;
                scui_string_str_utf8(handle_scroll_2, &str_utf8);
                SCUI_LOG_WARN("roller 2 center:%s", str_utf8);
            }
        }
        #endif
        
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        #if SCUI_MEM_FEAT_MINI == 0
        // digit picker == scroll + string * num
        scui_roller_maker_define(roller_maker);
        
        roller_maker.widget.clip.w          = SCUI_HOR_RES * 11 / 25;
        roller_maker.widget.clip.h          = SCUI_VER_RES * 11 / 25;
        roller_maker.widget.parent          = SCUI_UI_SCENE_TEST_UI_ROLLER;
        roller_maker.widget.child_num       = 60;
        roller_maker.scroll.pos  = scui_opt_pos_c;
        roller_maker.scroll.dir  = scui_opt_dir_ver;
        roller_maker.scroll.loop = true;
        
        scui_string_maker_define(string_maker);
        
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFF0000FF;
        string_maker.args.color.color_e.full    = 0xFF0000FF;
        string_maker.args.color.filter          = true;
        string_maker.widget.clip.w              = roller_maker.widget.clip.w;
        string_maker.widget.clip.h              = roller_maker.widget.clip.h / 5;
        string_maker.font_idx                   = SCUI_FONT_IDX_32;
        string_maker.args.lang                  = scui_lang_type_en;
        
        // 24
        roller_maker.widget.clip.x   = SCUI_HOR_RES *  1 / 25;
        roller_maker.widget.clip.y   = SCUI_VER_RES * 13 / 25;
        roller_maker.type = scui_roller_type_spin;
        roller_maker.grad = true;
        scui_widget_create(&roller_maker, &scui_ui_res_local->roller_1);
        
        for (uint8_t idx = 0; idx < 24; idx++) {
            uint8_t str_utf8[10] = {0};
            snprintf(str_utf8, sizeof(str_utf8), "@!00%02d!@", idx);
            scui_roller_string_str(scui_ui_res_local->roller_1, &string_maker, str_utf8);
        }
        
        // month
        roller_maker.widget.clip.x   = SCUI_HOR_RES * 13 / 25;
        roller_maker.widget.clip.y   = SCUI_VER_RES * 13 / 25;
        roller_maker.type = scui_roller_type_scale;
        roller_maker.grad = true;
        scui_widget_create(&roller_maker, &scui_ui_res_local->roller_2);
        
        const char *str_utf8_month[] = {
            "January", "February", "March", "April", "May\n", "June", "July",
            "August", "September", "October", "November", "December",
        };
        
        for (uint8_t idx = 0; idx < scui_arr_len(str_utf8_month); idx++) {
            uint8_t str_utf8[20] = {0};
            snprintf(str_utf8, sizeof(str_utf8), "%s", str_utf8_month[idx]);
            scui_roller_string_str(scui_ui_res_local->roller_2, &string_maker, str_utf8);
        }
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}
