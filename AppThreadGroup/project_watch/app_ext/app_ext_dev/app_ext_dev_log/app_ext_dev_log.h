#ifndef APP_EXT_DEV_LOG_H
#define APP_EXT_DEV_LOG_H

/* 设备log抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*msg)(app_dev_t *driver, const char *format, va_list list);
} app_dev_log_api_t;

/* 设备log抽象操作数据 */
typedef struct {
    void *data;
} app_dev_log_data_t;

/*@brief  log设备就绪
 *@param  driver 设备实例
 */
static inline void app_dev_log_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_log_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief  log设备输出
 *@param  driver 设备实例
 *@param  format 输出变参格式化字符串
 *@param  list   输出变参列表
 */
static inline void app_dev_log_msg(app_dev_t *driver, const char *format, va_list list)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_log_api_t *api = driver->api;
        api->msg(driver, format, list);
    }
}

/*@brief  变参函数式LOG输出接口(扩展封装)
 *@param  format 格式化字符串
 */
static inline void app_dev_log_msg1(const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    app_dev_log_msg(&app_dev_log, format, list);
    va_end(list);
}

/*@brief  变参列表式LOG输出接口(扩展封装)
 *@param  format 格式化字符串
 *@param  list   变参列表
 */
static inline void app_dev_log_msg2(const char *format, va_list list)
{
    app_dev_log_msg(&app_dev_log, format, list);
}

/*@brief  本层级模组使用的宏
 */
#if     APP_DEV_LOG_MSG_USE == 1
#define APP_DEV_LOG_MSG_FMT(...)    app_dev_log_msg1(__VA_ARGS__)
#define APP_DEV_LOG_MSG_LST(...)    app_dev_log_msg2(__VA_ARGS__)
#else
#define APP_DEV_LOG_MSG_FMT(...)
#define APP_DEV_LOG_MSG_LST(...)
#endif
#define APP_DEV_LOG_MSG_LINE    "\r\n"
#define APP_DEV_LOG_MSG_COM(...)                                    \
do {                                                                \
    APP_DEV_LOG_MSG_FMT("[%s][%d]", __func__, __LINE__);            \
    APP_DEV_LOG_MSG_FMT(__VA_ARGS__);                               \
    APP_DEV_LOG_MSG_FMT(APP_DEV_LOG_MSG_LINE);                      \
} while (0)

#endif
