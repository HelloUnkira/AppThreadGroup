/*实现目标:
 *    测试(widget inchar)
 *    输入字符控件: 输入框 + 输出 + 数字键盘 + 控制按钮
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 字符矩阵(点击输入) */
static const char scui_ui_inchar_chars[] = "789/456*123-0.+=";

/* 控制按钮(句柄/文本) */
static const char * const scui_ui_inchar_btn_text[] = {
    "next", "prev", "L/M/R", "c_p", "c_n", "hide", "ins",
    "rep",  "del",  "clr",  "head", "tail",
};

static struct {
    scui_handle_t box;          /* 输入框 */
    scui_handle_t out;          /* 输出框 */
    scui_handle_t btn[12];      /* 控制按钮 */
    bool          mode_rep;     /* 模式状态(替换:真;插入:假) */
} * scui_ui_res_local = NULL;

/*@brief 输出框同步(内容变更指令后自动执行)
 */
static void scui_test_ui_inchar_refr(void)
{
    if (scui_ui_res_local->out == SCUI_HANDLE_INVALID)
        return;
    
    scui_coord_t str_num = 0;
    scui_inchar_str_num(scui_ui_res_local->box, &str_num);
    
    uint8_t *out_text = SCUI_MEM_ZALLOC(scui_mem_type_user, str_num + 5 + 1);
    memcpy(out_text, "Out: ", 5);
    scui_inchar_str_utf8(scui_ui_res_local->box, out_text + 5);
    scui_string_update_str(scui_ui_res_local->out, out_text);
    SCUI_MEM_FREE(out_text);
}

/*@brief 字符矩阵条目事件回调(点击自动输入)
 *@param event 事件
 */
void scui_test_ui_inchar_char_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click: {
        scui_coord_t index = scui_widget_child_to_index(event->object);
        if (index < 0 || index >= (scui_coord_t)scui_arr_len(scui_ui_inchar_chars) - 1)
            break;
        
        /* 自动输入(应用当前模式状态) */
        uint8_t char_text[2] = {(uint8_t)scui_ui_inchar_chars[index], 0};
        if (scui_ui_res_local->mode_rep) {
            scui_coord_t cursor_idx = 0;
            scui_inchar_cursor_get(scui_ui_res_local->box, &cursor_idx);
            scui_inchar_char_rep(scui_ui_res_local->box,
                cursor_idx, char_text);
        } else {
            scui_inchar_char_add(scui_ui_res_local->box, char_text);
        }
        scui_test_ui_inchar_refr();
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}

/*@brief 控制按钮事件回调
 *@param event 事件
 */
void scui_test_ui_inchar_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click: {
        scui_coord_t index = -1;
        for (scui_coord_t idx = 0; idx < (scui_coord_t)scui_arr_len(scui_ui_inchar_btn_text); idx++)
        if (scui_ui_res_local->btn[idx] == event->object) {index = idx; break;}
        if (index < 0) break;
        
        switch (index) {
        case 0: {   /* next: 光标后移 */
            scui_coord_t index = 0;
            scui_inchar_cursor_get(scui_ui_res_local->box, &index);
            scui_inchar_cursor_set(scui_ui_res_local->box, index + 1);
            break;
        }
        case 1: {   /* prev: 光标前移 */
            scui_coord_t index = 0;
            scui_inchar_cursor_get(scui_ui_res_local->box, &index);
            scui_inchar_cursor_set(scui_ui_res_local->box, index - 1);
            break;
        }
        case 2: {   /* L/M/R: 左/中/右对齐循环切换 */
            scui_string_t *string = (void *)scui_handle_source_check(scui_ui_res_local->box);
            if (string->args.align_hor == 0)       string->args.align_hor = 2;
            else if (string->args.align_hor == 2)  string->args.align_hor = 1;
            else                                    string->args.align_hor = 0;
            string->args.update = true;
            scui_widget_draw(scui_ui_res_local->box, NULL, false, 0);
            break;
        }
        case 3: {   /* c_p: 方向标记为光标前 */
            scui_inchar_char_ins_way(scui_ui_res_local->box, true);
            break;
        }
        case 4: {   /* c_n: 方向标记为光标后 */
            scui_inchar_char_ins_way(scui_ui_res_local->box, false);
            break;
        }
        case 5: {   /* hide: 光标显示切换 */
            static bool hide = false;
            hide = hide ? false : true;
            scui_inchar_cursor(scui_ui_res_local->box, hide);
            break;
        }
        case 6: {   /* ins: 模式状态为插入(后续输入自动插入) */
            scui_ui_res_local->mode_rep = false;
            break;
        }
        case 7: {   /* rep: 模式状态为替换(后续输入自动替换) */
            scui_ui_res_local->mode_rep = true;
            break;
        }
        case 8: {   /* del: 独立删除光标位置字符 */
            scui_inchar_char_del(scui_ui_res_local->box);
            scui_test_ui_inchar_refr();
            break;
        }
        case 9: {   /* clr: 清空 */
            for (scui_coord_t num = 0; true; num) {
                scui_inchar_char_num(scui_ui_res_local->box, &num);
                if (num <= 0) break;
                
                scui_inchar_char_rem(scui_ui_res_local->box, 0);
            }
            scui_test_ui_inchar_refr();
            break;
        }
        case 10: {  /* head: 光标头部 */
            scui_inchar_cursor_set(scui_ui_res_local->box, 0);
            break;
        }
        case 11: {  /* tail: 光标尾部 */
            scui_coord_t num = 0;
            scui_inchar_char_num(scui_ui_res_local->box, &num);
            scui_inchar_cursor_set(scui_ui_res_local->box, num);
            break;
        }
        default:
            break;
        }
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_inchar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 输入框 */
        {
            scui_inchar_maker_define(inchar_maker);
            inchar_maker.widget.parent       = event->object;
            inchar_maker.widget.clip         = SCUI_AREA_MAKE_BM(20, 10, 426, 48);
            inchar_maker.widget.style.fully_bg = true;
            inchar_maker.widget.color.color.full = 0xFF2F2F2F;
            inchar_maker.string.font_idx   = SCUI_FONT_IDX_X32;
            inchar_maker.string.args.lang  = scui_lang_type_ascii;
            inchar_maker.string.args.color.color.full = 0xFFFFFFFF;
            inchar_maker.string.args.align_hor = 0;
            inchar_maker.string.args.align_ver = 2;
            inchar_maker.limit          = 255;
            inchar_maker.color.color.full = 0xFF4FC3F7;
            scui_widget_create(&inchar_maker, &scui_ui_res_local->box);
        }
        
        /* 输出框 */
        {
            scui_string_maker_define(string_maker);
            string_maker.widget.parent       = event->object;
            string_maker.widget.clip         = SCUI_AREA_MAKE_BM(20, 66, 426, 48);
            string_maker.widget.style.fully_bg = true;
            string_maker.widget.color.color.full = 0xFF3F3F3F;
            string_maker.font_idx        = SCUI_FONT_IDX_X32;
            string_maker.args.lang       = scui_lang_type_ascii;
            string_maker.args.color.color.full = 0xFFFFFFFF;
            string_maker.args.align_hor  = 0;
            string_maker.args.align_ver  = 2;
            scui_widget_create(&string_maker, &scui_ui_res_local->out);
            scui_string_update_str(scui_ui_res_local->out, (uint8_t *)"Out: ");
        }
        
        /* 控制按钮: flex 布局(行1 6个/行2 6个, 水平均分填充) */
        {
            scui_handle_t flex_handle = SCUI_HANDLE_INVALID;
            scui_layout_maker_define(layout_maker);
            scui_widget_maker_linker(&layout_maker.widget,
                (scui_coord_t)scui_arr_len(scui_ui_inchar_btn_text), event->object);
            layout_maker.widget.clip     = SCUI_AREA_MAKE_BM(0, 124, SCUI_HOR_RES, SCUI_WIDGET_AUTO_H);
            layout_maker.widget.style.fully_bg = true;
            layout_maker.widget.color.color.full = 0xFF1F1F1F;
            layout_maker.type            = scui_layout_type_flex;
            layout_maker.use             = false;
            layout_maker.offset          = (scui_point_t){0};
            scui_widget_create(&layout_maker, &flex_handle);
            scui_layout_flex_way(flex_handle, false, (scui_point_t){10, 12},
                (scui_point_t){1, 0}, scui_opt_pos_c, scui_opt_pos_c);
            
            for (scui_coord_t idx = 0; idx < (scui_coord_t)scui_arr_len(scui_ui_inchar_btn_text); idx++) {
                scui_string_maker_define(btn_maker);
                btn_maker.widget.parent       = flex_handle;
                btn_maker.widget.clip         = SCUI_AREA_MAKE_BM(0, 0, SCUI_WIDGET_AUTO_W, SCUI_WIDGET_AUTO_H);
                btn_maker.widget.style.fully_bg = true;
                btn_maker.widget.style.indev_ptr = true;
                btn_maker.widget.event_cb     = scui_test_ui_inchar_btn_event_proc;
                btn_maker.widget.color.color.full = 0xFF2A2A2A;
                btn_maker.font_idx            = SCUI_FONT_IDX_X32;
                btn_maker.args.lang           = scui_lang_type_ascii;
                btn_maker.args.color.color.full = 0xFFFFFFFF;
                btn_maker.args.align_hor      = 2;
                btn_maker.args.align_ver      = 2;
                
                scui_widget_create(&btn_maker, &scui_ui_res_local->btn[idx]);
                scui_string_update_str(scui_ui_res_local->btn[idx], (uint8_t *)scui_ui_inchar_btn_text[idx]);
                scui_layout_flex_group(flex_handle, scui_ui_res_local->btn[idx], idx < 6 ? 0 : 1);
            }
        }
        
        /* 字符矩阵: 数字键盘 grid 4列 x 4行 */
        {
            scui_handle_t grid_handle = SCUI_HANDLE_INVALID;
            scui_layout_maker_define(layout_maker);
            scui_widget_maker_linker(&layout_maker.widget,
                (scui_coord_t)scui_arr_len(scui_ui_inchar_chars) - 1, event->object);
            layout_maker.widget.clip         = SCUI_AREA_MAKE_BM(0, 220, SCUI_HOR_RES, 246);
            layout_maker.widget.style.fully_bg = true;
            layout_maker.widget.color.color.full = 0xFF252525;
            layout_maker.type               = scui_layout_type_grid;
            layout_maker.use                = false;
            layout_maker.offset             = (scui_point_t){0};
            scui_widget_create(&layout_maker, &grid_handle);
            
            scui_coord_t col_size[4] = {112, 112, 112, 112};
            scui_layout_grid_way(grid_handle, false, col_size, 4, 6);
            scui_coord_t row_size[4] = {57, 57, 57, 57};
            scui_layout_grid_way(grid_handle, true, row_size, 4, 6);
            
            for (scui_coord_t idx = 0; idx < (scui_coord_t)scui_arr_len(scui_ui_inchar_chars) - 1; idx++) {
                scui_string_maker_define(char_maker);
                char_maker.widget.parent    = grid_handle;
                char_maker.widget.clip      = SCUI_AREA_MAKE_BM(0, 0, 102, 57);
                char_maker.widget.style.fully_bg = true;
                char_maker.widget.style.indev_ptr = true;
                char_maker.widget.event_cb  = scui_test_ui_inchar_char_event_proc;
                char_maker.widget.color.color.full = 0xFF353535;
                char_maker.font_idx         = SCUI_FONT_IDX_X32;
                char_maker.args.lang        = scui_lang_type_ascii;
                char_maker.args.color.color.full = 0xFFFFFFFF;
                char_maker.args.align_hor   = 2;
                char_maker.args.align_ver   = 2;
                
                scui_handle_t char_handle = SCUI_HANDLE_INVALID;
                scui_widget_create(&char_maker, &char_handle);
                uint8_t char_text[2] = {(uint8_t)scui_ui_inchar_chars[idx], 0};
                scui_string_update_str(char_handle, char_text);
                
                scui_layout_grid_cell(grid_handle, char_handle,
                    (scui_point_t){idx % 4, idx / 4}, (scui_point_t){1, 1},
                    scui_opt_pos_hor | scui_opt_pos_ver, 0);
            }
        }
        break;
    }
    case scui_event_destroy:
        break;
    default:
        break;
    }
}
