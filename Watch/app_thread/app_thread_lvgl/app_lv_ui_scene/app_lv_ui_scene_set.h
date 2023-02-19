#ifndef app_lv_scene_SET_H
#define app_lv_scene_SET_H

/* test sys scene */
extern app_lv_scene_t app_lv_scene_test_list;
extern app_lv_scene_t app_lv_scene_test_clock;
/* main scene */
extern app_lv_scene_t app_lv_scene_watch;
extern app_lv_scene_t app_lv_scene_main;
/* group scene */
extern app_lv_scene_t app_lv_scene_list;
/* base scene */
extern app_lv_scene_t app_lv_scene_stopwatch;
extern app_lv_scene_t app_lv_scene_countdown;
extern app_lv_scene_t app_lv_scene_countdown_remind;

/*@brief 场景内事件处理集合
 */
void app_lv_ui_scene_set_event(uint32_t event, uint8_t *data, uint32_t size);

#endif
