#ifndef SCUI_IMAGE_FRAME_H
#define SCUI_IMAGE_FRAME_H

typedef struct {
    scui_image_type_t   type;   // 图像类型
    scui_image_t        image;  // 图像信息
    scui_handle_t       frame;  // 图像名
    scui_handle_t       handle; // 图像名
    uint8_t *data;  // 图像帧源
    uint32_t size;  // 图像帧源大小
    void    *local; // 内部数据
    // 有限支持度
    uint32_t loop;
} scui_image_frame_t;

/*@brief 图像帧数据销毁
 *@param image_frame 图像帧实例
 */
void scui_image_frame_burn(scui_image_frame_t *image_frame);

/*@brief 图像帧数据构造
 *@param image_frame 图像帧实例
 */
void scui_image_frame_make(scui_image_frame_t *image_frame);

/*@brief 图像帧数据获取
 *@param image_frame 图像帧实例
 *@retval 获得有效帧
 */
bool scui_image_frame_data(scui_image_frame_t *image_frame);

#endif
