/*实现目标:
 *    测试(widget symbol)
 *    使用flex布局: layout外部全局居中, auto宽高;
 *    symbol子控件auto宽高, 每组若干个, 轨道内/外均水平垂直居中+span;
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

/*@brief 画布控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_symbol_canvas_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        /* flex布局: auto宽高, 外部全局居中 */
        scui_layout_maker_define(layout_maker);
        scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
        
        layout_maker.widget.parent = event->object;
        layout_maker.widget.clip.w = SCUI_WIDGET_AUTO_W;
        layout_maker.widget.clip.h = SCUI_WIDGET_AUTO_H;
        layout_maker.widget.child_num = scui_arr_len(symbol_table);
        layout_maker.type  = scui_layout_type_flex;
        layout_maker.use   = true;           /* 开启外部对齐 */
        layout_maker.align = scui_align_icc; /* 全局居中 */
        layout_maker.offset = (scui_point_t){0};
        scui_widget_create(&layout_maker, &layout_handle);
        
        /* 轨道方向: 水平(way=0); span各轴8; 轨道内/外均水平垂直居中 */
        scui_layout_flex_way(layout_handle, 0, (scui_point_t){8, 8}, (scui_point_t){1, 0},
            scui_opt_pos_hor | scui_opt_pos_ver,   /* 轨道间(外)对齐: 水平垂直center */
            scui_opt_pos_hor | scui_opt_pos_ver);  /* 轨道内对齐:     水平垂直center */
        
        /* 每组加定数量的symbol, symbol本身auto宽高 */
        const scui_coord_t group_size = 8;   /* 每组个数 */
        for (uint8_t idx = 0; idx < scui_arr_len(symbol_table); idx++) {
            scui_symbol_maker_define(symbol_maker);
            scui_handle_t symbol_handle = SCUI_HANDLE_INVALID;
            
            symbol_maker.widget.parent = layout_handle;
            symbol_maker.widget.clip.w = SCUI_WIDGET_AUTO_W;
            symbol_maker.widget.clip.h = SCUI_WIDGET_AUTO_H;
            symbol_maker.color = SCUI_COLOR_WHITE;
            scui_widget_create(&symbol_maker, &symbol_handle);
            
            scui_symbol_update(symbol_handle, SCUI_FONT_IDX_X24, symbol_table[idx]);
            scui_layout_flex_group(layout_handle, symbol_handle, idx / group_size);
        }
        break;
    }
    default:
        break;
    }
}