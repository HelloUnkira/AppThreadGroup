/*实现目标:
 *    图像处理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像格式转换
 *@param image_format 图像格式
 *@param pixel_format 像素格式
 */
void scui_image_format_to_pixel_format(scui_image_format_t *image_format,
                                       scui_pixel_format_t *pixel_format)
{
    switch (*image_format) {
    case scui_image_format_rgb565:      *pixel_format = scui_pixel_format_rgb565;       break;
    case scui_image_format_rgb888:      *pixel_format = scui_pixel_format_rgb888;       break;
    case scui_image_format_argb8565:    *pixel_format = scui_pixel_format_argb8565;     break;
    case scui_image_format_argb8888:    *pixel_format = scui_pixel_format_argb8888;     break;
    case scui_image_format_p4:          *pixel_format = scui_pixel_format_p4;           break;
    case scui_image_format_p8:          *pixel_format = scui_pixel_format_p8;           break;
    default:                            SCUI_LOG_ERROR("unsupported image pixel");      break;
    }
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
        scui_image_t *image_inst = scui_handle_get(image[idx]);
        SCUI_ASSERT(image_inst != NULL);
        
        if (way)
           *dist += image_inst->pixel.height;
        else
           *dist += image_inst->pixel.width;
    }
}
