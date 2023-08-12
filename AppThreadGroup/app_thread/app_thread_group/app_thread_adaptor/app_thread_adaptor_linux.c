/*实现目标:
 *    Linux线程适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_clock.h"

#if APP_OS_IS_LINUX

/* @线程体<Start> */

app_thread_t app_thread_master = {
   .priority = 1,
   .routine  = app_thread_master_routine,
   .args     = NULL,
};

app_thread_t app_thread_mix_irq = {
   .priority = 2,
   .routine  = app_thread_mix_irq_routine,
   .args     = NULL,
};

app_thread_t app_thread_mix_custom = {
   .priority = 3,
   .routine  = app_thread_mix_custom_routine,
   .args     = NULL,
};

app_thread_t app_thread_manage = {
   .priority = 5,
   .routine  = app_thread_manage_routine,
   .args     = NULL,
};

app_thread_t app_thread_lvgl = {
   .priority = 4,
   .routine  = app_thread_lvgl_routine,
   .args     = NULL,
};

app_thread_t app_thread_jerryscript = {
   .priority = 5,
   .routine  = app_thread_jerryscript_routine,
   .args     = NULL,
};

/* @线程体<End> */

/*@brief     生成新的线程信息通过旧的线程信息
 *           旧线程的基本信息全部被新线程基础,除去执行回调
 *@param[in] thread_old 旧线程的基本信息
 *@param[in] thread_new 新线程的基本信息
 *@param[in] handler    新线程的线程体
 */
void app_thread_group_create(app_thread_t *thread_old,
                             app_thread_t *thread_new,
                             app_thread_handler handler)
{
    memcpy(thread_new, thread_old, sizeof(app_thread_t));
    thread_new->routine = handler;
}

/*@brief     销毁新的线程信息
 *@param[in] thread_new 新线程的基本信息
 */
void app_thread_group_destroy(app_thread_t *thread_new)
{
}

/*@brief OS扩展配置,线程就绪后配置
 */
void app_thread_group_extend(void)
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
    APP_SYS_LOG_WARN("dtime:%d/%d/%d %d:%d:%d utc:%d days:%d",
                      clock.year, clock.month,  clock.day,
                      clock.hour, clock.minute, clock.second,
                      clock.utc,  p->tm_yday);
    
    app_module_clock_set_system_clock(&clock);
}

#endif
