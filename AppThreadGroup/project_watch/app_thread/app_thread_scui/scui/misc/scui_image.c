/*实现目标:
 *    图像处理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像格式
 *@param handle 图像句柄
 *@retval 图像格式
 */
scui_pixel_cf_t scui_image_cf(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return image->format;
}

/*@brief 图像类型
 *@param handle 图像句柄
 *@retval 图像类型
 */
scui_image_type_t scui_image_type(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return image->type;
}

/*@brief 图像宽
 *@param handle 图像句柄
 *@retval 图像宽
 */
scui_coord_t scui_image_w(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return image->pixel.width;
}

/*@brief 图像高
 *@param handle 图像句柄
 *@retval 图像高
 */
scui_coord_t scui_image_h(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return image->pixel.height;
}

/*@brief 图像大小
 *@param image 图像实例
 *@retval 图像大小
 */
uintptr_t scui_image_size(scui_image_t *image)
{
    return image->pixel.width * image->pixel.height * scui_pixel_bits(image->format) / 8;
}

/*@brief 统计图像列表排列尺寸
 *       统计图像按水平或垂直排列时的宽度或高度
 *@param image 图像列表
 *@param num   图像列表数量
 *@param dist  尺寸
 *@param way   方向(0:水平方向;1:垂直方向)
 */
void scui_image_list_calc(scui_handle_t *image, scui_coord_t num,
                          scui_coord_t  *dist,  bool way)
{
    SCUI_ASSERT(dist != NULL);
    
    for (scui_coord_t idx = 0; idx < num; idx++) {
        if (image[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_image_t *image_inst = scui_handle_source_check(image[idx]);
        
        if (way)
           *dist += image_inst->pixel.height;
        else
           *dist += image_inst->pixel.width;
    }
}
