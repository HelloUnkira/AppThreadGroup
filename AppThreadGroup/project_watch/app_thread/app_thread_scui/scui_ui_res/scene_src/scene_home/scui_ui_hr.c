/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_HR_FRAME_NUM        11   /* 大心脏动画帧数(00~10) */
#define SCUI_UI_HR_FRAME_DIV        3    /* 每3次elapse切一帧(降频) */

static uint32_t hr_frame_idx = 0;        /* 当前动画帧 */
static uint32_t hr_frame_cnt = 0;        /* 帧切换计数器 */

/* 将第一个指定符号移到末尾后变成'\0'(目前用于将#移到末尾变成'\0') */
static void scui_ui_str_remove_discard_char(char *str, char t_char)
{
    int32_t len = strlen(str);
    char *ofs = NULL;
    ofs = strchr(str, t_char);
    if (ofs == NULL) {
        SCUI_LOG_DEBUG("ofs is NULL");
        return;
    }
    for (; ofs < str + len; ofs++) {
        *ofs = *(ofs + 1);
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_hr_event_proc(scui_event_t *event)
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
void scui_ui_scene_hr_body_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 心跳动画: 控制帧率轮播大心脏帧 */
        hr_frame_cnt++;
        if (hr_frame_cnt >= SCUI_UI_HR_FRAME_DIV) {
            hr_frame_cnt = 0;
            hr_frame_idx++;
            if (hr_frame_idx >= SCUI_UI_HR_FRAME_NUM)
                hr_frame_idx = 0;
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
        
        /* 大心脏跳动(主视觉) */
        scui_handle_t image_heart = scui_image_prj_hr_main_gif_big_00jpg + hr_frame_idx;
        scui_area_t clip_heart = {
            .x = (SCUI_HOR_RES - scui_image_w(image_heart)) / 2,
            .y = 80,
            .w = scui_image_w(image_heart),
            .h = scui_image_h(image_heart),
        };
        scui_widget_draw_image(event->object, &clip_heart, image_heart, NULL, SCUI_COLOR_UNUSED);
        
        /* 标题/单位/按钮文字(静态) */
        scui_string_update_text(SCUI_UI_SCENE_HR_TITLE, SCUI_LANG_0X0019);
        scui_string_update_text(SCUI_UI_SCENE_HR_UNIT, SCUI_LANG_0X0021);
        scui_string_update_text(SCUI_UI_SCENE_HR_BTN_TEXT, SCUI_LANG_0X002a);
        
        /* 当前心率 */
        char digit_str[10] = {0};
        snprintf(digit_str, sizeof(digit_str), "%d", scui_presenter.get_hr_cur());
        scui_string_update_str(SCUI_UI_SCENE_HR_VALUE, digit_str);
        
        /* 最近测量时间 */
        uint32_t passby_last = scui_presenter.get_hr_passby();
        scui_handle_t text_unit = SCUI_LANG_0X001d;
        if (passby_last < 1) text_unit = SCUI_LANG_0X001d;
        else if (passby_last < 59) text_unit = SCUI_LANG_0X001e;
        else if (passby_last < 60 * 24) text_unit = SCUI_LANG_0X001f;
        else if (passby_last < 60 * 24 * 7) text_unit = SCUI_LANG_0X0020;
        else text_unit = SCUI_LANG_0X01c4;
        
        char data_unit[50] = {0};
        if (passby_last != 0 && passby_last < 60 * 24 * 7) {
            if (passby_last > 60 * 24) passby_last /= 60 * 24;
            else if (passby_last > 60) passby_last /= 60;
            snprintf(data_unit, sizeof(data_unit), "%d ", passby_last);
        }
        
        scui_lang_type_t lang_type = 0;
        scui_lang_get(&lang_type);
        const char *lang_str = scui_lang_str(text_unit, lang_type);
        snprintf(data_unit + strlen(data_unit), sizeof(data_unit) - strlen(data_unit), "%s", lang_str);
        scui_ui_str_remove_discard_char(data_unit, '#');
        scui_string_update_str(SCUI_UI_SCENE_HR_TIME, data_unit);
        
        break;
    }
    default:
        break;
    }
}
