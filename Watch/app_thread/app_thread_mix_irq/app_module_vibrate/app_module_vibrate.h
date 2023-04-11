#ifndef APP_MODULE_VIBRATE_H
#define APP_MODULE_VIBRATE_H

/*@brief 震动节拍回调
 *@param[in] current 周期内的到达点,进度
 *@param[in] period  设置的周期
 *@retval    百分比振幅[0,100]
 */
typedef uint8_t (*app_module_vibrate_melody_t)(uint16_t current, uint16_t period);

typedef struct {
    app_module_vibrate_melody_t melody; /* 震动节拍回调 */
    uint16_t tick;      /* 震动周期记录(内部使用) */
    uint16_t count;     /* 震动次数记录(内部使用) */
    uint16_t period;    /* 震动周期 */
    uint16_t repeat;    /* 震动次数 */
    bool onoff;         /* 启停状态 */
} app_module_vibrate_t;

/*@brief        设置震动模组
 *@param[out]   vibrate 震动实例
 */
void app_module_vibrate_set(app_module_vibrate_t *vibrate);

/*@brief        获取震动模组
 *@param[out]   vibrate 震动实例
 */
void app_module_vibrate_get(app_module_vibrate_t *vibrate);

/*@brief 震动模组启动
 */
void app_module_vibrate_start(void);

/*@brief 震动模组停止
 */
void app_module_vibrate_stop(void);

/*@brief 震动模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_vibrate_msec_update(void);

/*@brief 震动模组初始化
 */
void app_module_vibrate_ready(void);

/*@brief 震动节拍默认回调1
*        周期内前半部分震动后半部分不震动
 *@param[in] current 周期内的到达点,进度
 *@param[in] period  设置的周期
 *@retval    百分比振幅[0,100]
 */
uint8_t app_module_vibrate_melody_default_1(uint16_t current, uint16_t period);

/*@brief 震动节拍默认回调2
 *       先升后降,波峰波谷式震动
 *@param[in] current 周期内的到达点,进度
 *@param[in] period  设置的周期
 *@retval    百分比振幅[0,100]
 */
uint8_t app_module_vibrate_melody_default_2(uint16_t current, uint16_t period);

#endif
