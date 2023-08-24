/*实现目标:
 *    关机前转存到外存的追踪日志信息
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@param 关机前的最后所需要转储的日志信息
 */
void app_module_shutdown_dump(void)
{
    char text[APP_SYS_LOG_TEXT_MAX] = {0};
    snprintf(text, APP_SYS_LOG_TEXT_MAX,
             "shutdown trace:");
    app_sys_log_text_dump(text, true);
    /* 当次关机条件信息: */
    snprintf(text, APP_SYS_LOG_TEXT_MAX,
             "shutdown condition:%" PRIu8,
              app_module_system_valid_get());
    app_sys_log_text_dump(text, true);
    /* 线程组工作时间: */
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "app thread group work time:%" PRIu64,
              app_module_clock_get_sec_tick());
    app_sys_log_text_dump(text, true);
    #if APP_THREAD_SLAVE_EXECUTE_TIME
    double execute_us = 0;
    app_thread_execute_us_get(app_thread_id_mix_irq, &execute_us);
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "app_thread_id_mix_irq:%.8lf",
            (double)execute_us / 1000000.0);
    app_sys_log_text_dump(text, true);
    app_thread_execute_us_get(app_thread_id_mix_custom, &execute_us);
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "app_thread_id_mix_custom:%.8lf",
            (double)execute_us / 1000000.0);
    app_sys_log_text_dump(text, true);
    app_thread_execute_us_get(app_thread_id_manage, &execute_us);
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "app_thread_id_manage:%.8lf",
            (double)execute_us / 1000000.0);
    app_sys_log_text_dump(text, true);
    app_thread_execute_us_get(app_thread_id_lvgl, &execute_us);
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "app_thread_id_lvgl:%.8lf",
            (double)execute_us / 1000000.0);
    app_sys_log_text_dump(text, true);
    #endif
    /* 关机的时间信息: */
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    snprintf(text, APP_SYS_LOG_TEXT_MAX, "dtime:%u-%u-%u;%u;%u:%u:%u",
             clock.year, clock.month,  clock.day, clock.week,
             clock.hour, clock.minute, clock.second);
    app_sys_log_text_dump(text, true);
    /* 继续追加其他关键信息: */
    /* 例:(电量,传感器状态,模组工作状态,敏感配置信息等) */
}
