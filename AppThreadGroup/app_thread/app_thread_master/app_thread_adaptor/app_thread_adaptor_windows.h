#ifndef APP_THREAD_ADAPTOR_WINDOWS_H
#define APP_THREAD_ADAPTOR_WINDOWS_H

#if APP_OS_IS_WINDOWS

/* @服务例程适配<Start> */

static DWORD WINAPI app_thread_master_routine_adaptor(LPVOID lpParam)
{
    app_thread_master_routine();
}

static DWORD WINAPI app_thread_mix_irq_routine_adaptor(LPVOID lpParam)
{
    app_thread_mix_irq_routine();
}

static DWORD WINAPI app_thread_mix_custom_routine_adaptor(LPVOID lpParam)
{
    app_thread_mix_custom_routine();
}

static DWORD WINAPI app_thread_manage_routine_adaptor(LPVOID lpParam)
{
    app_thread_manage_routine();
}

static DWORD WINAPI app_thread_lvgl_routine_adaptor(LPVOID lpParam)
{
    app_thread_lvgl_routine();
}

static DWORD WINAPI app_thread_jerryscript_routine_adaptor(LPVOID lpParam)
{
    app_thread_jerryscript_routine();
}

/* @服务例程适配<End> */

/* @线程体<Start> */

static app_thread_t app_thread_master = {
   .priority = THREAD_PRIORITY_HIGHEST,
   .routine  = app_thread_master_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_mix_irq = {
   .priority = THREAD_PRIORITY_ABOVE_NORMAL,
   .routine  = app_thread_mix_irq_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_mix_custom = {
   .priority = THREAD_PRIORITY_NORMAL,
   .routine  = app_thread_mix_custom_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_manage = {
   .priority = THREAD_PRIORITY_BELOW_NORMAL,
   .routine  = app_thread_manage_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_lvgl = {
   .priority = THREAD_PRIORITY_NORMAL,
   .routine  = app_thread_lvgl_routine_adaptor,
   .args     = NULL,
};

static app_thread_t app_thread_jerryscript = {
   .priority = THREAD_PRIORITY_BELOW_NORMAL,
   .routine  = app_thread_jerryscript_routine_adaptor,
   .args     = NULL,
};

/* @线程体<End> */

#endif

#endif
