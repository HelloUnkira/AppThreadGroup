/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_LINUX

/*@brief 初始化OS
 */
void app_os_ready(void)
{
}

/*@brief 调度OS
 */
void app_os_schedule(void)
{
}

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void)
{
    /* 模拟器环境不存在硬件中断环境 */
    return true;
}

/*@brief 线程操作流程集合
 *@param thread 实例
 *@param option 实例动作
 */
void app_thread_process(app_thread_t *thread, app_thread_option_t option)
{
    switch (option) {
    case app_thread_static: {
        thread->stacksize = 8 * 1024 * 1024;
        struct sched_param param = {.sched_priority = thread->priority};
        pthread_attr_init(&thread->attribute);
        pthread_attr_setschedparam(&thread->attribute,  &param);
        //pthread_attr_setschedpolicy(&thread->attribute,  SCHED_RR);
        pthread_attr_setguardsize(&thread->attribute,    7 * 1024 *1024);
        pthread_attr_setstacksize(&thread->attribute,    thread->stacksize);
        pthread_attr_setinheritsched(&thread->attribute, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setdetachstate(&thread->attribute,  PTHREAD_CREATE_JOINABLE);
        pthread_create(&thread->thread,
                       &thread->attribute,
                        thread->routine,
                        thread->args);
        break;
    }
    default:
        APP_OS_PRINT("app_thread_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 信号量操作流程集合
 *@param sem    实例
 *@param option 实例动作
 */
void app_sem_process(app_sem_t *sem, app_sem_option_t option)
{
    switch (option) {
    case app_sem_static: {
        sem_init(&sem->sem, 0, 100);
        break;
    }
    case app_sem_take: {
        sem_post(&sem->sem);
        break;
    }
    case app_sem_give: {
        sem_wait(&sem->sem);
        break;
    }
    default:
        APP_OS_PRINT("app_sem_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 互斥锁操作流程集合
 *@param mutex  实例
 *@param option 实例动作
 */
void app_mutex_process(app_mutex_t *mutex, app_mutex_option_t option)
{
    switch (option) {
    case app_mutex_static: {
        pthread_mutex_init(&mutex->mutex, NULL);
        break;
    }
    case app_mutex_take: {
        if (app_os_not_in_irq())
            pthread_mutex_lock(&mutex->mutex);
        break;
    }
    case app_mutex_give: {
        if (app_os_not_in_irq())
            pthread_mutex_unlock(&mutex->mutex);
        break;
    }
    default:
        APP_OS_PRINT("app_mutex_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 临界区操作流程集合
 *@param critical 实例
 *@param option   实例动作
 */
void app_critical_process(app_critical_t *critical, app_critical_option_t option)
{
    switch (option) {
    case app_critical_static: {
        /* Linux不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        app_mutex_process(&critical->mutex, app_mutex_static);
        critical->mutex_ready = true;
        break;
    }
    case app_critical_enter: {
        /* Linux不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        if (critical->mutex_ready)
            app_mutex_process(&critical->mutex, app_mutex_take);
        break;
    }
    case app_critical_exit: {
        /* Linux不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        if (critical->mutex_ready)
            app_mutex_process(&critical->mutex, app_mutex_give);
        break;
    }
    default:
        APP_OS_PRINT("app_critical_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 内存分配
 *@param size 分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size)
{
    return malloc(size);
}

/*@brief 内存分配
 *@param pointer 分配空间回收
 *@param size    分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size)
{
    return realloc(pointer, size);
}

/*@brief 内存释放
 *@param pointer 分配空间
 */
void app_mem_free(void *pointer)
{
    free(pointer);
}

#if 0 /* 暂存:1ms信号定时 */
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
#endif

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    usleep(ms * 1000);
}

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
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

/*@brief 计算一个延时点(us)
 */
uint64_t app_execute_us(void)
{
    timespec tick;
    clock_gettime(CLOCK_MONOTONIC, &tick);
    return (double)(tick.tv_sec) * 1000 * 1000 + (tick.tv_nsec) / 1000.0;
}

#endif
