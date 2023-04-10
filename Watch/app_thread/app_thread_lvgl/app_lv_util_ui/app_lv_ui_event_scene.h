#ifndef APP_LV_UI_EVENT_SCENE_H
#define APP_LV_UI_EVENT_SCENE_H

/*@brief 界面默认事件响应回调
 */
void app_lv_ui_event_default(lv_event_t *e);

/*@brief    场景默认事件响应回调设置
 *param[in] scene    事件捕获场景,为NULL默认为scr
 *param[in] enable   启用或者禁用
 *param[in] redirect 场景默认事件重定向
 */
void app_lv_ui_event_default_config(lv_obj_t *scene, bool enable, lv_event_cb_t redirect);

/*@brief 点击返回上一层
 */
void app_lv_ui_event_click_turn_back_cb(lv_event_t *e);

/*@brief 场景内事件处理集合
 */
void app_lv_ui_scene_event(uint32_t event, uint8_t *data, uint32_t size);

#endif
