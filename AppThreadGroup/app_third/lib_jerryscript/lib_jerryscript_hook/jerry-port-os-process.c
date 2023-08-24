/*需要适配...
 *        JS OS process适配
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_OS_IS_WINDOWS

void jerry_port_sleep(uint32_t sleep_time)
{
    Sleep(sleep_time);
}

#else
#error "jerry port os process is not adapted yet"
#endif
