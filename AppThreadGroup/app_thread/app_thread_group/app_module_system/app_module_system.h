#ifndef APP_MODULE_SYSTEM_H
#define APP_MODULE_SYSTEM_H

typedef enum {
    /* 系统工作状态 */
    app_module_system_valid,
    app_module_system_invalid,
} app_module_system_status_t;

typedef enum {
    /* 系统工作模式 */
    app_module_system_normal,
    app_module_system_shutdown,
    app_module_system_low_power,
} app_module_system_mode_t;

/*@brief     系统进出DLPS
 *@param[in] status true:进入dlps;false:退出dlps
 */
void app_module_system_dlps_set(bool status);

/*@brief  系统进出DLPS
 *@retval status true:进入dlps;false:退出dlps
 */
bool app_module_system_dlps_get(void);

/*@brief     设置系统状态
 *@param[in] status 系统状态枚举量
 */
void app_module_system_status_set(uint8_t status);

/*@brief  获取系统状态
 *@retval 系统状态枚举量
 */
uint8_t app_module_system_status_get(void);

/*@brief     设置系统延时
 *@param[in] delay 系统延时秒数
 */
void app_module_system_delay_set(uint8_t delay);

/*@brief  获取系统延时
 *@retval 系统延时秒数
 */
uint8_t app_module_system_delay_get(void);

/*@brief     设置系统工作模式
 *@param[in] status 系统工作模式枚举量
 */
void app_module_system_mode_set(uint8_t mode);

/*@brief  获取系统工作模式
 *@retval 系统工作模式枚举量
 */
uint8_t app_module_system_mode_get(void);

/*@brief 初始化系统模组
 */
void app_module_system_ready(void);

/*@brief     系统1毫秒更新事件
 *           硬件时钟中断中执行
 *@param[in] count 毫秒计数器,每毫秒+1
 */
void app_module_system_1msec_update(uint32_t count);

#endif