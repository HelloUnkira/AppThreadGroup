#ifndef APP_THREAD_ADAPTOR_FREERTOS_H
#define APP_THREAD_ADAPTOR_FREERTOS_H

#if APP_OS_IS_FREERTOS

/* @线程体栈信息<Start> */

#define APP_THREAD_MASTER_STACK_SIZE  (1024)
static  StackType_t app_thread_master_stack[APP_THREAD_MASTER_STACK_SIZE];

#define APP_THREAD_MIX_IRQ_STACK_SIZE  (1024)
static  StackType_t app_thread_mix_irq_stack[APP_THREAD_MIX_IRQ_STACK_SIZE];

#define APP_THREAD_MIX_CUSTOM_STACK_SIZE  (1024)
static  StackType_t app_thread_mix_custom_stack[APP_THREAD_MIX_CUSTOM_STACK_SIZE];

#define APP_THREAD_MANAGE_STACK_SIZE   (1024)
static  StackType_t app_thread_manage_stack[APP_THREAD_MANAGE_STACK_SIZE];

#define APP_THREAD_LVGL_STACK_SIZE  (1024)
static  StackType_t app_thread_lvgl_stack[APP_THREAD_LVGL_STACK_SIZE];

#define APP_THREAD_JERRYSCRIPT_STACK_SIZE  (1024)
static  StackType_t app_thread_jerryscript_stack[APP_THREAD_JERRYSCRIPT_STACK_SIZE];

/* @线程体栈信息<End> */

/* @服务例程适配<Start> */

static void * app_thread_master_routine_adaptor(void *pvParameters)
{
    app_thread_master_routine();
}

static void * app_thread_mix_irq_routine_adaptor(void *pvParameters)
{
    app_thread_mix_irq_routine();
}

static void * app_thread_mix_custom_routine_adaptor(void *pvParameters)
{
    app_thread_mix_custom_routine();
}

static void * app_thread_manage_routine_adaptor(void *pvParameters)
{
    app_thread_manage_routine();
}

static void * app_thread_lvgl_routine_adaptor(void *pvParameters)
{
    app_thread_lvgl_routine();
}

static void * app_thread_jerryscript_routine_adaptor(void *pvParameters)
{
    app_thread_jerryscript_routine();
}

/* @服务例程适配<End> */

/* @线程体<Start> */

static app_thread_t app_thread_master = {
    .stack      = app_thread_master_stack,
    .stack_size = APP_THREAD_MASTER_STACK_SIZE,
    .priority   = 1,
    .task       = app_thread_master_routine_adaptor,
    .name       = "app_thread_master",
};

static app_thread_t app_thread_mix_irq = {
    .stack      = app_thread_mix_irq_stack,
    .stack_size = APP_THREAD_MIX_IRQ_STACK_SIZE,
    .priority   = 2,
    .task       = app_thread_mix_irq_routine_adaptor,
    .name       = "app_thread_mix_irq",
};

static app_thread_t app_thread_mix_custom = {
    .stack      = app_thread_mix_custom_stack,
    .stack_size = APP_THREAD_MIX_CUSTOM_STACK_SIZE,
    .priority   = 3,
    .task       = app_thread_mix_custom_routine_adaptor,
    .name       = "app_thread_mix_custom",
};

static app_thread_t app_thread_manage = {
    .stack      = app_thread_manage_stack,
    .stack_size = APP_THREAD_MANAGE_STACK_SIZE,
    .priority   = 5,
    .task       = app_thread_manage_routine_adaptor,
    .name       = "app_thread_manage",
};

static app_thread_t app_thread_lvgl = {
    .stack      = app_thread_lvgl_stack,
    .stack_size = APP_THREAD_LVGL_STACK_SIZE,
    .priority   = 4,
    .task       = app_thread_lvgl_routine_adaptor,
    .name       = "app_thread_lvgl",
};

static app_thread_t app_thread_jerryscript = {
    .stack      = app_thread_jerryscript_stack,
    .stack_size = APP_THREAD_JERRYSCRIPT_STACK_SIZE,
    .priority   = 5,
    .task       = app_thread_jerryscript_routine_adaptor,
    .name       = "app_thread_jerryscript",
};

/* @线程体<End> */

#endif

#endif
