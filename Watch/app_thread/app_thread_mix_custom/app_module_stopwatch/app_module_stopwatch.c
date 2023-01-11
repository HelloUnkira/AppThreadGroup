/*实现目标:
 *    秒表相关的功能组件
 *注意:
 *    秒表功能本地有且只有一个实例
 *    因为不需要设定多个秒表策略
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_thread_interface.h"

static app_mutex_t app_module_stopwatch_mutex = {0};
static app_module_stopwatch_t app_module_stopwatch = {0};

/*@brief        设置秒表
 *@param[out]   stopwatch 秒表实例
 */
void app_module_stopwatch_set(app_module_stopwatch_t *stopwatch)
{
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch = *stopwatch;
    app_mutex_give(&app_module_stopwatch_mutex);
}

/*@brief        获取秒表
 *@param[out]   stopwatch 秒表实例
 */
void app_module_stopwatch_get(app_module_stopwatch_t *stopwatch)
{
    app_mutex_take(&app_module_stopwatch_mutex);
    *stopwatch = app_module_stopwatch;
    app_mutex_give(&app_module_stopwatch_mutex);
}

/*@brief 复位秒表
 */
void app_module_stopwatch_reset(void)
{
    app_module_stopwatch_t stopwatch = {0};
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch = stopwatch;
    app_mutex_give(&app_module_stopwatch_mutex);
}

/*@brief 启动秒表
 */
void app_module_stopwatch_start(void)
{
    /* 在这里调用一个接口,启动时钟源发送秒表事件功能,如果有必要 */
    /* ...... */
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch.onoff = true;
    app_mutex_give(&app_module_stopwatch_mutex);
}

/*@brief 停止秒表
 */
void app_module_stopwatch_stop(void)
{
    /* 在这里调用一个接口,关闭时钟源发送秒表事件功能,如果有必要 */
    /* ...... */
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch.onoff = false;
    app_mutex_give(&app_module_stopwatch_mutex);
}

/*@brief 秒表模组初始化
 */
void app_module_stopwatch_ready(void)
{
    app_mutex_process(&app_module_stopwatch_mutex);
}

/*@brief 更新秒表
 *       内部使用: 被mix custom线程使用
 */
void app_module_stopwatch_msec_update(void)
{
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch_t stopwatch = app_module_stopwatch;
    app_mutex_give(&app_module_stopwatch_mutex);
    if (!stopwatch.onoff)
        return;
    /*  */
    uint32_t seconds = stopwatch.hour   * 60 * 60 +
                       stopwatch.minute * 60 +
                       stopwatch.second;
    /*  */
    if ((stopwatch.msec += APP_MODULE_STOPWATCH_MSEC) > 1000) {
         stopwatch.msec -= 1000;
         seconds += 1;
    }
    /*  */
    stopwatch.second = seconds % 60;
    stopwatch.minute = seconds / 60 % 60;
    stopwatch.hour   = seconds / 60 / 60 % 60;
    app_mutex_take(&app_module_stopwatch_mutex);
    app_module_stopwatch = stopwatch;
    app_mutex_give(&app_module_stopwatch_mutex);
    
    #if APP_MODULE_CHECK
    APP_OS_LOG_INFO("stopwatch:%u:%u:%u:%u\n",
                    stopwatch.hour,stopwatch.minute,stopwatch.second,
                    stopwatch.msec);
    #endif
}
