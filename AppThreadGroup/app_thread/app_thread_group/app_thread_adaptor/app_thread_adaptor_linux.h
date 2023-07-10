#ifndef APP_THREAD_ADAPTOR_LINUX_H
#define APP_THREAD_ADAPTOR_LINUX_H

#if APP_OS_IS_LINUX

/* @服务例程适配<Start> */

static void * app_thread_master_routine_adaptor(void *args)
{
    app_thread_master_routine();
}

static void * app_thread_mix_irq_routine_adaptor(void *args)
{
    app_thread_mix_irq_routine();
}

static void * app_thread_mix_custom_routine_adaptor(void *args)
{
    app_thread_mix_custom_routine();
}

static void * app_thread_manage_routine_adaptor(void *args)
{
    app_thread_manage_routine();
}

static void * app_thread_lvgl_routine_adaptor(void *args)
{
    app_thread_lvgl_routine();
}

static void * app_thread_jerryscript_routine_adaptor(void *args)
{
    app_thread_jerryscript_routine();
}

/* @服务例程适配<End> */

/* @线程体<Start> */

static app_thread_t app_thread_master = {
   .priority = 1,
   .routine  = app_thread_master_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_mix_irq = {
   .priority = 2,
   .routine  = app_thread_mix_irq_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_mix_custom = {
   .priority = 3,
   .routine  = app_thread_mix_custom_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_manage = {
   .priority = 5,
   .routine  = app_thread_manage_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_lvgl = {
   .priority = 4,
   .routine  = app_thread_lvgl_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_jerryscript = {
   .priority = 5,
   .routine  = app_thread_jerryscript_routine_adaptor,
   .args     = NULL,
};

/* @线程体<End> */

#endif

#endif
