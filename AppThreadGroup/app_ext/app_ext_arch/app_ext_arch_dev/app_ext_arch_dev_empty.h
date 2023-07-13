#ifndef APP_EXT_ARCH_DEV_EMPTY_H
#define APP_EXT_ARCH_DEV_EMPTY_H

/* 设备empty抽象实例... */
extern const app_arch_dev_t app_arch_dev_empty;

/* 设备empty抽象操作参数 */
typedef struct {
    void *args;
} app_arch_dev_empty_cfg_t;

/* 设备empty抽象操作接口 */
typedef struct {
    int64_t (*init)(app_arch_dev_t *driver);
    int64_t (*deinit)(app_arch_dev_t *driver);
    int64_t (*open)(app_arch_dev_t *driver);
    int64_t (*close)(app_arch_dev_t *driver);
    int64_t (*read)(app_arch_dev_t *driver, uint8_t *data, uint32_t len);
    int64_t (*write)(app_arch_dev_t *driver, uint8_t *data, uint32_t len);
    int64_t (*ioctl)(app_arch_dev_t *driver, uint8_t *data, uint32_t len);
    void *  (*data_addr)(app_arch_dev_t *driver);
} app_arch_dev_empty_api_t;

/* 设备empty抽象操作数据 */
typedef struct {
    void *data;
} app_arch_dev_empty_data_t;

/*@brief     空设备初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_init(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->init(driver);
    }
}

/*@brief     空设备反初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_deinit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->deinit(driver);
    }
}

/*@brief     空设备打开
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_open(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->open(driver);
    }
}

/*@brief     空设备关闭
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_close(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->close(driver);
    }
}

/*@brief     空设备读取
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_read(app_arch_dev_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->read(driver, data, len);
    }
}

/*@brief     空设备写入
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_write(app_arch_dev_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->write(driver, data, len);
    }
}

/*@brief     空设备I/O流处理
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_arch_dev_empty_ioctl(app_arch_dev_t *driver, uint8_t *data, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->ioctl(driver, data, len);
    }
}

/*@brief     空设备数据流地址映射
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline void * app_arch_dev_empty_data_addr(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_dev_empty_api_t *api = driver->api;
        return api->data_addr(driver);
    }
}

#endif
