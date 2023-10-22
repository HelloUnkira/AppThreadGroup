#ifndef APP_LV_EVENT_DEFAULT_H
#define APP_LV_EVENT_DEFAULT_H

/* 物理按键复用为编码器事件 */
/* 这意味着平台将按键和编码器合并起来使用 */
#define APP_LV_EVENT_CLOCKWISE          187
#define APP_LV_EVENT_CLOCKWISE_ANTI     189

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
