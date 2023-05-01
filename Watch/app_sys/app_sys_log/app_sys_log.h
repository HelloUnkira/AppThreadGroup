#ifndef APP_SYS_LOG_H
#define APP_SYS_LOG_H

/* 线程检查与线程子模组检查 */
#define APP_SYS_LOG_THREAD_CHECK    1
#define APP_SYS_LOG_MODULE_CHECK    1
/* 线程事件包执行时间检查(ms) */
#define APP_SYS_LOG_EXECUTE         0
#define APP_SYS_LOG_EXECUTE_MS      10

/* 全局断言检查开关 */
#define APP_SYS_ASSERT_STATUS           1   /* 1:ENABLE,0:DISABLE */

/* 全局宏控开关及其等级 */
#define APP_SYS_LOG_GLOBAL_STATUS       1   /* 1:ENABLE,0:DISABLE */

/* 全局打印宏控检测,局部打印宏控检测 */
#if (APP_SYS_LOG_GLOBAL_STATUS && APP_SYS_LOG_LOCAL_STATUS)
/* DEBUG */
#if APP_SYS_LOG_LOCAL_LEVEL <= 0
#define APP_SYS_LOG_DEBUG(...)      app_sys_log_msg(1, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_DEBUG_RAW(...)  app_sys_log_msg(0, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* INFO */
#if APP_SYS_LOG_LOCAL_LEVEL <= 1
#define APP_SYS_LOG_INFO(...)       app_sys_log_msg(1, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_INFO_RAW(...)   app_sys_log_msg(0, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* WARN */
#if APP_SYS_LOG_LOCAL_LEVEL <= 2
#define APP_SYS_LOG_WARN(...)       app_sys_log_msg(1, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_WARN_RAW(...)   app_sys_log_msg(0, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* ERROR */
#if APP_SYS_LOG_LOCAL_LEVEL <= 3
#define APP_SYS_LOG_ERROR(...)      app_sys_log_msg(1, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_ERROR_RAW(...)  app_sys_log_msg(0, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* NONE */
#endif

#ifndef APP_SYS_LOG_DEBUG
#define APP_SYS_LOG_DEBUG(...)
#endif
#ifndef APP_SYS_LOG_DEBUG_RAW
#define APP_SYS_LOG_DEBUG_RAW(...)
#endif
#ifndef APP_SYS_LOG_INFO
#define APP_SYS_LOG_INFO(...)
#endif
#ifndef APP_SYS_LOG_INFO_RAW
#define APP_SYS_LOG_INFO_RAW(...)
#endif
#ifndef APP_SYS_LOG_WARN
#define APP_SYS_LOG_WARN(...)
#endif
#ifndef APP_SYS_LOG_WARN_RAW
#define APP_SYS_LOG_WARN_RAW(...)
#endif
#ifndef APP_SYS_LOG_ERROR
#define APP_SYS_LOG_ERROR(...)
#endif
#ifndef APP_SYS_LOG_ERROR_RAW
#define APP_SYS_LOG_ERROR_RAW(...)
#endif

/* 控制打印集合 */
/* 无格式打印:{内容} */
/* 带格式打印:[文件名][行数][级别]{内容}[换行] */

/*@brief     格式日志输出接口
 *           无格式打印:{内容}
 *           带格式打印:[文件名][行数][级别]{内容}[换行]
 *@param[in] status 日志信息是否带格式
 *@param[in] file   日志信息输出文件名
 *@param[in] line   日志信息输出文件内行数
 *@param[in] func   日志信息输出文件内函数名
 *@param[in] flag   日志信息重要性级别
 *@param[in] format 日志信息格式化信息
 *@param[in] ...    日志信息信息变参
 */
void app_sys_log_msg(unsigned char status, char flag, const char *file, const char *func, uint32_t line, const char *format, ...);

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_log_ready(void);

/* 扩展补充...... */

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond);

/* 断言:条件为真继续执行,为假时中断系统 */
#if APP_SYS_ASSERT_STATUS
#define APP_SYS_ASSERT(cond)    (app_sys_assert(__FILE__, __func__, __LINE__, cond))
#else
#define APP_SYS_ASSERT(cond)    (cond)
#endif

/*@brief 编译时间
 */
void app_sys_build_time(void);

#endif
