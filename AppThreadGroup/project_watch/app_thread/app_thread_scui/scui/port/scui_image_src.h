#ifndef SCUI_IMAGE_SRC_H
#define SCUI_IMAGE_SRC_H

/*@brief 图片资源卸载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_unload(scui_image_unit_t *image_unit);

/*@brief 图片资源加载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_load(scui_image_unit_t *image_unit);

/*@brief 图片资源检查
 *@param image 图片资源
 */
bool scui_image_src_check(scui_image_t *image);

#endif
