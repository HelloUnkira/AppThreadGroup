#ifndef LVGL_LIB_H
#define LVGL_LIB_H

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#include "lvgl.h"
#include "lv_conf.h"
#include "lv_drv_conf.h"

//
#include "lvgl_theme.h"
#include "lvgl_view_sched.h"
#include "lvgl_ui.h"

void lvgl_lib_ready(void);

void app_lvgl_tick_exec_update(void);
void app_lvgl_drv_update(void);

void app_lvgl_scene_start(void);
void app_lvgl_scene_stop(void);
void app_lvgl_scene_dlps(bool status);
void app_lvgl_scene_shutdown(void);

void app_lvgl_timer_ready(void);
void app_lvgl_timer_start(void);
void app_lvgl_timer_stop(void);

#endif
