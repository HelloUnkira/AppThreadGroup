#ifndef SCUI_IMAGE_SRC_H
#define SCUI_IMAGE_SRC_H

/*@brief 图片资源读取
 *@param image 图片资源信息
 *@param data  图片资源
 */
void scui_image_src_read(scui_image_t *image, void *data);

/*brief 图片文件资源读取
 *param name   包名
 *param offset 偏移量
 *param size   数据长度
 *param data   数据流
 */
void scui_image_bin_read(const char *name, uintptr_t offset, uintptr_t size, uint8_t *data);

#endif
