/*需要适配...
 *        JS错误退出
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_ARCH_IS_PC

/* 致命错误 */
void JERRY_ATTR_WEAK jerry_port_fatal (jerry_fatal_code_t code)
{
    /* 无论怎样都要退出 */
    APP_SYS_ASSERT(code != 0 && code != JERRY_FATAL_OUT_OF_MEMORY);
    APP_SYS_ASSERT(code != 0 && code == 0);
}

#else
#error "jerry port process is not adapted yet"
#endif
