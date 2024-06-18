#ifndef SCUI_IMAGE_H
#define SCUI_IMAGE_H

/*@brief 图片像素格式
 *       枚举值为(bits << 8) | index
 */
typedef enum {
    scui_image_format_palette4,
    scui_image_format_palette8,
    scui_image_format_bmp565,
    scui_image_format_bmp888,
    scui_image_format_bmp8565,
    scui_image_format_bmp8888,
} scui_image_format_t;

/*@brief 图片状态
 */
typedef enum {
    scui_image_status_none = 0,     // 默认图片
    scui_image_status_lz4,          // 压缩图片
    scui_image_status_num,
} scui_image_status_t;

/*@brief 图片像素数据
 */
typedef struct {
    uintptr_t width;        // 图片宽
    uintptr_t height;       // 图片高
    uintptr_t data_raw;     // 图片数据源(外存源数据流偏移)
    uintptr_t data_mem;     // 图片数据源(内存源数据流偏移,一般调试使用)
    uintptr_t size_raw;     // 图片数据源(外存源大小)
    uintptr_t size_mem;     // 图片数据源(内存源大小)
} scui_image_pixel_t;

/*@brief 图片数据
 */
typedef struct {
    scui_image_format_t format;     // 图片格式
    scui_image_status_t status;     // 图片状态
    scui_image_pixel_t  pixel;      // 图片色彩数据
} scui_image_t;

/*@brief 图像格式转换
 *@param image_format 图像格式
 *@param pixel_format 像素格式
 */
void scui_image_format_to_pixel_format(scui_image_format_t *image_format,
                                       scui_pixel_cf_t *pixel_format);

/*@brief 统计图像列表排列尺寸
 *       统计图像按水平或垂直排列时的宽度或高度
 *@param image 图像列表
 *@param num   图像列表数量
 *@param dist  尺寸
 *@param way   方向(0:水平方向;1:垂直方向)
 */
void scui_image_list_calc(scui_handle_t *image, scui_coord_t num,
                          scui_coord_t  *dist,  bool way);

#endif
