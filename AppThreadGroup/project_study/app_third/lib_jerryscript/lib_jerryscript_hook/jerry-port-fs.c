/*需要适配...
 *    JS文件系统,低端嵌入式平台一般不使用,文件是直接载入的字符流
 */

#include "app_ext_lib.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_ARCH_IS_PC

#if defined(__GLIBC__) || defined(_WIN32)
#include <sys/stat.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)(((mode) &S_IFMT) == S_IFDIR)
#endif
#endif

/* 获得文件大小 */
static jerry_size_t jerry_port_get_file_size(FILE *file_p)
{
    fseek(file_p, 0, SEEK_END);
    size_t size = ftell(file_p);
    fseek(file_p, 0, SEEK_SET);
    return (jerry_size_t)size;
}

/* 读取文件源,并创建文件源缓冲区 */
jerry_char_t *JERRY_ATTR_WEAK jerry_port_source_read(const char *file_name_p, jerry_size_t *out_size_p)
{
    #if defined(__GLIBC__) || defined(_WIN32)
    /* 带文件状态信息,检查一下文件有效性 */
    struct stat stat_buffer;
    if (stat(file_name_p, &stat_buffer) == -1 || S_ISDIR(stat_buffer.st_mode))
        return NULL;
    #endif
    
    /* 打开文件 */
    FILE *file_p = fopen(file_name_p, "rb");
    if (file_p == NULL)
        return NULL;
    /* 获得文件大小并生成对应大小的缓冲区 */
    jerry_size_t file_size = jerry_port_get_file_size(file_p);
    jerry_char_t *buffer_p = app_mem_alloc(file_size);
    if (buffer_p == NULL) {
        fclose(file_p);
        return NULL;
    }
    /* 读取文件源 */
    size_t bytes_read = fread(buffer_p, 1u, file_size, file_p);
    if (bytes_read != file_size) {
        app_mem_free(buffer_p);
        fclose(file_p);
        return NULL;
    }
    /* 信息转出 */
    *out_size_p = (jerry_size_t)bytes_read;
    fclose(file_p);
    return buffer_p;
}

/* 释放文件源缓冲区 */
void JERRY_ATTR_WEAK jerry_port_source_free(uint8_t *buffer_p)
{
    app_mem_free(buffer_p);
}

#if defined(JERRY_WEAK_SYMBOL_SUPPORT) && !(defined(__unix__) || defined(__APPLE__) || defined(_WIN32))

/* 序列化文件路径,创建文件路径缓冲区 */
jerry_char_t *JERRY_ATTR_WEAK jerry_port_path_normalize(const jerry_char_t *path_p, jerry_size_t path_size)
{
    jerry_char_t *buffer_p = app_mem_alloc(path_size + 1);
    if (buffer_p != NULL)
        memcpy(buffer_p, path_p, path_size + 1);
    return buffer_p;
}

/* 释放文件路径缓冲区 */
void JERRY_ATTR_WEAK jerry_port_path_free(jerry_char_t *path_p)
{
    app_mem_free(path_p);
}

/* 从文件路径中计算文件路径长度,不包括文件名 */
jerry_size_t JERRY_ATTR_WEAK jerry_port_path_base(const jerry_char_t *path_p)
{
    const jerry_char_t *basename_p =(jerry_char_t *) strrchr((char *) path_p, '/') + 1;
    return (jerry_size_t)(basename_p == NULL ? 0 : basename_p - path_p);
}

#endif

#else
#error "jerry port fs is not adapted yet"
#endif
