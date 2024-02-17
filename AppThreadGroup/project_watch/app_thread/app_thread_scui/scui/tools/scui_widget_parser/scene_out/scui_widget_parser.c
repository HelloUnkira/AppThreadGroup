/*一个通过scui_widget_parser脚本生成的widget表
 */

#include "scui_drv_conf.h"
#include "scui.h"

#if defined(SCUI_WIDGET_EVENT_USE_EMPTY) && SCUI_WIDGET_EVENT_USE_EMPTY == 1
static scui_event_retval_t scui_ui_scene_float_1_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_float_3_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_float_4_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_c_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_lu_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_ru_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_ld_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_home_rd_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_1_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_2_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_3_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_4_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_5_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
static scui_event_retval_t scui_ui_scene_6_event_proc(scui_event_t *event)
{
	SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
	switch (event->type) {
	default:
		return scui_event_retval_quit;
	}
}
#else
extern scui_event_retval_t scui_ui_scene_float_1_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_float_2_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_float_3_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_float_4_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_c_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_lu_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_ru_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_ld_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_home_rd_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_1_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_2_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_3_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_4_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_5_event_proc(scui_event_t *event);
extern scui_event_retval_t scui_ui_scene_6_event_proc(scui_event_t *event);
#endif

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_1 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_1,
	.widget.event_cb                = scui_ui_scene_float_1_event_proc,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_2 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_2,
	.widget.event_cb                = scui_ui_scene_float_2_event_proc,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_3 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_3,
	.widget.event_cb                = scui_ui_scene_float_3_event_proc,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_FLOAT_4 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_FLOAT_4,
	.widget.event_cb                = scui_ui_scene_float_4_event_proc,
	.level                          = 1,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_HOME = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_HOME,
	.widget.event_cb                = scui_ui_scene_home_event_proc,
	.widget.child_num               = 5,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_C = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.y                  = SCUI_DRV_VER_RES / 3,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 3,
	.widget.myself                  = SCUI_UI_SCENE_HOME_C,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.color.color.ch.a        = 0xFF,
	.widget.color.color.ch.g        = 0xFF,
	.widget.event_cb                = scui_ui_scene_home_c_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_LU = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = SCUI_DRV_VER_RES * 0 / 3,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 0 / 3,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 3,
	.widget.myself                  = SCUI_UI_SCENE_HOME_LU,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.color.color.ch.a        = 0xFF,
	.widget.color.color.ch.r        = 0xFF,
	.widget.event_cb                = scui_ui_scene_home_lu_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_RU = {
	.widget.type                    = scui_widget_type_custom,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = SCUI_DRV_VER_RES * 2 / 3,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 0 / 3,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 3,
	.widget.myself                  = SCUI_UI_SCENE_HOME_RU,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.color.color.ch.a        = 0xFF,
	.widget.color.color.ch.r        = 0xFF,
	.widget.event_cb                = scui_ui_scene_home_ru_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_LD = {
	.widget.type                    = scui_widget_type_custom,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 0 / 3,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 2 / 3,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 3,
	.widget.myself                  = SCUI_UI_SCENE_HOME_LD,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.color.color.ch.a        = 0xFF,
	.widget.color.color.ch.b        = 0xFF,
	.widget.event_cb                = scui_ui_scene_home_ld_event_proc,
};

static const scui_custom_maker_t scui_widget_SCUI_UI_SCENE_HOME_RD = {
	.widget.type                    = scui_widget_type_custom,
	.widget.clip.x                  = SCUI_DRV_HOR_RES * 2 / 3,
	.widget.clip.y                  = SCUI_DRV_VER_RES * 2 / 3,
	.widget.clip.w                  = SCUI_DRV_HOR_RES / 3,
	.widget.clip.h                  = SCUI_DRV_VER_RES / 3,
	.widget.myself                  = SCUI_UI_SCENE_HOME_RD,
	.widget.parent                  = SCUI_UI_SCENE_HOME,
	.widget.color.color.ch.a        = 0xFF,
	.widget.color.color.ch.b        = 0xFF,
	.widget.event_cb                = scui_ui_scene_home_rd_event_proc,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_1 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_1,
	.widget.event_cb                = scui_ui_scene_1_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_2 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_2,
	.widget.event_cb                = scui_ui_scene_2_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_3 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_3,
	.widget.event_cb                = scui_ui_scene_3_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_4 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_4,
	.widget.event_cb                = scui_ui_scene_4_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_5 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_5,
	.widget.event_cb                = scui_ui_scene_5_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

static const scui_window_maker_t scui_widget_SCUI_UI_SCENE_6 = {
	.widget.type                    = scui_widget_type_window,
	.widget.style.indev_ptr         = true,
	.widget.style.indev_enc         = true,
	.widget.style.indev_key         = true,
	.widget.clip.x                  = 0,
	.widget.clip.y                  = 0,
	.widget.clip.w                  = SCUI_DRV_HOR_RES,
	.widget.clip.h                  = SCUI_DRV_VER_RES,
	.widget.myself                  = SCUI_UI_SCENE_6,
	.widget.event_cb                = scui_ui_scene_6_event_proc,
	.level                          = 0,
	.buffer                         = true,
};

const void * scui_widget_parser_table[16] = {
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_1,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_2,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_3,
	(void *)&scui_widget_SCUI_UI_SCENE_FLOAT_4,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_C,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_LU,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_RU,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_LD,
	(void *)&scui_widget_SCUI_UI_SCENE_HOME_RD,
	(void *)&scui_widget_SCUI_UI_SCENE_1,
	(void *)&scui_widget_SCUI_UI_SCENE_2,
	(void *)&scui_widget_SCUI_UI_SCENE_3,
	(void *)&scui_widget_SCUI_UI_SCENE_4,
	(void *)&scui_widget_SCUI_UI_SCENE_5,
	(void *)&scui_widget_SCUI_UI_SCENE_6,
};
