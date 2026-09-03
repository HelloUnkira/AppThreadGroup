/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_STRESS_FRAME_NUM    12   /* 压力动画帧数(00~11) */
#define SCUI_UI_STRESS_FRAME_DIV    3    /* 每3次elapse切一帧(降频) */

static uint32_t stress_frame_idx = 0;    /* 当前动画帧 */
static uint32_t stress_frame_cnt = 0;    /* 帧切换计数器 */

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
void scui_ui_scene_stress_event_proc(scui_event_t *event)
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
void scui_ui_scene_stress_body_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 压力动画: 控制帧率轮播 */
        stress_frame_cnt++;
        if (stress_frame_cnt >= SCUI_UI_STRESS_FRAME_DIV) {
            stress_frame_cnt = 0;
            stress_frame_idx++;
            if (stress_frame_idx >= SCUI_UI_STRESS_FRAME_NUM)
                stress_frame_idx = 0;
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
        
        /* 压力动画(主视觉) */
        scui_handle_t image_stress = scui_image_prj_stress_1_gif_00 + stress_frame_idx;
        scui_area_t clip_stress = {
            .x = (SCUI_HOR_RES - scui_image_w(image_stress)) / 2,
            .y = 90,
            .w = scui_image_w(image_stress),
            .h = scui_image_h(image_stress),
        };
        scui_widget_draw_image(event->object, &clip_stress, image_stress, NULL, SCUI_COLOR_UNUSED);
        
        /* 标题(静态) */
        scui_string_update_text(SCUI_UI_SCENE_STRESS_TITLE, SCUI_LANG_0X0049);
        
        /* 当前压力大数字 */
        uint32_t stress_cur = scui_presenter.get_stress_cur();
        char digit_str[10] = {0};
        snprintf(digit_str, sizeof(digit_str), "%d", stress_cur);
        scui_string_update_str(SCUI_UI_SCENE_STRESS_VALUE, digit_str);
        
        /* 压力等级 */
        scui_handle_t level_handle = SCUI_LANG_0X004b;
        if (stress_cur >= 50) level_handle = SCUI_LANG_0X004c;
        scui_string_update_text(SCUI_UI_SCENE_STRESS_LEVEL, level_handle);
        
        /* 最近测量时间 */
        uint32_t passby_last = scui_presenter.get_stress_passby();
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
        scui_string_update_str(SCUI_UI_SCENE_STRESS_TIME, data_unit);
        
        break;
    }
    default:
        break;
    }
}
