
#include <stdio.h>

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_source_manage.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_alarm.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_ext_mem.h"
#include "app_module_ext_mem_table.h"
#include "app_module_system.h"
#include "app_function_test.h"

#if 0
#elif APP_OS_IS_LINUX

#include <signal.h>
#include <sys/time.h>

static void software_timer_handler(int signal)
{
    //SIGALRM:      以系统真实的时间来计算
    //SIGVTALRM:    以该进程在用户态下花费的时间来计算
    //SIGPROF:      以该进程在用户态下和内核态下所费的时间来计算
    static uint32_t count = 0;count++;
    #if 1
    if (signal == SIGALRM)
        if (count % 1000 == 0)
            printf("linux singal SIGALRM 1 second\n");
    #endif
    if (signal == SIGALRM) {
        /* test clock */
        if (count % 1000 == 0)
            app_module_clock_test();
        /* test reset load and dump */
        #if 1
        if (count % 5000 == 0) {
            app_module_system_delay_set(2);
            app_module_system_status_set(app_module_system_reset);
        }
        #endif
        /* test alarm group */
        #if 0
        if (count == 1000)
            app_module_alarm_test();
        #endif
        /* test stopwatch */
        #if 0
        if (count % APP_MODULE_STOPWATCH_MSEC == 0)
            app_module_stopwatch_test();
        if (count == 1000) {
            app_module_stopwatch_reset();
            app_module_stopwatch_start();
        }
        #endif
        /* test countdown */
        #if 0
        if (count % APP_MODULE_COUNTDOWN_MSEC == 0)
            app_module_countdown_test;
        if (count == 1000) {
            app_module_countdown_reset();
            app_module_countdown_t countdown = {.hour = 0, .minute = 0, .second = 17};
            app_module_countdown_set(&countdown);
            app_module_countdown_start();
        }
        #endif
        /* test package... */
        #if 0
        if (count % 1000 == 0)
            app_thread_workqueue_test();
        #endif
    }
}

void software_timer_ready(void)
{
    /* 指定系统:Linux */
    // note:
    // struct itimerval 
    // {
    //      struct timeval it_interval; //计时间隔
    //      struct timeval it_value;    //延时时间(信号延时触发)
    // };
    // struct timeval 
    // {
    //      time_t      tv_sec;         //秒
    //      suseconds_t tv_usec;        //微妙, 1/1'000'000秒
    // };
    // 函数原型：
    // int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
    // which:
    //       ITIMER_REAL(它送出SIGALRM信号):         以系统真实的时间来计算
    //       ITIMER_VIRTUAL(它送出SIGVTALRM信号):    以该进程在用户态下花费的时间来计算
    //       ITIMER_PROF(它送出SIGPROF信号):         以该进程在用户态下和内核态下所费的时间来计算
    // old_value：通常用不上,设置为NULL,存储上一次触发记录
    // new_value: it_interval为计时间隔,it_value为延时时长
    // it_value为0是不会触发信号的,所以要能触发信号,it_value得大于0
    // it_interval为零,只会延时,不会定时
    struct itimerval old_value = {0};
    struct itimerval new_value = {0};
    //将信号关联指定的回调
    signal(SIGALRM,   software_timer_handler);
    //设置间隔触发时间
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 1000;//1ms
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 1;//不能为0
    //启用定时器
    setitimer(ITIMER_REAL, &new_value, &old_value);
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
    #if 1
    /* 启动APP调度策略 */
    app_thread_set_work_now();
    /* 测试中我们在主线程使用软件定时器信号量发送包裹,以达到模拟事件源的生成 */
    #if 1
    software_timer_ready();
    #endif
    /* 主线程滚动阻塞 */
    while (true)
        sleep(1);
    #else
    /* chunk刷新,将其都刷为0 */
    app_module_ext_mem_chunk_reflush();
    #endif
    return 0;
}
