#ifndef APP_FUNCTION_TEST
#define APP_FUNCTION_TEST

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_timer.h"
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_data_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_alarm.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_ext_mem.h"
#include "app_module_ext_mem_table.h"
#include "app_module_system.h"
#include "app_module_trace.h"

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

/*@brief 闹钟模组测试
 */
static inline void app_module_alarm_test(void)
{
    static app_module_alarm_t array[5] = {0};
    uint32_t alarm_group_id = app_module_alarm_group_register(array, 5);
    app_module_alarm_t alarm1 = {.clock_base.year   = 2023,
                                 .clock_base.month  = 1,
                                 .clock_base.day    = 1,
                                 .clock_base.second = 2,
                                 .onoff = 1,
                                 .field_month = 0b00000001000,
                                 .field_week = 0b0000100,
                                 .type = app_module_alarm_custom,};
    app_module_alarm_t alarm2 = {.clock_base.year   = 2023,
                                 .clock_base.month  = 1,
                                 .clock_base.day    = 1,
                                 .clock_base.second = 4,
                                 .onoff = 1,
                                 .field_month = 0b00000001000,
                                 .field_week = 0b0000100,
                                 .type = app_module_alarm_custom,};
    app_module_alarm_t alarm3 = {.clock_base.year   = 2023,
                                 .clock_base.month  = 1,
                                 .clock_base.day    = 1,
                                 .clock_base.second = 5,
                                 .onoff = 1,
                                 .repeat = 3,
                                 .type = app_module_alarm_repeat,};
    app_module_clock_to_utc(&alarm1.clock_base);
    app_module_clock_to_utc(&alarm2.clock_base);
    app_module_clock_to_utc(&alarm3.clock_base);
    app_module_clock_to_week(&alarm1.clock_base);
    app_module_clock_to_week(&alarm2.clock_base);
    app_module_clock_to_week(&alarm3.clock_base);

    app_module_alarm_add(alarm_group_id, &alarm1);
    app_module_alarm_add(alarm_group_id, &alarm2);
    app_module_alarm_add(alarm_group_id, &alarm3);
}

/*@brief 日志转储测试
 */
static inline void app_module_trace_test(void)
{
    uint8_t tin[APP_MODULE_TRACE_LOG_MAX * 2] ={0};
    for (uint32_t idx = 0; idx < APP_MODULE_TRACE_LOG_MAX; idx++)
        tin[idx] = tin[idx + APP_MODULE_TRACE_LOG_MAX] = '0' + idx % 10;

    static uint32_t offset = 0;
    offset %= APP_MODULE_TRACE_LOG_MAX;
    app_module_trace_text_dump(tin + offset);
    uint8_t tout[APP_MODULE_TRACE_LOG_MAX] = {};
    app_module_trace_text_load(tout);
    if (memcmp(tout, tin + offset, APP_MODULE_TRACE_LOG_MAX) != 0)
        printf("track log error\n");
    offset++;
}

/*@brief 外存chunk刷新测试
 */
static inline void app_module_ext_mem_chunk_reflush(void)
{
    uint8_t block[4096] = {0};
    for (uint32_t idx = 0; idx < sizeof(block); block[idx] = ~0, idx++);
    /* 打开并关闭一次,保证chunk文件存在 */
    fclose(fopen("mix_chunk_small", "wb"));
    fclose(fopen("mix_chunk_large", "wb"));
    /*  */
    const app_module_ext_mem_t *ext_mem1 = app_module_ext_mem_find_by_name("mix_chunk_small");
    const app_module_ext_mem_t *ext_mem2 = app_module_ext_mem_find_by_name("mix_chunk_large");
    /*  */
    printf("reflush start...\n");
    for (uint32_t idx = 0; idx < ext_mem1->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem1, idx, block, sizeof(block));
    for (uint32_t idx = 0; idx < ext_mem2->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem2, idx, block, sizeof(block));
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
