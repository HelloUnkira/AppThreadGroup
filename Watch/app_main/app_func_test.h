#ifndef APP_FUNCTION_TEST
#define APP_FUNCTION_TEST

#include "app_ext_lib.h"

#include "app_sys_timer.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_data_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_system.h"
#include "app_sys_trace_text.h"

void app_module_timer_test_callback(void *timer)
{
    app_sys_timer_t *local = timer;
    uint32_t *data = local->user_data;
    
    printf("timer %d callback %d\n", data[0], data[1]++);
    
    if (data[1] == 3)
        app_sys_timer_stop(timer);
}

/*@brief 软件定时器模组测试
 */
static inline void app_module_timer_test(void)
{
    static uint32_t label_1[2] = {1, 0};
    static uint32_t label_2[2] = {2, 0};
    static uint32_t label_3[2] = {3, 0};
    static app_sys_timer_t timer1 = {
        .expired   = app_module_timer_test_callback,
        .user_data = &label_1,
        .peroid    = 1000,
        .reload    = true};
    static app_sys_timer_t timer2 = {
        .expired   = app_module_timer_test_callback,
        .user_data = &label_2,
        .peroid    = 2000,
        .reload    = true};
    static app_sys_timer_t timer3 = {
        .expired   = app_module_timer_test_callback,
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

/*@brief 日志转储测试
 */
static inline void app_sys_trace_test(void)
{
    uint8_t tin[APP_MODULE_TRACE_TEXT_MAX * 2] ={0};
    for (uint32_t idx = 0; idx < APP_MODULE_TRACE_TEXT_MAX; idx++)
        tin[idx] = tin[idx + APP_MODULE_TRACE_TEXT_MAX] = '0' + idx % 10;

    static uint32_t offset = 0;
    offset %= APP_MODULE_TRACE_TEXT_MAX;
    app_sys_trace_text_dump(tin + offset, true);
    uint8_t tout[APP_MODULE_TRACE_TEXT_MAX] = {0};
    app_sys_trace_text_load(tout);
    if (memcmp(tout, tin + offset, APP_MODULE_TRACE_TEXT_MAX) != 0)
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

static void app_main_fake_hard_clock_irq(void)
{
    //SIGALRM:      以系统真实的时间来计算
    //SIGVTALRM:    以该进程在用户态下花费的时间来计算
    //SIGPROF:      以该进程在用户态下和内核态下所费的时间来计算
    static uint32_t count = 0;count++;
    /* linux system 1 ms tick */
    if (count % 1000 == 0)
        printf("signal 1 second handler\n");
    /* 1msec system update: */
    app_module_system_1msec_update(count);
    /* ........ */
    /* test:... */
    /* ........ */
    /* test reset load and dump */
    #if 0
    if (count % 5000 == 0) {
        app_module_system_delay_set(2);
        app_module_system_status_set(app_module_system_reset);
    }
    #endif
    /* test timer */
    #if 0
    if (count == 1000)
        app_module_timer_test();
    #endif
    /* test alarm group */
    #if 0
    if (count == 1000)
        app_module_alarm_test();
    #endif
    /* test stopwatch */
    #if 0
    if (count == 1000)
        app_module_stopwatch_test();
    #endif
    /* test countdown */
    #if 0
    if (count == 1000)
        app_module_countdown_test();
    #endif
    /* test package... */
    #if 0
    if (count % 1000 == 0)
        app_thread_workqueue_test();
    #endif
}

#endif
