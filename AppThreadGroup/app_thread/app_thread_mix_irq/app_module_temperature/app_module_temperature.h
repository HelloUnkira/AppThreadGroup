#ifndef APP_MODULE_TEMPERATURE_H
#define APP_MODULE_TEMPERATURE_H

typedef struct {
    /* 一天周期测量: */
    #define APP_MODULE_TEMPERATURE_X_CNT    (24)        //自动测量累积次数
    #define APP_MODULE_TEMPERATURE_X_MGE    (12)        //自动测量累积次数合并
    #define APP_MODULE_TEMPERATURE_XS       (5 * 60)    //自动测量一次间隔
    /* 单次周期测量: */
    #define APP_MODULE_TEMPERATURE_XMS      (200)       //手动测量一次间隔
    #define APP_MODULE_TEMPERATURE_CNT      (5 * 5)     //手动测量缓存数据
    
    /* 自动记录数据: */
    float    measure_day_arr[APP_MODULE_TEMPERATURE_X_CNT];
    float    measure_one_arr[APP_MODULE_TEMPERATURE_X_MGE];
    uint16_t measure_day_cnt;
    uint16_t measure_one_cnt;
    
    uint16_t measure_xs_cnt;
    uint16_t measure_xs_status:1;
    
    /* 手动测量数据: */
    float    measure_val;
    float    measure_tmp_arr[APP_MODULE_TEMPERATURE_CNT];
    uint16_t measure_tmp_cnt;
    uint16_t measure_status:1;
    uint16_t measure_result:1;
} app_module_temperature_t;

/*@brief  温度模组测量状态
 *@retval 是否在测量之中
 */
bool app_module_temperature_measure_status(void);

/*@brief  温度模组测量结果
 *@retval 是否有测量结果
 */
bool app_module_temperature_measure_result(void);

/*@brief 温度模组启动测量
 */
void app_module_temperature_measure_run(void);

/*@brief     温度模组启动测量
 *@param[in] run 是否启动
 */
void app_module_temperature_measure_auto(bool run);

/*@brief 温度模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_temperature_xms_update(void);

/*@brief 温度模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_temperature_xs_update(void);

/*@brief 温度模组初始化
 */
void app_module_temperature_ready(void);

#endif
