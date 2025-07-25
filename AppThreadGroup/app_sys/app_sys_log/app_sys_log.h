#ifndef APP_SYS_LOG_H
#define APP_SYS_LOG_H

/* 本地宏控(本文件被包含前配置) */
#ifndef APP_SYS_LOG_LOCAL_STATUS
#define APP_SYS_LOG_LOCAL_STATUS        0   /* 1:ENABLE,0:DISABLE */
#endif
#ifndef APP_SYS_LOG_LOCAL_LEVEL
#define APP_SYS_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */
#endif

/* 全局宏控覆盖(覆盖所有本地宏控) */
#if     APP_SYS_LOG_GLOBAL_OVERLAY
#ifdef  APP_SYS_LOG_LOCAL_STATUS
#undef  APP_SYS_LOG_LOCAL_STATUS
#define APP_SYS_LOG_LOCAL_STATUS        APP_SYS_LOG_GLOBAL_STATUS
#endif
#ifdef  APP_SYS_LOG_LOCAL_LEVEL
#undef  APP_SYS_LOG_LOCAL_LEVEL
#define APP_SYS_LOG_LOCAL_LEVEL         APP_SYS_LOG_GLOBAL_LEVEL
#endif
#endif

/* 全局持久化宏控限制 */
/* 备注:不是所有的模组都需要使用该功能, 例如持久化模组自己 */
#ifndef APP_SYS_LOG_LIMIT_RECORD
#define APP_SYS_LOG_LIMIT_RECORD        0   /* 1:ENABLE,0:DISABLE */
#endif

/* 全局持久化宏控覆盖(依赖本地宏控) */
#if    (APP_SYS_LOG_RECORD_STATUS && !APP_SYS_LOG_LIMIT_RECORD)
#define APP_SYS_LOG_RECORD_0          APP_SYS_LOG_RECORD_LEVEL <= 0 ? true : false
#define APP_SYS_LOG_RECORD_1          APP_SYS_LOG_RECORD_LEVEL <= 1 ? true : false
#define APP_SYS_LOG_RECORD_2          APP_SYS_LOG_RECORD_LEVEL <= 2 ? true : false
#define APP_SYS_LOG_RECORD_3          APP_SYS_LOG_RECORD_LEVEL <= 3 ? true : false
#else
#define APP_SYS_LOG_RECORD_0          false
#define APP_SYS_LOG_RECORD_1          false
#define APP_SYS_LOG_RECORD_2          false
#define APP_SYS_LOG_RECORD_3          false
#endif

/* 全局宏控检测,局部宏控检测 */
#if    (APP_SYS_LOG_GLOBAL_STATUS && APP_SYS_LOG_LOCAL_STATUS)
/* DEBUG */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 0
#define APP_SYS_LOG_DEBUG(...)      app_sys_log_msg(true,  APP_SYS_LOG_RECORD_0, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_DEBUG_RAW(...)  app_sys_log_msg(false, APP_SYS_LOG_RECORD_0, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define APP_SYS_LOG_DEBUG(...)
#define APP_SYS_LOG_DEBUG_RAW(...)
#endif
/* INFO */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 1
#define APP_SYS_LOG_INFO(...)       app_sys_log_msg(true,  APP_SYS_LOG_RECORD_1, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_INFO_RAW(...)   app_sys_log_msg(false, APP_SYS_LOG_RECORD_1, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define APP_SYS_LOG_INFO(...)
#define APP_SYS_LOG_INFO_RAW(...)
#endif
/* WARN */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 2
#define APP_SYS_LOG_WARN(...)       app_sys_log_msg(true,  APP_SYS_LOG_RECORD_2, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_WARN_RAW(...)   app_sys_log_msg(false, APP_SYS_LOG_RECORD_2, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define APP_SYS_LOG_WARN(...)
#define APP_SYS_LOG_WARN_RAW(...)
#endif
/* ERROR */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 3
#define APP_SYS_LOG_ERROR(...)      app_sys_log_msg(true,  APP_SYS_LOG_RECORD_3, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_ERROR_RAW(...)  app_sys_log_msg(false, APP_SYS_LOG_RECORD_3, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define APP_SYS_LOG_ERROR(...)
#define APP_SYS_LOG_ERROR_RAW(...)
#endif
/* NONE */
#else
#define APP_SYS_LOG_DEBUG(...)
#define APP_SYS_LOG_DEBUG_RAW(...)
#define APP_SYS_LOG_INFO(...)
#define APP_SYS_LOG_INFO_RAW(...)
#define APP_SYS_LOG_WARN(...)
#define APP_SYS_LOG_WARN_RAW(...)
#define APP_SYS_LOG_ERROR(...)
#define APP_SYS_LOG_ERROR_RAW(...)
#endif

#define APP_SYS_LOG_MSG_LINE    "\r\n"

/* 断言:条件为真继续执行,为假时中断系统 */
#define APP_SYS_ASSERT(cond)    (app_sys_assert(__FILE__, __func__, __LINE__, cond))

/* 追踪执行:标明执行到函数的哪一个步骤 */
#if     APP_SYS_TRACE_STATUS
#define APP_SYS_TRACE(step)     (app_sys_trace(__FILE__, __func__, __LINE__, step))
#else
#define APP_SYS_TRACE(step)
#endif

/*@brief 换行字符串
 *@retval 返回换行字符串
 */
const char * app_sys_log_line(void);

/* ----------下面是未使用宏包装的函数,不建议直接使用,不利于项目代码裁剪---------- */

typedef struct {
    void (*message1)(const char *format, ...);
    void (*message2)(const char *format, va_list list);
    void (*persistent)(const char *text);
    void (*notify_assert)(void);
} app_sys_log_t;

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 *@param log 配置实例
 */
void app_sys_log_ready(app_sys_log_t *log);

/*@brief 格式日志输出接口
 *       内部使用: 被宏函数使用
 *       无格式打印:{内容}
 *       带格式打印:[文件名][行数][级别]{内容}[换行]
 *@param status 日志信息是否带格式
 *@param record 日志信息是否需记录
 *@param file   日志信息输出文件名
 *@param line   日志信息输出文件内行数
 *@param func   日志信息输出文件内函数名
 *@param flag   日志信息重要性级别
 *@param format 日志信息格式化信息
 *@param ...    日志信息信息变参
 */
void app_sys_log_msg(bool status, bool record, char flag, const char *file, const char *func, uint32_t line, const char *format, ...);

/*@brief 断言
 *@param file 文件名
 *@param func 函数名
 *@param line 文件行数
 *@param cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond);

/*@brief 函数追踪执行
 *@param file 文件名
 *@param func 函数名
 *@param line 文件行数
 *@param step 执行编号
 */
void app_sys_trace(const char *file, const char *func, uint32_t line, uint32_t step);

/*@brief 编译时间
 */
void app_sys_build_time(void);

#endif
