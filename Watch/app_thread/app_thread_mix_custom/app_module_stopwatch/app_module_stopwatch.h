#ifndef APP_MODULE_STOPWATCH_H
#define APP_MODULE_STOPWATCH_H

typedef struct {
    uint8_t  hour;       /* 时 */
    uint8_t  minute;     /* 分 */
    uint8_t  second;     /* 秒 */
    uint16_t msec;       /* 毫秒 */
    uint8_t  onoff:1;    /* 启停状态 */
} app_module_stopwatch_t;

#define APP_MODULE_STOPWATCH_MSEC   99

/*@brief        设置秒表
 *@param[out]   stopwatch 秒表实例
 */
void app_module_stopwatch_set(app_module_stopwatch_t *stopwatch);

/*@brief        获取秒表
 *@param[out]   stopwatch 秒表实例
 */
void app_module_stopwatch_get(app_module_stopwatch_t *stopwatch);

/*@brief 复位秒表
 */
void app_module_stopwatch_reset(void);

/*@brief 启动秒表
 */
void app_module_stopwatch_start(void);

/*@brief 停止秒表
 */
void app_module_stopwatch_stop(void);

/*@brief 秒表模组初始化
 */
void app_module_stopwatch_ready(void);

/*@brief 秒表模组更新
 */
void app_module_stopwatch_xmsec_update(void);

/*@brief 更新秒表
 *       内部使用: 被mix custom线程使用
 */
void app_module_stopwatch_msec_update(void);

#endif
