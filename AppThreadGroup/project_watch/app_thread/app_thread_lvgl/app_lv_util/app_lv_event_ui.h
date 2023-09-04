#ifndef APP_LV_EVENT_UI_H
#define APP_LV_EVENT_UI_H

/*@brief 界面默认事件响应回调
 */
void app_lv_event_ui_default(lv_event_t *e);

/*@brief    场景默认事件响应回调设置
 *param[in] scene    事件捕获场景,为NULL默认为scr
 *param[in] enable   启用或者禁用
 *param[in] redirect 场景默认事件重定向
 */
void app_lv_event_ui_default_config(lv_obj_t *scene, bool enable, lv_event_cb_t redirect);

/*@brief 点击返回上一层
 */
void app_lv_event_ui_backtrack_cb(lv_event_t *e);

/*@brief 滚轮渐变蒙层回调
 */
void app_lv_event_ui_roller_mask_cb(lv_event_t * e);

/*@brief 折线图表下部渐变回调
 */
void app_lv_event_ui_chart_fade_cb(lv_event_t * e);

#endif
