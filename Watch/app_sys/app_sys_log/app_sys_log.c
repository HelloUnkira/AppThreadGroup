/*实现目标:
 *    泛用的格式输出流模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

/*@brief 待适配接口
 */
static void app_sys_log_msg1(const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
}

/*@brief 待适配接口
 */
static void app_sys_log_msg2(const char *format, va_list list)
{
    vprintf(format, list);
}

static app_mutex_t app_sys_log_mutex = {0};

/*@brief 日志模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_log_ready(void)
{
    app_mutex_process(&app_sys_log_mutex);
}

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
void app_sys_log_msg(unsigned char status, char flag, const char *file, const char *func, uint32_t line, const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    
    app_mutex_take(&app_sys_log_mutex);
    if (status == 0) {
        app_sys_log_msg2(format, list);
    } else {
        /* 格式化一般有俩种选择(1:文件名+行数,2:函数名+行数),按需求选取即可 */
        #if 0
        #elif 1
        app_sys_log_msg1("[%s][%u][%c]", func, line, flag);
        app_sys_log_msg2(format, list);
        app_sys_log_msg1("\r\n");
        #elif 0
        app_sys_log_msg1("[%s][%u][%c]", file, line, flag);
        app_sys_log_msg2(format, list);
        app_sys_log_msg1("\r\n");
        #else
        app_sys_log_msg1("[%s][%s][%u][%c]", file, func, line, flag);
        app_sys_log_msg2(format, list);
        app_sys_log_msg1("\r\n");
        #endif
    }
    app_mutex_give(&app_sys_log_mutex);
    
    va_end(list);
}

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_sys_assert(const char *file, const char *func, uint32_t line, bool cond)
{
    if (cond)
        return;
    /* 输出错误信息 */
    APP_SYS_LOG_ERROR_RAW("APP_SYS_ASSERT:[%s][%s][%d]", file, func, line);
    /* 异常导致的错误直接重启系统 */
    app_os_reset();
}

/*@brief 编译时间
 */
void app_sys_build_time(void)
{
    APP_SYS_LOG_INFO_RAW("FW Build Time:%s,%s", __DATE__, __TIME__);
}
