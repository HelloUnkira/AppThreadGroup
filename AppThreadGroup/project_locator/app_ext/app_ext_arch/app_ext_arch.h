#ifndef APP_EXT_ARCH_H
#define APP_EXT_ARCH_H

/*@brief 初始化平台
 */
void app_arch_ready(void);

/*@brief 重启平台
 */
void app_arch_reset(void);

/*@brief 重启平台
 *@param ms 延时时间(ms)
 */
void app_arch_reset_delay_ms(uint32_t ms);

#endif
