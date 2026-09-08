/*实现目标:
 *    测试(widget ximage)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* sequence测试: 0~9数字序列 */
static const scui_handle_t scui_seq_num_list[] = {
    scui_image_prj_num_22_white_12x18_00,
    scui_image_prj_num_22_white_12x18_01,
    scui_image_prj_num_22_white_12x18_02,
    scui_image_prj_num_22_white_12x18_03,
    scui_image_prj_num_22_white_12x18_04,
    scui_image_prj_num_22_white_12x18_05,
    scui_image_prj_num_22_white_12x18_06,
    scui_image_prj_num_22_white_12x18_07,
    scui_image_prj_num_22_white_12x18_08,
    scui_image_prj_num_22_white_12x18_09,
};

/* sequence测试: 内部align循环轮换 */
static const scui_align_t scui_seq_align_list[] = {
    scui_align_itl, scui_align_itm, scui_align_itr, scui_align_ilm,
    scui_align_icc, scui_align_irm, scui_align_ibl, scui_align_ibm, scui_align_ibr,
};

/*@brief 图像子控件事件回调(布局在json, 参数在此注入)
 *@param event 事件
 */
void scui_test_ui_ximage_indicator_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_handle_t handle = event->object;
        scui_coord_t  idx    = scui_widget_child_to_index(handle);
        bool          way    = (idx == 8);   /* 8:垂直; 9:水平 */
        /* indicator形态: 未选中wait灰点 + 选中focus白点 */
        scui_handle_t wait_ = scui_image_prj_repeat_dot_01_grey;
        scui_handle_t focus = scui_image_prj_repeat_dot_02_white;
        scui_coord_t count = 5;
        scui_coord_t span  = 6;
        
        /* 相对sequence: 垂直=右外侧底对齐; 水平=下外侧右对齐 */
        scui_handle_t seq = scui_widget_child_by_index(scui_widget_parent(handle), 4);
        scui_point_t ofs = {.x = way ? 8 : 0, .y = way ? 0 : 8,};
        scui_widget_align_pos(handle, seq, way ? scui_align_orb : scui_align_obr, &ofs);
        
        /* 注入初始(首点选中) */
        scui_handle_t list[8];
        for (scui_coord_t k = 0; k < count; k++)
            list[k] = (k == 0) ? focus : wait_;
        scui_ximage_sequence(handle, list, count,
            SCUI_COLOR_FILTER_TRANS, scui_align_itl, span, way);
        break;
    }
    case scui_event_anima_elapse: {
        scui_handle_t handle = event->object;
        scui_coord_t  idx    = scui_widget_child_to_index(handle);
        bool          way    = (idx == 8);
        scui_coord_t  bi     = idx - 8;
        static const scui_handle_t wait_ = scui_image_prj_repeat_dot_01_grey;
        static const scui_handle_t focus = scui_image_prj_repeat_dot_02_white;
        scui_coord_t count = 5;
        scui_coord_t span  = 6;
        
        /* ~0.7s轮换高亮位置 */
        static scui_multi_t ind_tick[2] = {0};
        static scui_coord_t ind_focus[2] = {0};
        ind_tick[bi] += event->tick;
        if (ind_tick[bi] < 700)
             break;
        ind_tick[bi] = 0;
        ind_focus[bi] = (ind_focus[bi] + 1) % count;
        
        /* 重建list: 选中位focus, 其余wait */
        scui_handle_t list[8];
        for (scui_coord_t k = 0; k < count; k++)
            list[k] = (k == ind_focus[bi]) ? focus : wait_;
        scui_ximage_sequence(handle, list, count,
            SCUI_COLOR_FILTER_TRANS, scui_align_itl, span, way);
        break;
    }
    default:
        break;
    }
}

/*@brief 图像子控件事件回调(布局在json, 参数在此注入)
 *@param event 事件
 */
void scui_test_ui_ximage_item_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_create: {
        scui_handle_t handle = event->object;
        scui_coord_t  index = scui_widget_child_to_index(event->object);
        const char *url = "https://github.com/HelloUnkira/AppThreadGroup.git";
        switch (index) {
        case 0: /* barcode */
            scui_ximage_barcode(handle, (uint8_t *)url, strlen(url),
                SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
                SCUI_SCALE_COF);
            break;
        case 1: /* qrcode */
            scui_ximage_qrcode(handle, (uint8_t *)url, strlen(url),
                SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
                SCUI_SCALE_COF);
            break;
        case 2: /* vedio:gif 无限 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_bulb, SCUI_ANIMA_TICK, -1);
            break;
        case 3: /* vedio:lottie 无限 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_musiclottie, SCUI_ANIMA_TICK, -1);
            break;
        case 4: /* vedio:gif 3次 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_bulb, SCUI_ANIMA_TICK, 3);
            break;
        case 5: /* vedio:lottie 5次 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_comfirmlottie, SCUI_ANIMA_TICK, 5);
            break;
        case 6: { /* replace:6个index 1秒循环 */
            static const scui_handle_t index_list[] = {
                scui_image_prj_weather_arrow_up,
                scui_image_prj_weather_arrow_down,
                scui_image_prj_weather_up,
                scui_image_prj_weather_down,
                scui_image_prj_weather_uv,
                scui_image_prj_weather_ring_uv,
            };
            scui_coord_t index_num = scui_arr_len(index_list);
            scui_multi_t index_time = 1000 / index_num;
            scui_ximage_replace_play(handle, (scui_handle_t *)index_list, index_num, index_time, -1);
            break;
        }
        case 7: {/* sequence:0-9数字 初始内部align */
            scui_ximage_sequence(handle, (scui_handle_t *)scui_seq_num_list,
                scui_arr_len(scui_seq_num_list),
                SCUI_COLOR_FILTER_TRANS, scui_align_itl, 2, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    case scui_event_anima_elapse: {  /* sequence: 1s轮换内部align */
        scui_coord_t index = scui_widget_child_to_index(event->object);
        if (index != 7) break;
        static scui_multi_t seq_tick   = 0;
        static scui_coord_t seq_idx    = 0;
        seq_tick += SCUI_SCALE_COF;
        /* ~1s(SCUI_ANIMA_TICK ms/帧) */
        if (seq_tick >= SCUI_SCALE_COF * 1000 / SCUI_ANIMA_TICK) {
            seq_tick = 0;
            seq_idx  = (seq_idx + 1) % scui_arr_len(scui_seq_align_list);
            scui_ximage_sequence(event->object, (scui_handle_t *)scui_seq_num_list,
                scui_arr_len(scui_seq_num_list),
                SCUI_COLOR_FILTER_TRANS, scui_seq_align_list[seq_idx], 2, false);
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_ximage_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    default:
        break;
    }
}