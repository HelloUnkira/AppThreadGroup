/*需要适配...
 *        JS OS module适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0      /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-hook.h"

#if APP_OS_IS_LINUX
#include <libgen.h>
#include <sys/stat.h>
#endif

#if APP_ARCH_IS_PC

static size_t jerry_port_get_file_size(FILE *file_p)
{
    fseek(file_p, 0, SEEK_END);
    long size = ftell(file_p);
    fseek(file_p, 0, SEEK_SET);

    return (size_t)size;
}

uint8_t * jerry_port_read_source(const char *file_name_p, size_t *out_size_p)
{
    #if APP_OS_IS_LINUX
    struct stat stat_buffer;
    if (stat(file_name_p, &stat_buffer) == -1 || stat_buffer.st_mode & S_IFMT == S_IFDIR)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error: Failed to open file: %s\n", file_name_p);
        return NULL;
    }
    #endif
    
    FILE *file_p = fopen(file_name_p, "rb");
    if (file_p == NULL)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error: Failed to open file: %s\n", file_name_p);
        return NULL;
    }
    
    size_t file_size  = jerry_port_get_file_size(file_p);
    uint8_t *buffer_p = app_mem_alloc(file_size);
    
    if (buffer_p == NULL)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error: Failed to allocate memory for file: %s\n", file_name_p);
        fclose(file_p);
        return NULL;
    }
    
    size_t bytes_read = fread(buffer_p, 1u, file_size, file_p);
    
    if (bytes_read != file_size)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error: Failed to read file: %s\n", file_name_p);
        app_mem_free(buffer_p);
        fclose(file_p);
        return NULL;
    }
    
    *out_size_p = bytes_read;
    fclose(file_p);
    return buffer_p;
}

#else
#error "not adaptor yet"
#endif

void jerry_port_release_source(uint8_t *buffer_p)
{
    app_mem_free(buffer_p);
}

size_t jerry_port_normalize_path (const char *in_path_p, char *out_buf_p, size_t out_buf_size, char *base_file_p)
{
    size_t ret = 0;
    
    #if 0
    #elif APP_OS_IS_WINDOWS
    
    char *path_p;
    size_t base_drive_dir_len;
    const size_t in_path_len = strnlen(in_path_p, _MAX_PATH);
    
    if (base_file_p != NULL)
    {
        char drive[_MAX_DRIVE];
        char *dir_p = app_mem_alloc(_MAX_DIR);

        _splitpath_s (base_file_p, drive, _MAX_DRIVE, dir_p, _MAX_DIR, NULL, 0, NULL, 0);
        const size_t dir_len = strnlen (dir_p, _MAX_DIR);
        const size_t drive_len = strnlen (&drive, _MAX_DRIVE);
        
        base_drive_dir_len = drive_len + dir_len;
        path_p = app_mem_alloc(base_drive_dir_len + in_path_len + 1);

        memcpy(path_p, &drive, drive_len);
        memcpy(path_p + drive_len, dir_p, dir_len);

        app_mem_free(dir_p);
    }
    else
    {
        base_drive_dir_len = 0;
        path_p = app_mem_alloc(in_path_len + 1);
    }
    
    memcpy(path_p + base_drive_dir_len, in_path_p, in_path_len + 1);
    char *norm_p = _fullpath(out_buf_p, path_p, out_buf_size);
    app_mem_free(path_p);
    if (norm_p != NULL)
    {
        ret = strnlen(norm_p, out_buf_size);
    }
    
    #elif APP_OS_IS_LINUX
    
    char *base_dir_p = dirname(base_file_p);
    const size_t in_path_len = strnlen(in_path_p, PATH_MAX);
    const size_t base_dir_len = strnlen(base_dir_p, PATH_MAX);
    char *path_p = app_mem_alloc(base_dir_len + 1 + in_path_len + 1);
    
    memcpy(path_p, base_dir_p, base_dir_len);
    memcpy(path_p + base_dir_len, "/", 1);
    memcpy(path_p + base_dir_len + 1, in_path_p, in_path_len + 1);
    
    char *norm_p = realpath(path_p, NULL);
    app_mem_free(path_p);
    
    if (norm_p != NULL)
    {
        const size_t norm_len = strnlen(norm_p, out_buf_size);
        if (norm_len < out_buf_size)
        {
            memcpy(out_buf_p, norm_p, norm_len + 1);
            ret = norm_len;
        }
        app_mem_free(norm_p);
    }
    #else
        
    (void)base_file_p;
    const size_t in_path_len = strnlen(in_path_p, out_buf_size);
    if (in_path_len < out_buf_size)
    {
        memcpy(out_buf_p, in_path_p, in_path_len + 1);
        ret = in_path_len;
    }
    
    #endif
    
    return ret;
}









jerry_value_t jerry_port_get_native_module(jerry_value_t name)
{
    (void) name;
    return jerry_create_undefined();
}
