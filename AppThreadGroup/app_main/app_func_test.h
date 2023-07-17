#ifndef APP_FUNCTION_TEST
#define APP_FUNCTION_TEST

#include "app_ext_lib.h"

#include "app_sys_timer.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_log_text.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_protocol.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_system.h"

/*@brief 日志转储测试
 */
static inline void app_sys_log_test(void)
{
    uint8_t tin[APP_SYS_LOG_TEXT_MAX * 2] ={0};
    for (uint32_t idx = 0; idx < APP_SYS_LOG_TEXT_MAX; idx++)
        tin[idx] = tin[idx + APP_SYS_LOG_TEXT_MAX] = '0' + idx % 10;

    static uint32_t offset = 0;
    offset %= APP_SYS_LOG_TEXT_MAX;
    app_sys_log_text_dump(tin + offset, true);
    uint8_t tout[APP_SYS_LOG_TEXT_MAX] = {0};
    app_sys_log_text_load(tout);
    if (memcmp(tout, tin + offset, APP_SYS_LOG_TEXT_MAX) != 0)
        printf("track log error\n");
    offset++;
}

/*@brief 外存chunk刷新测试
 */
static inline void app_sys_ext_mem_chunk_reflush(void)
{
    uint8_t block[4096] = {0};
    for (uint32_t idx = 0; idx < sizeof(block); block[idx] = ~0, idx++);
    /* 打开并关闭一次,保证chunk文件存在 */
    fclose(fopen("mix_chunk_small", "wb"));
    fclose(fopen("mix_chunk_large", "wb"));
    /*  */
    const app_sys_ext_mem_t *ext_mem1 = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_mem_t *ext_mem2 = app_sys_ext_mem_find_by_name("mix_chunk_large");
    /*  */
    printf("reflush start...\n");
    for (uint32_t idx = 0; idx < ext_mem1->chunk_size; idx += sizeof(block))
        app_sys_ext_mem_write(ext_mem1, idx, block, sizeof(block));
    for (uint32_t idx = 0; idx < ext_mem2->chunk_size; idx += sizeof(block))
        app_sys_ext_mem_write(ext_mem2, idx, block, sizeof(block));
    printf("reflush end...\n");
}

void app_sys_timer_test_callback(void *timer)
{
    app_sys_timer_t *local = timer;
    uint32_t *data = local->user_data;
    
    printf("timer %d callback %d\n", data[0], data[1]++);
    
    if (data[1] == 3)
        app_sys_timer_stop(timer);
}

/*@brief 软件定时器模组测试
 */
static inline void app_sys_timer_test(void)
{
    static uint32_t label_1[2] = {1, 0};
    static uint32_t label_2[2] = {2, 0};
    static uint32_t label_3[2] = {3, 0};
    static app_sys_timer_t timer1 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_1,
        .peroid    = 1000,
        .reload    = true};
    static app_sys_timer_t timer2 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_2,
        .peroid    = 2000,
        .reload    = true};
    static app_sys_timer_t timer3 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_3,
        .peroid    = 3000,
        .reload    = true};
    
    app_sys_timer_start(&timer1);
    app_sys_timer_start(&timer2);
    app_sys_timer_start(&timer3);
}

/*@brief 秒表模组测试
 */
static inline void app_module_stopwatch_test(void)
{
    app_module_stopwatch_reset();
    app_module_stopwatch_start();
}

/*@brief 倒计时模组测试
 */
static inline void app_module_countdown_test(void)
{
    app_module_countdown_reset();
    app_module_countdown_t countdown = {.hour = 0, .minute = 0, .second = 17};
    app_module_countdown_set(&countdown);
    app_module_countdown_start();
}

/*@brief 提醒闹钟模组测试
 */
static inline void app_module_remind_alarm_test(void)
{
    app_module_remind_item_t       *remind_item = NULL;
    app_module_remind_alarm_info_t *alarm_info = NULL;
    
    app_module_remind_alarm_array_lock();
    app_module_remind_alarm_array(&remind_item, &alarm_info);
    /* 提醒闹钟0(常规模式): */
    const char *alarm_name_0 = "Alarm 0";
    alarm_info[0].snooze_count = 0;
    alarm_info[0].duration = 300;
    memcpy(alarm_info[0].name, alarm_name_0, sizeof(alarm_name_0));
    remind_item[0].valid = true;
    remind_item[0].onoff = true;
    remind_item[0].type  = app_module_remind_item_custom;
    remind_item[0].month = 0b00000001000;
    remind_item[0].week  = 0b0000100;
    remind_item[0].clock.year   = 2023;
    remind_item[0].clock.month  = 1;
    remind_item[0].clock.day    = 1;
    remind_item[0].clock.hour   = 0;
    remind_item[0].clock.minute = 0;
    remind_item[0].clock.second = 2;
    app_module_clock_to_utc(&remind_item[0].clock);
    app_module_clock_to_week(&remind_item[0].clock);
    remind_item[0].offset_utc = remind_item[0].clock.utc /* +-xxx sec */;
    /* 提醒闹钟1(滚动模式) */
    const char *alarm_name_1 = "Alarm 1";
    alarm_info[1].snooze_count = 0;
    alarm_info[1].duration = 300;
    memcpy(alarm_info[1].name, alarm_name_1, sizeof(alarm_name_1));
    remind_item[1].valid  = true;
    remind_item[1].onoff  = true;
    remind_item[1].type   = app_module_remind_item_repeat;
    remind_item[1].repeat = 3;
    remind_item[1].clock.year   = 2023;
    remind_item[1].clock.month  = 1;
    remind_item[1].clock.day    = 1;
    remind_item[1].clock.hour   = 1;
    remind_item[1].clock.minute = 1;
    remind_item[1].clock.second = 2;
    app_module_clock_to_utc(&remind_item[1].clock);
    app_module_clock_to_week(&remind_item[1].clock);
    remind_item[1].offset_utc = remind_item[1].clock.utc /* +-xxx sec */;
    /* 继续添加 */
    app_module_remind_alarm_array_unlock();
}

static void app_main_1ms_loop(void)
{
    #if 0
    #elif 0
    /* chunk刷新,将其都刷为0 */
    app_sys_ext_mem_chunk_reflush();
    #elif 1
    #if 0
    /* 测试日志追踪 */
    app_sys_log_test();
    #endif
    /* fake hard clock 1ms irq */
    static uint32_t count = 0;
    /* 1msec system update: */
    app_module_system_1msec_update(count++);
    app_delay_us(1000);
    /* 1msec system update: */
    if (0 && count % 1000 == 0)
        printf("signal 1 second handler\n");
    /* ........ */
    #if 1
    /* 一些补充的扩展配置,与OS相关 */
    if (count == 3000) {
        void app_thread_os_extend(void);
        app_thread_os_extend();
    }
    #endif
    /* test:... */
    #if 0
    #elif 0
    /* test reset load and dump */
    if (count % 5000 == 0) {
        app_module_system_delay_set(2);
        app_module_system_status_set(app_module_system_reset);
    }
    #elif 0
    /* test timer */
    if (count == 1000)
        app_sys_timer_test();
    #elif 0
    /* test stopwatch */
    if (count == 1000)
        app_module_stopwatch_test();
    #elif 0
    /* test countdown */
    if (count == 1000)
        app_module_countdown_test();
    #elif 0
    /* test alarm group */
    if (count == 1000)
        app_module_remind_alarm_test();
    #elif 0
    /* test package... */
    if (count % 1000 == 0)
        app_thread_workqueue_test();
    #elif 0
    /* test protocol(7s later) */
    if (count == 1000 * 7) {
        app_module_protocol_t protocol = {
          //.notify.type = app_module_protocol_system_clock,
            .notify.type = app_module_protocol_trace_text,
        };
        app_module_protocol_notify(&protocol);
    }
    #else
    #endif
    #endif
}

#endif
