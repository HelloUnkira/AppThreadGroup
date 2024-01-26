/*实现目标:
 *    震动相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"

static app_mutex_t app_module_vibrate_mutex = {0};
static app_sys_timer_t app_module_vibrate_timer = {0};
static app_module_vibrate_t app_module_vibrate  = {0};

/*@brief 震动模组工作有效性检查
 *@retval 模组是否有效
 */
static bool app_module_vibrate_invalid(void)
{
    bool invalid_check = false;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_take);
    if (app_module_vibrate.period == 0)
        invalid_check = true;
    if (app_module_vibrate.melody == NULL)
        invalid_check = true;
    if (app_module_vibrate.pwm_max <
        app_module_vibrate.pwm_min)
        invalid_check = true;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_give);
    return invalid_check;
}

/*@brief 设置震动模组
 *@param vibrate 震动实例
 */
void app_module_vibrate_set(app_module_vibrate_t *vibrate)
{
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_take);
    app_module_vibrate = *vibrate;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_give);
}

/*@brief 获取震动模组
 *@param vibrate 震动实例
 */
void app_module_vibrate_get(app_module_vibrate_t *vibrate)
{
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_take);
    *vibrate = app_module_vibrate;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_give);
}

/*@brief 震动模组启动
 *       错误的参数导致启动失败
 */
void app_module_vibrate_start(void)
{
    if (app_module_vibrate_invalid())
        return;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_take);
    app_module_vibrate.tick = 0;
    app_module_vibrate.count = 0;
    app_module_vibrate.onoff = true;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_give);
    app_sys_timer_start(&app_module_vibrate_timer);
    APP_SYS_LOG_WARN("vibrate start");
    app_dev_vibrate_open(&app_dev_vibrate);
}

/*@brief 震动模组停止
 */
void app_module_vibrate_stop(void)
{
    app_sys_timer_stop(&app_module_vibrate_timer);
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_take);
    app_module_vibrate.onoff = false;
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_give);
    APP_SYS_LOG_WARN("vibrate end");
    app_dev_vibrate_close(&app_dev_vibrate);
}

/*@brief 震动模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_vibrate_msec_update(void)
{
    app_module_vibrate_t vibrate = {0};
    app_module_vibrate_get(&vibrate);
    if (!vibrate.onoff)
        return;
    /* 所有周期结束,关闭震动模组 */
    if (vibrate.period == vibrate.tick &&
        vibrate.repeat >= vibrate.count &&
        vibrate.repeat != -1) {
        app_module_vibrate_stop();
        return;
    }
    if (vibrate.melody == NULL)
        return;
    /* 更新震动节拍 */
    static uint8_t melody_last = 0;
    static uint8_t melody_curr = 0;
    melody_curr = vibrate.melody(&vibrate, vibrate.tick, vibrate.period);
    if (vibrate.tick == 0)
        APP_SYS_LOG_INFO_RAW("melody:");
    if (melody_last != melody_curr) {
        melody_last  = melody_curr;
        APP_SYS_LOG_INFO_RAW("%u ", melody_curr);
        /* 在此处更新震动,百分比 */
        app_dev_vibrate_update(&app_dev_vibrate, melody_curr);
    }
    /* 更新震动计数 */
    if (vibrate.tick >= vibrate.period) {
        vibrate.tick -= vibrate.period;
        vibrate.count++;
    }
    vibrate.tick++;
    app_module_vibrate_set(&vibrate);
}

/*@brief 震动模组软件定时器回调
 */
static void app_module_vibrate_timer_handler(void *timer)
{
    /* 发送震动模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_vibrate,
        .event  = app_thread_mix_irq_vibrate_msec_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 震动模组初始化
 */
void app_module_vibrate_ready(void)
{
    app_mutex_process(&app_module_vibrate_mutex, app_mutex_static);
    app_module_vibrate_timer.expired = app_module_vibrate_timer_handler;
    app_module_vibrate_timer.peroid  = APP_MODULE_VIBRATE_XMS;
    app_module_vibrate_timer.reload  = true;
}

/*@brief 震动节拍默认回调1
 *       周期内前半部分震动后半部分不震动
 *@param current 周期内的到达点,进度
 *@param period  设置的周期
 *@retval 百分比振幅[0,100]
 */
uint8_t app_module_vibrate_melody_default_1(void *instance, uint32_t current, uint32_t period)
{
    app_module_vibrate_t *vibrate = instance;
    
    if (current <= period / 2)
        return vibrate->pwm_max;
    else
        return vibrate->pwm_min;
}

/*@brief 震动节拍默认回调2
 *       先升后降,波峰波谷式震动
 *@param current 周期内的到达点,进度
 *@param period  设置的周期
 *@retval 百分比振幅[0,100]
 */
uint8_t app_module_vibrate_melody_default_2(void *instance, uint32_t current, uint32_t period)
{
    app_module_vibrate_t *vibrate = instance;
    
    if (current * 2 <= period)
        return vibrate->pwm_min + (vibrate->pwm_max - vibrate->pwm_min) * (current * 2) / period;
    else
        return vibrate->pwm_max - (vibrate->pwm_max - vibrate->pwm_min) * (current * 2 - period) / period;
}
