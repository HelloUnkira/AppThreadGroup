/*实现目标:
 *    图像处理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像格式转换
 *@param image_cf 图像格式
 *@param pixel_cf 像素格式
 */
void scui_image_cf_by_pixel_cf(scui_image_cf_t *image_cf, scui_pixel_cf_t *pixel_cf)
{
    switch (*pixel_cf) {
    case scui_pixel_cf_palette4:    *image_cf = scui_image_cf_palette4;         break;
    case scui_pixel_cf_palette8:    *image_cf = scui_image_cf_palette8;         break;
    case scui_pixel_cf_bmp565:      *image_cf = scui_image_cf_bmp565;           break;
    case scui_pixel_cf_bmp888:      *image_cf = scui_image_cf_bmp888;           break;
    case scui_pixel_cf_bmp8565:     *image_cf = scui_image_cf_bmp8565;          break;
    case scui_pixel_cf_bmp8888:     *image_cf = scui_image_cf_bmp8888;          break;
    default: SCUI_LOG_ERROR("unsupported image pixel");                         break;
    }
}

/*@brief 图像格式转换
 *@param image_cf 图像格式
 *@param pixel_cf 像素格式
 */
void scui_image_cf_to_pixel_cf(scui_image_cf_t *image_cf, scui_pixel_cf_t *pixel_cf)
{
    switch (*image_cf) {
    case scui_image_cf_palette4:    *pixel_cf = scui_pixel_cf_palette4;         break;
    case scui_image_cf_palette8:    *pixel_cf = scui_pixel_cf_palette8;         break;
    case scui_image_cf_bmp565:      *pixel_cf = scui_pixel_cf_bmp565;           break;
    case scui_image_cf_bmp888:      *pixel_cf = scui_pixel_cf_bmp888;           break;
    case scui_image_cf_bmp8565:     *pixel_cf = scui_pixel_cf_bmp8565;          break;
    case scui_image_cf_bmp8888:     *pixel_cf = scui_pixel_cf_bmp8888;          break;
    default: SCUI_LOG_ERROR("unsupported image pixel");                         break;
    }
}

/*@brief 图像宽
 *@param handle 图像句柄
 *@retval 图像宽
 */
scui_coord_t scui_image_w(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    return image->pixel.width;
}

/*@brief 图像高
 *@param handle 图像句柄
 *@retval 图像高
 */
scui_coord_t scui_image_h(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    return image->pixel.height;
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
