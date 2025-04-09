#ifndef SCUI_IMAGE_H
#define SCUI_IMAGE_H

/*@brief 图片状态
 */
typedef enum {
    scui_image_type_bmp = 0,    // 默认图片(无压缩)
    scui_image_type_lz4,        // 压缩图片(lz4,只压缩bitmap)
    scui_image_type_jpg,        // JPG压缩图片(原图)
    scui_image_type_png,        // PNG压缩图片(原图)
    scui_image_type_gif,        // GIF压缩序列图片(原图)
    scui_image_type_lottie,     // Lottie压缩序列图片(原图)
    scui_image_type_mp4,        // MP4压缩序列图片(原图)
    scui_image_type_mem,        // 内存图片(bmp)
    scui_image_type_num,
} scui_image_type_t;

/*@brief 图片像素数据
 */
typedef struct {
    uintptr_t width;        // 图片宽
    uintptr_t height;       // 图片高
    uintptr_t data_bin;     // 图片数据源(外存源数据流偏移)
    uintptr_t size_bin;     // 图片数据源(外存源大小)
} scui_image_pixel_t;

/*@brief 图片数据
 */
typedef struct {
    scui_pixel_cf_t     format;     // 图片格式
    scui_image_type_t   type;       // 图片类型
    scui_image_pixel_t  pixel;      // 图片色彩数据
    scui_handle_t       from;       // 图库来源
} scui_image_t;

/*@brief 图像格式
 *@param handle 图像句柄
 *@retval 图像格式
 */
scui_pixel_cf_t scui_image_cf(scui_handle_t handle);

/*@brief 图像类型
 *@param handle 图像句柄
 *@retval 图像类型
 */
scui_image_type_t scui_image_type(scui_handle_t handle);

/*@brief 图像宽
 *@param handle 图像句柄
 *@retval 图像宽
 */
scui_coord_t scui_image_w(scui_handle_t handle);

/*@brief 图像高
 *@param handle 图像句柄
 *@retval 图像高
 */
scui_coord_t scui_image_h(scui_handle_t handle);

/*@brief 图像区域
 *@param handle 图像句柄
 *@retval 图像区域
 */
scui_size2_t scui_image_s2(scui_handle_t handle);

/*@brief 图像区域
 *@param handle 图像句柄
 *@retval 图像区域
 */
scui_area_t scui_image_area(scui_handle_t handle);

/*@brief 图像大小
 *@param image 图像实例
 *@retval 图像大小
 */
uintptr_t scui_image_size(scui_image_t *image);

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
