/*实现目标:
 *    图像处理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像格式转换(就地转换)
 *@param image 图像实例
 *@param rev 通道颠倒
 *@retval 是否支持
 */
bool scui_image_cf_cvt(scui_image_t *image, bool rev)
{
    scui_coord_t image_w = image->pixel.width;
    scui_coord_t image_h = image->pixel.height;
    scui_multi_t image_s = image_w * image_h;
    uint8_t *image_pixel = image->pixel.data_bin;
    
    if (image->format == SCUI_PIXEL_CF_DEF ||
        image->format == SCUI_PIXEL_CF_DEF_A)
        return true;
    
    if (SCUI_PIXEL_CF_DEF == scui_pixel_cf_bmp565 &&
        image->format == scui_pixel_cf_bmp888) {
        image->format  = scui_pixel_cf_bmp565;
        
        scui_color565_t  bmp565_t = {0};
        scui_color565_t *bmp565 = image_pixel;
        scui_color888_t *bmp888 = image_pixel;
        
        if (rev) {
            for(uint32_t idx = 0; idx < image_s; idx++) {
                bmp565_t.ch.b = bmp888[idx].ch.r >> 3;
                bmp565_t.ch.g = bmp888[idx].ch.g >> 2;
                bmp565_t.ch.r = bmp888[idx].ch.b >> 3;
                bmp565 = image_pixel + 2 * idx;
               *bmp565 = bmp565_t;
            }
        } else {
            for(uint32_t idx = 0; idx < image_s; idx++) {
                bmp565_t.ch.r = bmp888[idx].ch.r >> 3;
                bmp565_t.ch.g = bmp888[idx].ch.g >> 2;
                bmp565_t.ch.b = bmp888[idx].ch.b >> 3;
                bmp565 = image_pixel + 2 * idx;
               *bmp565 = bmp565_t;
            }
        }
        
        return true;
    }
    if (SCUI_PIXEL_CF_DEF_A == scui_pixel_cf_bmp8565 &&
        image->format == scui_pixel_cf_bmp8888) {
        image->format  = scui_pixel_cf_bmp8565;
        
        scui_color8565_t  bmp8565_t = {0};
        scui_color8565_t *bmp8565 = image_pixel;
        scui_color8888_t *bmp8888 = image_pixel;
        
        if (rev) {
            for(uint32_t idx = 0; idx < image_s; idx++) {
                bmp8565_t.ch.a = bmp8888[idx].ch.a;
                bmp8565_t.ch.b = bmp8888[idx].ch.r >> 3;
                bmp8565_t.ch.g = bmp8888[idx].ch.g >> 2;
                bmp8565_t.ch.r = bmp8888[idx].ch.b >> 3;
                bmp8565 = image_pixel + 3 * idx;
               *bmp8565 = bmp8565_t;
            }
        } else {
            for(uint32_t idx = 0; idx < image_s; idx++) {
                bmp8565_t.ch.a = bmp8888[idx].ch.a;
                bmp8565_t.ch.r = bmp8888[idx].ch.r >> 3;
                bmp8565_t.ch.g = bmp8888[idx].ch.g >> 2;
                bmp8565_t.ch.b = bmp8888[idx].ch.b >> 3;
                bmp8565 = image_pixel + 3 * idx;
               *bmp8565 = bmp8565_t;
            }
        }
        
        return true;
    }
    
    SCUI_LOG_ERROR("unsupprot format convert");
    return false;
}

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

/*@brief 图像区域
 *@param handle 图像句柄
 *@retval 图像区域
 */
scui_size2_t scui_image_s2(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return (scui_size2_t){
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
}

/*@brief 图像区域
 *@param handle 图像句柄
 *@retval 图像区域
 */
scui_area_t scui_image_area(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    return (scui_area_t){
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
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
