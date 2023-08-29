#ifndef APP_EXT_ARCH_H
#define APP_EXT_ARCH_H

/* APP arch adaptor: */
#define APP_ARCH_IS_UNKNOWN     0
#define APP_ARCH_IS_PC          1

/*@brief 初始化平台
 */
void app_arch_ready(void);

/*@brief 重启平台
 */
void app_arch_reset(void);

/*@brief     重启平台
 *@param[in] ms 延时时间(ms)
 */
void app_arch_reset_delay_ms(uint32_t ms);

#endif
