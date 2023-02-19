#ifndef APP_LV_UI_UTIL_H
#define APP_LV_UI_UTIL_H

/*@brief    场景默认事件响应回调设置
 *param[in] scene 场景
 */
void app_lv_ui_event_default_set(lv_obj_t *scene);

/*@brief    场景默认事件响应回调清除
 *param[in] scene 场景
 */
void app_lv_ui_event_default_clr(lv_obj_t *scene);

/*@brief 滚轮事件自定义回调
 */
void app_lv_ui_roller_mask_event_cb(lv_event_t * e);

#endif
