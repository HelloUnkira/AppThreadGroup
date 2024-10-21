/*需要适配...
 *        JS OS fatal适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"

/* 致命错误 */
void jerry_port_fatal(jerry_fatal_code_t code)
{
    /* 无论怎样都要退出 */
    APP_SYS_ASSERT(code != 0 && code != ERR_OUT_OF_MEMORY);
    APP_SYS_ASSERT(code != 0 && code == 0);
}
