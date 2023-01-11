#ifndef APP_FUNCTION_TEST
#define APP_FUNCTION_TEST

/* 工作队列测试<data start> */
static uint8_t app_thread_mix_work_arg = 0;
static void app_thread_mix_work_routine(void *parameter)
{
    (*(uint8_t *)parameter)++;
    printf("misc_work_routine exec:%d\n", *(uint8_t *)parameter);
}

app_thread_source_manage_work_t app_thread_source_manage_work_entry = {
    .routine   =  app_thread_mix_work_routine,
    .parameter = &app_thread_mix_work_arg,
};
app_thread_mix_custom_work_t app_thread_mix_custom_work_entry = {
    .routine   =  app_thread_mix_work_routine,
    .parameter = &app_thread_mix_work_arg,
};
app_thread_mix_irq_work_t app_thread_mix_irq_work_entry = {
    .routine   =  app_thread_mix_work_routine,
    .parameter = &app_thread_mix_work_arg,
};
/* 工作队列测试<data end> */

/*@brief 工作队列测试
 */
static inline void app_module_workqueue_test(void)
{
    {
        app_package_t package = {
            .send_tid = app_thread_id_unknown,
            .recv_tid = app_thread_id_mix_custom,
            .module   = app_thread_mix_custom_work,
            .event    = 0,
            .dynamic  = false,
            .size     = sizeof(app_thread_mix_custom_work_t),
            .data     = &app_thread_mix_custom_work_entry,
        };
        app_thread_package_notify(&package);
    }
    {
        app_package_t package = {
            .send_tid = app_thread_id_unknown,
            .recv_tid = app_thread_id_mix_irq,
            .module   = app_thread_mix_irq_work,
            .event    = 0,
            .dynamic  = false,
            .size     = sizeof(app_thread_mix_irq_work_t),
            .data     = &app_thread_mix_irq_work_entry,
        };
        app_thread_package_notify(&package);
    }
    {
        app_package_t package = {
            .send_tid = app_thread_id_unknown,
            .recv_tid = app_thread_id_source_manage,
            .module   = app_thread_source_manage_work,
            .event    = 0,
            .dynamic  = false,
            .size     = sizeof(app_thread_source_manage_work_t),
            .data     = &app_thread_source_manage_work_entry,
        };
        app_thread_package_notify(&package);
    }
}

/*@brief 时钟模组测试
 */
static inline void app_module_clock_test(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_mix_custom,
        .module   = app_thread_mix_custom_clock,
        .event    = app_thread_mix_custom_clock_timestamp_update,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_thread_package_notify(&package);
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

/*@brief 秒表模组测试
 */
static inline void app_module_stopwatch_test(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_mix_custom,
        .module   = app_thread_mix_custom_stopwatch,
        .event    = app_thread_mix_custom_stopwatch_msec_update,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_thread_package_notify(&package);
}

/*@brief 倒计时模组测试
 */
static inline void app_module_countdown_test(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_mix_custom,
        .module   = app_thread_mix_custom_countdown,
        .event    = app_thread_mix_custom_countdown_msec_update,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_thread_package_notify(&package);
}

/*@brief 外存chunk刷新测试
 */
static inline void app_module_ext_mem_chunk_reflush(void)
{
    uint8_t block[4096] = {0};
    for (uint32_t idx = 0; idx < sizeof(block); block[idx] = ~0, idx++);
    /* 打开并关闭一次,保证chunk文件存在 */
    fclose(fopen("thread_master", "wb"));
    fclose(fopen("thread_mix_irq", "wb"));
    fclose(fopen("thread_mix_custom", "wb"));
    fclose(fopen("thread_source_manage", "wb"));
    /*  */
    const app_module_ext_mem_t *ext_mem1 = app_module_ext_mem_find_by_name("thread_master");
    const app_module_ext_mem_t *ext_mem2 = app_module_ext_mem_find_by_name("thread_mix_irq");
    const app_module_ext_mem_t *ext_mem3 = app_module_ext_mem_find_by_name("thread_mix_custom");
    const app_module_ext_mem_t *ext_mem4 = app_module_ext_mem_find_by_name("thread_source_manage");
    /*  */
    printf("reflush start...\n");
    for (uint32_t idx = 0; idx < ext_mem1->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem1, idx, block, sizeof(block));
    for (uint32_t idx = 0; idx < ext_mem2->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem2, idx, block, sizeof(block));
    for (uint32_t idx = 0; idx < ext_mem3->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem3, idx, block, sizeof(block));
    for (uint32_t idx = 0; idx < ext_mem4->chunk_size; idx += sizeof(block))
        app_module_ext_mem_write(ext_mem4, idx, block, sizeof(block));
    printf("reflush end...\n");
}

#endif
