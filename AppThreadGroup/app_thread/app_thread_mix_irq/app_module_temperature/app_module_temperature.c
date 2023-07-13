/*实现目标:
 *    温度测量相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_thread_group.h"
#include "app_module_temperature.h"

static app_mutex_t app_module_temperature_mutex = {0};
static app_sys_timer_t app_module_temperature_xms_timer = {0};
static app_sys_timer_t app_module_temperature_xs_timer  = {0};
static app_module_temperature_t app_module_temperature  = {0};

/*@brief  温度模组测量状态
 *@retval 是否在测量之中
 */
bool app_module_temperature_measure_status(void)
{
    bool measure_status = false;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_take);
    measure_status = app_module_temperature.measure_status;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_give);
    return measure_status;
}

/*@brief  温度模组测量结果
 *@retval 是否有测量结果
 */
bool app_module_temperature_measure_result(void)
{
    bool measure_result = false;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_take);
    measure_result = app_module_temperature.measure_result;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_give);
    return measure_result;
}

/*@brief 温度模组启动测量
 */
void app_module_temperature_measure_run(void)
{
    app_sys_timer_stop(&app_module_temperature_xms_timer);
    app_mutex_process(&app_module_temperature_mutex, app_mutex_take);
    app_module_temperature.measure_tmp_cnt = 0;
    app_module_temperature.measure_status  = true;
    app_module_temperature.measure_result  = false;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_give);
    app_sys_timer_start(&app_module_temperature_xms_timer);
}

/*@brief 温度模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_temperature_xms_update(void)
{
    bool measure_over = false;
    app_mutex_process(&app_module_temperature_mutex, app_mutex_take);
    app_module_temperature_t *temperature = &app_module_temperature;
    /* 测量结束 */
    if (temperature->measure_tmp_cnt >= APP_MODULE_TEMPERATURE_CNT) {
        temperature->measure_status = false;
        temperature->measure_result = true;
        measure_over = true;
    }
    /* 测量中... */
    if (temperature->measure_status) {
        /* 获得此刻温度测量数据 */
        float val = app_arch_temperature_get_value(&app_arch_temperature);
        /* 暂存温度数据 */
        temperature->measure_tmp_arr[temperature->measure_tmp_cnt++] = val;
        /* 暂存温度数据不足 */
        if (temperature->measure_tmp_cnt == 0) 
            temperature->measure_val      = 0.0;
        /* 暂存温度中值滤波 */
        if (temperature->measure_tmp_cnt >= 2) {
            /* 简单排序一下 */
            for (uint8_t i = 0; i < temperature->measure_tmp_cnt; i++)
            for (uint8_t j = i + 1; j < temperature->measure_tmp_cnt; j++)
                if (temperature->measure_tmp_arr[i] > temperature->measure_tmp_arr[j]) {
                    float val_i = temperature->measure_tmp_arr[i];
                    float val_j = temperature->measure_tmp_arr[j];
                    temperature->measure_tmp_arr[i] = val_j;
                    temperature->measure_tmp_arr[j] = val_i;
                }
            /* 取中值 */
            if (temperature->measure_tmp_cnt % 2 != 0) {
                val = temperature->measure_tmp_arr[temperature->measure_tmp_cnt / 2];
                temperature->measure_val = val;
            } else {
                val  = 0.0;
                val += temperature->measure_tmp_arr[temperature->measure_tmp_cnt / 2];
                val += temperature->measure_tmp_arr[temperature->measure_tmp_cnt / 2 + 1];
                temperature->measure_val = val / 2;
            }
        }
    }
    app_mutex_process(&app_module_temperature_mutex, app_mutex_give);
    /* 测量结束则关闭测量定时器 */
    if (measure_over)
        app_sys_timer_stop(&app_module_temperature_xms_timer);
}

/*@brief 温度模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_temperature_xs_update(void)
{
}

/*@brief 温度模组软件定时器回调
 */
static void app_module_temperature_xms_timer_handler(void *timer)
{
    /* 发送震动模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_temperature,
        .event  = app_thread_mix_irq_temperature_xms_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 温度模组软件定时器回调
 */
static void app_module_temperature_xs_timer_handler(void *timer)
{
    /* 发送震动模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_temperature,
        .event  = app_thread_mix_irq_temperature_xs_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 温度模组初始化
 */
void app_module_temperature_ready(void)
{
    app_mutex_process(&app_module_temperature_mutex, app_mutex_static);
    app_module_temperature_xms_timer.expired = app_module_temperature_xms_timer_handler;
    app_module_temperature_xms_timer.peroid  = APP_MODULE_TEMPERATURE_XMS;
    app_module_temperature_xms_timer.reload  = true;
    app_module_temperature_xs_timer.expired  = app_module_temperature_xs_timer_handler;
    app_module_temperature_xs_timer.peroid   = APP_MODULE_TEMPERATURE_XS;
    app_module_temperature_xs_timer.reload   = true;
}
