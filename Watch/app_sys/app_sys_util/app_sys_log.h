#ifndef APP_SYS_LOG_H
#define APP_SYS_LOG_H



/* 线程检查与线程子模组检查 */
#define APP_THREAD_CHECK    1
#define APP_MODULE_CHECK    1



/* 做最后一层适配:系统或者平台 */
#define APP_SYS_PRINT   APP_OS_PRINT



/* 全局宏打印开关(Enable:1,Disable:0) */
#define APP_SYS_LOG_GLOBAL_STATUS   1



/* 全局打印宏控检测,局部打印宏控检测 */
#if (APP_SYS_LOG_GLOBAL_STATUS && APP_SYS_LOG_LOCAL_STATUS)
/* DEBUG */
#if APP_SYS_LOG_LOCAL_LEVEL <= 0
#define APP_SYS_LOG_DEBUG    APP_SYS_PRINT
#else
#define APP_SYS_LOG_DEBUG
#endif
/* INFO */
#if APP_SYS_LOG_LOCAL_LEVEL <= 1
#define APP_SYS_LOG_INFO     APP_SYS_PRINT
#else
#define APP_SYS_LOG_INFO
#endif
/* WARN */
#if APP_SYS_LOG_LOCAL_LEVEL <= 2
#define APP_SYS_LOG_WARN     APP_SYS_PRINT
#else
#define APP_SYS_LOG_WARN
#endif
/* ERROR */
#if APP_SYS_LOG_LOCAL_LEVEL <= 3
#define APP_SYS_LOG_ERROR    APP_SYS_PRINT
#else
#define APP_SYS_LOG_ERROR
#endif
/*  */
#else
/* status:true带格式,false不带格式 */
#define APP_SYS_LOG_DEBUG
#define APP_SYS_LOG_INFO
#define APP_SYS_LOG_WARN
#define APP_SYS_LOG_ERROR
/*  */
#endif

#endif
