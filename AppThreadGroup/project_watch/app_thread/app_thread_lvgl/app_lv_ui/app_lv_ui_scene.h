#ifndef APP_LV_UI_SCENE_H
#define APP_LV_UI_SCENE_H



/* base */
extern app_lv_scene_t app_lv_ui_null;
extern app_lv_scene_t app_lv_ui_dlps;
/* watch face */
extern app_lv_scene_t app_lv_ui_watch_face;
/* theme */
extern app_lv_scene_t app_lv_ui_theme;
/* function */
extern app_lv_scene_t app_lv_ui_calculator;
extern app_lv_scene_t app_lv_ui_stopwatch;
extern app_lv_scene_t app_lv_ui_countdown;
extern app_lv_scene_t app_lv_ui_countdown_remind;
/* health */
extern app_lv_scene_t app_lv_ui_data_daily;
extern app_lv_scene_t app_lv_ui_women_health;
/*  */



/* base */
#include "app_lv_ui_null.h"
#include "app_lv_ui_dlps.h"
/* watch face */
#include "app_lv_ui_watch_face.h"
/* theme */
#include "app_lv_ui_theme.h"
#include "app_lv_ui_list.h"
#include "app_lv_ui_list_arc.h"
#include "app_lv_ui_star_robit.h"
#include "app_lv_ui_thumb_wheel.h"
/* function */
#include "app_lv_ui_calculator.h"
#include "app_lv_ui_stopwatch.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_remind.h"
/* health */
#include "app_lv_ui_data_daily.h"
#include "app_lv_ui_women_health.h"
/*  */



/* presenter */
#include "app_lv_ui_presenter_clock.h"
/*  */
#include "app_lv_ui_presenter_calculator.h"
#include "app_lv_ui_presenter_stopwatch.h"
#include "app_lv_ui_presenter_countdown.h"
/*  */
#include "app_lv_ui_presenter_data_daily.h"
#include "app_lv_ui_presenter_women_health.h"
/*  */



#endif
