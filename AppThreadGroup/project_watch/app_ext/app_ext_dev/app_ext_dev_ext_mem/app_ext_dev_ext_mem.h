#ifndef APP_EXT_DEV_EXT_MEM_H
#define APP_EXT_DEV_EXT_MEM_H

/* 设备ext_mem抽象操作接口 */
typedef struct {
    int32_t (*ready)(app_dev_t *driver);
    int32_t (*read)(app_dev_t *driver);
    int32_t (*write)(app_dev_t *driver);
    void *  (*data_addr)(app_dev_t *driver);
} app_dev_ext_mem_api_t;

/* 设备ext_mem抽象操作数据 */
typedef struct {
    struct {
        uintptr_t chunk_base;       /* 使用base读写chunk */
        uintptr_t chunk_size;       /* chunk大小 */
        uintptr_t chunk_offset;     /* 相对chunk_base的基址偏移量 */
    } ext_mem;
    struct {
        uintptr_t offset;
        uint8_t  *buffer;
        uintptr_t size;
    } rw_args;
} app_dev_ext_mem_data_t;

/*@brief ext_mem设备初始化
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static inline int32_t app_dev_ext_mem_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_ext_mem_api_t *api = driver->api;
        return api->ready(driver);
    }
    return -1;
}

/*@brief ext_mem设备读取
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static inline int32_t app_dev_ext_mem_read(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_ext_mem_api_t *api = driver->api;
        return api->read(driver);
    }
    return -1;
}

/*@brief ext_mem设备写入
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static inline int32_t app_dev_ext_mem_write(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_ext_mem_api_t *api = driver->api;
        return api->write(driver);
    }
    return -1;
}

/*@brief ext_mem设备数据流地址映射
 *@param driver 设备实例
 *@retval 返回值(app_dev_ext_mem_data_t *)类型地址
 */
static inline void * app_dev_ext_mem_data_addr(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_ext_mem_api_t *api = driver->api;
        return api->data_addr(driver);
    }
    return NULL;
}

#endif
