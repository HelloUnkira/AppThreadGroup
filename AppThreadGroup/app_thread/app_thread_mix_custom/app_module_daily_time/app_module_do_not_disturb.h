#ifndef APP_MODULE_DO_NOT_DISTURB_H
#define APP_MODULE_DO_NOT_DISTURB_H

#include "app_module_daily_time.h"
/* 时间表检查函数:app_module_daily_time_do_not_disturb_t */
APP_MODULE_DAILY_TIME_T(app_module_daily_time_do_not_disturb_t, 2);

typedef struct {
    app_module_daily_time_do_not_disturb_t daily_time;
} app_module_do_not_disturb_t;

/*@brief     勿扰模式设置
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_set(app_module_do_not_disturb_t *do_not_disturb);

/*@brief      勿扰模式获取
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_get(app_module_do_not_disturb_t *do_not_disturb);

/*@brief  勿扰模式状态检查
 *@retval 是否为勿扰态
 */
bool app_module_do_not_disturb_status(void);

/*@brief      勿扰模式默认设置
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_reset(app_module_do_not_disturb_t *do_not_disturb);

#endif
