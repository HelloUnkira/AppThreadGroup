/* 实现目标:
 *     APP需要对系统驱动进行的封装体
 *     APP会在一个专门的地点和时刻准备和使用这些资源
 *     针对所有所有平台使用一个全通配的抽象模板
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 包含平台文件,生成下述抽象接口的实体 */

/*@brief     空设备初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_init(app_dev_t *driver)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备反初始化
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_deinit(app_dev_t *driver)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备打开
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_open(app_dev_t *driver)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备关闭
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_close(app_dev_t *driver)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备读取
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_read(app_dev_t *driver, uint8_t *data, uint32_t len)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备写入
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_write(app_dev_t *driver, uint8_t *data, uint32_t len)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     空设备I/O流处理
 *@param[in] driver 设备实例
 *@retval    返回值
 */
static inline int64_t app_dev_empty_hal_ioctl(app_dev_t *driver, uint8_t *data, uint32_t len)
{
    const app_dev_empty_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/* 静态配置的参数集合 */
static const app_dev_empty_cfg_t app_dev_empty_cfg = {
    .parameter = NULL,
};

/* 静态配置的功能集合 */
static const app_dev_empty_api_t app_dev_empty_api = {
    .init       = app_dev_empty_hal_init,
    .deinit     = app_dev_empty_hal_deinit,
    .open       = app_dev_empty_hal_open,
    .close      = app_dev_empty_hal_close,
    .read       = app_dev_empty_hal_read,
    .write      = app_dev_empty_hal_write,
    .ioctl      = app_dev_empty_hal_ioctl,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_empty = {
    .name = "app_dev_empty",
    .api  = &app_dev_empty_api,
    .cfg  = &app_dev_empty_cfg,
};

#endif
