/*需要适配...
 *        JS OS promise适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"
#include "jerryscript-debugger.h"

void jerry_port_track_promise_rejection(const jerry_value_t promise,
                                        const jerry_promise_rejection_operation_t operation)
{
    (void)operation;

    jerry_value_t reason = jerry_get_promise_result(promise);
    jerry_value_t reason_to_string = jerry_value_to_string(reason);
    jerry_size_t req_sz = jerry_get_utf8_string_size(reason_to_string);
    JERRY_VLA(jerry_char_t, str_buf_p, req_sz + 1);
    jerry_string_to_utf8_char_buffer(reason_to_string, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    jerry_release_value(reason);
    jerry_release_value(reason_to_string);

    jerry_port_log(JERRY_LOG_LEVEL_WARNING, "Uncaught (in promise) %s\n", str_buf_p);
}
