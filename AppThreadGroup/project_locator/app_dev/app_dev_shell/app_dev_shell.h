#ifndef APP_DEV_SHELL_H
#define APP_DEV_SHELL_H

#include "app_dev_def.h"

/* shell 设备操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*read)(app_dev_t *driver, char *buf, uint32_t size, uint32_t *len, uint32_t *prompt);
    void (*write)(app_dev_t *driver, const char *buf, uint32_t size);
} app_dev_shell_api_t;

/* shell 设备数据 */
typedef struct {
    uint8_t  buf[1024];
    uint32_t len;
#if APP_ARCH_IS_PC
    void    *hconin;
    void    *hconout;
    uint32_t prompt;
#endif
} app_dev_shell_data_t;

/*@brief shell设备就绪
 *@param driver 设备实例
 */
static inline void app_dev_shell_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_shell_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief shell设备读取
 *@param driver 设备实例
 *@param buf    缓冲区
 *@param size   缓冲区大小
 *@param len    数据长度
 *@param prompt 输入轮次状态
 */
static inline void app_dev_shell_read(app_dev_t *driver, char *buf, uint32_t size, uint32_t *len, uint32_t *prompt)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_shell_api_t *api = driver->api;
        api->read(driver, buf, size, len, prompt);
    }
}

/*@brief shell设备写入
 *@param driver 设备实例
 *@param buf    缓冲区
 *@param size   数据长度
 */
static inline void app_dev_shell_write(app_dev_t *driver, const char *buf, uint32_t size)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_shell_api_t *api = driver->api;
        api->write(driver, buf, size);
    }
}

#endif
