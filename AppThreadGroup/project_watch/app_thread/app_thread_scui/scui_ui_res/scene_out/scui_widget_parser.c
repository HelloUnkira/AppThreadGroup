/*一个通过scui_widget_parser脚本生成的widget表
 */

#include "scui.h"

#if defined(SCUI_WIDGET_PARSER_EVENT_CB_EMPTY) && SCUI_WIDGET_PARSER_EVENT_CB_EMPTY == 1
static void scui_widget_parser_event_cb_empty(scui_event_t *event)
{
}
#define scui_ui_scene_butterfly_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_butterfly_custom_event_proc              scui_widget_parser_event_cb_empty
#define scui_ui_scene_cube_event_proc                          scui_widget_parser_event_cb_empty
#define scui_ui_scene_cube_custom_event_proc                   scui_widget_parser_event_cb_empty
#define scui_ui_scene_lantern_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_lantern_custom_event_proc                scui_widget_parser_event_cb_empty
#define scui_ui_scene_soccer_event_proc                        scui_widget_parser_event_cb_empty
#define scui_ui_scene_soccer_custom_event_proc                 scui_widget_parser_event_cb_empty
#define scui_ui_scene_honeycomb_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_honeycomb_scroll_event_proc              scui_widget_parser_event_cb_empty
#define scui_ui_scene_list_arc_event_proc                      scui_widget_parser_event_cb_empty
#define scui_ui_scene_list_arc_scroll_event                    scui_widget_parser_event_cb_empty
#define scui_ui_scene_list_arc_bar_arc_event                   scui_widget_parser_event_cb_empty
#define scui_ui_mask_all_event_proc                            scui_widget_parser_event_cb_empty
#define scui_ui_scene_list_scale_event_proc                    scui_widget_parser_event_cb_empty
#define scui_ui_scene_list_scale_bar_arc_event                 scui_widget_parser_event_cb_empty
#define scui_ui_scene_spread_event_proc                        scui_widget_parser_event_cb_empty
#define scui_ui_scene_spread_custom_event_proc                 scui_widget_parser_event_cb_empty
#define scui_ui_scene_thumbwheel_event_proc                    scui_widget_parser_event_cb_empty
#define scui_ui_scene_thumbwheel_custom_event_proc             scui_widget_parser_event_cb_empty
#define scui_ui_scene_waterfall_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_waterfall_scroll_event                   scui_widget_parser_event_cb_empty
#define scui_ui_scene_waterfall_bar_arc_event                  scui_widget_parser_event_cb_empty
#define scui_ui_scene_popup_event_proc                         scui_widget_parser_event_cb_empty
#define scui_ui_scene_popup_bg_event_proc                      scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_event_proc                          scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_1_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_2_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_3_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_4_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_5_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_6_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_7_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_8_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_graph_9_event_proc                        scui_widget_parser_event_cb_empty
#define scui_test_ui_icon_event_proc                           scui_widget_parser_event_cb_empty
#define scui_test_ui_list_item_event_proc                      scui_widget_parser_event_cb_empty
#define scui_test_ui_main_home_event_proc                      scui_widget_parser_event_cb_empty
#define scui_test_ui_main_test_event_proc                      scui_widget_parser_event_cb_empty
#define scui_test_ui_object_event_proc                         scui_widget_parser_event_cb_empty
#define scui_test_ui_object_scroll_event_proc                  scui_widget_parser_event_cb_empty
#define scui_test_ui_object_page_1_event_proc                  scui_widget_parser_event_cb_empty
#define scui_test_ui_object_page_2_event_proc                  scui_widget_parser_event_cb_empty
#define scui_test_ui_object_page_3_event_proc                  scui_widget_parser_event_cb_empty
#define scui_test_ui_scroll_event_proc                         scui_widget_parser_event_cb_empty
#define scui_test_ui_string_event_proc                         scui_widget_parser_event_cb_empty
#define scui_test_ui_symbol_canvas_event_proc                  scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_event_proc                      scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_event                    scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_bar_arc_event_proc              scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ring_big_3_event_proc    scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ditail_title_event_proc  scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ditail_sum_event_proc    scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ditail_kcal_event_proc   scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ditail_step_event_proc   scui_widget_parser_event_cb_empty
#define scui_ui_scene_activity_scroll_ditail_dist_event_proc   scui_widget_parser_event_cb_empty
#define scui_ui_scene_home_event_proc                          scui_widget_parser_event_cb_empty
#define scui_ui_scene_mini_card_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_mini_card_bar_arc_event                  scui_widget_parser_event_cb_empty
#define scui_ui_scene_notify_event_proc                        scui_widget_parser_event_cb_empty
#define scui_ui_scene_quick_card_event_proc                    scui_widget_parser_event_cb_empty
#define scui_ui_scene_standby_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_1_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_1_1_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_1_2_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_2_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_3_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_3_ring_event_proc                  scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_3_btn_event_proc                   scui_widget_parser_event_cb_empty
#define scui_ui_scene_float_4_event_proc                       scui_widget_parser_event_cb_empty
#define scui_ui_scene_1_event_proc                             scui_widget_parser_event_cb_empty
#define scui_ui_scene_1_scroll_event                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_1_scroll_page_1_event_proc               scui_widget_parser_event_cb_empty
#define scui_ui_scene_1_scroll_page_2_event_proc               scui_widget_parser_event_cb_empty
#define scui_ui_scene_1_scroll_page_3_event_proc               scui_widget_parser_event_cb_empty
#define scui_ui_scene_2_event_proc                             scui_widget_parser_event_cb_empty
#define scui_ui_scene_2_bar_arc_event_proc                     scui_widget_parser_event_cb_empty
#define scui_ui_scene_3_event_proc                             scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_event_proc                             scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_1_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_2_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_3_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_4_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_5_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_6_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_7_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_8_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_6_9_event_proc                           scui_widget_parser_event_cb_empty
#define scui_ui_scene_test_event_proc                          scui_widget_parser_event_cb_empty
#else
extern void scui_ui_scene_butterfly_event_proc(scui_event_t *event);
extern void scui_ui_scene_butterfly_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_cube_event_proc(scui_event_t *event);
extern void scui_ui_scene_cube_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_lantern_event_proc(scui_event_t *event);
extern void scui_ui_scene_lantern_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_soccer_event_proc(scui_event_t *event);
extern void scui_ui_scene_soccer_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_honeycomb_event_proc(scui_event_t *event);
extern void scui_ui_scene_honeycomb_scroll_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_arc_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_arc_scroll_event(scui_event_t *event);
extern void scui_ui_scene_list_arc_bar_arc_event(scui_event_t *event);
extern void scui_ui_mask_all_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_scale_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event);
extern void scui_ui_scene_spread_event_proc(scui_event_t *event);
extern void scui_ui_scene_spread_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_thumbwheel_event_proc(scui_event_t *event);
extern void scui_ui_scene_thumbwheel_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_waterfall_event_proc(scui_event_t *event);
extern void scui_ui_scene_waterfall_scroll_event(scui_event_t *event);
extern void scui_ui_scene_waterfall_bar_arc_event(scui_event_t *event);
extern void scui_ui_scene_popup_event_proc(scui_event_t *event);
extern void scui_ui_scene_popup_bg_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_1_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_2_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_3_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_4_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_5_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_6_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_7_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_8_event_proc(scui_event_t *event);
extern void scui_test_ui_graph_9_event_proc(scui_event_t *event);
extern void scui_test_ui_icon_event_proc(scui_event_t *event);
extern void scui_test_ui_list_item_event_proc(scui_event_t *event);
extern void scui_test_ui_main_home_event_proc(scui_event_t *event);
extern void scui_test_ui_main_test_event_proc(scui_event_t *event);
extern void scui_test_ui_object_event_proc(scui_event_t *event);
extern void scui_test_ui_object_scroll_event_proc(scui_event_t *event);
extern void scui_test_ui_object_page_1_event_proc(scui_event_t *event);
extern void scui_test_ui_object_page_2_event_proc(scui_event_t *event);
extern void scui_test_ui_object_page_3_event_proc(scui_event_t *event);
extern void scui_test_ui_scroll_event_proc(scui_event_t *event);
extern void scui_test_ui_string_event_proc(scui_event_t *event);
extern void scui_test_ui_symbol_canvas_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_event(scui_event_t *event);
extern void scui_ui_scene_activity_bar_arc_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ring_big_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ditail_title_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ditail_sum_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ditail_kcal_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ditail_step_event_proc(scui_event_t *event);
extern void scui_ui_scene_activity_scroll_ditail_dist_event_proc(scui_event_t *event);
extern void scui_ui_scene_home_event_proc(scui_event_t *event);
extern void scui_ui_scene_mini_card_event_proc(scui_event_t *event);
extern void scui_ui_scene_mini_card_bar_arc_event(scui_event_t *event);
extern void scui_ui_scene_notify_event_proc(scui_event_t *event);
extern void scui_ui_scene_quick_card_event_proc(scui_event_t *event);
extern void scui_ui_scene_standby_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_3_ring_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_3_btn_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_4_event_proc(scui_event_t *event);
extern void scui_ui_scene_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_1_scroll_event(scui_event_t *event);
extern void scui_ui_scene_1_scroll_page_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_1_scroll_page_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_1_scroll_page_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_2_bar_arc_event_proc(scui_event_t *event);
extern void scui_ui_scene_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_4_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_5_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_6_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_7_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_8_event_proc(scui_event_t *event);
extern void scui_ui_scene_6_9_event_proc(scui_event_t *event);
extern void scui_ui_scene_test_event_proc(scui_event_t *event);
#endif

static void scui_widget_json_widget_child_num(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->child_num = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_widget_clip_h(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->clip.h = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_widget_clip_w(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->clip.w = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_widget_clip_x(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->clip.x = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_widget_clip_y(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->clip.y = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_widget_color_color_full(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->color.color.full = ((scui_widget_json_val_t *)field)->color;
}

static void scui_widget_json_widget_event_cb(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->event_cb = ((scui_widget_json_val_t *)field)->event;
}

static void scui_widget_json_widget_format(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->format = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_widget_image(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->image = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_widget_myself(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->myself = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_widget_parent(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->parent = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_widget_style_buffer(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.buffer = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_style_fully_bg(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.fully_bg = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_style_indev_enc(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.indev_enc = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_style_indev_key(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.indev_key = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_style_indev_ptr(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.indev_ptr = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_style_sched_anima(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->style.sched_anima = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_widget_type(void *maker, void *field)
{
	scui_widget_maker_t *widget_maker = (scui_widget_maker_t *)maker;
	widget_maker->type = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_scroll_dir(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->dir = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_scroll_fling_page(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->fling_page = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_scroll_freedom(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->freedom = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_scroll_loop(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->loop = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_scroll_pos(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->pos = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_scroll_route_enc(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->route_enc = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_scroll_route_key(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->route_key = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_scroll_skip(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->skip = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_scroll_space(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->space = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_scroll_springback(void *maker, void *field)
{
	scui_scroll_maker_t *scroll_maker = (scui_scroll_maker_t *)maker;
	scroll_maker->springback = ((scui_widget_json_val_t *)field)->coord;
}

static void scui_widget_json_string_args_align_hor(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.align_hor = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_string_args_align_ver(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.align_ver = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_string_args_color_color_e_full(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.color.color_e.full = ((scui_widget_json_val_t *)field)->color;
}

static void scui_widget_json_string_args_color_color_s_full(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.color.color_s.full = ((scui_widget_json_val_t *)field)->color;
}

static void scui_widget_json_string_args_color_filter(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.color.filter = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_string_args_lang(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->args.lang = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_string_font_idx(void *maker, void *field)
{
	scui_string_maker_t *string_maker = (scui_string_maker_t *)maker;
	string_maker->font_idx = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_level(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->level = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_window_preload(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->preload = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_window_resident(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->resident = ((scui_widget_json_val_t *)field)->sbitfd;
}

static void scui_widget_json_window_sibling_0(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->sibling[0] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_sibling_1(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->sibling[1] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_sibling_2(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->sibling[2] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_sibling_3(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->sibling[3] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_switch_type_0(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->switch_type[0] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_switch_type_1(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->switch_type[1] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_switch_type_2(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->switch_type[2] = ((scui_widget_json_val_t *)field)->handle;
}

static void scui_widget_json_window_switch_type_3(void *maker, void *field)
{
	scui_window_maker_t *window_maker = (scui_window_maker_t *)maker;
	window_maker->switch_type[3] = ((scui_widget_json_val_t *)field)->handle;
}

static void (*const scui_widget_SCUI_UI_SCENE_BUTTERFLY_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_1,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_BUTTERFLY_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_butterfly_event_proc, },
	{ .handle = SCUI_UI_SCENE_BUTTERFLY, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_BUTTERFLY_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_BUTTERFLY_cfg),
	.val = scui_widget_SCUI_UI_SCENE_BUTTERFLY_val,
	.cfg = scui_widget_SCUI_UI_SCENE_BUTTERFLY_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_BUTTERFLY, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_butterfly_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_BUTTERFLY_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_CUBE_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_CUBE_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_1, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_cube_event_proc, },
	{ .handle = SCUI_UI_SCENE_CUBE, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_CUBE_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_CUBE_cfg),
	.val = scui_widget_SCUI_UI_SCENE_CUBE_val,
	.cfg = scui_widget_SCUI_UI_SCENE_CUBE_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_CUBE, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_cube_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_CUBE_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LANTERN_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LANTERN_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_lantern_event_proc, },
	{ .handle = SCUI_UI_SCENE_LANTERN, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LANTERN_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LANTERN_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LANTERN_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LANTERN_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_LANTERN, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_lantern_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_LANTERN_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_SOCCER_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_SOCCER_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_soccer_event_proc, },
	{ .handle = SCUI_UI_SCENE_SOCCER, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_SOCCER_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_SOCCER_cfg),
	.val = scui_widget_SCUI_UI_SCENE_SOCCER_val,
	.cfg = scui_widget_SCUI_UI_SCENE_SOCCER_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_SOCCER, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_soccer_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_SOCCER_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_HONEYCOMB_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_HONEYCOMB_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_ui_scene_honeycomb_event_proc, },
	{ .handle = SCUI_UI_SCENE_HONEYCOMB, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_HONEYCOMB_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_HONEYCOMB_cfg),
	.val = scui_widget_SCUI_UI_SCENE_HONEYCOMB_val,
	.cfg = scui_widget_SCUI_UI_SCENE_HONEYCOMB_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_dir,
	scui_widget_json_scroll_freedom,
	scui_widget_json_scroll_skip,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_HONEYCOMB, },
	{ .handle = 0, },
	{ .handle = scui_opt_dir_all, },
	{ .sbitfd = true, },
	{ .handle = scui_opt_pos_all, },
	{ .coord = 35, },
	{ .handle = 50, },
	{ .event = scui_ui_scene_honeycomb_scroll_event_proc, },
	{ .handle = SCUI_UI_SCENE_HONEYCOMB_SCROLL, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_ARC_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_ui_scene_list_arc_event_proc, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 3, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_space,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC, },
	{ .handle = 0, },
	{ .coord = 80, },
	{ .coord = 80, },
	{ .coord = 10, },
	{ .coord = 70, },
	{ .handle = 50, },
	{ .event = scui_ui_scene_list_arc_scroll_event, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC_SCROLL, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_list_arc_bar_arc_event, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_mask_all_event_proc, },
	{ .handle = SCUI_UI_SCENE_LIST_ARC_MASK, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_SCALE_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_list_scale_event_proc, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_SCALE_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_SCALE_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_SCALE_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_list_scale_bar_arc_event, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_mask_all_event_proc, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE_MASK, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_cfg),
	.val = scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_val,
	.cfg = scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_SPREAD_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_SPREAD_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_ui_scene_spread_event_proc, },
	{ .handle = SCUI_UI_SCENE_SPREAD, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_SPREAD_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_SPREAD_cfg),
	.val = scui_widget_SCUI_UI_SCENE_SPREAD_val,
	.cfg = scui_widget_SCUI_UI_SCENE_SPREAD_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_SPREAD, },
	{ .handle = 0, },
	{ .handle = 3, },
	{ .coord = SCUI_VER_RES - 10 * 2, },
	{ .coord = SCUI_HOR_RES - 10 * 2, },
	{ .coord = 10, },
	{ .coord = 10, },
	{ .event = scui_ui_scene_spread_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_SPREAD_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_THUMBWHEEL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_ui_scene_thumbwheel_event_proc, },
	{ .handle = SCUI_UI_SCENE_THUMBWHEEL, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_THUMBWHEEL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_THUMBWHEEL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_THUMBWHEEL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_THUMBWHEEL, },
	{ .handle = 0, },
	{ .handle = 3, },
	{ .coord = SCUI_VER_RES - 5 * 2, },
	{ .coord = SCUI_HOR_RES - 5 * 2, },
	{ .coord = 5, },
	{ .coord = 5, },
	{ .event = scui_ui_scene_thumbwheel_custom_event_proc, },
	{ .handle = SCUI_UI_SCENE_THUMBWHEEL_CUSTOM, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_WATERFALL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_WATERFALL_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_ui_scene_waterfall_event_proc, },
	{ .handle = SCUI_UI_SCENE_WATERFALL, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_WATERFALL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_WATERFALL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_WATERFALL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_WATERFALL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_freedom,
	scui_widget_json_scroll_pos,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_WATERFALL, },
	{ .handle = 0, },
	{ .sbitfd = true, },
	{ .handle = scui_opt_pos_all, },
	{ .coord = 117, },
	{ .coord = 117, },
	{ .coord = 35, },
	{ .handle = 50, },
	{ .event = scui_ui_scene_waterfall_scroll_event, },
	{ .handle = SCUI_UI_SCENE_WATERFALL_SCROLL, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_WATERFALL, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_waterfall_bar_arc_event, },
	{ .handle = SCUI_UI_SCENE_WATERFALL_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_POPUP_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_level,
	scui_widget_json_window_resident,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_format,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_POPUP_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .sbitfd = 10, },
	{ .sbitfd = true, },
	{ .handle = 5, },
	{ .coord = 100, },
	{ .coord = 350, },
	{ .coord = 58, },
	{ .coord = 60, },
	{ .event = scui_ui_scene_popup_event_proc, },
	{ .handle = SCUI_PIXEL_CF_DEF_A, },
	{ .handle = SCUI_UI_SCENE_POPUP, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_POPUP_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_POPUP_cfg),
	.val = scui_widget_SCUI_UI_SCENE_POPUP_val,
	.cfg = scui_widget_SCUI_UI_SCENE_POPUP_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_POPUP_SCALE_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_POPUP_SCALE_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_POPUP, },
	{ .handle = 0, },
	{ .coord = 100, },
	{ .coord = 350, },
	{ .handle = SCUI_UI_SCENE_POPUP_SCALE, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_POPUP_SCALE_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_POPUP_SCALE_cfg),
	.val = scui_widget_SCUI_UI_SCENE_POPUP_SCALE_val,
	.cfg = scui_widget_SCUI_UI_SCENE_POPUP_SCALE_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_POPUP_BG_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_POPUP_BG_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_POPUP_SCALE, },
	{ .handle = 0, },
	{ .handle = 5, },
	{ .coord = 100, },
	{ .coord = 350, },
	{ .event = scui_ui_scene_popup_bg_event_proc, },
	{ .handle = SCUI_UI_SCENE_POPUP_BG, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_POPUP_BG_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_POPUP_BG_cfg),
	.val = scui_widget_SCUI_UI_SCENE_POPUP_BG_val,
	.cfg = scui_widget_SCUI_UI_SCENE_POPUP_BG_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_test_ui_graph_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 9, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_1, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_2, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_3, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_4_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_4, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_5_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_5, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_6_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_6, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_7_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_7, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_8_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_8, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_test_ui_graph_9_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_GRAPH_9, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 10, },
	{ .color = 0xFF101010, },
	{ .event = scui_test_ui_icon_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_ICON, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .color = 0xFF101010, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_dir,
	scui_widget_json_scroll_fling_page,
	scui_widget_json_scroll_loop,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_skip,
	scui_widget_json_scroll_space,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST, },
	{ .handle = 0, },
	{ .handle = scui_opt_dir_ver, },
	{ .coord = 5, },
	{ .sbitfd = false, },
	{ .coord = 117, },
	{ .coord = 117, },
	{ .handle = scui_opt_pos_all, },
	{ .coord = 10, },
	{ .coord = 70, },
	{ .handle = 20, },
	{ .coord = SCUI_VER_RES * 707 / 1000, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .coord = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2, },
	{ .coord = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2, },
	{ .color = 0xFF6F6F6F, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_SCROLL, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFFFFFFFF, },
	{ .color = 0xFFFFFFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = -1, },
	{ .coord = SCUI_HOR_RES * 707 / 1000, },
	{ .color = 0xFF202020, },
	{ .event = scui_test_ui_list_item_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .color = 0xFF101010, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_MAIN, },
	{ .handle = 2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_MAIN, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFF00FF00, },
	{ .color = 0xFF00FF00, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = 60, },
	{ .coord = 200, },
	{ .coord = SCUI_HOR_RES / 2 - 100, },
	{ .coord = SCUI_VER_RES / 4 - 30, },
	{ .color = 0xFF404040, },
	{ .event = scui_test_ui_main_home_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_MAIN_HOME, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_string_args_align_hor,
	scui_widget_json_string_args_align_ver,
	scui_widget_json_string_args_color_color_e_full,
	scui_widget_json_string_args_color_color_s_full,
	scui_widget_json_string_args_color_filter,
	scui_widget_json_string_args_lang,
	scui_widget_json_string_font_idx,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_val[] = {
	{ .handle = scui_widget_type_string, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_MAIN, },
	{ .handle = 0, },
	{ .sbitfd = 2, },
	{ .sbitfd = 2, },
	{ .color = 0xFF00FFFF, },
	{ .color = 0xFF00FFFF, },
	{ .sbitfd = true, },
	{ .handle = scui_lang_type_en, },
	{ .handle = SCUI_FONT_IDX_36, },
	{ .coord = 60, },
	{ .coord = 200, },
	{ .coord = SCUI_HOR_RES / 2 - 100, },
	{ .coord = SCUI_VER_RES * 3 / 4 - 30, },
	{ .color = 0xFF404040, },
	{ .event = scui_test_ui_main_test_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_MAIN_TEST, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .event = scui_test_ui_object_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT, },
	{ .sbitfd = true, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_skip,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_VER_RES, },
	{ .handle = scui_opt_pos_all, },
	{ .coord = 70, },
	{ .event = scui_test_ui_object_scroll_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL, },
	{ .handle = 3, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_test_ui_object_page_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_test_ui_object_page_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_test_ui_object_page_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 10, },
	{ .color = 0xFF101010, },
	{ .event = scui_test_ui_scroll_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_SCROLL, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 20, },
	{ .color = 0xFF101010, },
	{ .event = scui_test_ui_string_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_STRING, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 10, },
	{ .color = 0xFF101010, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_SYMBOL, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_SYMBOL, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .coord = 0, },
	{ .coord = 0, },
	{ .event = scui_test_ui_symbol_canvas_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .handle = SCUI_UI_SCENE_1, },
	{ .event = scui_ui_scene_activity_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_space,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY, },
	{ .handle = 0, },
	{ .coord = SCUI_HOR_RES * 2 / 3, },
	{ .coord = SCUI_HOR_RES * 2 / 3, },
	{ .coord = 10, },
	{ .coord = 70, },
	{ .event = scui_ui_scene_activity_scroll_event, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 7, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_bar_arc_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_buffer,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ring_big_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .handle = 1, },
	{ .coord = 90, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ditail_title_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .coord = 155, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ditail_sum_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .handle = 1, },
	{ .coord = 250, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ditail_kcal_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .handle = 1, },
	{ .coord = 250, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ditail_step_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .handle = 1, },
	{ .coord = 250, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_activity_scroll_ditail_dist_event_proc, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL, },
	{ .handle = 0, },
	{ .coord = 95, },
	{ .coord = SCUI_HOR_RES, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_cfg),
	.val = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_val,
	.cfg = scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_HOME_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_window_switch_type_0,
	scui_widget_json_window_switch_type_1,
	scui_widget_json_window_switch_type_2,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_HOME_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_NOTIFY, },
	{ .handle = SCUI_UI_SCENE_QUICK_CARD, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_home_event_proc, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_HOME_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_HOME_cfg),
	.val = scui_widget_SCUI_UI_SCENE_HOME_val,
	.cfg = scui_widget_SCUI_UI_SCENE_HOME_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_MINI_CARD_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_3,
	scui_widget_json_window_switch_type_3,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_MINI_CARD_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_mini_card_event_proc, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_MINI_CARD_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_MINI_CARD_cfg),
	.val = scui_widget_SCUI_UI_SCENE_MINI_CARD_val,
	.cfg = scui_widget_SCUI_UI_SCENE_MINI_CARD_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_mini_card_bar_arc_event, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_mask_all_event_proc, },
	{ .handle = SCUI_UI_SCENE_MINI_CARD_MASK, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_cfg),
	.val = scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_val,
	.cfg = scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_NONE_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_NONE_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_NONE, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_NONE_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_NONE_cfg),
	.val = scui_widget_SCUI_UI_SCENE_NONE_val,
	.cfg = scui_widget_SCUI_UI_SCENE_NONE_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_NOTIFY_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_switch_type_1,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_NOTIFY_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_notify_event_proc, },
	{ .handle = SCUI_UI_SCENE_NOTIFY, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_NOTIFY_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_NOTIFY_cfg),
	.val = scui_widget_SCUI_UI_SCENE_NOTIFY_val,
	.cfg = scui_widget_SCUI_UI_SCENE_NOTIFY_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_QUICK_CARD_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_switch_type_0,
	scui_widget_json_window_switch_type_1,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_QUICK_CARD_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .handle = SCUI_UI_SCENE_LIST_SCALE, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = scui_window_switch_move, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_quick_card_event_proc, },
	{ .handle = SCUI_UI_SCENE_QUICK_CARD, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_QUICK_CARD_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_QUICK_CARD_cfg),
	.val = scui_widget_SCUI_UI_SCENE_QUICK_CARD_val,
	.cfg = scui_widget_SCUI_UI_SCENE_QUICK_CARD_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_STANDBY_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_STANDBY_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_standby_event_proc, },
	{ .handle = SCUI_UI_SCENE_STANDBY, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_STANDBY_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_STANDBY_cfg),
	.val = scui_widget_SCUI_UI_SCENE_STANDBY_val,
	.cfg = scui_widget_SCUI_UI_SCENE_STANDBY_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_level,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_switch_type_1,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_1_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .handle = scui_window_switch_cover_out, },
	{ .event = scui_ui_scene_float_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .handle = 2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_1_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_1_1_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1, },
	{ .handle = 0, },
	{ .handle = 2, },
	{ .coord = SCUI_VER_RES / 2, },
	{ .coord = SCUI_HOR_RES / 2, },
	{ .coord = SCUI_HOR_RES / 4, },
	{ .coord = SCUI_VER_RES * 1 / 8, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_float_1_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1_1, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_1_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_1_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_1_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_1_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_1_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_1_2_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1, },
	{ .handle = 0, },
	{ .handle = 2, },
	{ .coord = SCUI_VER_RES / 4, },
	{ .coord = SCUI_HOR_RES - 30, },
	{ .coord = 15, },
	{ .coord = SCUI_VER_RES * 11 / 16, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_float_1_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1_2, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_1_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_1_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_1_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_1_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_level,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_switch_type_0,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_2_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = 20, },
	{ .event = scui_ui_scene_float_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_2, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_level,
	scui_widget_json_window_sibling_3,
	scui_widget_json_window_switch_type_3,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_3_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_float_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_float_3_ring_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3_RING, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3, },
	{ .handle = 0, },
	{ .handle = 10, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_float_3_btn_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3_BTN, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_FLOAT_4_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_level,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_switch_type_2,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_FLOAT_4_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .handle = scui_window_switch_cover_out, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_float_4_event_proc, },
	{ .handle = SCUI_UI_SCENE_FLOAT_4, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_FLOAT_4_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_FLOAT_4_cfg),
	.val = scui_widget_SCUI_UI_SCENE_FLOAT_4_val,
	.cfg = scui_widget_SCUI_UI_SCENE_FLOAT_4_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_1_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .handle = SCUI_UI_SCENE_CUBE, },
	{ .handle = SCUI_UI_SCENE_ACTIVITY, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .event = scui_ui_scene_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_1, },
	{ .handle = 1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_1_SCROLL_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_scroll_route_enc,
	scui_widget_json_scroll_route_key,
	scui_widget_json_scroll_skip,
	scui_widget_json_scroll_springback,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_1_SCROLL_val[] = {
	{ .handle = scui_widget_type_scroll, },
	{ .handle = SCUI_UI_SCENE_1, },
	{ .handle = 0, },
	{ .coord = SCUI_HOR_RES * 2 / 3, },
	{ .coord = SCUI_HOR_RES * 2 / 3, },
	{ .handle = scui_opt_pos_all, },
	{ .coord = 70, },
	{ .event = scui_ui_scene_1_scroll_event, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL, },
	{ .handle = 3, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_1_SCROLL_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_1_SCROLL_cfg),
	.val = scui_widget_SCUI_UI_SCENE_1_SCROLL_val,
	.cfg = scui_widget_SCUI_UI_SCENE_1_SCROLL_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_1_scroll_page_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL_PAGE_1, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_1_scroll_page_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL_PAGE_2, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL, },
	{ .handle = 0, },
	{ .handle = 50, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_1_scroll_page_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_1_SCROLL_PAGE_3, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_image,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_2_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_BUTTERFLY, },
	{ .handle = SCUI_UI_SCENE_SOCCER, },
	{ .handle = SCUI_UI_SCENE_1, },
	{ .handle = SCUI_UI_SCENE_3, },
	{ .handle = 5, },
	{ .event = scui_ui_scene_2_event_proc, },
	{ .handle = scui_image_prj_watchface_bg_01_bgjpg, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_2_BAR_ARC_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_2_BAR_ARC_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES, },
	{ .coord = SCUI_HOR_RES, },
	{ .event = scui_ui_scene_2_bar_arc_event_proc, },
	{ .handle = SCUI_UI_SCENE_2_BAR_ARC, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_2_BAR_ARC_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_2_BAR_ARC_cfg),
	.val = scui_widget_SCUI_UI_SCENE_2_BAR_ARC_val,
	.cfg = scui_widget_SCUI_UI_SCENE_2_BAR_ARC_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_image,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_enc,
	scui_widget_json_widget_style_indev_key,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_3_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_2, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .event = scui_ui_scene_3_event_proc, },
	{ .handle = scui_image_prj_watchface_bg_01_bgjpg, },
	{ .handle = SCUI_UI_SCENE_3, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_image,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_child_num,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 1, },
	{ .handle = SCUI_UI_SCENE_3, },
	{ .handle = SCUI_UI_SCENE_HOME, },
	{ .event = scui_ui_scene_6_event_proc, },
	{ .handle = scui_image_prj_watchface_bg_01_bgjpg, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 9, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_1_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_1_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_1_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_1, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_1_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_1_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_1_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_1_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_2_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_2_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_2_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_2, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_2_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_2_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_2_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_2_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_3_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_3_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 1 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_3_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_3, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_3_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_3_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_3_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_3_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_4_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_4_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_4_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_4, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_4_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_4_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_4_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_4_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_5_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_5_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_5_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_5, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_5_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_5_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_5_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_5_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_6_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_6_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 5 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_6_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_6, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_6_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_6_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_6_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_6_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_7_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_7_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 1 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_7_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_7, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_7_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_7_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_7_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_7_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_8_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_8_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 5 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_8_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_8, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_8_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_8_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_8_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_8_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_6_9_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	NULL,
	scui_widget_json_widget_clip_h,
	scui_widget_json_widget_clip_w,
	scui_widget_json_widget_clip_x,
	scui_widget_json_widget_clip_y,
	scui_widget_json_widget_color_color_full,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_fully_bg,
	scui_widget_json_widget_style_indev_ptr,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_6_9_val[] = {
	{ .handle = scui_widget_type_custom, },
	{ .handle = SCUI_UI_SCENE_6, },
	{ .handle = 0, },
	{ .coord = SCUI_VER_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 3 / 13, },
	{ .coord = SCUI_HOR_RES * 9 / 13, },
	{ .coord = SCUI_VER_RES * 9 / 13, },
	{ .color = 0xFFFFFFFF, },
	{ .event = scui_ui_scene_6_9_event_proc, },
	{ .handle = SCUI_UI_SCENE_6_9, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_6_9_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_6_9_cfg),
	.val = scui_widget_SCUI_UI_SCENE_6_9_val,
	.cfg = scui_widget_SCUI_UI_SCENE_6_9_cfg,
};

static void (*const scui_widget_SCUI_UI_SCENE_TEST_cfg[])(void *maker, void *field) = {
	scui_widget_json_widget_type,
	scui_widget_json_widget_parent,
	scui_widget_json_window_preload,
	scui_widget_json_window_sibling_0,
	scui_widget_json_window_sibling_1,
	scui_widget_json_window_sibling_2,
	scui_widget_json_window_sibling_3,
	scui_widget_json_window_switch_type_0,
	scui_widget_json_window_switch_type_1,
	scui_widget_json_window_switch_type_2,
	scui_widget_json_window_switch_type_3,
	scui_widget_json_widget_child_num,
	scui_widget_json_widget_event_cb,
	scui_widget_json_widget_myself,
	scui_widget_json_widget_style_indev_ptr,
	scui_widget_json_widget_style_sched_anima,
};

static const scui_widget_json_val_t scui_widget_SCUI_UI_SCENE_TEST_val[] = {
	{ .handle = scui_widget_type_window, },
	{ .handle = SCUI_HANDLE_INVALID, },
	{ .sbitfd = 0, },
	{ .handle = SCUI_UI_SCENE_FLOAT_1, },
	{ .handle = SCUI_UI_SCENE_FLOAT_2, },
	{ .handle = SCUI_UI_SCENE_FLOAT_3, },
	{ .handle = SCUI_UI_SCENE_FLOAT_4, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = scui_window_switch_cover_in, },
	{ .handle = 10, },
	{ .event = scui_ui_scene_test_event_proc, },
	{ .handle = SCUI_UI_SCENE_TEST, },
	{ .sbitfd = true, },
	{ .sbitfd = true, },
};

const scui_widget_json_key_t scui_widget_SCUI_UI_SCENE_TEST_key = {
	.num = scui_arr_len(scui_widget_SCUI_UI_SCENE_TEST_cfg),
	.val = scui_widget_SCUI_UI_SCENE_TEST_val,
	.cfg = scui_widget_SCUI_UI_SCENE_TEST_cfg,
};

const void * const scui_widget_parser_table[107] = {
	(void *)&scui_widget_SCUI_UI_SCENE_BUTTERFLY_key,
	(void *)&scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_CUBE_key,
	(void *)&scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LANTERN_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_SOCCER_key,
	(void *)&scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_HONEYCOMB_key,
	(void *)&scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK_key,
	(void *)&scui_widget_SCUI_UI_SCENE_SPREAD_key,
	(void *)&scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_THUMBWHEEL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP_key,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP_SCALE_key,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP_BG_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_4_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_5_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_6_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_7_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_8_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_GRAPH_9_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_ICON_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_4_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_5_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_6_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_7_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_8_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_9_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_LIST_ITEM_10_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_HOME_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_MAIN_TEST_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_STRING_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_UI_SYMBOL_CANVAS_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST_key,
	(void *)&scui_widget_SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK_key,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_key,
	(void *)&scui_widget_SCUI_UI_SCENE_MINI_CARD_key,
	(void *)&scui_widget_SCUI_UI_SCENE_MINI_CARD_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_MINI_CARD_MASK_key,
	(void *)&scui_widget_SCUI_UI_SCENE_NONE_key,
	(void *)&scui_widget_SCUI_UI_SCENE_NOTIFY_key,
	(void *)&scui_widget_SCUI_UI_SCENE_QUICK_CARD_key,
	(void *)&scui_widget_SCUI_UI_SCENE_STANDBY_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3_RING_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3_BTN_key,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_4_key,
	(void *)&scui_widget_SCUI_UI_SCENE_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_1_SCROLL_key,
	(void *)&scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_1_SCROLL_PAGE_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_2_BAR_ARC_key,
	(void *)&scui_widget_SCUI_UI_SCENE_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_1_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_2_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_3_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_4_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_5_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_6_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_7_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_8_key,
	(void *)&scui_widget_SCUI_UI_SCENE_6_9_key,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST_key,
};
