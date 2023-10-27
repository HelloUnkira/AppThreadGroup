#ifndef APP_LV_UI_WATERFALL_H
#define APP_LV_UI_WATERFALL_H

/* 瀑布的图片资源选择偏移(最小) */
/* 瀑布的图片资源选择偏移(最大) */
#define APP_LV_UI_WATERFALL_OFS_MIN     0
#define APP_LV_UI_WATERFALL_OFS_MAX     5
#define APP_LV_UI_WATERFALL_NUM         5

/*@brief     界面显示
 *@param[in] scene 场景
 */
void app_lv_ui_waterfall_show(void *scene);

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
void app_lv_ui_waterfall_hide(void *scene);

#endif
