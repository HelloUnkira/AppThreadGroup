#ifndef SCUI_FONT_SRC_H
#define SCUI_FONT_SRC_H

/*@brief 字库文件读取指定资源
 *@param name   字库文件名字(无路径)
 *@param offset 文件偏移点
 *@param buffer 数据缓冲区
 *@param size   数据大小
 */
void scui_font_src_read(char *name, uintptr_t offset, uint8_t *buffer, uintptr_t size);

#endif
