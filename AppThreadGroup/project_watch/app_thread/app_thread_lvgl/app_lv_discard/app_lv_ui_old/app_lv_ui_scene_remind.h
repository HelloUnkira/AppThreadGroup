#ifndef APP_LV_UI_SCENE_REMIND_H
#define APP_LV_UI_SCENE_REMIND_H

/*@brief 特殊提醒场景优先级检查
 *       静止不低于当前提醒场景优先级的新提醒场景打断
 *       设置高优先级的新提醒场景切换
 */
bool app_lv_ui_scene_remind(app_lv_scene_t *remind_scene);

#endif
