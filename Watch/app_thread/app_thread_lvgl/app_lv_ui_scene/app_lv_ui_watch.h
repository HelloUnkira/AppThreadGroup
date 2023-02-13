#ifndef APP_LV_UI_WATCH_H
#define APP_LV_UI_WATCH_H

typedef enum {
    app_lv_ui_watch_null = 0,
    app_lv_ui_watch_stop,
    app_lv_ui_watch_start,
} app_lv_ui_watch_status_t;

/*@brief     设置显示界面
 *@param[in] status 界面(true:启动界面,false:结束界面)
 */
void app_lv_ui_watch_status_update(app_lv_ui_watch_status_t status);

#endif
