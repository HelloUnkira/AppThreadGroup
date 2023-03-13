#ifndef APP_ARCH_DRIVER_PC_EMPTY_H
#define APP_ARCH_DRIVER_PC_EMPTY_H

#if APP_ARCH_IS_PC

/* 设备empty抽象实例... */
extern const app_driver_t app_driver_empty;

/* 设备empty抽象功能接口配置 */
typedef struct {
    void *parameter;
} app_driver_empty_cfg_t;

/* 设备empty抽象功能接口 */
typedef struct {
    int64_t (*init)(app_driver_t *driver);
    int64_t (*deinit)(app_driver_t *driver);
    int64_t (*open)(app_driver_t *driver);
    int64_t (*close)(app_driver_t *driver);
    int64_t (*read)(app_driver_t *driver, uint8_t *data, uint32_t len);
    int64_t (*write)(app_driver_t *driver, uint8_t *data, uint32_t len);
    int64_t (*ioctl)(app_driver_t *driver, uint8_t *data, uint32_t len);
} app_driver_empty_api_t;

/*@brief     空设备初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_init(app_driver_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->init(driver);
    }
}

/*@brief     空设备反初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_deinit(app_driver_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->deinit(driver);
    }
}

/*@brief     空设备打开
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_open(app_driver_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->open(driver);
    }
}

/*@brief     空设备关闭
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_close(app_driver_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->close(driver);
    }
}

/*@brief     空设备读取
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_read(app_driver_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->read(driver, data, len);
    }
}

/*@brief     空设备写入
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_write(app_driver_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->write(driver, data, len);
    }
}

/*@brief     空设备I/O流处理
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_driver_empty_ioctl(app_driver_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_driver_empty_api_t *api = driver->api;
        return api->ioctl(driver, data, len);
    }
}

#endif

#endif
