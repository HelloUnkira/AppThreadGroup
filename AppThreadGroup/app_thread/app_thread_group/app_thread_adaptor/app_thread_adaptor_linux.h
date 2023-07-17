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

/*@brief OS扩展配置,线程就绪后配置
 */
void app_thread_os_extend(void)
{
    time_t timep;
    struct tm *p;
    time (&timep);
    p=gmtime(&timep);
    
    app_module_clock_t clock = {
        .year   = 1900+p->tm_year,      /*获取当前年份,从1900开始，所以要加1900*/
        .month  = 1+p->tm_mon,          /*获取当前月份,范围是0-11,所以要加1*/
        .day    = p->tm_mday,           /*获取当前月份日数,范围是1-31*/
        .hour   = p->tm_hour,           /*获取当前时,这里获取西方的时间,刚好相差八个小时*/
        .minute = p->tm_min,
        .second = p->tm_sec,
        .zone_sec = + 8 * 60 * 60,
        .is_24  = false,
    };
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
    
    /*获取当前年份,从1900开始,所以要加1900*/
    /*获取当前月份,范围是0-11,所以要加1*/
    /*获取当前月份日数,范围是1-31*/
    /*获取当前时,这里获取西方的时间,刚好相差八个小时*/
    /*获取当前分*/
    /*获取当前秒*/
    /*从今年1月1日算起至今的天数，范围为0-365*/
    printf("dtime:%d/%d/%d %d:%d:%d utc:%d days:%d\n",
            clock.year, clock.month,  clock.day,
            clock.hour, clock.minute, clock.second,
            clock.utc,  p->tm_yday);
    
    app_module_clock_set_system_clock(&clock);
}

#endif

#endif
