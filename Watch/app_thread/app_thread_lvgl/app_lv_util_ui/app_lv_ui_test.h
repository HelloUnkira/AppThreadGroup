#ifndef APP_LV_UI_TEST_H
#define APP_LV_UI_TEST_H

/* 开发阶段使用,验证场景的效果 */
#define APP_LV_UI_TEST_USE              0

/* 开发阶段使用 */
/* 去除因外部逻辑导致的界面阻塞 */
/* 同时简化部分流程界面 */
#define APP_LV_UI_MODE_DEVELOPMENT      1

/*@brief 测试指定场景
 */
void app_lv_ui_test_scene(void);

#endif
