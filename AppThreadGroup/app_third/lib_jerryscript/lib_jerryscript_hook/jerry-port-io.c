/*需要适配...
 *        JS数据IO
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_ARCH_IS_PC

/* 数据流日志 */
void JERRY_ATTR_WEAK jerry_port_log(const char *message_p)
{
    APP_SYS_LOG_INFO_RAW(message_p);
}

/* 数据日志 */
void JERRY_ATTR_WEAK jerry_port_print_byte(jerry_char_t byte)
{
    APP_SYS_LOG_INFO_RAW("%02x ", byte);
}

/* 数据流日志 */
void JERRY_ATTR_WEAK jerry_port_print_buffer(const jerry_char_t *buffer_p,
                                             jerry_size_t buffer_size)
{
    for(jerry_size_t idx = 0; idx < buffer_size; idx++)
    {
        jerry_port_print_byte(buffer_p[idx]);
    }
}

/* 输入流获得数据,创建数据流缓冲区 */
jerry_char_t *JERRY_ATTR_WEAK jerry_port_line_read(jerry_size_t *out_size_p)
{
    char *line_p = NULL;
    size_t allocated = 0;
    size_t bytes = 0;

    while(true)
    {
        allocated += 64;
        line_p = app_mem_realloc(line_p, allocated);

        while(bytes < allocated - 1)
        {
            char ch =(char) fgetc(stdin);

            if(feof(stdin))
            {
                app_mem_free(line_p);
                return NULL;
            }

            line_p[bytes++] = ch;

            if(ch == '\n')
            {
                *out_size_p =(jerry_size_t) bytes;
                line_p[bytes++] = '\0';
                return(jerry_char_t *) line_p;
            }
        }
    }
}

/* 释放数据流缓冲区 */
void JERRY_ATTR_WEAK jerry_port_line_free(jerry_char_t *buffer_p)
{
    app_mem_free(buffer_p);
}

#else
#error "jerry port io is not adapted yet"
#endif
