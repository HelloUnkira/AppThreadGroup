#ifndef APP_EXT_ARCH_H
#define APP_EXT_ARCH_H

/* APP arch adaptor: */
#define APP_ARCH_IS_UNKNOWN     0
#define APP_ARCH_IS_PC          1

/* arch ext mem adaptor */
#include "app_ext_arch_ext_mem.h"

/*@brief 初始化平台
 */
void app_arch_ready(void);

/*@brief 运行平台
 */
void app_arch_execute(void);

#endif
