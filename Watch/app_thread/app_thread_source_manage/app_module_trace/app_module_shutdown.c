/*实现目标:
 *    关机前转存到外存的追踪日志信息
 */

#include "app_std_lib.h"
#include "app_module_trace.h"
#include "app_module_system.h"
#include "app_module_clock.h"

/*@param 关机前的最后所需要转储的日志信息
 */
void app_module_shutdown_dump(void)
{
    char text[APP_MODULE_TRACE_LOG_MAX] = {0};
    snprintf(text, APP_MODULE_TRACE_LOG_MAX,
             "app_module_shutdown_dump:\r\n");
    app_module_trace_text_dump(text);
    /* 当次关机条件信息: */
    snprintf(text, APP_MODULE_TRACE_LOG_MAX,
             "shutdown condition:%d\r\n",
             app_module_system_status_get());
    app_module_trace_text_dump(text);
    /* 线程组工作时间: */
    snprintf(text, APP_MODULE_TRACE_LOG_MAX,
             "app thread group work time:%lu\n\n",
              app_module_clock_get_sec_tick());
    app_module_trace_text_dump(text);
    /* 关机的时间信息: */
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    snprintf(text, APP_MODULE_TRACE_LOG_MAX,
             "dtime:%d-%d-%d;%d;%d:%d:%d\r\n",
             clock.year, clock.month,  clock.day, clock.week,
             clock.hour, clock.minute, clock.second);
    app_module_trace_text_dump(text);
    /* 继续追加其他关键信息: */
    /* 例:(电量,传感器状态,模组工作状态,敏感配置信息等) */
}
