/*一个通过scui_widget_parser脚本生成的widget表
 */

#include "scui.h"

#if defined(SCUI_WIDGET_EVENT_USE_EMPTY) && SCUI_WIDGET_EVENT_USE_EMPTY == 1
static void scui_ui_scene_butterfly_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_butterfly_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_cube_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_cube_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_lantern_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_lantern_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_soccer_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_soccer_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_honeycomb_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_list_arc_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_list_arc_scroll_notify_event(scui_event_t *event)
{
}
static void scui_ui_scene_list_arc_bar_arc_event(scui_event_t *event)
{
}
static void scui_ui_mask_all_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_list_scale_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_list_scale_scroll_notify_event(scui_event_t *event)
{
}
static void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event)
{
}
static void scui_ui_mask_all_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_spread_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_spread_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_thumbwheel_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_thumbwheel_custom_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_waterfall_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_waterfall_scroll_notify_event(scui_event_t *event)
{
}
static void scui_ui_scene_waterfall_bar_arc_event(scui_event_t *event)
{
}
static void scui_ui_scene_monitor_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_popup_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_popup_bg_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_1_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_1_1_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_1_2_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_c_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_1_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_2_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_3_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_4_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_2_5_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_3_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_3_ring_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_float_4_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_1_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_2_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_2_bar_arc_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_3_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_4_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_4_ring_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_5_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_5_ring_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_1_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_2_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_3_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_4_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_5_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_6_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_7_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_8_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_6_9_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_home_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_home_c_event_proc(scui_event_t *event)
{
}
static void scui_ui_scene_test_event_proc(scui_event_t *event)
{
}
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
extern void scui_ui_scene_list_arc_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_arc_scroll_notify_event(scui_event_t *event);
extern void scui_ui_scene_list_arc_bar_arc_event(scui_event_t *event);
extern void scui_ui_mask_all_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_scale_event_proc(scui_event_t *event);
extern void scui_ui_scene_list_scale_scroll_notify_event(scui_event_t *event);
extern void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event);
extern void scui_ui_mask_all_event_proc(scui_event_t *event);
extern void scui_ui_scene_spread_event_proc(scui_event_t *event);
extern void scui_ui_scene_spread_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_thumbwheel_event_proc(scui_event_t *event);
extern void scui_ui_scene_thumbwheel_custom_event_proc(scui_event_t *event);
extern void scui_ui_scene_waterfall_event_proc(scui_event_t *event);
extern void scui_ui_scene_waterfall_scroll_notify_event(scui_event_t *event);
extern void scui_ui_scene_waterfall_bar_arc_event(scui_event_t *event);
extern void scui_ui_scene_monitor_event_proc(scui_event_t *event);
extern void scui_ui_scene_popup_event_proc(scui_event_t *event);
extern void scui_ui_scene_popup_bg_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_1_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_c_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_4_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_2_5_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_3_ring_event_proc(scui_event_t *event);
extern void scui_ui_scene_float_4_event_proc(scui_event_t *event);
extern void scui_ui_scene_1_event_proc(scui_event_t *event);
extern void scui_ui_scene_2_event_proc(scui_event_t *event);
extern void scui_ui_scene_2_bar_arc_event_proc(scui_event_t *event);
extern void scui_ui_scene_3_event_proc(scui_event_t *event);
extern void scui_ui_scene_4_event_proc(scui_event_t *event);
extern void scui_ui_scene_4_ring_event_proc(scui_event_t *event);
extern void scui_ui_scene_5_event_proc(scui_event_t *event);
extern void scui_ui_scene_5_ring_event_proc(scui_event_t *event);
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
extern void scui_ui_scene_home_event_proc(scui_event_t *event);
extern void scui_ui_scene_home_c_event_proc(scui_event_t *event);
extern void scui_ui_scene_test_event_proc(scui_event_t *event);
#endif

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_BUTTERFLY = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_BUTTERFLY,
	.widget.event_cb                = scui_ui_scene_butterfly_event_proc,
	.widget.child_num               = 11,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_BUTTERFLY_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_BUTTERFLY,
	.widget.event_cb                = scui_ui_scene_butterfly_custom_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_CUBE = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_CUBE,
	.widget.event_cb                = scui_ui_scene_cube_event_proc,
	.widget.child_num               = 11,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_CUBE_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_CUBE,
	.widget.event_cb                = scui_ui_scene_cube_custom_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_LANTERN = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LANTERN,
	.widget.event_cb                = scui_ui_scene_lantern_event_proc,
	.widget.child_num               = 11,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LANTERN_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_LANTERN,
	.widget.event_cb                = scui_ui_scene_lantern_custom_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_SOCCER = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_SOCCER,
	.widget.event_cb                = scui_ui_scene_soccer_event_proc,
	.widget.child_num               = 11,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_SOCCER_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_SOCCER,
	.widget.event_cb                = scui_ui_scene_soccer_custom_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_HONEYCOMB = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_key         = true,
	.widget.style.indev_enc         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_HONEYCOMB,
	.widget.event_cb                = scui_ui_scene_honeycomb_event_proc,
	.widget.child_num               = 1,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_scroll_maker_t scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL = {
	.widget.type                    = scui_widget_type_scroll,
	.widget.style.indev_ptr         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_HONEYCOMB_SCROLL,
	.widget.parent                  = SCUI_UI_SCENE_HONEYCOMB,
	.widget.child_num               = 50,
	.pos                            = scui_opt_pos_c,
	.dir                            = scui_opt_dir_all,
	.springback                     = 35,
	.freedom                        = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_LIST_ARC = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_ARC,
	.widget.event_cb                = scui_ui_scene_list_arc_event_proc,
	.widget.child_num               = 3,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_scroll_maker_t scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL = {
	.widget.type                    = scui_widget_type_scroll,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_ARC_SCROLL,
	.widget.parent                  = SCUI_UI_SCENE_LIST_ARC,
	.widget.child_num               = 50,
	.notify_cb                      = scui_ui_scene_list_arc_scroll_notify_event,
	.pos                            = scui_opt_pos_c,
	.dir                            = scui_opt_dir_ver,
	.space                          = 10,
	.route_enc                      = 80,
	.route_key                      = 80,
	.keyid_fdir                     = SCUI_WIDGET_SCROLL_KEY_FDIR,
	.keyid_bdir                     = SCUI_WIDGET_SCROLL_KEY_BDIR,
	.springback                     = 70,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_ARC_BAR_ARC,
	.widget.parent                  = SCUI_UI_SCENE_LIST_ARC,
	.widget.event_cb                = scui_ui_scene_list_arc_bar_arc_event,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_ARC_MASK,
	.widget.parent                  = SCUI_UI_SCENE_LIST_ARC,
	.widget.event_cb                = scui_ui_mask_all_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_LIST_SCALE = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_SCALE,
	.widget.event_cb                = scui_ui_scene_list_scale_event_proc,
	.widget.child_num               = 3,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_scroll_maker_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_SCROLL = {
	.widget.type                    = scui_widget_type_scroll,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_SCALE_SCROLL,
	.widget.parent                  = SCUI_UI_SCENE_LIST_SCALE,
	.widget.child_num               = 50,
	.notify_cb                      = scui_ui_scene_list_scale_scroll_notify_event,
	.pos                            = scui_opt_pos_c,
	.dir                            = scui_opt_dir_ver,
	.space                          = 10,
	.route_enc                      = 117,
	.route_key                      = 117,
	.keyid_fdir                     = SCUI_WIDGET_SCROLL_KEY_FDIR,
	.keyid_bdir                     = SCUI_WIDGET_SCROLL_KEY_BDIR,
	.springback                     = 70,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC,
	.widget.parent                  = SCUI_UI_SCENE_LIST_SCALE,
	.widget.event_cb                = scui_ui_scene_list_scale_bar_arc_event,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_LIST_SCALE_MASK,
	.widget.parent                  = SCUI_UI_SCENE_LIST_SCALE,
	.widget.event_cb                = scui_ui_mask_all_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_SPREAD = {
	.widget.type                    = scui_widget_type_window,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_SPREAD,
	.widget.event_cb                = scui_ui_scene_spread_event_proc,
	.widget.child_num               = 1,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 10,
	.widget.clip.y                  = 10,
	.widget.clip.w                  = SCUI_DRV_HOR_RES - 10 * 2,
	.widget.clip.h                  = SCUI_DRV_VER_RES - 10 * 2,
	.widget.myself                  = SCUI_UI_SCENE_SPREAD_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_SPREAD,
	.widget.event_cb                = scui_ui_scene_spread_custom_event_proc,
	.widget.child_num               = 3,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL = {
	.widget.type                    = scui_widget_type_window,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_THUMBWHEEL,
	.widget.event_cb                = scui_ui_scene_thumbwheel_event_proc,
	.widget.child_num               = 1,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 5,
	.widget.clip.y                  = 5,
	.widget.clip.w                  = SCUI_DRV_HOR_RES - 5 * 2,
	.widget.clip.h                  = SCUI_DRV_VER_RES - 5 * 2,
	.widget.myself                  = SCUI_UI_SCENE_THUMBWHEEL_CUSTOM,
	.widget.parent                  = SCUI_UI_SCENE_THUMBWHEEL,
	.widget.event_cb                = scui_ui_scene_thumbwheel_custom_event_proc,
	.widget.child_num               = 3,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_WATERFALL = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_key         = true,
	.widget.style.indev_enc         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_WATERFALL,
	.widget.event_cb                = scui_ui_scene_waterfall_event_proc,
	.widget.child_num               = 2,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_scroll_maker_t scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL = {
	.widget.type                    = scui_widget_type_scroll,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_WATERFALL_SCROLL,
	.widget.parent                  = SCUI_UI_SCENE_WATERFALL,
	.widget.child_num               = 50,
	.notify_cb                      = scui_ui_scene_waterfall_scroll_notify_event,
	.pos                            = scui_opt_pos_all,
	.dir                            = scui_opt_dir_ver,
	.route_enc                      = 117,
	.route_key                      = 117,
	.keyid_fdir                     = SCUI_WIDGET_SCROLL_KEY_FDIR,
	.keyid_bdir                     = SCUI_WIDGET_SCROLL_KEY_BDIR,
	.springback                     = 35,
	.freedom                        = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_WATERFALL_BAR_ARC,
	.widget.parent                  = SCUI_UI_SCENE_WATERFALL,
	.widget.event_cb                = scui_ui_scene_waterfall_bar_arc_event,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_MONITOR = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = SCUI_DRV_VER_RES - 100,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = 100,
	.widget.myself                  = SCUI_UI_SCENE_MONITOR,
	.widget.event_cb                = scui_ui_scene_monitor_event_proc,
	.widget.child_num               = 10,
	.level                          = 10,
	.buffer                         = true,
	.resident                       = true,
	.hang_only                      = true,
	.format                         = SCUI_PIXEL_CF_DEF_A,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_POPUP = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = 58,
	.widget.clip.y                  = 60,
	.widget.clip.w                  = 350,
	.widget.clip.h                  = 100,
	.widget.myself                  = SCUI_UI_SCENE_POPUP,
	.widget.event_cb                = scui_ui_scene_popup_event_proc,
	.widget.child_num               = 6,
	.level                          = 10,
	.buffer                         = true,
	.resident                       = true,
	.hang_only                      = true,
	.format                         = SCUI_PIXEL_CF_DEF_A,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_POPUP_SCALE = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.clip.w                  = 350,
	.widget.clip.h                  = 100,
	.widget.myself                  = SCUI_UI_SCENE_POPUP_SCALE,
	.widget.parent                  = SCUI_UI_SCENE_POPUP,
	.widget.child_num               = 1,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_POPUP_BG = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.clip.w                  = 350,
	.widget.clip.h                  = 100,
	.widget.myself                  = SCUI_UI_SCENE_POPUP_BG,
	.widget.parent                  = SCUI_UI_SCENE_POPUP_SCALE,
	.widget.event_cb                = scui_ui_scene_popup_bg_event_proc,
	.widget.child_num               = 5,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_1 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_1,
	.widget.event_cb                = scui_ui_scene_float_1_event_proc,
	.widget.child_num               = 2,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_1_1 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.clip.x                  = SCUI_DRV_HOR_RES / 4,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 8,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 2,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 2,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_1_1,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_1,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_1_1_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_1_2 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.clip.x                  = 15,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 11 / 16,
	.widget.clip.w                  = SCUI_DRV_HOR_RES - 30,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 4,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_1_2,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_1,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_1_2_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.event_cb                = scui_ui_scene_float_2_event_proc,
	.widget.child_num               = 6,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_C = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_C,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.event_cb                = scui_ui_scene_float_2_c_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_1 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 1 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_1,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_2_1_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_2 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_2,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_2_2_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_3 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 9 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_3,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_2_3_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_4 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 1 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_4,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_2_4_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2_5 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 7 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 7 / 13,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2_5,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_float_2_5_event_proc,
	.widget.child_num               = 5,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_3 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_3,
	.widget.event_cb                = scui_ui_scene_float_3_event_proc,
	.widget.child_num               = 11,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_3_RING = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_3_RING,
	.widget.parent                  = SCUI_UI_SCENE_FLOAT_3,
	.widget.event_cb                = scui_ui_scene_float_3_ring_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_4 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_4,
	.widget.event_cb                = scui_ui_scene_float_4_event_proc,
	.widget.child_num               = 5,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_1 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_1,
	.widget.event_cb                = scui_ui_scene_1_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_2 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_2,
	.widget.event_cb                = scui_ui_scene_2_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.widget.child_num               = 6,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_2_BAR_ARC = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_2_BAR_ARC,
	.widget.parent                  = SCUI_UI_SCENE_2,
	.widget.event_cb                = scui_ui_scene_2_bar_arc_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_3 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_3,
	.widget.event_cb                = scui_ui_scene_3_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_4 = {
	.widget.type                    = scui_widget_type_window,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_4,
	.widget.event_cb                = scui_ui_scene_4_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.widget.child_num               = 1,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_4_RING = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_4_RING,
	.widget.parent                  = SCUI_UI_SCENE_4,
	.widget.event_cb                = scui_ui_scene_4_ring_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_5 = {
	.widget.type                    = scui_widget_type_window,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_5,
	.widget.event_cb                = scui_ui_scene_5_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.widget.child_num               = 1,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_5_RING = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_5_RING,
	.widget.parent                  = SCUI_UI_SCENE_5,
	.widget.event_cb                = scui_ui_scene_5_ring_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_6 = {
	.widget.type                    = scui_widget_type_window,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_6,
	.widget.event_cb                = scui_ui_scene_6_event_proc,
	.widget.image                   = scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp,
	.widget.child_num               = 9,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_1 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 1 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_1,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_1_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_2 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_2,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_2_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_3 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 9 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 1 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_3,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_3_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_4 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 1 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_4,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_4_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_5 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_5,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_5_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_6 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 9 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 5 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_6,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_6_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_7 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 1 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 9 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_7,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_7_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_8 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 5 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 9 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_8,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_8_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_6_9 = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.sched_anima       = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 9 / 13,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 9 / 13,
	.widget.clip.w                  = SCUI_DRV_HOR_RES * 3 / 13,
	.widget.clip.h                  = SCUI_DRV_VER_RES * 3 / 13,
	.widget.myself                  = SCUI_UI_SCENE_6_9,
	.widget.parent                  = SCUI_UI_SCENE_6,
	.widget.color.color.full        = 0xFFFFFFFF,
	.widget.event_cb                = scui_ui_scene_6_9_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_HOME = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_HOME,
	.widget.event_cb                = scui_ui_scene_home_event_proc,
	.widget.child_num               = 6,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_C = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.trans             = true,
	.widget.style.sched_anima       = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_HOME_C,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.event_cb                = scui_ui_scene_home_c_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_TEST = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.sched_anima       = true,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_TEST,
	.widget.event_cb                = scui_ui_scene_test_event_proc,
	.widget.child_num               = 10,
	.level                          = 0,
	.buffer                         = true,
};

const void * scui_widget_parser_table[63] = {
	(void *)&scui_widget_SCUI_UI_SCENE_BUTTERFLY,
	(void *)&scui_widget_SCUI_UI_SCENE_BUTTERFLY_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_CUBE,
	(void *)&scui_widget_SCUI_UI_SCENE_CUBE_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_LANTERN,
	(void *)&scui_widget_SCUI_UI_SCENE_LANTERN_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_SOCCER,
	(void *)&scui_widget_SCUI_UI_SCENE_SOCCER_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_HONEYCOMB,
	(void *)&scui_widget_SCUI_UI_SCENE_HONEYCOMB_SCROLL,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_SCROLL,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_BAR_ARC,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_ARC_MASK,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_SCROLL,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_BAR_ARC,
	(void *)&scui_widget_SCUI_UI_SCENE_LIST_SCALE_MASK,
	(void *)&scui_widget_SCUI_UI_SCENE_SPREAD,
	(void *)&scui_widget_SCUI_UI_SCENE_SPREAD_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_THUMBWHEEL,
	(void *)&scui_widget_SCUI_UI_SCENE_THUMBWHEEL_CUSTOM,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL_SCROLL,
	(void *)&scui_widget_SCUI_UI_SCENE_WATERFALL_BAR_ARC,
	(void *)&scui_widget_SCUI_UI_SCENE_MONITOR,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP_SCALE,
	(void *)&scui_widget_SCUI_UI_SCENE_POPUP_BG,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1_1,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1_2,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_C,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_1,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_2,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_3,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_4,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2_5,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3_RING,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_4,
	(void *)&scui_widget_SCUI_UI_SCENE_1,
	(void *)&scui_widget_SCUI_UI_SCENE_2,
	(void *)&scui_widget_SCUI_UI_SCENE_2_BAR_ARC,
	(void *)&scui_widget_SCUI_UI_SCENE_3,
	(void *)&scui_widget_SCUI_UI_SCENE_4,
	(void *)&scui_widget_SCUI_UI_SCENE_4_RING,
	(void *)&scui_widget_SCUI_UI_SCENE_5,
	(void *)&scui_widget_SCUI_UI_SCENE_5_RING,
	(void *)&scui_widget_SCUI_UI_SCENE_6,
	(void *)&scui_widget_SCUI_UI_SCENE_6_1,
	(void *)&scui_widget_SCUI_UI_SCENE_6_2,
	(void *)&scui_widget_SCUI_UI_SCENE_6_3,
	(void *)&scui_widget_SCUI_UI_SCENE_6_4,
	(void *)&scui_widget_SCUI_UI_SCENE_6_5,
	(void *)&scui_widget_SCUI_UI_SCENE_6_6,
	(void *)&scui_widget_SCUI_UI_SCENE_6_7,
	(void *)&scui_widget_SCUI_UI_SCENE_6_8,
	(void *)&scui_widget_SCUI_UI_SCENE_6_9,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_C,
	(void *)&scui_widget_SCUI_UI_SCENE_TEST,
};
