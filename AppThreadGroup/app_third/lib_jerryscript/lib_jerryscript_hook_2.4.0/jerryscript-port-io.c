/*需要适配...
 *        JS OS io适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"

static jerry_log_level_t jerry_port_hook_log_level = JERRY_LOG_LEVEL_ERROR;

jerry_log_level_t jerry_port_hook_get_log_level(void)
{
    return jerry_port_hook_log_level;
}

void jerry_port_hook_set_log_level(jerry_log_level_t level)
{
    jerry_port_hook_log_level = level;
}

void jerry_port_log(jerry_log_level_t level, const char *format, ...)
{
    if (level <= jerry_port_hook_log_level)
    {
        va_list  args;
        va_start(args, format);
        jerry_size_t length = vsnprintf(NULL, 0, format, args);
        va_end(args);
        
        va_start(args, format);
        JERRY_VLA(char, buffer, length + 1);
        vsnprintf(buffer, length + 1, format, args);
        APP_SYS_LOG_INFO_RAW(buffer);
        #if defined (JERRY_DEBUGGER) && (JERRY_DEBUGGER == 1)
        jerry_debugger_send_log(level, (jerry_char_t *)buffer, (jerry_size_t)length);
        #endif
        va_end(args);
    }
}

void jerry_port_print_char(char c)
{
    APP_SYS_LOG_INFO_RAW("%c", c);

    #if defined (JERRY_DEBUGGER) && (JERRY_DEBUGGER == 1)
    
    #define DEBUG_BUFFER_SIZE   (256)
    static jerry_size_t debug_buffer_index = 0;
    static jerry_char_t debug_buffer[DEBUG_BUFFER_SIZE];
    debug_buffer[debug_buffer_index++] = c;
    
    if (debug_buffer_index == DEBUG_BUFFER_SIZE || c == '\n')
    {
        jerry_debugger_send_output(debug_buffer, debug_buffer_index);
        debug_buffer_index = 0;
    }
    #endif
}
