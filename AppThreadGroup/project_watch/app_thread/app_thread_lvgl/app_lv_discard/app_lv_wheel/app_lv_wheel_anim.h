#ifndef APP_LV_WHEEL_ANIM_H
#define APP_LV_WHEEL_ANIM_H

/*@brief 跟手动画迭代回调
 */
void app_lv_wheel_anim_exec_cb(void *var, int32_t val);

/*@brief 跟手动画开始回调
 */
void app_lv_wheel_anim_start_cb(lv_anim_t *a);

/*@brief 跟手动画结束回调
 */
void app_lv_wheel_anim_ready_cb(lv_anim_t *a);

#endif
