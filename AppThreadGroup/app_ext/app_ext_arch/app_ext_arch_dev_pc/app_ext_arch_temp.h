#ifndef APP_EXT_ARCH_TEMP_H
#define APP_EXT_ARCH_TEMP_H

/* temperature */
#include "app_ext_arch_temp_pc.h"

/* 设备tempetature抽象实例... */
extern const app_arch_dev_t app_arch_temp;

/*@brief     temperature设备打开
 *@param[in] driver 设备实例
 */
static inline void app_arch_temp_open(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temp_api_t *api = driver->api;
        api->open(driver);
    }
}

/*@brief     temperature设备关闭
 *@param[in] driver 设备实例
 */
static inline void app_arch_temp_close(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temp_api_t *api = driver->api;
        api->close(driver);
    }
}

/*@brief     temperature设备每分钟自动定时器调用
 *@param[in] driver 设备实例
 */
static inline void app_arch_temp_auto_xs_measure(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temp_api_t *api = driver->api;
        api->auto_xs_measure(driver);
    }
}

/*@brief     temperature设备X毫秒手动定时器调用
 *@param[in] driver 设备实例
 *@retval    测量是否完毕
 */
static inline bool app_arch_temp_manual_xms_measure(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temp_api_t *api = driver->api;
        return api->manual_xms_measure(driver);
    }
    return true;
}

/*@brief     空设备数据流地址映射
 *@param[in] driver 设备实例
 *@retval    动态数据映射首地址
 */
static inline void * app_arch_temp_data_addr(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temp_api_t *api = driver->api;
        return api->data_addr(driver);
    }
    return NULL;
}

#endif
