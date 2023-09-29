#ifndef APP_LV_UI_SCENE_OLD_H
#define APP_LV_UI_SCENE_OLD_H

/* default scene */
extern app_lv_scene_t app_lv_ui_start;
extern app_lv_scene_t app_lv_ui_stop;
/* main scene */
/* group scene */
extern app_lv_scene_t app_lv_ui_test_list;
/* base scene */
extern app_lv_scene_t app_lv_ui_log_text;
extern app_lv_scene_t app_lv_ui_fs_info;
extern app_lv_scene_t app_lv_ui_qrcode;
extern app_lv_scene_t app_lv_ui_clock;
extern app_lv_scene_t app_lv_ui_calendar;
extern app_lv_scene_t app_lv_ui_countdown;
extern app_lv_scene_t app_lv_ui_countdown_remind;
extern app_lv_scene_t app_lv_ui_remind_sedentary;
extern app_lv_scene_t app_lv_ui_remind_drink;
extern app_lv_scene_t app_lv_ui_data_daily;
/* test function */
extern app_lv_scene_t app_lv_ui_temperature;
/* extern: mini game */
extern app_lv_scene_t app_lv_ui_2048;
extern app_lv_scene_t app_lv_ui_tetris;
extern app_lv_scene_t app_lv_ui_maze;
extern app_lv_scene_t app_lv_ui_snake;

#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_scene_remind.h"

#include "app_lv_ui_start.h"
#include "app_lv_ui_stop.h"
#include "app_lv_ui_test_list.h"

#include "app_lv_ui_2048.h"
#include "app_lv_ui_maze.h"
#include "app_lv_ui_snake.h"
#include "app_lv_ui_tetris.h"

#include "app_lv_ui_clock.h"
#include "app_lv_ui_fs_info.h"
#include "app_lv_ui_log_text.h"
#include "app_lv_ui_qrcode.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_remind.h"
#include "app_lv_ui_data_daily.h"
#include "app_lv_ui_temperature.h"

#include "app_lv_ui_calendar.h"
#include "app_lv_ui_remind_drink.h"
#include "app_lv_ui_remind_sedentary.h"

#endif
