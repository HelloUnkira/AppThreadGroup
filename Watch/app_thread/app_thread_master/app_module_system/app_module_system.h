#ifndef APP_MODULE_SYSTEM_H
#define APP_MODULE_SYSTEM_H

typedef enum {
    app_module_system_valid,
    app_module_system_reset,
    app_module_system_shutdown,
} app_module_system_status_t;

/*@brief 设置系统转储成功标记
 *@param[in] over 系统转储结束
 */
void app_module_system_dump_set(bool over);

/*@brief     设置系统延时
 *@param[in] delay 系统延时秒数
 */
void app_module_system_delay_set(uint8_t delay);

/*@brief     设置系统状态
 *@param[in] status 系统状态枚举量
 */
void app_module_system_status_set(uint8_t status);

/*@brief  获取系统状态
 *@retval 系统状态枚举量
 */
uint8_t app_module_system_status_get(void);

/*@brief 初始化系统模组
 */
void app_module_system(void);

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_module_assert(char *file, uint32_t line, bool cond);

/* 断言:条件为真继续执行,为假时中断系统 */
#define APP_MODULE_ASSERT(cond)    \
        app_module_assert(__FILE__, __LINE__, cond)

#endif
