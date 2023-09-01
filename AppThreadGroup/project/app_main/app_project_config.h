#ifndef APP_PROJECT_CONFIG_H
#define APP_PROJECT_CONFIG_H

/*
 * APP EXT 配置项起始
 */

/* APP OS adaptor: */
#define APP_OS_IS_WINDOWS   1
#define APP_OS_IS_LINUX     0
#define APP_OS_IS_ZEPHYR    0
#define APP_OS_IS_FREERTOS  0

/* APP arch adaptor: */
#define APP_ARCH_IS_PC      1

/*
 * APP EXT 配置项结束
 */


/*
 * APP SYS 配置项起始
 */

/* 全局宏控开关(完全启用或禁用) */
#define APP_SYS_LOG_GLOBAL_STATUS       1   /* 1:ENABLE,0:DISABLE */
/* 全局宏控覆盖开关(覆盖本地宏控等级) */
#define APP_SYS_LOG_GLOBAL_OVERLAY      0   /* 1:ENABLE,0:DISABLE */
/* 全局宏控覆盖等级(全局宏控覆盖开关启用时有效, 覆盖本地宏控等级) */
#define APP_SYS_LOG_GLOBAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */
/* 全局宏控本地持久化开关(受外部宏控管制, 需全局宏控覆盖才可完全使用) */
#define APP_SYS_LOG_RECORD_STATUS       1   /* 1:ENABLE,0:DISABLE */
/* 全局宏控本地持久化等级 */
#define APP_SYS_LOG_RECORD_LEVEL        3   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

/* 单次存储日志文本最大长度(包括字符串结尾'\0') */
#define APP_SYS_LOG_TEXT_MAX            128

/* 全局断言检查开关 */
#define APP_SYS_ASSERT_STATUS           1   /* 1:ENABLE,0:DISABLE */
/* 全局函数追踪检查开关 */
#define APP_SYS_TRACE_STATUS            1   /* 1:ENABLE,0:DISABLE */

/*
 * APP SYS 配置项结束
 */

/*
 * APP THREAD 配置项起始
 */

/* 最长阻塞时间(秒) */
#define APP_MODULE_WATCHDOG_XS  5

/*
 * APP THREAD 配置项结束
 */

#endif
