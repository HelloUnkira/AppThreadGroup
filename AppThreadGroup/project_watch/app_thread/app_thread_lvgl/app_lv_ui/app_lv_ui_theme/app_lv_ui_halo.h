#ifndef APP_LV_UI_HALO_H
#define APP_LV_UI_HALO_H

/* 光环的图片资源选择偏移(中心) */
/* 光环的图片资源选择偏移(外部) */
/* 光环的单位旋转度(一圈图片显示数量 * 本数值 == 360(必须相等)) */
/* 光环的外部边界 */
/* 光环的拨正单位旋转度 */
#define APP_LV_UI_HALO_OFS_MIN      0
#define APP_LV_UI_HALO_OFS_MAX      6
#define APP_LV_UI_HALO_UNIT         45
#define APP_LV_UI_HALO_SPAN_MAX     5
#define APP_LV_UI_HALO_SPAN_MIN     40
#define APP_LV_UI_HALO_RECTIFY      9

/*@brief 界面显示
 *@param scene 场景
 */
void app_lv_ui_halo_show(void *scene);

/*@brief 界面隐藏
 *@param scene 场景
 */
void app_lv_ui_halo_hide(void *scene);

#endif
