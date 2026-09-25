/*实现目标:
 *    测试(string auto_w/auto_h 环排动画)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_hobby_event_proc(scui_event_t *event)
{
    static struct {
        scui_handle_t string[12];   /* 环排字符控件 */
        scui_coord3_t angle;        /* 环排旋转角(float, 0.1度精度) */
        scui_coord3_t radius;       /* 环排半径(2/3/2=1/3屏幕, 圆在屏内) */
    } * scui_ui_res_local = NULL;

    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_ui_res_local->angle  = 0;
        scui_ui_res_local->radius = SCUI_HOR_RES * 2 / 3 / 2;   /* 2/3/2 = 1/3屏幕 */

        static const char * const char_table[12] = {
            "\xE8\x83\x9C",
            "\xE5\x85\xB5",
            "\xE5\xBF\x85",
            "\xE9\xAA\x84",
            "\xE5\x85\xB5",
            "\xE5\xBF\x85",
            "\xE8\xB4\xA5",
            "\xE5\x85\xB5",
            "\xE5\xBF\x85",
            "\xE5\x93\x80",
            "\xE5\x85\xB5",
            "\xE5\xBF\x85",
        };
        static const uint32_t color_table[12] = {
            0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF,
        };

        for (scui_coord_t idx = 0; idx < scui_arr_len(scui_ui_res_local->string); idx++) {
            scui_string_maker_define(string_maker);
            scui_handle_t string_handle = SCUI_HANDLE_INVALID;

            string_maker.widget.parent           = event->object;
            string_maker.widget.clip           = SCUI_AREA_MAKE_BM(0, 0, SCUI_WIDGET_AUTO_W, SCUI_WIDGET_AUTO_H);
            string_maker.widget.style.fully_bg   = true;
            string_maker.widget.color.color.full = 0xFF6F6F6F;
            string_maker.font_idx                = SCUI_FONT_IDX_36;
            string_maker.args.lang               = scui_lang_type_zh;
            string_maker.args.align_hor          = 2;
            string_maker.args.align_ver          = 2;
            string_maker.args.color            = SCUI_COLOR_MAKE32_SE(true, 0, color_table[idx], color_table[idx]);
            scui_widget_create(&string_maker, &string_handle);
            scui_string_update_str(string_handle, (uint8_t *)char_table[idx]);
            scui_ui_res_local->string[idx] = string_handle;
        }
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_local->angle += event->tick * 0.05f;
        if (scui_ui_res_local->angle >= 360.0f)
            scui_ui_res_local->angle -= 360.0f;

        const scui_coord3_t center_x = SCUI_HOR_RES / 2;
        const scui_coord3_t center_y = SCUI_VER_RES / 2;
        const scui_coord3_t step     = 360.0f / scui_arr_len(scui_ui_res_local->string);

        for (scui_coord_t idx = 0; idx < scui_arr_len(scui_ui_res_local->string); idx++) {
            scui_widget_t *widget = scui_handle_source_check(scui_ui_res_local->string[idx]);
            /* AUTO未解析(首帧)时跳过, 待layout解析出实际宽高 */
            if (widget->clip.w == SCUI_WIDGET_AUTO_W)
                continue;
            if (widget->clip.h == SCUI_WIDGET_AUTO_H)
                continue;

            /* 以控件中心为圈上点, 反算左上角xy(验证AUTO宽高) */
            scui_coord3_t angle = scui_ui_res_local->angle + idx * step;
            scui_point_t point = {
                .x = (scui_coord_t)(center_x + scui_ui_res_local->radius * scui_cos(SCUI_RAD_BY_A(angle)) - widget->clip.w / 2),
                .y = (scui_coord_t)(center_y + scui_ui_res_local->radius * scui_sin(SCUI_RAD_BY_A(angle)) - widget->clip.h / 2),
            };
            scui_widget_move_pos(scui_ui_res_local->string[idx], &point, true);
        }
        break;
    }
    default:
        break;
    }
}
