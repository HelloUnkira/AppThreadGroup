#ifndef APP_LV_EVENT_UI_H
#define APP_LV_EVENT_UI_H

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
