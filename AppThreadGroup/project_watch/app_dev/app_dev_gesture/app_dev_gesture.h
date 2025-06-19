#ifndef APP_DEV_GESTURE_H
#define APP_DEV_GESTURE_H

/* 设备gesture抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*irq_cb_reg)(app_dev_t *driver, void (*cb)(void));
    void (*irq_switch)(app_dev_t *driver, bool flag);
    void (*get_frame)(app_dev_t *driver, int32_t frame[][3], uint8_t *length);
    void (*proc_exec)(app_dev_t *driver);
    void (*dlps_exec)(app_dev_t *driver, bool dlps, uint8_t mode);
} app_dev_gesture_api_t;

/* 设备gesture抽象操作数据 */
typedef struct {
#define APP_DEV_GESTURE_FRAME_LIMIT     (8)
    int32_t frame[APP_DEV_GESTURE_FRAME_LIMIT][3];  //小数化整,单位为mg/(s^2)
    /* GESTURE事件通报回调 */
    void (*irq_cb)(void);
} app_dev_gesture_data_t;

/*@brief gesture设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gesture_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief gesture设备事件通报回调注册
 *@param driver 设备实例
 *@param cb     事件通报回调
 */
static inline void app_dev_gesture_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->irq_cb_reg(driver, cb);
    }
}

/*@brief gesture设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static inline void app_dev_gesture_irq_switch(app_dev_t *driver, bool flag)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->irq_switch(driver, flag);
    }
}

/*@brief gesture设备帧数据获取
 *@param driver 设备实例
 *@param frame  帧数据
 *@param length 帧数据实际数量
 */
static inline void app_dev_gesture_get_frame(app_dev_t *driver, int32_t frame[APP_DEV_GESTURE_FRAME_LIMIT][3], uint8_t *length)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->get_frame(driver, frame, length);
    }
}

/*@brief gesture设备帧测量流程执行
 *@param driver 设备实例
 */
static inline void app_dev_gesture_proc_exec(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->proc_exec(driver);
    }
}

/*@brief gesture设备进出dlps模式
 *@param driver 设备实例
 *@param dlps   true:进入dlps模式;false:退出dlps模式
 *@param mode   other:正常模式;1:低电量模式;2:关机模式
 */
static inline void app_dev_gesture_dlps_exec(app_dev_t *driver, bool dlps, uint8_t mode)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_gesture_api_t *api = driver->api;
        api->dlps_exec(driver, dlps, mode);
    }
}

#endif
