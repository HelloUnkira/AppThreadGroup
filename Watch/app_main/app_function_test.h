#ifndef APP_FUNCTION_TEST
#define APP_FUNCTION_TEST

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
    uint8_t tin[APP_MODULE_TRACE_LOG_MAX * 2] =
        {'0', '1', '2', '3', '4', '5', '6',
         '0', '1', '2', '3', '4', '5', '6',};
    while (1) {
        static uint32_t offset = 0;
        offset %= APP_MODULE_TRACE_LOG_MAX;
        app_module_trace_text_dump(tin + offset);
        uint8_t tout[APP_MODULE_TRACE_LOG_MAX] = {};
        app_module_trace_text_load(tout);
        if (memcmp(tout, tin + offset, APP_MODULE_TRACE_LOG_MAX) != 0)
            printf("track log error\n");
        offset++;
        sleep(1);
    }
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

#endif
