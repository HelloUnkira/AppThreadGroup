#ifndef APP_SCUI_EVENT_H
#define APP_SCUI_EVENT_H

/*@brief scui tick执行
 */
void app_scui_tick_exec_update(void);

/*@brief scui drv更新
 */
void app_scui_drv_update(void);

/*@brief scui 场景启动
 */
void app_scui_window_start(void);

/*@brief scui 场景停止
 */
void app_scui_window_stop(void);

/*@brief scui 场景低功耗
 */
void app_scui_window_dlps(bool status);

/*@brief scui 场景停止
 */
void app_scui_window_shutdown(void);

#endif
