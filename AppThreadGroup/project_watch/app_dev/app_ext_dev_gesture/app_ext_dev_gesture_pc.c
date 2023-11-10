/* 实现目标:
 *     模拟手势
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_ARCH_IS_PC

/* 设备gesture抽象操作参数 */
typedef struct {
    void *args;
} app_dev_gesture_cfg_t;

/*@brief     gesture设备初始化
 *@param[in] driver 设备实例
 *@retval    当前温度值
 */
static inline void app_dev_gesture_hal_ready(app_dev_t *driver)
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     gesture设备事件通报回调注册
 *@param[in] driver 设备实例
 *@param[in] cb     事件通报回调
 */
static inline void app_dev_gesture_hal_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->irq_cb = cb;
}

/*@brief     gesture设备事件通报开关
 *@param[in] driver 设备实例
 *@param[in] flag   true:开启事件通报;false:关闭事件通报
 */
static inline void app_dev_gesture_hal_irq_switch(app_dev_t *driver, bool flag)
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    APP_SYS_LOG_WARN("irq_switch:%d", flag);
}

/*@brief     gesture设备帧数据获取
 *@param[in] driver 设备实例
 *@param[in] frame  帧数据
 *@param[in] length 帧数据实际数量
 */
static inline void app_dev_gesture_hal_get_frame(app_dev_t *driver, int32_t frame[][3], uint8_t *length)
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     gesture设备帧测量流程执行
 *@param[in] driver 设备实例
 */
static inline void app_dev_gesture_hal_proc_exec(app_dev_t *driver)
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     gesture设备进出dlps模式
 *@param[in] driver 设备实例
 *@param[in] dlps   true:进入dlps模式;false:退出dlps模式
 *@param[in] mode   other:正常模式;1:低电量模式;2:关机模式
 */
static inline void app_dev_gesture_hal_dlps_exec(app_dev_t *driver, bool dlps, uint8_t mode)
{
    const app_dev_gesture_cfg_t *cfg = driver->cfg;
    app_dev_gesture_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static const app_dev_gesture_cfg_t app_dev_gesture_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_gesture_api_t app_dev_gesture_api = {
    .ready      = app_dev_gesture_hal_ready,
    .irq_cb_reg = app_dev_gesture_hal_irq_cb_reg,
    .irq_switch = app_dev_gesture_hal_irq_switch,
    .get_frame  = app_dev_gesture_hal_get_frame,
    .proc_exec  = app_dev_gesture_hal_proc_exec,
    .dlps_exec  = app_dev_gesture_hal_dlps_exec,
};

/* 动态的设备操作数据 */
static app_dev_gesture_data_t app_dev_gesture_data = {
    .frame  = {0},
    .irq_cb = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gesture = {
    .name = "app_dev_gesture",
    .cfg  = &app_dev_gesture_cfg,
    .api  = &app_dev_gesture_api,
    .data = &app_dev_gesture_data,
};

#endif
