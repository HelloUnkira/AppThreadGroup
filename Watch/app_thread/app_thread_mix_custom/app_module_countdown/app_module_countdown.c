/*实现目标:
 *    倒计时相关的功能组件
 *注意:
 *    倒计时功能本地有且只有一个实例
 *    因为不需要设定多个倒计时策略
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_timer.h"
#include "app_module_countdown.h"

static app_mutex_t app_module_countdown_mutex = {0};
static app_module_countdown_t app_module_countdown = {0};
static app_module_timer_t app_module_countdown_timer = {0};

/*@brief        设置倒计时
 *@param[in]    countdown 倒计时实例
 */
void app_module_countdown_set(app_module_countdown_t *countdown)
{
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown = *countdown;
    app_mutex_give(&app_module_countdown_mutex);
}

/*@brief        获取倒计时
 *@param[out]   countdown 倒计时实例
 */
void app_module_countdown_get(app_module_countdown_t *countdown)
{
    app_mutex_take(&app_module_countdown_mutex);
    *countdown = app_module_countdown;
    app_mutex_give(&app_module_countdown_mutex);
}

/*@brief 复位倒计时
 */
void app_module_countdown_reset(void)
{
    app_module_countdown_t countdown = {0};
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown = countdown;
    app_mutex_give(&app_module_countdown_mutex);
}

/*@brief 启动倒计时
 */
void app_module_countdown_start(void)
{
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown.onoff = true;
    app_mutex_give(&app_module_countdown_mutex);
    app_module_timer_start(&app_module_countdown_timer);
}

/*@brief 停止倒计时
 */
void app_module_countdown_stop(void)
{
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown.onoff = false;
    app_mutex_give(&app_module_countdown_mutex);
    app_module_timer_stop(&app_module_countdown_timer);
}

/*@brief 倒计时软件定时器模组回调
 */
static void app_module_countdown_xmsec_update(void *timer)
{
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown_t countdown = app_module_countdown;
    app_mutex_give(&app_module_countdown_mutex);
    if (!countdown.onoff)
        return;
    /*  */
    uint32_t seconds = countdown.hour   * 60 * 60 +
                       countdown.minute * 60 +
                       countdown.second;
    /*  */
    if (countdown.msec >= APP_MODULE_COUNTDOWN_MSEC) {
        countdown.msec -= APP_MODULE_COUNTDOWN_MSEC;
    } else {
        if (seconds == 0) {
            /* 发送倒计时事件 */
            app_package_t package = {
                .send_tid = app_thread_id_unknown,
                .recv_tid = app_thread_id_mix_custom,
                .module   = app_thread_mix_custom_countdown,
                .event    = app_thread_mix_custom_countdown_expired,
                .dynamic  = false,
                .size     = 0,
                .data     = NULL,
            };
            app_package_notify(&package);
            /* 中止倒计时 */
            app_module_countdown_stop();
            /* 本地归零 */
            app_module_countdown_t countdown_0 = {0};
            countdown = countdown_0;
        }
        if (seconds != 0) {
            seconds--;
            countdown.msec += 1000 - APP_MODULE_COUNTDOWN_MSEC;
        }
    }
    /*  */
    countdown.second = seconds % 60;
    countdown.minute = seconds / 60 % 60;
    countdown.hour   = seconds / 60 / 60 % 60;
    app_mutex_take(&app_module_countdown_mutex);
    app_module_countdown = countdown;
    app_mutex_give(&app_module_countdown_mutex);
    
    #if APP_MODULE_CHECK
    APP_SYS_LOG_INFO("countdown:%u:%u:%u:%u - %u",
                      countdown.hour,countdown.minute,countdown.second,
                      countdown.msec,countdown.onoff);
    #endif
}

/*@brief 倒计时模组初始化
 */
void app_module_countdown_ready(void)
{
    app_mutex_process(&app_module_countdown_mutex);
    app_module_countdown_timer.expired = app_module_countdown_xmsec_update;
    app_module_countdown_timer.peroid  = APP_MODULE_COUNTDOWN_MSEC;
    app_module_countdown_timer.reload  = true;
}
