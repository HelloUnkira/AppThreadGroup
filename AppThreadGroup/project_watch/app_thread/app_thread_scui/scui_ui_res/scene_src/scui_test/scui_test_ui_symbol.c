/*实现目标:
 *    窗口:scui_test_ui_symbol
 *    用途: 测试 symbol 符号字符(从 scui_ui_1 挖取)
 *    构件: custom 画布全屏
 *    绘制: 所有 symbol 居中聚集排布
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 符号字符表(全部 SCUI_SYMBOL_STR_*)
 */
static const uint8_t * const symbol_table[] = {
    SCUI_SYMBOL_STR_AUDIO,
    SCUI_SYMBOL_STR_VIDEO,
    SCUI_SYMBOL_STR_LIST,
    SCUI_SYMBOL_STR_OK,
    SCUI_SYMBOL_STR_CLOSE,
    SCUI_SYMBOL_STR_POWER,
    SCUI_SYMBOL_STR_SETTINGS,
    SCUI_SYMBOL_STR_HOME,
    SCUI_SYMBOL_STR_DOWNLOAD,
    SCUI_SYMBOL_STR_DRIVE,
    SCUI_SYMBOL_STR_REFRESH,
    SCUI_SYMBOL_STR_MUTE,
    SCUI_SYMBOL_STR_VOLUME_MID,
    SCUI_SYMBOL_STR_VOLUME_MAX,
    SCUI_SYMBOL_STR_IMAGE,
    SCUI_SYMBOL_STR_TINT,
    SCUI_SYMBOL_STR_PREV,
    SCUI_SYMBOL_STR_PLAY,
    SCUI_SYMBOL_STR_PAUSE,
    SCUI_SYMBOL_STR_STOP,
    SCUI_SYMBOL_STR_NEXT,
    SCUI_SYMBOL_STR_EJECT,
    SCUI_SYMBOL_STR_LEFT,
    SCUI_SYMBOL_STR_RIGHT,
    SCUI_SYMBOL_STR_PLUS,
    SCUI_SYMBOL_STR_MINUS,
    SCUI_SYMBOL_STR_EYE_OPEN,
    SCUI_SYMBOL_STR_EYE_CLOSE,
    SCUI_SYMBOL_STR_WARNING,
    SCUI_SYMBOL_STR_SHUFFLE,
    SCUI_SYMBOL_STR_UP,
    SCUI_SYMBOL_STR_DOWN,
    SCUI_SYMBOL_STR_LOOP,
    SCUI_SYMBOL_STR_DIRECTORY,
    SCUI_SYMBOL_STR_UPLOAD,
    SCUI_SYMBOL_STR_CALL,
    SCUI_SYMBOL_STR_CUT,
    SCUI_SYMBOL_STR_COPY,
    SCUI_SYMBOL_STR_SAVE,
    SCUI_SYMBOL_STR_BARS,
    SCUI_SYMBOL_STR_ENVELOPE,
    SCUI_SYMBOL_STR_CHARGE,
    SCUI_SYMBOL_STR_PASTE,
    SCUI_SYMBOL_STR_BELL,
    SCUI_SYMBOL_STR_KEYBOARD,
    SCUI_SYMBOL_STR_GPS,
    SCUI_SYMBOL_STR_FILE,
    SCUI_SYMBOL_STR_WIFI,
    SCUI_SYMBOL_STR_BATTERY_FULL,
    SCUI_SYMBOL_STR_BATTERY_3,
    SCUI_SYMBOL_STR_BATTERY_2,
    SCUI_SYMBOL_STR_BATTERY_1,
    SCUI_SYMBOL_STR_BATTERY_EMPTY,
    SCUI_SYMBOL_STR_USB,
    SCUI_SYMBOL_STR_BLUETOOTH,
    SCUI_SYMBOL_STR_TRASH,
    SCUI_SYMBOL_STR_EDIT,
    SCUI_SYMBOL_STR_BACKSPACE,
    SCUI_SYMBOL_STR_SD_CARD,
    SCUI_SYMBOL_STR_NEW_LINE,
};

/*@brief 符号行布局信息
 */
typedef struct {
    scui_coord_t count;    /* 行内符号数 */
    scui_coord_t width;    /* 行内符号总宽(不含间隙) */
    scui_coord_t height;   /* 行高(行内最大符号高) */
} scui_symbol_row_t;

/*@brief 画布控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_symbol_canvas_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_graph: {
        
        scui_handle_t font = scui_font_symbol_24bin;
        scui_coord_t  gap  = 8;
        scui_coord_t  row_gap = 8;
        
        scui_area_t widget_clip = {
            .x = 0,
            .y = 0,
            .w = scui_widget_clip(event->object).w,
            .h = scui_widget_clip(event->object).h,
        };
        scui_coord_t draw_w = widget_clip.w / 2;   /* 中间 1/2 宽度绘制区 */
        
        /* 第一遍: 按行分组, 计算每行宽度/行高 */
        scui_symbol_row_t row[scui_arr_len(symbol_table)] = {{0}};
        scui_coord_t      row_num = 0;
        scui_coord_t      row_idx = 0;
        
        for (uint8_t idx = 0; idx < scui_arr_len(symbol_table); idx++) {
            uint32_t    symbol = scui_symbol_code((uint8_t *)symbol_table[idx]);
            scui_area_t area   = scui_symbol_area(font, symbol);
            
            if (row[row_idx].width + area.w + gap > draw_w && row[row_idx].count > 0)
                row_idx++;
            
            row[row_idx].count++;
            row[row_idx].width  += area.w;
            if (area.h > row[row_idx].height)
                row[row_idx].height = area.h;
        }
        row_num = row_idx + 1;
        
        /* 整体垂直居中: 计算起始 y */
        scui_coord_t total_h = 0;
        for (row_idx = 0; row_idx < row_num; row_idx++) {
            total_h += row[row_idx].height;
            if (row_idx < row_num - 1)
                total_h += row_gap;
        }
        scui_coord_t cur_y = (widget_clip.h - total_h) / 2;
        
        /* 第二遍: 逐行绘制, 每行在绘制区水平居中 */
        row_idx = 0;
        scui_coord_t idx = 0;
        for (row_idx = 0; row_idx < row_num; row_idx++) {
            scui_coord_t row_w = row[row_idx].width + (row[row_idx].count - 1) * gap;
            scui_coord_t cur_x = widget_clip.w / 4 + (draw_w - row_w) / 2;
            
            for (scui_coord_t sub = 0; sub < row[row_idx].count; sub++) {
                uint32_t    symbol = scui_symbol_code((uint8_t *)symbol_table[idx]);
                scui_area_t area   = scui_symbol_area(font, symbol);
                
                scui_area_t target = {
                    .x = cur_x,
                    .y = cur_y + (row[row_idx].height - area.h) / 2,
                    .w = area.w,
                    .h = area.h,
                };
                scui_widget_draw_symbol(event->object, &target, NULL, SCUI_COLOR_WHITE, font, symbol);
                
                cur_x += area.w + gap;
                idx++;
            }
            cur_y += row[row_idx].height + row_gap;
        }
        break;
    }
    default:
        break;
    }
}
