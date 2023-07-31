/*需要适配...
 *        JS OS context适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"

static jerry_context_t *current_context_p = NULL;

void jerry_port_hook_set_current_context(jerry_context_t *context_p)
{
    current_context_p = context_p;
}

jerry_context_t * jerry_port_get_current_context(void)
{
    return current_context_p;
}
