/* 实现目标:
 *     模拟实时钟
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备rtc抽象操作参数 */
typedef struct {
    void *args;
} app_arch_rtc_cfg_t;

/*@brief     rtc设备就绪
 *@param[in] driver 设备实例
 */
static void app_arch_rtc_hal_ready(app_arch_dev_t *driver)
{
    const app_arch_rtc_cfg_t *cfg = driver->cfg;
    app_arch_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     rtc设备事件通报回调注册
 *@param[in] driver 设备实例
 *@param[in] cb     充电事件通报回调
 */
static void app_arch_rtc_hal_irq_cb_reg(app_arch_dev_t *driver, void(*cb)(void))
{
    const app_arch_rtc_cfg_t *cfg = driver->cfg;
    app_arch_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->irq_cb = cb;
}

/*@brief     rtc设备运行
 *@param[in] driver 设备实例
 */
static void app_arch_rtc_hal_execute(app_arch_dev_t *driver)
{
    const app_arch_rtc_cfg_t *cfg = driver->cfg;
    app_arch_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     rtc设备获取utc
 *@param[in] driver 设备实例
 */
static void app_arch_rtc_hal_get_utc(app_arch_dev_t *driver, uint64_t *utc)
{
    const app_arch_rtc_cfg_t *cfg = driver->cfg;
    app_arch_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    *utc = data->utc;
}

/*@brief     rtc设备设置utc
 *@param[in] driver 设备实例
 */
static void app_arch_rtc_hal_set_utc(app_arch_dev_t *driver, uint64_t *utc)
{
    const app_arch_rtc_cfg_t *cfg = driver->cfg;
    app_arch_rtc_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->utc = *utc;
}

/* 静态配置的设备操作参数 */
static const app_arch_rtc_cfg_t app_arch_rtc_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_arch_rtc_api_t app_arch_rtc_api = {
    .ready      = app_arch_rtc_hal_ready,
    .irq_cb_reg = app_arch_rtc_hal_irq_cb_reg,
    .execute    = app_arch_rtc_hal_execute,
    .get_utc    = app_arch_rtc_hal_get_utc,
    .set_utc    = app_arch_rtc_hal_set_utc,
};

/* 动态的设备操作数据 */
static app_arch_rtc_data_t app_arch_rtc_data = {
    .utc    = 0,
    .irq_cb = NULL,
};

/* 静态配置的设备实例 */
const app_arch_dev_t app_arch_rtc = {
    .name = "app_arch_rtc",
    .cfg  = &app_arch_rtc_cfg,
    .api  = &app_arch_rtc_api,
    .data = &app_arch_rtc_data,
};

#endif
