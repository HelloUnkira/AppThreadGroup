/* 实现目标:
 *     APP需要对系统驱动进行的封装体
 *     这是一个参考样板
 */

#include "app_ext_lib.h"
#include "app_dev_lib.h"

/* 设备empty抽象操作参数 */
typedef struct {
    void *args;
} app_dev_empty_cfg_t;

/*@brief 空设备初始化
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_init(app_dev_t *driver)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备反初始化
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_deinit(app_dev_t *driver)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备打开
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_open(app_dev_t *driver)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备关闭
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_close(app_dev_t *driver)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备读取
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_read(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备写入
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_write(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备I/O流处理
 *@param driver 设备实例
 *@retval 返回值
 */
static int64_t app_dev_empty_hal_ioctl(app_dev_t *driver, uint8_t *buf, uint32_t len)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief 空设备数据流地址映射
 *@param driver 设备实例
 *@retval 返回值
 */
static inline void * app_dev_empty_hal_data_addr(app_dev_t *driver)
{
    app_dev_empty_cfg_t *cfg = driver->cfg;
    app_dev_empty_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static app_dev_empty_cfg_t app_dev_empty_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_empty_api_t app_dev_empty_api = {
    .init       = app_dev_empty_hal_init,
    .deinit     = app_dev_empty_hal_deinit,
    .open       = app_dev_empty_hal_open,
    .close      = app_dev_empty_hal_close,
    .read       = app_dev_empty_hal_read,
    .write      = app_dev_empty_hal_write,
    .ioctl      = app_dev_empty_hal_ioctl,
    .data_addr  = app_dev_empty_hal_data_addr,
};

/* 动态的设备操作数据 */
static app_dev_empty_data_t app_dev_empty_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_empty = {
    .name = "app_dev_empty",
    .cfg  = &app_dev_empty_cfg,
    .api  = &app_dev_empty_api,
    .data = &app_dev_empty_data,
};
