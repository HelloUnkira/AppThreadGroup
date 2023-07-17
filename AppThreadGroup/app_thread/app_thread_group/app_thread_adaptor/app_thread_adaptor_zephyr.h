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

#define APP_THREAD_MANAGE_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_manage_stack, APP_THREAD_MANAGE_STACK_SIZE);

#define APP_THREAD_LVGL_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_lvgl_stack, APP_THREAD_LVGL_STACK_SIZE);

#define APP_THREAD_JERRYSCRIPT_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_jerryscript_stack, APP_THREAD_JERRYSCRIPT_STACK_SIZE);

/* @线程体栈信息<End> */

/* @服务例程适配<Start> */

static void * app_thread_master_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_master_routine();
}

static void * app_thread_mix_irq_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_mix_irq_routine();
}

static void * app_thread_mix_custom_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_mix_custom_routine();
}

static void * app_thread_manage_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_manage_routine();
}

static void * app_thread_lvgl_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_lvgl_routine();
}

static void * app_thread_jerryscript_routine_adaptor(void *args1, void *args2, void *args3)
{
    app_thread_jerryscript_routine();
}

/* @服务例程适配<End> */

/* @线程体<Start> */

static app_thread_t app_thread_master = {
    .stack      = app_thread_master_stack,
    .stack_size = APP_THREAD_MASTER_STACK_SIZE,
    .priority   = 1,
    .entry      = app_thread_master_routine_adaptor,
};

static app_thread_t app_thread_mix_irq = {
    .stack      = app_thread_mix_irq_stack,
    .stack_size = APP_THREAD_MIX_IRQ_STACK_SIZE,
    .priority   = 2,
    .entry      = app_thread_mix_irq_routine_adaptor,
};

static app_thread_t app_thread_mix_custom = {
    .stack      = app_thread_mix_custom_stack,
    .stack_size = APP_THREAD_MIX_CUSTOM_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_mix_custom_routine_adaptor,
};

static app_thread_t app_thread_manage = {
    .stack      = app_thread_manage_stack,
    .stack_size = APP_THREAD_MANAGE_STACK_SIZE,
    .priority   = 5,
    .entry      = app_thread_manage_routine_adaptor,
};

static app_thread_t app_thread_lvgl = {
    .stack      = app_thread_lvgl_stack,
    .stack_size = APP_THREAD_LVGL_STACK_SIZE,
    .priority   = 4,
    .entry      = app_thread_lvgl_routine_adaptor,
};

static app_thread_t app_thread_jerryscript = {
    .stack      = app_thread_jerryscript_stack,
    .stack_size = APP_THREAD_JERRYSCRIPT_STACK_SIZE,
    .priority   = 5,
    .entry      = app_thread_jerryscript_routine_adaptor,
};

/* @线程体<End> */

/*@brief OS扩展配置,线程就绪后配置
 */
void app_thread_os_extend(void)
{
}

#endif

#endif
