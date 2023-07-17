#ifndef APP_EXT_ARCH_H
#define APP_EXT_ARCH_H

/* APP arch adaptor: */
#define APP_ARCH_IS_UNKNOWN     0
#define APP_ARCH_IS_PC          1

/* arch ext mem adaptor */
#include "app_ext_arch_ext_mem.h"
/* arch driver abstract adaptor */
#include "app_ext_arch_dev.h"

/*@brief 初始化平台
 */
void app_arch_ready(void);

/*@brief 运行平台
 */
void app_arch_execute(void);

/*@brief     变参函数式LOG输出接口
 *@param[in] format 格式化字符串
 */
void app_arch_log_msg1(const char *format, ...);

/*@brief     变参列表式LOG输出接口
 *@param[in] format 格式化字符串
 *@param[in] list   变参列表
 */
void app_arch_log_msg2(const char *format, va_list list);

#endif
