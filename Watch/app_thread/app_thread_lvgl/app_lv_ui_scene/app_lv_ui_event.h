#ifndef APP_LV_UI_EVENT_H
#define APP_LV_UI_EVENT_H

/*@brief 界面默认事件响应回调
 */
void app_lv_ui_event_default(lv_event_t *e);

/*@brief    场景默认事件响应回调设置
 *param[in] enable 启用或者禁用
 */
void app_lv_ui_event_default_config(bool enable);

/*@brief 点击返回上一层
 */
void app_lv_ui_event_click_turn_back_cb(lv_event_t *e);

/*@brief 滚轮渐变蒙层回调
 */
void app_lv_ui_roller_mask_event_cb(lv_event_t * e);

/*@brief 折线图表下部渐变回调
 */
void app_lv_ui_chart_fade_event_cb(lv_event_t * e);

/*@brief 场景内事件处理集合
 */
void app_lv_ui_scene_event(uint32_t event, uint8_t *data, uint32_t size);

#endif
