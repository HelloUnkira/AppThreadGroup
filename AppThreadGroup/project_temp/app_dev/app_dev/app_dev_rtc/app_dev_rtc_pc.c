/* 实现目标:
 *     模拟实时钟
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_ARCH_IS_PC

/* 设备rtc抽象操作参数 */
typedef struct {
    void *args;
} app_dev_rtc_cfg_t;

/*@brief rtc设备就绪
 *@param driver 设备实例
 */
static void app_dev_rtc_hal_ready(app_dev_t *driver)
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief rtc设备事件通报回调注册
 *@param driver 设备实例
 *@param cb     事件通报回调
 */
static void app_dev_rtc_hal_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->irq_cb = cb;
    // cb是1ms递增一次, 并上报一个cb事件
}

/*@brief rtc设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static void app_dev_rtc_hal_irq_switch(app_dev_t *driver, bool flag)
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief rtc设备获取utc
 *@param driver 设备实例
 *@param utc    标准时
 *@param utc_ms 标准时(ms余留)
 */
static void app_dev_rtc_hal_get_utc(app_dev_t *driver, uint64_t *utc, uint64_t *utc_ms)
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    *utc = data->utc;
    *utc_ms = data->utc_ms;
}

/*@brief rtc设备设置utc
 *@param driver 设备实例
 *@param utc    标准时
 *@param utc_ms 标准时(ms余留)
 */
static void app_dev_rtc_hal_set_utc(app_dev_t *driver, uint64_t *utc, uint64_t *utc_ms)
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->utc = *utc;
    data->utc_ms = *utc_ms;
}

/*@brief rtc设备同步
 *@param driver 设备实例
 */
static void app_dev_rtc_hal_sync(app_dev_t *driver)
{
    app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    
    time_t timep;
    struct tm *p;
    time (&timep);
    p=gmtime(&timep);
    
    app_sys_clock_t clock = {
        .year   = 1900+p->tm_year,      /*获取当前年份,从1900开始，所以要加1900*/
        .month  = 1+p->tm_mon,          /*获取当前月份,范围是0-11,所以要加1*/
        .day    = p->tm_mday,           /*获取当前月份日数,范围是1-31*/
        .hour   = p->tm_hour,           /*获取当前时,这里获取西方的时间,刚好相差八个小时*/
        .minute = p->tm_min,
        .second = p->tm_sec,
    };
    app_sys_clock_to_utc(&clock);
    
    data->utc = clock.utc;
    
    uint64_t ms = app_execute_us() / 1000;
    data->utc_ms = ms % 1000;
}

/* 静态配置的设备操作参数 */
static app_dev_rtc_cfg_t app_dev_rtc_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_rtc_api_t app_dev_rtc_api = {
    .ready      = app_dev_rtc_hal_ready,
    .irq_cb_reg = app_dev_rtc_hal_irq_cb_reg,
    .irq_switch = app_dev_rtc_hal_irq_switch,
    .get_utc    = app_dev_rtc_hal_get_utc,
    .set_utc    = app_dev_rtc_hal_set_utc,
    .sync       = app_dev_rtc_hal_sync,
};

/* 动态的设备操作数据 */
static app_dev_rtc_data_t app_dev_rtc_data = {
    .utc    = 0,
    .utc_ms = 0,
    .irq_cb = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_rtc = {
    .name = "app_dev_rtc",
    .cfg  = &app_dev_rtc_cfg,
    .api  = &app_dev_rtc_api,
    .data = &app_dev_rtc_data,
};

#endif
