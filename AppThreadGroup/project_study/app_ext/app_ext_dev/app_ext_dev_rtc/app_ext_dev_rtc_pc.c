/* 实现目标:
 *     模拟实时钟
 */

#include "app_ext_lib.h"

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
    const app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief rtc设备事件通报回调注册
 *@param driver 设备实例
 *@param cb     事件通报回调
 */
static void app_dev_rtc_hal_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    const app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->irq_cb = cb;
}

/*@brief rtc设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static void app_dev_rtc_hal_irq_switch(app_dev_t *driver, bool flag)
{
    const app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief rtc设备获取utc
 *@param driver 设备实例
 */
static void app_dev_rtc_hal_get_utc(app_dev_t *driver, uint64_t *utc)
{
    const app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    *utc = data->utc;
}

/*@brief rtc设备设置utc
 *@param driver 设备实例
 */
static void app_dev_rtc_hal_set_utc(app_dev_t *driver, uint64_t *utc)
{
    const app_dev_rtc_cfg_t *cfg = driver->cfg;
    app_dev_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->utc = *utc;
}

/* 静态配置的设备操作参数 */
static const app_dev_rtc_cfg_t app_dev_rtc_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_rtc_api_t app_dev_rtc_api = {
    .ready      = app_dev_rtc_hal_ready,
    .irq_cb_reg = app_dev_rtc_hal_irq_cb_reg,
    .irq_switch = app_dev_rtc_hal_irq_switch,
    .get_utc    = app_dev_rtc_hal_get_utc,
    .set_utc    = app_dev_rtc_hal_set_utc,
};

/* 动态的设备操作数据 */
static app_dev_rtc_data_t app_dev_rtc_data = {
    .utc    = 0,
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
