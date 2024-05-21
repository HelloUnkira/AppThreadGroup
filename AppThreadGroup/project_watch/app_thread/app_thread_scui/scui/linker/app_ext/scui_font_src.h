#ifndef SCUI_FONT_SRC_H
#define SCUI_FONT_SRC_H

typedef struct {
    #if APP_ARCH_IS_PC
    FILE *file;
    #endif
} scui_font_src_t;

/*@brief 字库文件操作(打开)
 *@param src  字库文件实例
 *@param name 字库文件名字
 */
void scui_font_src_open(scui_font_src_t *src, char *name);

/*@brief 字库文件操作(关闭)
 *@param src 字库文件实例
 */
void scui_font_src_close(scui_font_src_t *src);

/*@brief 字库文件操作(数据偏移,头部偏移)
 *@param src 字库文件实例
 *@param ofs 数据偏移
 */
void scui_font_src_seek(scui_font_src_t *src, uintptr_t ofs);

/*@brief 字库文件读取指定资源
 *@param src  字库文件实例
 *@param buf  数据缓冲区
 *@param size 数据大小
 */
void scui_font_src_read(scui_font_src_t *src, void *buf, uintptr_t size);

#endif
