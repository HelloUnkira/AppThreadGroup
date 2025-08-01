/*实现目标:
 *    泛用的格式输出流模组
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

static app_mutex_t app_sys_log_mutex = {0};
static app_sys_log_t app_sys_log = {0};

/*@brief 日志模组输出文件名去除路径
 *@param file 带路径的文件名称
 *@retval 去除路径后的文件名
 */
static const char * app_sys_log_file(const char *file)
{
    /* 文件名跳过路径,不需要使用时屏蔽即可 */
    /* 这里可以让编译器自优化 */
    int32_t separator = 0;
    int32_t file_len  = strlen(file);
    for (separator = file_len - 1; separator >= 0; separator--)
        if (file[separator] == '/' || file[separator] == '\\') {
            if (separator < file_len)
                file += separator + 1;
            break;
        }
    return file;
}

/*@brief 换行字符串
 *@retval 返回换行字符串
 */
const char * app_sys_log_line(void)
{
    return APP_SYS_LOG_MSG_LINE;
}

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 *@param log 配置实例
 */
void app_sys_log_ready(app_sys_log_t *log)
{
    app_mutex_process(&app_sys_log_mutex, app_mutex_static);
    app_sys_log = *log;
}

/*@brief 格式日志输出接口
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
void app_sys_log_msg(bool status, bool record, char flag, const char *file, const char *func, uint32_t line, const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    
    app_mutex_process(&app_sys_log_mutex, app_mutex_take);
    if (status) {
        file = app_sys_log_file(file);
        /* 格式化选择,按需求选取即可 */
           app_sys_log.message1("[%s][%u][%c]", func, line, flag);
        // app_sys_log.message1("[%s][%u][%c]", file, line, flag);
        // app_sys_log.message1("[%s][%s][%u][%c]", file, func, line, flag);
           app_sys_log.message2(format, list);
           app_sys_log.message1(app_sys_log_line());
        /* 格式化信息持久化 */
        if (record) {
            char text[APP_SYS_LOG_TEXT_LIMIT] = {0};
            /* 格式化选择,按需求选取即可 */
               snprintf(text, APP_SYS_LOG_TEXT_LIMIT, "[%s][%u][%c]", func, line, flag);
            // snprintf(text, APP_SYS_LOG_TEXT_LIMIT - 1, "[%s][%u][%c]", file, line, flag);
            // snprintf(text, APP_SYS_LOG_TEXT_LIMIT - 1, "[%s][%s][%u][%c]", file, func, line, flag);
            text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            app_sys_log.persistent(text);
            vsnprintf(text, APP_SYS_LOG_TEXT_LIMIT, format, list);
            text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            app_sys_log.persistent(text);
            snprintf(text, APP_SYS_LOG_TEXT_LIMIT, app_sys_log_line());
            text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            app_sys_log.persistent(text);
        }
    } else {
        // app_sys_log.message1("");
           app_sys_log.message2(format, list);
        // app_sys_log.message1(app_sys_log_line());
        /* 格式化信息持久化 */
        if (record) {
            char text[APP_SYS_LOG_TEXT_LIMIT] = {0};
            /* 格式化选择,按需求选取即可 */
            // snprintf(text, APP_SYS_LOG_TEXT_LIMIT, "");
            // text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            // app_sys_log.persistent(text);
            vsnprintf(text, APP_SYS_LOG_TEXT_LIMIT, format, list);
            text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            app_sys_log.persistent(text);
            // snprintf(text, APP_SYS_LOG_TEXT_LIMIT, app_sys_log_line());
            // text[APP_SYS_LOG_TEXT_LIMIT - 1] = '\0';
            // app_sys_log.persistent(text);
        }
    }
    app_mutex_process(&app_sys_log_mutex, app_mutex_give);
    
    va_end(list);
}

/*@brief 断言
 *@param file 文件名
 *@param func 函数名
 *@param line 文件行数
 *@param cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond)
{
    if (cond)
        return;
    
    if (app_sys_log.notify_assert != NULL)
        app_sys_log.notify_assert();
    
    file = app_sys_log_file(file);
    /* 输出错误信息 */
    /* 格式化选择,按需求选取即可 */
       app_sys_log_msg(false, true, 'A', "", "", 0, "[%s][%u]%s", func, line, app_sys_log_line());
    // app_sys_log_msg(false, true, 'A', "", "", 0, "[%s][%u]%s", file, line, app_sys_log_line());
    // app_sys_log_msg(false, true, 'A', "", "", 0, "[%s][%s][%u]%s", file, func, line, app_sys_log_line());
    
    #if APP_SYS_ASSERT_STATUS
    /* 异常导致的错误直接重启系统 */
    app_arch_reset();
    #endif
}

/*@brief 函数执行追踪
 *@param file 文件名
 *@param func 函数名
 *@param line 文件行数
 *@param step 执行编号
 */
void app_sys_trace(const char *file, const char *func, uint32_t line, uint32_t step)
{
    file = app_sys_log_file(file);
    /* 输出警告信息 */
    /* 格式化选择,按需求选取即可 */
       app_sys_log_msg(false, true, 'T', "", "", 0, "[%s][%u]:%d%s",     func, line, step, app_sys_log_line());
    // app_sys_log_msg(false, true, 'T', "", "", 0, "[%s][%u]:%d%s",     file, line, step, app_sys_log_line());
    // app_sys_log_msg(false, true, 'T', "", "", 0, "[%s][%s][%u]:%d%s", file, func, line, step, app_sys_log_line());
}

/*@brief 编译时间
 */
void app_sys_build_time(void)
{
    const char *file = __FILE__;
    const char *func = __func__;
    uint32_t    line = __LINE__;
    
    file = app_sys_log_file(file);
    /* 输出警告信息 */
    /* 格式化选择,按需求选取即可 */
       app_sys_log_msg(false, false, ' ', "", "", 0, "[%s][%u]:%s,%s%s", func, line, __DATE__, __TIME__, app_sys_log_line());
    // app_sys_log_msg(false, false, ' ', "", "", 0, "[%s][%u]:%s,%s%s", file, line, __DATE__, __TIME__, app_sys_log_line());
    // app_sys_log_msg(false, false, ' ', "", "", 0, "[%s][%s][%u]:%s,%s%s", file, func, line, __DATE__, __TIME__, app_sys_log_line());
}
