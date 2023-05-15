#ifndef APP_SYS_LOG_H
#define APP_SYS_LOG_H

/*
 * 配置项起始
 */

/* 子线程执行时间检查(ms) */
#define APP_SYS_LOG_EXECUTE             1
/* 线程检查与线程子模组检查 */
#define APP_SYS_LOG_THREAD_CHECK        1
#define APP_SYS_LOG_MODULE_CHECK        1
/* 线程交互数据流检查 */
#define APP_SYS_LOG_PROTOCOL_CHECK      1
/* 线程事件包执行时间检查(ms) */
#define APP_SYS_LOG_EXECUTE_CHECK       0
#define APP_SYS_LOG_EXECUTE_CHECK_MS    10

/* 全局断言检查开关 */
#define APP_SYS_ASSERT_STATUS           1   /* 1:ENABLE,0:DISABLE */

/* 全局函数追踪检查开关 */
#define APP_SYS_EXECUTE_TRACE_STATUS    1   /* 1:ENABLE,0:DISABLE */

/* 全局宏控开关(完全启用或禁用) */
#define APP_SYS_LOG_GLOBAL_STATUS       1   /* 1:ENABLE,0:DISABLE */
/* 全局宏控覆盖开关(覆盖本地宏控等级) */
#define APP_SYS_LOG_GLOBAL_OVERLAY      0   /* 1:ENABLE,0:DISABLE */
/* 全局宏控等级(全局宏控覆盖开关启用时有效, 用于覆盖本地宏控等级) */
#define APP_SYS_LOG_GLOBAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

/* 全局宏控本地持久化开关 */
/* 注意:它受外部宏控管制,完全使用它需要激活全局宏控覆盖 */
#define APP_SYS_LOG_RECORD_STATUS       1   /* 1:ENABLE,0:DISABLE */
/* 全局宏控本地持久化等级 */
#define APP_SYS_LOG_RECORD_LEVEL        3   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */
/* 全局宏控本地持久化单条目长度限制(包含结尾字符) */
#define APP_SYS_LOG_RECORD_LENGTH       128

/*
 * 配置项结束
 */

/* 本地宏控(本文件被包含前配置) */
#ifndef APP_SYS_LOG_LOCAL_STATUS
#define APP_SYS_LOG_LOCAL_STATUS        0   /* 1:ENABLE,0:DISABLE */
#endif
#ifndef APP_SYS_LOG_LOCAL_LEVEL
#define APP_SYS_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */
#endif

/* 全局宏控覆盖(覆盖所有本地宏控) */
#if     APP_SYS_LOG_GLOBAL_OVERLAY
#undef  APP_SYS_LOG_LOCAL_STATUS
#define APP_SYS_LOG_LOCAL_STATUS    APP_SYS_LOG_GLOBAL_STATUS
#undef  APP_SYS_LOG_LOCAL_LEVEL
#define APP_SYS_LOG_LOCAL_LEVEL     APP_SYS_LOG_GLOBAL_LEVEL
#endif

/* 全局持久化宏控限制 */
/* 备注:不是所有的模组都需要使用该功能, 例如持久化模组自己 */
#ifndef APP_SYS_LOG_RECORD_LIMIT
#define APP_SYS_LOG_RECORD_LIMIT    0   /* 1:ENABLE,0:DISABLE */
#endif

/* 全局持久化宏控覆盖(依赖本地宏控) */
#if    (APP_SYS_LOG_RECORD_STATUS && !APP_SYS_LOG_RECORD_LIMIT)
#define APP_SYS_LOG_RECORD_0        APP_SYS_LOG_RECORD_LEVEL <= 0 ? true : false
#define APP_SYS_LOG_RECORD_1        APP_SYS_LOG_RECORD_LEVEL <= 1 ? true : false
#define APP_SYS_LOG_RECORD_2        APP_SYS_LOG_RECORD_LEVEL <= 2 ? true : false
#define APP_SYS_LOG_RECORD_3        APP_SYS_LOG_RECORD_LEVEL <= 3 ? true : false
#else
#define APP_SYS_LOG_RECORD_0        false
#define APP_SYS_LOG_RECORD_1        false
#define APP_SYS_LOG_RECORD_2        false
#define APP_SYS_LOG_RECORD_3        false
#endif

/* 全局宏控检测,局部宏控检测 */
#if    (APP_SYS_LOG_GLOBAL_STATUS && APP_SYS_LOG_LOCAL_STATUS)
/* DEBUG */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 0
#define APP_SYS_LOG_DEBUG(...)      app_sys_log_msg(true,  APP_SYS_LOG_RECORD_0, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_DEBUG_RAW(...)  app_sys_log_msg(false, APP_SYS_LOG_RECORD_0, 'D', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* INFO */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 1
#define APP_SYS_LOG_INFO(...)       app_sys_log_msg(true,  APP_SYS_LOG_RECORD_1, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_INFO_RAW(...)   app_sys_log_msg(false, APP_SYS_LOG_RECORD_1, 'I', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* WARN */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 2
#define APP_SYS_LOG_WARN(...)       app_sys_log_msg(true,  APP_SYS_LOG_RECORD_2, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_WARN_RAW(...)   app_sys_log_msg(false, APP_SYS_LOG_RECORD_2, 'W', __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif
/* ERROR */
#if     APP_SYS_LOG_LOCAL_LEVEL <= 3
#define APP_SYS_LOG_ERROR(...)      app_sys_log_msg(true,  APP_SYS_LOG_RECORD_3, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
#define APP_SYS_LOG_ERROR_RAW(...)  app_sys_log_msg(false, APP_SYS_LOG_RECORD_3, 'E', __FILE__, __func__, __LINE__, __VA_ARGS__)
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

/* 扩展:日志换行的宏转换 */
#define APP_SYS_LOG_LINE    "\r\n"

typedef struct {
    void (*message1)(const char *format, ...);
    void (*message2)(const char *format, va_list list);
    void (*persistent)(const char *text);
} app_sys_log_t;

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_log_ready(app_sys_log_t sys_log);

/*@brief     格式日志输出接口
 *           内部使用: 被宏函数使用
 *           无格式打印:{内容}
 *           带格式打印:[文件名][行数][级别]{内容}[换行]
 *@param[in] status 日志信息是否带格式
 *@param[in] record 日志信息是否需记录
 *@param[in] file   日志信息输出文件名
 *@param[in] line   日志信息输出文件内行数
 *@param[in] func   日志信息输出文件内函数名
 *@param[in] flag   日志信息重要性级别
 *@param[in] format 日志信息格式化信息
 *@param[in] ...    日志信息信息变参
 */
void app_sys_log_msg(bool status, bool record, char flag, const char *file, const char *func, uint32_t line, const char *format, ...);

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] func 函数名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond);

/* 断言:条件为真继续执行,为假时中断系统 */
#if     APP_SYS_ASSERT_STATUS
#define APP_SYS_ASSERT(cond)    (app_sys_assert(__FILE__, __func__, __LINE__, cond))
#else
#define APP_SYS_ASSERT(cond)    (cond)
#endif

/*@brief     函数执行追踪
 *@param[in] file 文件名
 *@param[in] func 函数名
 *@param[in] line 文件行数
 *@param[in] step 执行编号
 */
void app_sys_execute_trace(const char *file, const char *func, uint32_t line, uint32_t step);

#if     APP_SYS_EXECUTE_TRACE_STATUS
#define APP_SYS_EXECUTE_TRACE(step)     (app_sys_execute_trace(__FILE__, __func__, __LINE__, step))
#endif

/*@brief 编译时间
 */
void app_sys_build_time(void);

#endif
