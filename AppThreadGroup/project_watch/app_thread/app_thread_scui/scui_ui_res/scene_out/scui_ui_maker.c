/* 本文件由 scui_pack_tools.exe 生成 */

#include "scui.h"

/*@brief 控件构造器默认初始化
 *@param maker 控件构造器实例指针
 *@param type  控件类型(scui_widget_type_t)
 */
void scui_ui_maker(void *maker, scui_widget_type_t type)
{
	switch (type) {
	case scui_widget_type_window: {
		scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
		
		window_maker->widget.type            = scui_widget_type_window;
		window_maker->widget.style.buffer    = true;
		window_maker->widget.style.fully_bg  = true;
		window_maker->widget.clip.w          = SCUI_HOR_RES;
		window_maker->widget.clip.h          = SCUI_VER_RES;
		window_maker->preload                = 1;
		window_maker->level                  = 0;
		window_maker->switch_enc             = scui_opt_pos_all;
		window_maker->switch_bar             = scui_opt_pos_all;
		window_maker->switch_key             = scui_opt_pos_all;
		window_maker->switch_enc_way         = scui_opt_dir_ver;
		window_maker->switch_bar_way         = scui_opt_dir_ver;
		window_maker->switch_key_id[0]       = scui_event_key_val_down;
		window_maker->switch_key_id[1]       = scui_event_key_val_up;
		window_maker->switch_key_id[2]       = scui_event_key_val_right;
		window_maker->switch_key_id[3]       = scui_event_key_val_left;
		break;
	}
	case scui_widget_type_layout: {
		scui_layout_maker_t *layout_maker = (scui_layout_maker_t *)maker;
		
		layout_maker->widget.type    = scui_widget_type_layout;
		layout_maker->widget.clip.w  = SCUI_WIDGET_AUTO_W;
		layout_maker->widget.clip.h  = SCUI_WIDGET_AUTO_H;
		break;
	}
	case scui_widget_type_scroll: {
		scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
		
		scroll_maker->widget.type             = scui_widget_type_scroll;
		scroll_maker->widget.style.indev_enc  = true;
		scroll_maker->widget.style.indev_key  = true;
		scroll_maker->widget.clip.w           = SCUI_HOR_RES;
		scroll_maker->widget.clip.h           = SCUI_VER_RES;
		scroll_maker->pos                     = scui_opt_pos_c;
		scroll_maker->dir                     = scui_opt_dir_ver;
		scroll_maker->skip                    = scui_opt_pos_all;
		scroll_maker->space                   = 10;
		scroll_maker->fling_page              = 1;
		scroll_maker->springback              = 70;
		scroll_maker->keyid_fdir              = SCUI_WIDGET_SCROLL_KEY_FDIR;
		scroll_maker->keyid_bdir              = SCUI_WIDGET_SCROLL_KEY_BDIR;
		break;
	}
	case scui_widget_type_custom: {
		scui_custom_maker_t *custom_maker = (scui_custom_maker_t *)maker;
		
		custom_maker->widget.type  = scui_widget_type_custom;
		break;
	}
	case scui_widget_type_string: {
		scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
		
		string_maker->widget.type              = scui_widget_type_string;
		string_maker->widget.clip.w            = SCUI_WIDGET_AUTO_W;
		string_maker->widget.clip.h            = SCUI_WIDGET_AUTO_H;
		string_maker->args.stroke              = 2;
		string_maker->args.align_ver           = 2;
		string_maker->args.color.color_s.full  = 0xFFFFFFFF;
		string_maker->args.color.color_e.full  = 0xFFFFFFFF;
		string_maker->args.color.filter        = true;
		break;
	}
	case scui_widget_type_inchar: {
		scui_inchar_maker_t *inchar_maker = (scui_inchar_maker_t *)maker;
		
		inchar_maker->widget.type                     = scui_widget_type_inchar;
		inchar_maker->string.args.align_ver           = 2;
		inchar_maker->string.args.color.color_s.full  = 0xFFFFFFFF;
		inchar_maker->string.args.color.color_e.full  = 0xFFFFFFFF;
		inchar_maker->string.args.color.filter        = true;
		break;
	}
	case scui_widget_type_symbol: {
		scui_symbol_maker_t *symbol_maker = (scui_symbol_maker_t *)maker;
		
		symbol_maker->widget.type       = scui_widget_type_symbol;
		symbol_maker->widget.clip.w     = SCUI_WIDGET_AUTO_W;
		symbol_maker->widget.clip.h     = SCUI_WIDGET_AUTO_H;
		symbol_maker->color.color.full  = 0xFFFFFFFF;
		break;
	}
	case scui_widget_type_roller: {
		scui_roller_maker_t *roller_maker = (scui_roller_maker_t *)maker;
		
		roller_maker->widget.type       = scui_widget_type_roller;
		roller_maker->widget.child_num  = 60;
		roller_maker->scroll.pos        = scui_opt_pos_c;
		roller_maker->scroll.dir        = scui_opt_dir_ver;
		roller_maker->scroll.skip       = scui_opt_pos_none;
		roller_maker->scroll.loop       = true;
		break;
	}
	case scui_widget_type_ximage: {
		scui_ximage_maker_t *ximage_maker = (scui_ximage_maker_t *)maker;
		
		ximage_maker->widget.type    = scui_widget_type_ximage;
		ximage_maker->widget.clip.w  = SCUI_WIDGET_AUTO_W;
		ximage_maker->widget.clip.h  = SCUI_WIDGET_AUTO_H;
		break;
	}
	case scui_widget_type_xwatch: {
		scui_xwatch_maker_t *xwatch_maker = (scui_xwatch_maker_t *)maker;
		
		xwatch_maker->widget.type  = scui_widget_type_xwatch;
		break;
	}
	case scui_widget_type_object: {
		scui_object_maker_t *object_maker = (scui_object_maker_t *)maker;
		
		object_maker->widget.type  = scui_widget_type_object;
		break;
	}
	case scui_widget_type_obj_btn: {
		scui_obj_btn_maker_t *obj_btn_maker = (scui_obj_btn_maker_t *)maker;
		
		obj_btn_maker->widget.type  = scui_widget_type_obj_btn;
		obj_btn_maker->fixed        = 0;
		obj_btn_maker->check        = 0;
		break;
	}
	case scui_widget_type_obj_arc: {
		scui_obj_arc_maker_t *obj_arc_maker = (scui_obj_arc_maker_t *)maker;
		
		obj_arc_maker->widget.type  = scui_widget_type_obj_arc;
		obj_arc_maker->angle_c      = 0;
		obj_arc_maker->anti         = 0;
		obj_arc_maker->touch        = 0;
		break;
	}
	case scui_widget_type_obj_bar: {
		scui_obj_bar_maker_t *obj_bar_maker = (scui_obj_bar_maker_t *)maker;
		
		obj_bar_maker->widget.type  = scui_widget_type_obj_bar;
		obj_bar_maker->way          = 0;
		obj_bar_maker->rev          = 0;
		obj_bar_maker->value_lim    = 100;
		obj_bar_maker->value_int    = 0;
		break;
	}
	case scui_widget_type_obj_cht: {
		scui_obj_cht_maker_t *obj_cht_maker = (scui_obj_cht_maker_t *)maker;
		
		obj_cht_maker->widget.type  = scui_widget_type_obj_cht;
		obj_cht_maker->type         = 0;
		obj_cht_maker->value_min    = 0;
		obj_cht_maker->value_max    = 100;
		obj_cht_maker->loop         = 0;
		break;
	}
	case scui_widget_type_obj_slr: {
		scui_obj_slr_maker_t *obj_slr_maker = (scui_obj_slr_maker_t *)maker;
		
		obj_slr_maker->widget.type  = scui_widget_type_obj_slr;
		obj_slr_maker->press        = 1;
		break;
	}
	case scui_widget_type_obj_swt: {
		scui_obj_swt_maker_t *obj_swt_maker = (scui_obj_swt_maker_t *)maker;
		
		obj_swt_maker->widget.type        = scui_widget_type_obj_swt;
		obj_swt_maker->obj_bar.way        = 0;
		obj_swt_maker->obj_bar.rev        = 0;
		obj_swt_maker->obj_bar.value_lim  = 100;
		obj_swt_maker->obj_bar.value_int  = 0;
		break;
	}
	case scui_widget_type_obj_spn: {
		scui_obj_spn_maker_t *obj_spn_maker = (scui_obj_spn_maker_t *)maker;
		
		obj_spn_maker->widget.type      = scui_widget_type_obj_spn;
		obj_spn_maker->obj_arc.angle_c  = 0;
		obj_spn_maker->obj_arc.anti     = 0;
		obj_spn_maker->obj_arc.touch    = 0;
		break;
	}
	case scui_widget_type_obj_led: {
		scui_obj_led_maker_t *obj_led_maker = (scui_obj_led_maker_t *)maker;
		
		obj_led_maker->widget.type  = scui_widget_type_obj_led;
		obj_led_maker->brightness   = 100;
		obj_led_maker->on           = 0;
		break;
	}
	case scui_widget_type_obj_chk: {
		scui_obj_chk_maker_t *obj_chk_maker = (scui_obj_chk_maker_t *)maker;
		
		obj_chk_maker->widget.type             = scui_widget_type_obj_chk;
		obj_chk_maker->obj_btn.check           = 1;
		obj_chk_maker->font                    = SCUI_FONT_IDX_X24;
		obj_chk_maker->lang                    = scui_lang_type_symbol;
		obj_chk_maker->sym_chk                 = "\xEF\x80\x8C";
		obj_chk_maker->sym_color.color_s.full  = 0xFFFFFFFF;
		obj_chk_maker->sym_color.color_e.full  = 0xFFFFFFFF;
		break;
	}
	case scui_widget_type_obj_bmat: {
		scui_obj_bmat_maker_t *obj_bmat_maker = (scui_obj_bmat_maker_t *)maker;
		
		obj_bmat_maker->widget.type  = scui_widget_type_obj_bmat;
		obj_bmat_maker->row_num      = 1;
		obj_bmat_maker->gap.x        = 10;
		obj_bmat_maker->gap.y        = 10;
		break;
	}
	case scui_widget_type_obj_line: {
		scui_obj_line_maker_t *obj_line_maker = (scui_obj_line_maker_t *)maker;
		
		obj_line_maker->widget.type  = scui_widget_type_obj_line;
		obj_line_maker->mode         = 0;
		break;
	}
	default:
		break;
	}
}
