
#include <stdio.h>
#include "app_function_test.h"

#if 0
#elif APP_OS_IS_LINUX

#include <unistd.h> 
#include <signal.h>
#include <sys/time.h>
#include <time.h> 

/* 问题点:使用SIGALRM,主线程有时候无法接收到该信号 */
/*        并且信号会一直丢弃,以至于线程组无法工作 */

static void software_timer_handler(int signal)
{
    //SIGALRM:      以系统真实的时间来计算
    //SIGVTALRM:    以该进程在用户态下花费的时间来计算
    //SIGPROF:      以该进程在用户态下和内核态下所费的时间来计算
    if (signal == SIGALRM)
        app_main_fake_hard_clock_irq();
}

static void software_timer_ready(void)
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
    signal(SIGALRM, software_timer_handler);
    //设置间隔触发时间
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 1000;//1ms
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 1;//不能为0
    //启用定时器
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

static void app_main_usleep(uint64_t ns)
{
	struct timeval begin;
	struct timeval now;
	uint64_t past_us = 0;
	gettimeofday(&begin,NULL);
	now.tv_sec = begin.tv_sec;
	now.tv_usec = begin.tv_usec;
	while(past_us < ns)
	{
		gettimeofday(&now,NULL);
        past_us  = now.tv_sec;
        past_us -= begin.tv_sec;
        past_us *= 1000000;
        past_us += now.tv_usec;
        past_us -= begin.tv_usec;
	}
}

static void app_main_update_sys_time(void)
{
    time_t timep;
    struct tm *p;
    time (&timep);
    p=gmtime(&timep);
    
    printf("年:%d\n",1900+p->tm_year);/*获取当前年份,从1900开始，所以要加1900*/
    printf("月:%d\n",1+p->tm_mon);/*获取当前月份,范围是0-11,所以要加1*/
    printf("日:%d\n",p->tm_mday);/*获取当前月份日数,范围是1-31*/
    printf("时::%d\n",8+p->tm_hour);/*获取当前时,这里获取西方的时间,刚好相差八个小时*/
    printf("分:%d\n",p->tm_min); /*获取当前分*/
    printf("秒:%d\n",p->tm_sec); /*获取当前秒*/
    printf("天:%d\n",p->tm_yday); /*从今年1月1日算起至今的天数，范围为0-365*/
    
    app_module_clock_t clock = {
        .year   = 1900+p->tm_year,      /*获取当前年份,从1900开始，所以要加1900*/
        .month  = 1+p->tm_mon,          /*获取当前月份,范围是0-11,所以要加1*/
        .day    = p->tm_mday,           /*获取当前月份日数,范围是1-31*/
        .hour   = 8+p->tm_hour,         /*获取当前时,这里获取西方的时间,刚好相差八个小时*/
        .minute = p->tm_min,
        .second = p->tm_sec,
    };
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
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
        app_module_trace_test();
        #endif
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        /* 我是实在没想到这种方式 */
        /* 居然是最简单的做法...... */
        app_main_fake_hard_clock_irq();
        app_main_usleep(1000);
        /* 更新系统时钟(5s later) */
        {
            static uint64_t count = 0;
            if (count++ == 5 * 1000)
                app_main_update_sys_time();
        }
    }
    #elif 0
    /* chunk刷新,将其都刷为0 */
    app_module_ext_mem_chunk_reflush();
    #else
    #endif
    return 0;
}
