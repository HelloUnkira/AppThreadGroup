#ifndef APP_LV_UI_STAR_ROBIT_H
#define APP_LV_UI_STAR_ROBIT_H

/* 星轨的图片资源选择偏移(中心) */
/* 星轨的图片资源选择偏移(外部) */
/* 星轨的单位旋转度(一圈图片显示数量 * 本数值 == 360(必须相等)) */
/* 星轨的外部边界 */
/* 星轨的拨正单位旋转度 */
#define APP_LV_UI_STAR_ROBIT_CEN       7
#define APP_LV_UI_STAR_ROBIT_OFS       4
#define APP_LV_UI_STAR_ROBIT_UNIT      30
#define APP_LV_UI_STAR_ROBIT_SPAN      5
#define APP_LV_UI_STAR_ROBIT_RECTIFY   5

/*@brief     界面显示
 *@param[in] scene 场景
 */
void app_lv_ui_star_robit_show(void *scene);

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
void app_lv_ui_star_robit_hide(void *scene);

#endif

