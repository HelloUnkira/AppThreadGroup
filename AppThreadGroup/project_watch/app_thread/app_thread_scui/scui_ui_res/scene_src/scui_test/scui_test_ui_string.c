/*实现目标:
 *    窗口:scui_test_ui_string
 *    用途: 测试 string 控件(从 scui_ui_test 挖取)
 *    构件: 4个渐变string(下划线/删除线/多行/滚动模式)
 *    交互: 编码器调整字号, 按键切换语言
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t string[4];    /* 字符串控件句柄 */
    uint16_t      font_size;    /* 字体尺寸 */
    uint16_t      lang_idx;     /* 语言索引 */
} * scui_ui_res_local = NULL;

/*@brief 全文本渐变配置
 */
static const scui_color_t string_grads[8] = {
    {.color.full = 0xFFFF5733}, {.color.full = 0xFFFFBD33},
    {.color.full = 0xFF75FF33}, {.color.full = 0xFF33FF57},
    {.color.full = 0xFF33FFBD}, {.color.full = 0xFF3375FF},
    {.color.full = 0xFF5733FF}, {.color.full = 0xFFBD33FF},
};

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_string_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_ui_res_local->font_size = 32;
        scui_ui_res_local->lang_idx  = 0;
        
        /* 备注: 如果需要使用动态文字缩放, 则应该放弃掉缓冲帧和级联着色, 否则可能会爆内存 */
        
        scui_string_maker_define(string_maker);
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        
        string_maker.widget.parent     = event->object;
        string_maker.args.line_width   = 2;
        string_maker.args.mode_scroll  = 1;
        string_maker.args.align_hor    = 2;
        string_maker.args.align_ver    = 2;
        string_maker.args.regrad       = true;
        string_maker.draw_cache        = true;
        
        string_maker.font_idx  = SCUI_FONT_IDX_MZ;
        string_maker.args.lang = scui_lang_type_multi;
        string_maker.args.size = scui_ui_res_local->font_size;
        
        /* test:全文本渐变 */
        scui_coord_t y_offset = 25;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 50;
        string_maker.text             = SCUI_LANG_0X0029;
        string_maker.args.line_under  = 1;
        string_maker.args.line_delete = 0;
        string_maker.args.color.color_s.full = 0xFFFF00FF;
        string_maker.args.color.color_e.full = 0xFFFF00FF;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_upgrade_grads(string_handle, (scui_color_t *)string_grads, 8, false);
        scui_ui_res_local->string[0] = string_handle;
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 50;
        string_maker.text             = SCUI_LANG_0X002e;
        string_maker.args.line_under  = 0;
        string_maker.args.line_delete = 1;
        string_maker.args.color.color_s.full = 0xFFFF0000;
        string_maker.args.color.color_e.full = 0xFFFF0000;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_upgrade_grads(string_handle, (scui_color_t *)string_grads, 8, false);
        scui_ui_res_local->string[1] = string_handle;
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 130;
        string_maker.text             = SCUI_LANG_0X0034;
        string_maker.args.line_under  = 1;
        string_maker.args.line_delete = 0;
        string_maker.args.color.color_s.full = 0xFF00FF00;
        string_maker.args.color.color_e.full = 0xFF00FF00;
        string_maker.args.line_multi = true;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_upgrade_grads(string_handle, (scui_color_t *)string_grads, 8, false);
        scui_ui_res_local->string[2] = string_handle;
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 130;
        string_maker.text             = SCUI_LANG_0X0042;
        string_maker.args.line_under  = 0;
        string_maker.args.line_delete = 1;
        string_maker.args.color.color_s.full = 0xFF0000FF;
        string_maker.args.color.color_e.full = 0xFF0000FF;
        string_maker.args.line_multi = true;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_upgrade_grads(string_handle, (scui_color_t *)string_grads, 8, true);
        scui_ui_res_local->string[3] = string_handle;
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        static const scui_lang_type_t lang_type_table[] = {
            scui_lang_type_zh,
            scui_lang_type_en,
            scui_lang_type_de,
            scui_lang_type_fra,
            scui_lang_type_nl,
            scui_lang_type_pt,
            scui_lang_type_jp,
        };
        scui_lang_type_t lang_type = 0;
        scui_lang_get(&lang_type);
        scui_coord_t lang_type_num = scui_arr_len(lang_type_table);
        for (scui_coord_t lang_idx = 0; lang_idx < lang_type_num; lang_idx++) {
            if (lang_type_table[lang_idx] != lang_type)
                continue;
            
            lang_idx += 1;
            if (lang_idx >= lang_type_num)
                lang_idx = 0;
            
            scui_lang_set(&lang_type_table[lang_idx]);
            break;
        }
        
        scui_event_mask_over(event);
        break;
    }
    case scui_event_enc_fdir: {
        const uint16_t font_size_max = 40;
        const uint16_t font_size_gap = 2;
        if (scui_ui_res_local->font_size + font_size_gap <= font_size_max)
            scui_ui_res_local->font_size += font_size_gap;
        for (uint8_t idx = 0; idx < scui_arr_len(scui_ui_res_local->string); idx++)
            scui_string_adjust_size(scui_ui_res_local->string[idx], scui_ui_res_local->font_size);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_enc_bdir: {
        const uint16_t font_size_min = 10;
        const uint16_t font_size_gap = 2;
        if (scui_ui_res_local->font_size - font_size_gap >= font_size_min)
            scui_ui_res_local->font_size -= font_size_gap;
        for (uint8_t idx = 0; idx < scui_arr_len(scui_ui_res_local->string); idx++)
            scui_string_adjust_size(scui_ui_res_local->string[idx], scui_ui_res_local->font_size);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
