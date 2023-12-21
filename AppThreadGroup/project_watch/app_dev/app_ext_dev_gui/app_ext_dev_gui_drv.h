#ifndef APP_EXT_DEV_GUI_DRV_H
#define APP_EXT_DEV_GUI_DRV_H

/* 设备gui_drv抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*timer_handler)(app_dev_t *driver);
    /*  */
    #if 0
    #elif APP_EXT_DEV_GUI_USE_WIN || APP_EXT_DEV_GUI_USE_SDL
    bool (*shutdown)(app_dev_t *driver);
    #else
    #endif
    /*  */
} app_dev_gui_drv_api_t;

/* 设备gui_drv抽象操作数据 */
typedef struct {
    /*  */
    #if 0
    #elif APP_EXT_DEV_GUI_USE_WIN || APP_EXT_DEV_GUI_USE_SDL
    bool shutdown;
    #else
    #endif
    /*  */
} app_dev_gui_drv_data_t;

/*@brief gui_drv设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_drv_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_drv_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief gui_drv设备定时回调
 *@param driver 设备实例
 */
static inline void app_dev_gui_drv_timer_handler(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_drv_api_t *api = driver->api;
        api->timer_handler(driver);
    }
}

#if 0
#elif APP_EXT_DEV_GUI_USE_WIN || APP_EXT_DEV_GUI_USE_SDL
/*@brief 设备组终止
 *@param driver 设备实例
 *@retval true:终止;false:继续;
 */
static inline bool app_dev_gui_drv_shutdown(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gui_drv_api_t *api = driver->api;
        return api->shutdown(driver);
    }
    return false;
}
#else
#endif

#endif
