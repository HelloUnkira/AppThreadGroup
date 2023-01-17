#ifndef APP_THREAD_ADAPTOR_ZEPHYR_H
#define APP_THREAD_ADAPTOR_ZEPHYR_H

#if APP_OS_IS_ZEPHYR

/* @线程体栈信息<Start> */

#define APP_THREAD_MASTER_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_master_stack, APP_THREAD_MASTER_STACK_SIZE);

#define APP_THREAD_MIX_IRQ_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_mix_irq_stack, APP_THREAD_MIX_IRQ_STACK_SIZE);

#define APP_THREAD_MIX_CUSTOM_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_mix_custom_stack, APP_THREAD_MIX_CUSTOM_STACK_SIZE);

#define APP_THREAD_SOURCE_MANAGE_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_source_manage_stack, APP_THREAD_SOURCE_MANAGE_STACK_SIZE);

#define APP_THREAD_PROTOCOL_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_protocol_stack, APP_THREAD_PROTOCOL_STACK_SIZE);

#define APP_THREAD_LVGL_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_lvgl_stack, APP_THREAD_LVGL_STACK_SIZE);

/* @线程体栈信息<End> */

/* @服务例程适配<Start> */

void * app_thread_master_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_master_routine();
}

void * app_thread_mix_irq_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_mix_irq_routine();
}

void * app_thread_mix_custom_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_mix_custom_routine();
}

void * app_thread_source_manage_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_source_manage_routine();
}

void * app_thread_source_protocol_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_protocol_routine();
}

void * app_thread_lvgl_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_lvgl_routine();
}

/* @服务例程适配<End> */

/* @线程体<Start> */

app_thread_t app_thread_master = {
    .stack      = app_thread_master_stack,
    .stack_size = APP_THREAD_MASTER_STACK_SIZE,
    .priority   = 1,
    .entry      = app_thread_master_routine_adaptor,
};

app_thread_t app_thread_mix_irq = {
    .stack      = app_thread_mix_irq_stack,
    .stack_size = APP_THREAD_MIX_IRQ_STACK_SIZE,
    .priority   = 2,
    .entry      = app_thread_mix_irq_routine_adaptor,
};

app_thread_t app_thread_mix_custom = {
    .stack      = app_thread_mix_custom_stack,
    .stack_size = APP_THREAD_MIX_CUSTOM_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_mix_custom_routine_adaptor,
};

app_thread_t app_thread_source_manage = {
    .stack      = app_thread_source_manage_stack,
    .stack_size = APP_THREAD_SOURCE_MANAGE_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_source_manage_routine_adaptor,
};

app_thread_t app_thread_protocol = {
    .stack      = app_thread_protocol_stack,
    .stack_size = APP_THREAD_PROTOCOL_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_protocol_routine_adaptor,
};

app_thread_t app_thread_lvgl = {
    .stack      = app_thread_lvgl_stack,
    .stack_size = APP_THREAD_LVGL_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_lvgl_routine_adaptor,
};

/* @线程体<End> */

#endif

#endif
