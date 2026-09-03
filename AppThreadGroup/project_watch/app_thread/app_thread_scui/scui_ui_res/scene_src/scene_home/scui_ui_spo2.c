/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_SPO2_FRAME_NUM      12   /* 血氧动画帧数(00~11) */
#define SCUI_UI_SPO2_FRAME_DIV      3    /* 每3次elapse切一帧(降频) */

static uint32_t spo2_frame_idx = 0;      /* 当前动画帧 */
static uint32_t spo2_frame_cnt = 0;      /* 帧切换计数器 */

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_spo2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
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

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_spo2_body_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 血氧动画: 控制帧率轮播大图标帧 */
        spo2_frame_cnt++;
        if (spo2_frame_cnt >= SCUI_UI_SPO2_FRAME_DIV) {
            spo2_frame_cnt = 0;
            spo2_frame_idx++;
            if (spo2_frame_idx >= SCUI_UI_SPO2_FRAME_NUM)
                spo2_frame_idx = 0;
            scui_widget_draw(event->object, NULL, false, 0);
        }
        break;
    }
    case scui_event_create: {
        break;
    }
    case scui_event_draw_graph: {
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x = clip.y = 0;
        
        /* 血氧大图标(动态) */
        scui_handle_t image_spo2 = scui_image_prj_spo2_gif_00 + spo2_frame_idx;
        scui_area_t clip_spo2 = {
            .x = (SCUI_HOR_RES - scui_image_w(image_spo2)) / 2,
            .y = 40,
            .w = scui_image_w(image_spo2),
            .h = scui_image_h(image_spo2),
        };
        scui_widget_draw_image(event->object, &clip_spo2, image_spo2, NULL, SCUI_COLOR_UNUSED);
        
        /* 标题(静态) */
        scui_string_update_text(SCUI_UI_SCENE_SPO2_TITLE, SCUI_LANG_0X0040);
        
        /* 当前血氧 */
        char digit_str[10] = {0};
        snprintf(digit_str, sizeof(digit_str), "%d", scui_presenter.get_spo2_cur());
        scui_string_update_str(SCUI_UI_SCENE_SPO2_VALUE, digit_str);
        
        /* 单位% */
        scui_string_update_str(SCUI_UI_SCENE_SPO2_UNIT, "%");
        
        break;
    }
    default:
        break;
    }
}
