#ifndef APP_EXT_OS_H
#define APP_EXT_OS_H

/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

/* APP OS adaptor: */
#define APP_OS_IS_WINDOWS   1
#define APP_OS_IS_LINUX     0
#define APP_OS_IS_ZEPHYR    0
#define APP_OS_IS_FREERTOS  0

/* OS custom adaptor */
#include "app_ext_os_windows.h"
#include "app_ext_os_linux.h"
#include "app_ext_os_zephyr.h"
#include "app_ext_os_freertos.h"
/* OS ext mem adaptor */
#include "app_ext_os_ext_mem.h"

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void);

typedef enum {
    app_thread_default = 0,     /* 占位符 */
    app_thread_static,          /* 线程准备(内存常驻) */
    app_thread_create,          /* 线程创建 */
    app_thread_destroy,         /* 线程销毁 */
    app_thread_suspend,         /* 线程悬挂 */
    app_thread_resume,          /* 线程恢复 */
} app_thread_option_t;

/*@brief        线程操作流程集合
 *@param[in]    thread 实例
 *@param[in]    option 实例动作
 */
void app_thread_process(app_thread_t *thread, app_thread_option_t option);

typedef enum {
    app_sem_default = 0,    /* 占位符 */
    app_sem_static,         /* 信号量准备(内存常驻) */
    app_sem_create,         /* 信号量创建 */
    app_sem_destroy,        /* 信号量销毁 */
    app_sem_take,           /* 信号量获取 */
    app_sem_give,           /* 信号量释放 */
} app_sem_option_t;

/*@brief        信号量操作流程集合
 *@param[in]    sem    实例
 *@param[in]    option 实例动作
 */
void app_sem_process(app_sem_t *sem, app_sem_option_t option);

typedef enum {
    app_mutex_default = 0,  /* 占位符 */
    app_mutex_static,       /* 互斥锁准备(内存常驻) */
    app_mutex_create,       /* 互斥锁创建 */
    app_mutex_destroy,      /* 互斥锁销毁 */
    app_mutex_take,         /* 互斥锁获取 */
    app_mutex_give,         /* 互斥锁释放 */
} app_mutex_option_t;

/*@brief        互斥锁操作流程集合
 *@param[in]    mutex  实例
 *@param[in]    option 实例动作
 */
void app_mutex_process(app_mutex_t *mutex, app_mutex_option_t option);

typedef enum {
    app_critical_default = 0,   /* 占位符 */
    app_critical_static,        /* 临界区准备(内存常驻) */
    app_critical_create,        /* 临界区创建 */
    app_critical_destroy,       /* 临界区销毁 */
    app_critical_enter,         /* 临界区进入 */
    app_critical_exit,          /* 临界区退出 */
} app_critical_option_t;

/*@brief        临界区操作流程集合
 *@param[in]    critical 实例
 *@param[in]    option   实例动作
 */
void app_critical_process(app_critical_t *critical, app_critical_option_t option);

/*@brief        内存分配
 *@param[in]    size 分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size);

/*@brief        内存分配
 *@param[in]    pointer 分配空间回收
 *@param[in]    size    分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size);

/*@brief        内存释放
 *@param[in]    pointer 分配空间
 */
void app_mem_free(void *pointer);

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms);

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us);

/*@brief 计算一段代码的延时时间(us)
 */
double app_execute_us(app_execute_us_t *execute_us, bool run);

/*@brief 重启
 */
void app_os_reset(void);

#endif
