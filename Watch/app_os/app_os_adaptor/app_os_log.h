#ifndef APP_OS_LOG_H
#define APP_OS_LOG_H

/* 全局宏打印开关 */
#define APP_OS_LOG_GLOBAL_STATUS    1   /* Enable:1,Disable:0 */

/* 全局打印宏控检测,局部打印宏控检测 */
#if (APP_OS_LOG_GLOBAL_STATUS && APP_OS_LOG_LOCAL_STATUS)
/* DEBUG */
#if APP_OS_LOG_LOCAL_LEVEL <= 0
#define APP_OS_LOG_DEBUG    APP_OS_PRINT
#else
#define APP_OS_LOG_DEBUG
#endif
/* INFO */
#if APP_OS_LOG_LOCAL_LEVEL <= 1
#define APP_OS_LOG_INFO     APP_OS_PRINT
#else
#define APP_OS_LOG_INFO
#endif
/* WARN */
#if APP_OS_LOG_LOCAL_LEVEL <= 2
#define APP_OS_LOG_WARN     APP_OS_PRINT
#else
#define APP_OS_LOG_WARN
#endif
/* ERROR */
#if APP_OS_LOG_LOCAL_LEVEL <= 3
#define APP_OS_LOG_ERROR    APP_OS_PRINT
#else
#define APP_OS_LOG_ERROR
#endif
/*  */
#else

/* status:true带格式,false不带格式 */
#define APP_OS_LOG_DEBUG
#define APP_OS_LOG_INFO
#define APP_OS_LOG_WARN
#define APP_OS_LOG_ERROR

#endif

#endif
