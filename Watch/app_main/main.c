
#include "app_func_test.h"

#if 0
#elif APP_OS_IS_WINDOWS

static void app_main_update_sys_clock(void)
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

#elif APP_OS_IS_LINUX

static void app_main_update_sys_clock(void)
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

#elif APP_OS_IS_ZEPHYR
#error "arch os not adaptor yet"
#elif APP_OS_IS_FREERTOS
#error "arch os not adaptor yet"
#else
#error "unknown os"
#endif

int main(int argc, char *argv[])
{
    /* 启动APP调度策略 */
    app_thread_group_run();
    /* 主线程滚动阻塞 */
    #if 0
    #elif 1
    while (true) {
        #if 0
        /* 测试日志追踪 */
        app_sys_trace_test();
        #endif
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        /* 我是实在没想到这种方式 */
        /* 居然是最简单的做法...... */
        app_main_fake_hard_clock_irq();
        app_delay_us(1000);
        /* 更新系统时钟(5s later) */
        {
            static uint64_t count = 0;
            if (count++ == 5 * 1000)
                app_main_update_sys_clock();
        }
    }
    #elif 0
    /* chunk刷新,将其都刷为0 */
    app_sys_ext_mem_chunk_reflush();
    #else
    #endif
    return 0;
}
