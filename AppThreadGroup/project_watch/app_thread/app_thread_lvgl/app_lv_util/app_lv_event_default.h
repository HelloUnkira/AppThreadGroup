#ifndef APP_LV_EVENT_DEFAULT_H
#define APP_LV_EVENT_DEFAULT_H

/*@brief 界面默认事件响应回调
 */
void app_lv_event_default_cb(lv_event_t *e);

/*@brief    场景默认事件响应回调设置
 *param[in] scene    事件捕获场景,为NULL默认为wheel
 *param[in] enable   启用或者禁用
 *param[in] redirect 场景默认事件重定向
 */
void app_lv_event_default_config(lv_obj_t *scene, bool enable, lv_event_cb_t redirect);

#endif
