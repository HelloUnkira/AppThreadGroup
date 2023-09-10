#ifndef APP_LV_WHEEL_EVENT_H
#define APP_LV_WHEEL_EVENT_H

/*@brief 轮盘跟手特效回调
 */
void app_lv_wheel_event_cb(lv_event_t * e);

/*@brief 轮盘跟手特效回滚(如果需要回滚)
 */
bool app_lv_wheel_rollback(void);

/*@brief 轮盘跟手特效工作状态
 */
bool app_lv_wheel_status(lv_dir_t dir);

#endif
