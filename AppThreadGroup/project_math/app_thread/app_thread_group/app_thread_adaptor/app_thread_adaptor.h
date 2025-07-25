#ifndef APP_THREAD_ADAPTOR_H
#define APP_THREAD_ADAPTOR_H

/* APP thread adaptor: */

/*系统线程资源配置(静态线程)
 *1.主线程优先级
 *2.混合中断线程
 *3.混合常规线程
 *3.其余自定线程次之(让系统使用时间片轮转调度)
 *动态线程应该根据所生成的子线程做生成
 */

/* 线程体通配转接宏 */
/* 线程体通配转接定义 */
#if 0
#elif APP_OS_IS_WINDOWS
#define APP_THREAD_GROUP_HANDLER(app_thread_handler)    \
        DWORD  WINAPI  app_thread_handler (LPVOID lpParam)
typedef DWORD (WINAPI *app_thread_handler)(LPVOID lpParam);
#elif APP_OS_IS_LINUX
#define APP_THREAD_GROUP_HANDLER(app_thread_handler)    \
        void *   app_thread_handler (void *args)
typedef void * (*app_thread_handler)(void *args);
#elif APP_OS_IS_FREERTOS
#define APP_THREAD_GROUP_HANDLER(app_thread_handler)    \
        void *   app_thread_handler (void *pvParameters)
typedef void * (*app_thread_handler)(void *pvParameters);
#elif APP_OS_IS_ZEPHYR
#define APP_THREAD_GROUP_HANDLER(app_thread_handler)    \
        void *   app_thread_handler (void *args1, void *args2, void *args3)
typedef void * (*app_thread_handler)(void *args1, void *args2, void *args3);
#else
#error "unknown os type"
#endif

/* 主线程线程体声明 */
APP_THREAD_GROUP_HANDLER(app_thread_master_routine);
/* 静态子线程线程体声明 */
APP_THREAD_GROUP_HANDLER(app_thread_mix_irq_routine);
APP_THREAD_GROUP_HANDLER(app_thread_mix_custom_routine);
APP_THREAD_GROUP_HANDLER(app_thread_math_routine);

/* 静态线程信息声明 */
extern app_thread_t app_thread_master;
extern app_thread_t app_thread_mix_irq;
extern app_thread_t app_thread_mix_custom;
extern app_thread_t app_thread_math;

/*@brief 生成新的线程信息通过旧的线程信息
 *           旧线程的基本信息全部被新线程基础,除去执行回调
 *@param thread_old 旧线程的基本信息
 *@param thread_new 新线程的基本信息
 *@param handler    新线程的线程体
 */
void app_thread_group_create(app_thread_t *thread_old,
                             app_thread_t *thread_new,
                             app_thread_handler handler);

/*@brief 销毁新的线程信息
 *@param thread_new 新线程的基本信息
 */
void app_thread_group_destroy(app_thread_t *thread_new);

#endif
