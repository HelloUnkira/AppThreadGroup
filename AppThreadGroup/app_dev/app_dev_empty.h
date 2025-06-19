#ifndef APP_DEV_EMPTY_H
#define APP_DEV_EMPTY_H

/* 设备empty抽象操作接口 */
typedef struct {
    int64_t (*init)(app_dev_t *driver);
    int64_t (*deinit)(app_dev_t *driver);
    int64_t (*open)(app_dev_t *driver);
    int64_t (*close)(app_dev_t *driver);
    int64_t (*read)(app_dev_t *driver, uint8_t *buf, uint32_t len);
    int64_t (*write)(app_dev_t *driver, uint8_t *buf, uint32_t len);
    int64_t (*ioctl)(app_dev_t *driver, uint8_t *buf, uint32_t len);
    void *  (*data_addr)(app_dev_t *driver);
} app_dev_empty_api_t;

/* 设备empty抽象操作数据 */
typedef struct {
    void *data;
} app_dev_empty_data_t;

/*@brief 空设备初始化
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_init(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->init(driver);
    }
}

/*@brief 空设备反初始化
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_deinit(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->deinit(driver);
    }
}

/*@brief 空设备打开
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_open(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->open(driver);
    }
}

/*@brief 空设备关闭
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_close(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->close(driver);
    }
}

/*@brief 空设备读取
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_read(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->read(driver, buf, len);
    }
}

/*@brief 空设备写入
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_write(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->write(driver, buf, len);
    }
}

/*@brief 空设备I/O流处理
 *@param driver 设备实例
 *@retval 返回值
 */
static inline int64_t app_dev_empty_ioctl(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->ioctl(driver, buf, len);
    }
}

/*@brief 空设备数据流地址映射
 *@param driver 设备实例
 *@retval 返回值
 */
static inline void * app_dev_empty_data_addr(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_empty_api_t *api = driver->api;
        return api->data_addr(driver);
    }
}

#endif
