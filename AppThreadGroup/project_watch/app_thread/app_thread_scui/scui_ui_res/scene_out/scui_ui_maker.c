/*一个通过scui_widget_parser脚本生成的widget动态构造器配置
 */

#include "scui.h"

/*@brief 控件构造器默认初始化
 *@param maker 控件构造器实例指针
 *@param type  控件类型(scui_widget_type_t)
 */
void scui_widget_maker_def_cfg(void *maker, scui_widget_type_t type)
{
	switch (type) {
	case scui_widget_type_window: {
		scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
		
		window_maker->widget.type                    = scui_widget_type_window;
		window_maker->widget.style.fully_bg          = true;
		window_maker->buffer                         = true;
		window_maker->widget.clip.w                  = SCUI_HOR_RES;
		window_maker->widget.clip.h                  = SCUI_VER_RES;
		window_maker->level                          = 0;
		window_maker->switch_enc                     = scui_opt_pos_all;
		window_maker->switch_key                     = scui_opt_pos_all;
		window_maker->switch_enc_way                 = scui_opt_dir_ver;
		window_maker->switch_key_id[0]               = scui_event_key_val_down;
		window_maker->switch_key_id[1]               = scui_event_key_val_up;
		window_maker->switch_key_id[2]               = scui_event_key_val_right;
		window_maker->switch_key_id[3]               = scui_event_key_val_left;
		break;
	}
	case scui_widget_type_custom: {
		scui_custom_maker_t *custom_maker = (scui_custom_maker_t *)maker;
		
		custom_maker->widget.type                    = scui_widget_type_custom;
		custom_maker->widget.style.sched_anima       = true;
		break;
	}
	case scui_widget_type_scroll: {
		scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
		
		scroll_maker->widget.type                    = scui_widget_type_scroll;
		scroll_maker->widget.clip.w                  = SCUI_HOR_RES;
		scroll_maker->widget.clip.h                  = SCUI_VER_RES;
		scroll_maker->widget.style.indev_enc         = true;
		scroll_maker->widget.style.indev_key         = true;
		scroll_maker->keyid_fdir                     = SCUI_WIDGET_SCROLL_KEY_FDIR;
		scroll_maker->keyid_bdir                     = SCUI_WIDGET_SCROLL_KEY_BDIR;
		scroll_maker->pos                            = scui_opt_pos_c;
		scroll_maker->dir                            = scui_opt_dir_ver;
		break;
	}
	case scui_widget_type_canvas: {
		scui_canvas_maker_t *canvas_maker = (scui_canvas_maker_t *)maker;
		
		canvas_maker->widget.type                    = scui_widget_type_canvas;
		break;
	}
	case scui_widget_type_string: {
		scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
		
		string_maker->widget.type                    = scui_widget_type_string;
		break;
	}
	case scui_widget_type_linear: {
		scui_linear_maker_t *linear_maker = (scui_linear_maker_t *)maker;
		
		linear_maker->widget.type                    = scui_widget_type_linear;
		break;
	}
	case scui_widget_type_linear_m: {
		scui_linear_m_maker_t *linear_m_maker = (scui_linear_m_maker_t *)maker;
		
		linear_m_maker->widget.type                    = scui_widget_type_linear_m;
		break;
	}
	case scui_widget_type_linear_s: {
		scui_linear_s_maker_t *linear_s_maker = (scui_linear_s_maker_t *)maker;
		
		linear_s_maker->widget.type                    = scui_widget_type_linear_s;
		break;
	}
	case scui_widget_type_roller: {
		scui_roller_maker_t *roller_maker = (scui_roller_maker_t *)maker;
		
		roller_maker->widget.type                    = scui_widget_type_roller;
		break;
	}
	case scui_widget_type_xvedio: {
		scui_xvedio_maker_t *xvedio_maker = (scui_xvedio_maker_t *)maker;
		
		xvedio_maker->widget.type                    = scui_widget_type_xvedio;
		break;
	}
	case scui_widget_type_xwatch: {
		scui_xwatch_maker_t *xwatch_maker = (scui_xwatch_maker_t *)maker;
		
		xwatch_maker->widget.type                    = scui_widget_type_xwatch;
		break;
	}
	case scui_widget_type_menial: {
		scui_menial_maker_t *menial_maker = (scui_menial_maker_t *)maker;
		
		menial_maker->widget.type                    = scui_widget_type_menial;
		break;
	}
	case scui_widget_type_button: {
		scui_button_maker_t *button_maker = (scui_button_maker_t *)maker;
		
		button_maker->widget.type                    = scui_widget_type_button;
		break;
	}
	case scui_widget_type_xchart: {
		scui_xchart_maker_t *xchart_maker = (scui_xchart_maker_t *)maker;
		
		xchart_maker->widget.type                    = scui_widget_type_xchart;
		break;
	}
	default:
		break;
	}
}
