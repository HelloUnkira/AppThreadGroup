/*实现目标:
 *    控件样式应用
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件应用样式回调
 *@param handle 控件句柄
 */
void scui_ui_apply(scui_handle_t handle)
{
    switch (scui_widget_type(handle)) {
    case scui_widget_type_obj_btn: {
        /* 常规 res: 天蓝圆角, 点击变红, 居中 */
        scui_obj_btn_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.align  = scui_opt_pos_c;
        res.color[0].color_s.full = 0xFF2196F3;
        res.color[1].color_s.full = 0xFFF44336;
        res.radius = -1;
        
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_base;
        scui_obj_btn_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_arc: {
        /* 常规 res: 背景灰, 前景天蓝, 0~360度 */
        scui_obj_arc_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.round = true;
        
        /* bg: 灰 */
        res.color.color_s.full = 0xFF9E9E9E;
        res.color.color_e.full = 0xFF9E9E9E;
        res.part = scui_object_part_arc_bg;
        res.form = scui_object_form_arc_base;
        scui_obj_arc_style(handle, &res);
        /* fg: 天蓝 */
        res.color.color_s.full = 0xFF2196F3;
        res.color.color_e.full = 0xFF2196F3;
        res.part = scui_object_part_arc_fg;
        res.form = scui_object_form_arc_base;
        scui_obj_arc_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_bar: {
        /* 常规 res: 背景浅灰, 前景天蓝 */
        scui_obj_bar_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.align = scui_opt_pos_l | scui_opt_pos_u;
        res.color.color_s.full = 0xFFE0E0E0;
        res.color.color_e.full = 0xFFE0E0E0;
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        res.color.color_s.full = 0xFF2196F3;
        res.color.color_e.full = 0xFF2196F3;
        res.part = scui_object_part_rect_fg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_cht: {
        /* 常规 res: 天蓝 */
        scui_obj_cht_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.color.color.full = 0xFF2196F3;
        
        scui_coord_t type = 0;
        scui_obj_cht_type(handle, &type);
        
        if (type == 0) {
            res.part = scui_object_part_rect_fg;
            res.form = scui_object_form_rect_base;
            scui_obj_cht_style(handle, &res);
        } else {
            res.part = scui_object_part_line_item;
            res.form = 0;
            scui_obj_cht_style(handle, &res);
        }
        break;
    }
    case scui_widget_type_obj_slr: {
        /* 常规 res: 背景浅灰, 前景天蓝 */
        scui_obj_bar_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.align = scui_opt_pos_l | scui_opt_pos_u;
        res.color.color_s.full = 0xFFE0E0E0;
        res.color.color_e.full = 0xFFE0E0E0;
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        res.color.color_s.full = 0xFF2196F3;
        res.color.color_e.full = 0xFF2196F3;
        res.part = scui_object_part_rect_fg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_swt: {
        /* 常规 res: 背景浅灰, 前景天蓝, 端点白色 */
        scui_obj_bar_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.align = scui_opt_pos_l | scui_opt_pos_u;
        res.color.color_s.full = 0xFFE0E0E0;
        res.color.color_e.full = 0xFFE0E0E0;
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        res.color.color_s.full = 0xFF2196F3;
        res.color.color_e.full = 0xFF2196F3;
        res.part = scui_object_part_rect_fg;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        res.color.color_s.full = 0xFFFFFFFF;
        res.color.color_e.full = 0xFFFFFFFF;
        res.part = scui_object_part_rect_knob;
        res.form = scui_object_form_rect_base;
        scui_obj_bar_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_spn: {
        /* 常规 res: 背景灰, 前景天蓝, 0~360度 */
        scui_obj_arc_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.round = true;
        
        /* bg: 灰 */
        res.color.color_s.full = 0xFF9E9E9E;
        res.color.color_e.full = 0xFF9E9E9E;
        res.part = scui_object_part_arc_bg;
        res.form = scui_object_form_arc_base;
        scui_obj_arc_style(handle, &res);
        /* fg: 天蓝 */
        res.color.color_s.full = 0xFF2196F3;
        res.color.color_e.full = 0xFF2196F3;
        res.part = scui_object_part_arc_fg;
        res.form = scui_object_form_arc_base;
        scui_obj_arc_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_led: {
        /* 常规 res: 默认淡白(off), 点亮天蓝(带阴影光晕) */
        scui_obj_led_res_t res = {0};
        res.alpha = scui_alpha_cover;
        res.align = scui_opt_pos_c;
        res.radius = -1;
        res.glow   = 12;
        res.color_on  = SCUI_COLOR_MAKE32(false, 0, 0xFF87CEEB);     /* 天蓝 */
        res.color_off = SCUI_COLOR_MAKE32(false, 0, 0xFFC8C8C8);     /* 淡白 */
        scui_obj_led_style(handle, &res);
        break;
    }
    case scui_widget_type_obj_chk: {
        /* 常规 res: 白底+天蓝edge; 点击后双天蓝; 圆角3; 居中 */
        scui_obj_btn_res_t res = {0};
        res.alpha  = scui_alpha_cover;
        res.align  = scui_opt_pos_c;
        res.radius = 3;
        
        /* def/pre/chk/pre: 白底 -> 天蓝 */
        res.color[0].color_s.full = 0xFFFFFFFF;    /* def: 白 */
        res.color[1].color_s.full = 0xFF2196F3;    /* pre: 天蓝 */
        res.color[2].color_s.full = 0xFF2196F3;    /* chk: 天蓝 */
        res.color[3].color_s.full = 0xFF2196F3;    /* pre: 天蓝 */
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_base;
        scui_obj_btn_style(handle, &res);
        
        /* edge: 天蓝(全状态) */
        res.color[0].color_s.full = 0xFF2196F3;
        res.color[1].color_s.full = 0xFF2196F3;
        res.color[2].color_s.full = 0xFF2196F3;
        res.color[3].color_s.full = 0xFF2196F3;
        res.width = 3;
        res.part = scui_object_part_rect_bg;
        res.form = scui_object_form_rect_edge;
        scui_obj_btn_style(handle, &res);
        
        scui_object_type_t state = scui_object_state_def;
        scui_obj_chk_state(handle, &state);
        /* 固定且初始态为def(未选中): base/edge半透明50% */
        if (scui_obj_chk_fixed(handle) && state == scui_object_state_def) {
            
            scui_object_prop_add_s(handle, scui_object_part_rect_bg, scui_object_form_rect_base,
                scui_object_style_rect_alpha, scui_object_state_def,
                scui_object_data_alpha(scui_alpha_pct50));
            
            scui_object_prop_add_s(handle, scui_object_part_rect_bg, scui_object_form_rect_edge,
                scui_object_style_rect_alpha, scui_object_state_def,
                scui_object_data_alpha(scui_alpha_pct50));
        }
        break;
    }
    default:
        break;
    }
}
