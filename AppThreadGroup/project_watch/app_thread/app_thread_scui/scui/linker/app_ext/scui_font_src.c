/*实现目标:
 *    字体源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字库文件读取指定资源
 *@param name   字库文件名字(无路径)
 *@param offset 文件偏移点
 *@param buffer 数据缓冲区
 *@param size   数据大小
 */
void scui_font_src_read(char *name, uintptr_t offset, uint8_t *buffer, uintptr_t size)
{
    char *path_name[128] = {0};
    const char*path = ".";
    snprintf(path_name, 127, "%s\\%s", path, name);
    
    size_t retval = -1;
    FILE *file = fopen(path_name, "rb+");
    fseek(file, offset, SEEK_SET);
    retval = fread(buffer, size, 1, file);
    fclose(file);
}
