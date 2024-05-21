/*实现目标:
 *    字体源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字库文件操作(打开)
 *@param src  字库文件实例
 *@param name 字库文件名字
 */
void scui_font_src_open(scui_font_src_t *src, char *name)
{
    // 字库文件共用一个文件夹
    const char *path = ".";
    
    char path_name[128] = {0};
    snprintf(path_name, 127, "%s\\%s", path, name);
    
    src->file = fopen(path_name, "rb+");
}

/*@brief 字库文件操作(关闭)
 *@param src 字库文件实例
 */
void scui_font_src_close(scui_font_src_t *src)
{
    fclose(src->file);
    src->file = NULL;
}

/*@brief 字库文件操作(数据偏移,头部偏移)
 *@param src 字库文件实例
 *@param ofs 数据偏移
 */
void scui_font_src_seek(scui_font_src_t *src, uintptr_t ofs)
{
    fseek(src->file, ofs, SEEK_SET);
}

/*@brief 字库文件读取指定资源
 *@param src  字库文件实例
 *@param buf  数据缓冲区
 *@param size 数据大小
 */
void scui_font_src_read(scui_font_src_t *src, void *buf, uintptr_t size)
{
    fread(buf, size, 1, src->file);
}
