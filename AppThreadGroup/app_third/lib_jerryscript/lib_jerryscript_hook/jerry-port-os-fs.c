/*需要适配...
 *        JS OS fs适配
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_OS_IS_WINDOWS

 /* 序列化文件路径,创建文件路径缓冲区 */
jerry_char_t *jerry_port_path_normalize(const jerry_char_t *path_p,
                                        jerry_size_t path_size)
{
    (void)path_size;
    return (jerry_char_t *)_fullpath(NULL, path_p, _MAX_PATH);
}

/* 释放文件路径缓冲区 */
void jerry_port_path_free(jerry_char_t *path_p)
{
    app_mem_free(path_p);
}

/* 从文件路径中计算文件路径长度,不包括文件名 */
jerry_size_t jerry_port_path_base(const jerry_char_t *path_p)
{
    const jerry_char_t *end_p = path_p + strlen ((const char *) path_p);

    while (end_p > path_p)
    {
        if (end_p[-1] == '/' || end_p[-1] == '\\')
        {
            return (jerry_size_t)(end_p - path_p);
        }

        end_p--;
    }

    return 0;
}

#else
#error "jerry port os data is not adapted yet"
#endif
