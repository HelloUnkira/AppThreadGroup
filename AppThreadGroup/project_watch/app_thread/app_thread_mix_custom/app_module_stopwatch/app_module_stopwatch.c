/*实现目标:
 *    秒表相关的功能组件
 *注意:
 *    秒表功能本地有且只有一个实例
 *    因为不需要设定多个秒表策略
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_sys_timer_t app_module_stopwatch_timer = {0};

/*@brief 设置秒表
 *@param stopwatch 秒表实例
 */
void app_module_stopwatch_set(app_module_stopwatch_t *stopwatch)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.stopwatch, stopwatch, sizeof(app_module_stopwatch_t));
    app_module_data_center_give();
}

/*@brief 获取秒表
 *@param stopwatch 秒表实例
 */
void app_module_stopwatch_get(app_module_stopwatch_t *stopwatch)
{
    /* 提取数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(stopwatch, &data_center_src->module_source.stopwatch, sizeof(app_module_stopwatch_t));
    app_module_data_center_give();
}

/*@brief 复位秒表
 */
void app_module_stopwatch_reset(void)
{
    /* 更新数据中心资源 */
    app_module_stopwatch_t stopwatch = {0};
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.stopwatch, &stopwatch, sizeof(app_module_stopwatch_t));
    app_module_data_center_give();
}

/*@brief 启动秒表
 */
void app_module_stopwatch_start(void)
{
    app_module_stopwatch_t stopwatch = {0};
    app_module_stopwatch_get(&stopwatch);
    stopwatch.onoff = true;
    app_module_stopwatch_set(&stopwatch);
    app_sys_timer_start(&app_module_stopwatch_timer);
}

/*@brief 停止秒表
 */
void app_module_stopwatch_stop(void)
{
    app_module_stopwatch_t stopwatch = {0};
    app_module_stopwatch_get(&stopwatch);
    stopwatch.onoff = false;
    app_module_stopwatch_set(&stopwatch);
    app_sys_timer_stop(&app_module_stopwatch_timer);
}

/*@brief 秒表模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_stopwatch_xmsec_update(void)
{
    app_module_stopwatch_t stopwatch = {0};
    app_module_stopwatch_get(&stopwatch);
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
    app_module_stopwatch_set(&stopwatch);
    
    APP_SYS_LOG_INFO("stopwatch:%u:%u:%u:%u",
                      stopwatch.hour,stopwatch.minute,stopwatch.second,
                      stopwatch.msec);
}

/*@brief 倒计时软件定时器模组回调
 */
static void app_module_stopwatch_timer_handler(void *timer)
{
    /* 发送倒计时事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_stopwatch,
        .event  = app_thread_mix_custom_stopwatch_msec_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 秒表模组初始化
 */
void app_module_stopwatch_ready(void)
{
    app_module_stopwatch_timer.expired = app_module_stopwatch_timer_handler;
    app_module_stopwatch_timer.peroid  = APP_MODULE_STOPWATCH_MSEC;
    app_module_stopwatch_timer.reload  = true;
}
