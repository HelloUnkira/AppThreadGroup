/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 图片资源在代码空间中,一般调试使用 */
#define SCUI_IMAGE_COMBINE_USE_ROM      1

/*@brief 图片资源卸载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_unload(scui_image_unit_t *image_unit)
{
    // 回收image_unit->data
    SCUI_MEM_FREE(image_unit->data);
    image_unit->data = NULL;
}

/*@brief 图片资源加载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_load(scui_image_unit_t *image_unit)
{
    image_unit->data = SCUI_MEM_ALLOC(scui_mem_is_image, image_unit->image->pixel.size_mem);
    // 使用image_uint->image生成==>image_unit->data
    
    #if SCUI_IMAGE_COMBINE_USE_ROM
    
    size_t retval = -1;
    FILE *file = fopen("scui_image_combine.mem", "rb+");
    fseek(file, image_unit->image->pixel.data_mem, SEEK_SET);
    retval = fread(image_unit->data, image_unit->image->pixel.size_mem, 1, file);
    fclose(file);
    
    #if 0 /* lz4跨平台测试不通过,提取data_raw的数据解压缩完毕应该刚好等于data_mem中数据 */
    int ret = LZ4_decompress_safe((const void *)image_unit->image->pixel.data_raw,
                                  (char *)image_unit->data,
                                  image_unit->image->pixel.size_raw,
                                  image_unit->image->pixel.size_mem);
    
    SCUI_ASSERT(ret == image_unit->image->pixel.size_mem);
    #endif
    
    #else
    SCUI_ASSERT(false);
    #endif
}

/*@brief 图片资源检查
 *@param image 图片资源
 */
bool scui_image_src_check(scui_image_t *image)
{
    /* 有必要就可以做 */
    return true;
}
