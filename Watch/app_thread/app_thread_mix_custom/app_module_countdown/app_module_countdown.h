#ifndef APP_MODULE_COUNTDOWN_H
#define APP_MODULE_COUNTDOWN_H

typedef struct {
    uint8_t  hour;       /* 时 */
    uint8_t  minute;     /* 分 */
    uint8_t  second;     /* 秒 */
    uint16_t msec;       /* 毫秒 */
    uint8_t  onoff:1;    /* 启停状态 */
} app_module_countdown_t;

#define APP_MODULE_COUNTDOWN_MSEC   99

/*@brief        设置倒计时
 *@param[in]    countdown 倒计时实例
 */
void app_module_countdown_set(app_module_countdown_t *countdown);

/*@brief        获取倒计时
 *@param[out]   countdown 倒计时实例
 */
void app_module_countdown_get(app_module_countdown_t *countdown);

/*@brief 复位倒计时
 */
void app_module_countdown_reset(void);

/*@brief 启动倒计时
 */
void app_module_countdown_start(void);

/*@brief 停止倒计时
 */
void app_module_countdown_stop(void);

/*@brief 倒计时模组初始化
 */
void app_module_countdown_ready(void);

/*@brief 倒计时模组更新
 */
void app_module_countdown_xmsec_update(void);

/*@brief 更新倒计时
 *       内部使用: 被mix custom线程使用
 */
void app_module_countdown_msec_update(void);

#endif
