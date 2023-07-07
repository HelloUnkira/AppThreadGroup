/*实现目标:
 *    泛用的格式输出流模组
 */

#include "app_ext_lib.h"
#include "app_sys_log.h"

static app_mutex_t app_sys_log_mutex = {0};
static app_sys_log_t app_sys_log = {0};

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_log_ready(app_sys_log_t log)
{
    app_mutex_process(&app_sys_log_mutex, app_mutex_create);
    app_sys_log = log;
}

/*@brief     格式日志输出接口
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
void app_sys_log_msg(bool status, bool record, char flag, const char *file, const char *func, uint32_t line, const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    
    app_mutex_process(&app_sys_log_mutex, app_mutex_take);
    if (status) {
        /* 格式化一般有俩种选择(1:文件名+行数,2:函数名+行数),按需求选取即可 */
        #if 0
        #elif 1
        app_sys_log.message1("[%s][%u][%c]", func, line, flag);
        app_sys_log.message2(format, list);
        app_sys_log.message1(APP_SYS_LOG_LINE);
        #elif 0
        app_sys_log.message1("[%s][%u][%c]", file, line, flag);
        app_sys_log.message2(format, list);
        app_sys_log.message1(APP_SYS_LOG_LINE);
        #else
        app_sys_log.message1("[%s][%s][%u][%c]", file, func, line, flag);
        app_sys_log.message2(format, list);
        app_sys_log.message1(APP_SYS_LOG_LINE);
        #endif
        /* 格式化信息持久化 */
        if (record) {
            uint32_t offset = 0;
            char text[APP_SYS_LOG_RECORD_LENGTH] = {0};
            offset +=  snprintf(text + offset, APP_SYS_LOG_RECORD_LENGTH - offset, "[%s][%u][%c]", func, line, flag);
            offset += vsnprintf(text + offset, APP_SYS_LOG_RECORD_LENGTH - offset, format, list);
            offset +=  snprintf(text + offset, APP_SYS_LOG_RECORD_LENGTH - offset, APP_SYS_LOG_LINE);
            text[APP_SYS_LOG_RECORD_LENGTH - 1] = '\0';
            app_sys_log.persistent(text);
        }
    } else {
        app_sys_log.message2(format, list);
        /* 格式化信息持久化 */
        if (record) {
            uint32_t offset = 0;
            char text[APP_SYS_LOG_RECORD_LENGTH] = {0};
            offset += vsnprintf(text + offset, APP_SYS_LOG_RECORD_LENGTH - offset, format, list);
            text[APP_SYS_LOG_RECORD_LENGTH - 1] = '\0';
            app_sys_log.persistent(text);
        }
    }
    app_mutex_process(&app_sys_log_mutex, app_mutex_give);
    
    va_end(list);
}

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] func 函数名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond)
{
    if (cond)
        return;
    /* 输出错误信息 */
    /* 格式化一般有俩种选择(1:文件名+行数,2:函数名+行数),按需求选取即可 */
    #if 0
    #elif 1
    app_sys_log_msg(false, true, 'E', "", "", 0, "APP_SYS_ASSERT:[%s][%d]", func, line);
    app_sys_log_msg(false, true, 'E', "", "", 0,  APP_SYS_LOG_LINE);
    #elif 0
    app_sys_log_msg(false, true, 'E', "", "", 0, "APP_SYS_ASSERT:[%s][%d]", file, line);
    app_sys_log_msg(false, true, 'E', "", "", 0,  APP_SYS_LOG_LINE);
    #else
    app_sys_log_msg(false, true, 'E', "", "", 0, "APP_SYS_ASSERT:[%s][%s][%d]", file, func, line);
    app_sys_log_msg(false, true, 'E', "", "", 0,  APP_SYS_LOG_LINE);
    #endif
    /* 异常导致的错误直接重启系统 */
    app_os_reset();
}

/*@brief     函数执行追踪
 *@param[in] file 文件名
 *@param[in] func 函数名
 *@param[in] line 文件行数
 *@param[in] step 执行编号
 */
void app_sys_execute_trace(const char *file, const char *func, uint32_t line, uint32_t step)
{
    /* 格式化一般有俩种选择(1:文件名+行数,2:函数名+行数),按需求选取即可 */
    #if 0
    #elif 1
    app_sys_log_msg(false, true, 'D', "", "", 0, "APP_SYS_EXECUTE_TRACE:[%s][%d]:%d", func, line, step);
    app_sys_log_msg(false, true, 'D', "", "", 0,  APP_SYS_LOG_LINE);
    #elif 0
    app_sys_log_msg(false, true, 'D', "", "", 0, "APP_SYS_EXECUTE_TRACE:[%s][%d]:%d", file, line, step);
    app_sys_log_msg(false, true, 'D', "", "", 0,  APP_SYS_LOG_LINE);
    #else
    app_sys_log_msg(false, true, 'D', "", "", 0, "APP_SYS_EXECUTE_TRACE:[%s][%s][%d]:%d", file, func, line, step);
    app_sys_log_msg(false, true, 'D', "", "", 0,  APP_SYS_LOG_LINE);
    #endif
}

/*@brief 编译时间
 */
void app_sys_build_time(void)
{
    app_sys_log_msg(false, false, 'D', "", "", 0, "FW Build Time:%s,%s", __DATE__, __TIME__);
    app_sys_log_msg(false, false, 'D', "", "", 0, APP_SYS_LOG_LINE);
}
