/*需要适配...
 *        JS OS debugger适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"

void jerry_port_sleep(uint32_t sleep_time)
{
    app_delay_ms(sleep_time);
}
