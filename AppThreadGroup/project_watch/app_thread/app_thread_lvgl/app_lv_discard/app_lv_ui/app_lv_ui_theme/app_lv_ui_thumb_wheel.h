#ifndef APP_LV_UI_THUMB_WHEEL_H
#define APP_LV_UI_THUMB_WHEEL_H

/* 拨轮的图片资源选择偏移 */
/* 拨轮的单位旋转度(一圈图片显示数量 * 本数值 == 360(必须相等)) */
/* 拨轮的外部边界 */
/* 拨轮的拨正单位旋转度 */
#define APP_LV_UI_THUMB_WHEEL_OFS       4
#define APP_LV_UI_THUMB_WHEEL_UNIT      30
#define APP_LV_UI_THUMB_WHEEL_SPAN      5
#define APP_LV_UI_THUMB_WHEEL_RECTIFY   5

/*@brief 界面显示
 *@param scene 场景
 */
void app_lv_ui_thumb_wheel_show(void *scene);

/*@brief 界面隐藏
 *@param scene 场景
 */
void app_lv_ui_thumb_wheel_hide(void *scene);

#endif
