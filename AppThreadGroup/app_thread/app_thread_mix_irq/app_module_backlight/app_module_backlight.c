/*实现目标:
 *    背光相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_thread_group.h"
#include "app_module_backlight.h"

static app_mutex_t app_module_backlight_mutex = {0};
static app_module_backlight_t app_module_backlight = {0};

/*@brief        设置背光模组
 *@param[out]   backlight 背光实例
 */
void app_module_backlight_set(app_module_backlight_t *backlight)
{
    app_mutex_process(&app_module_backlight_mutex, app_mutex_take);
    app_module_backlight = *backlight;
    app_mutex_process(&app_module_backlight_mutex, app_mutex_give);
    app_arch_backlight_update(&app_arch_backlight, backlight->duty_cycle);
}

/*@brief        获取背光模组
 *@param[out]   backlight 背光实例
 */
void app_module_backlight_get(app_module_backlight_t *backlight)
{
    app_mutex_process(&app_module_backlight_mutex, app_mutex_take);
    *backlight = app_module_backlight;
    app_mutex_process(&app_module_backlight_mutex, app_mutex_give);
}

/*@brief 背光模组初始化
 */
void app_module_backlight_ready(void)
{
    app_mutex_process(&app_module_backlight_mutex, app_mutex_static);
    app_arch_backlight_ready(&app_arch_backlight);
}
