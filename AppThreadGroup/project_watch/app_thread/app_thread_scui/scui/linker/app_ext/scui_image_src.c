/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 图片资源在代码空间中,一般调试使用 */
#define SCUI_IMAGE_PARSER_USE_ROM      1

/*@brief 图片资源读取
 *@param image 图片资源信息
 *@param data  图片资源
 */
void scui_image_src_read(scui_image_t *image, void *data)
{
    // 使用image生成==>data
    
    #if SCUI_IMAGE_PARSER_USE_ROM
    
    size_t retval = -1;
    FILE *file = fopen("scui_image_parser.mem", "rb+");
    fseek(file, image->pixel.data_mem, SEEK_SET);
    retval = fread(data, image->pixel.size_mem, 1, file);
    fclose(file);
    
    #if 0 /* lz4跨平台测试不通过,提取data_raw的数据解压缩完毕应该刚好等于data_mem中数据 */
    int ret = LZ4_decompress_safe(image->pixel.data_raw, data,
                                  image->pixel.size_raw,
                                  image->pixel.size_mem);
    
    SCUI_ASSERT(ret == image->pixel.size_mem);
    #endif
    
    #else
    SCUI_ASSERT(false);
    #endif
}
