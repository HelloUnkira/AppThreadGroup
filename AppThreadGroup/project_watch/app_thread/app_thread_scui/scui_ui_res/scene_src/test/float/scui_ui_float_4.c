/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t roller_1;     // 滚轮控件
    scui_handle_t roller_2;     // 滚轮控件
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_4_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        
        #if SCUI_MEM_FEAT_MINI == 0
        /* 滚动中心子控件 */
        scui_handle_t handle_scroll_1 = SCUI_HANDLE_INVALID;
        static scui_handle_t handle_scroll_1_rcd = SCUI_HANDLE_INVALID;
        scui_roller_center_target_get(scui_ui_res_local->roller_1, &handle_scroll_1);
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
        scui_roller_center_target_get(scui_ui_res_local->roller_2, &handle_scroll_2);
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
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        if (scui_event_check_prepare(event)) {
            
            scui_coord_t vlist[20] = {0};
            scui_coord_t vlist_min[20] = {0};
            scui_coord_t vlist_max[20] = {0};
            for (uint32_t idx = 0; idx < 20; idx++) {
                vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;   //统一底点就是柱状图,否则为股价图
                vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
                vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60) / 3);
            }
            
            // chart histogram
            scui_chart_maker_t chart1_maker = {0};
            scui_handle_t chart1_handle = SCUI_HANDLE_INVALID;
            chart1_maker.widget.type = scui_widget_type_chart;
            chart1_maker.widget.style.indev_ptr = true;
            chart1_maker.widget.style.indev_enc = true;
            chart1_maker.widget.style.indev_key = true;
            chart1_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
            chart1_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
            chart1_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
            chart1_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
            chart1_maker.widget.parent = SCUI_UI_SCENE_FLOAT_4;
            chart1_maker.widget.color.color.full = 0xFF4F4F4F;
            chart1_maker.type = scui_chart_type_histogram;
            chart1_maker.histogram.edge = scui_image_prj_image_src_repeat_05_dotbmp;
            chart1_maker.histogram.value_min = 60;
            chart1_maker.histogram.value_max = 220;
            chart1_maker.histogram.offset.x  = 5;
            chart1_maker.histogram.offset.y  = 0;
            chart1_maker.histogram.number    = 20;
            chart1_maker.histogram.height    = chart1_maker.widget.clip.h - 10;
            chart1_maker.histogram.space     = 4;
            chart1_maker.histogram.color.color.full = 0xFFFF0000;
            scui_widget_create(&chart1_maker, &chart1_handle, false);
            scui_chart_histogram_data(chart1_handle, vlist_min, vlist_max);
            
            // chart line
            scui_chart_maker_t chart2_maker = {0};
            scui_handle_t chart2_handle = SCUI_HANDLE_INVALID;
            chart2_maker.widget.type = scui_widget_type_chart;
            chart2_maker.widget.style.indev_ptr = true;
            chart2_maker.widget.style.indev_enc = true;
            chart2_maker.widget.style.indev_key = true;
            chart2_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
            chart2_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
            chart2_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
            chart2_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
            chart2_maker.widget.parent = SCUI_UI_SCENE_FLOAT_4;
            chart2_maker.widget.color.color.full = 0xFF4F4F4F;
            chart2_maker.type = scui_chart_type_line;
            chart2_maker.line.edge = scui_image_prj_image_src_repeat_05_dotbmp;
            chart2_maker.line.width     = 2;
            chart2_maker.line.value_min = 60;
            chart2_maker.line.value_max = 220;
            chart2_maker.line.offset.x  = 5;
            chart2_maker.line.offset.y  = 0;
            chart2_maker.line.number    = 20;
            chart2_maker.line.height    = chart2_maker.widget.clip.h - 10;
            chart2_maker.line.space     = 10;
            chart2_maker.line.color.color.full = 0xFFFF0000;
            scui_widget_create(&chart2_maker, &chart2_handle, false);
            
            for (uint8_t idx = 0; idx < chart2_maker.line.number; idx++) {
                int32_t angle = scui_map(idx, 0, chart2_maker.line.number, 0, 360);
                scui_coord_t base = chart2_maker.line.value_min;
                scui_coord_t offset = (chart2_maker.line.value_max - base) / 2;
                vlist[idx] = base + offset + ((scui_sin4096(angle) * offset) >> 12);
            }
            scui_chart_line_data(chart2_handle, vlist);
            
            #if SCUI_MEM_FEAT_MINI == 0
            // digit picker == scroll + string * num
            scui_roller_maker_t roller_maker = {0};
            roller_maker.widget.type            = scui_widget_type_roller;
            roller_maker.widget.clip.w          = SCUI_HOR_RES * 11 / 25;
            roller_maker.widget.clip.h          = SCUI_VER_RES * 11 / 25;
            roller_maker.widget.parent          = SCUI_UI_SCENE_FLOAT_4;
            roller_maker.widget.child_num       = 60;
            roller_maker.scroll.pos  = scui_opt_pos_c;
            roller_maker.scroll.dir  = scui_opt_dir_ver;
            roller_maker.scroll.loop = true;
            
            scui_string_maker_t string_maker = {0};
            string_maker.widget.type                = scui_widget_type_string;
            string_maker.widget.style.trans         = true;
            string_maker.args.align_hor             = 2;
            string_maker.args.align_ver             = 2;
            string_maker.args.color.color_s.full    = 0xFF0000FF;
            string_maker.args.color.color_e.full    = 0xFF0000FF;
            string_maker.args.color.filter          = true;
            string_maker.args.name                  = SCUI_FONT_TYPE_32_EN;
            string_maker.widget.clip.w              = roller_maker.widget.clip.w;
            string_maker.widget.clip.h              = roller_maker.widget.clip.h / 5;
            string_maker.font_idx                   = SCUI_FONT_IDX_32;
            
            // 24
            roller_maker.widget.clip.x   = SCUI_HOR_RES *  1 / 25;
            roller_maker.widget.clip.y   = SCUI_VER_RES * 13 / 25;
            roller_maker.linear.list_num = 24;
            roller_maker.type = scui_roller_type_spin;
            roller_maker.grad = true;
            scui_widget_create(&roller_maker, &scui_ui_res_local->roller_1, false);
            
            for (uint8_t idx = 0; idx < 24; idx++) {
                uint8_t str_utf8[10] = {0};
                snprintf(str_utf8, sizeof(str_utf8), "@!00%02d!@", idx);
                scui_roller_string_str(scui_ui_res_local->roller_1, &string_maker, str_utf8);
            }
            
            // month
            roller_maker.widget.clip.x   = SCUI_HOR_RES * 13 / 25;
            roller_maker.widget.clip.y   = SCUI_VER_RES * 13 / 25;
            roller_maker.linear.list_num = 12;
            roller_maker.type = scui_roller_type_scale;
            roller_maker.grad = true;
            scui_widget_create(&roller_maker, &scui_ui_res_local->roller_2, false);
            
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
        }
        
        #if 0   // discard, we don't need this
        scui_window_float_event_grasp_show(event);
        #endif
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        
        #if 0   // discard, we don't need this
        scui_window_float_event_grasp_hide(event);
        #endif
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    default:
        #if 0   // discard, we don't need this
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_grasp_ptr(event);
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e)
            scui_window_float_event_grasp_key(event);
        #endif
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
