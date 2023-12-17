#ifndef APP_EXT_DEV_GUI_ENC_H
#define APP_EXT_DEV_GUI_ENC_H

/* 设备gui_enc抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*dlps_enter)(app_dev_t *driver);
    void (*dlps_exit)(app_dev_t *driver);
} app_dev_gui_enc_api_t;

/* 设备gui_enc抽象操作数据 */
typedef struct {
    void *args;
} app_dev_gui_enc_data_t;

/*@brief gui_enc设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_enc_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief gui_enc设备进入dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_dlps_enter(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_enc_api_t *api = driver->api;
        api->dlps_enter(driver);
    }
}

/*@brief gui_enc设备退出dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_enc_dlps_exit(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_enc_api_t *api = driver->api;
        api->dlps_exit(driver);
    }
}

#endif
