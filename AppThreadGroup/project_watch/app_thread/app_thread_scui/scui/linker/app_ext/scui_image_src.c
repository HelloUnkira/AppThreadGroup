/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#include "lz4.h"
#include "lz4frame_static.h"
#include "lz4frame.h"

static size_t scui_LZ4F_size = 0;
static size_t scui_LZ4F_size_rcd = 0;

static void * scui_LZ4F_AllocFunction(void* opaqueState, size_t size)
{
    scui_LZ4F_size += size;
    void *ptr = SCUI_MEM_ALLOC(scui_mem_type_graph, size);
    return ptr;
}
static void * scui_LZ4F_CallocFunction(void* opaqueState, size_t size)
{
    scui_LZ4F_size += size;
    void *ptr = SCUI_MEM_ALLOC(scui_mem_type_graph, size);
    memset(ptr, 0, size);
    return ptr;
}
static void   scui_LZ4F_FreeFunction(void* opaqueState, void* address)
{
    SCUI_MEM_FREE(address);
}

static const  LZ4F_CustomMem scui_LZ4F_CMem = {
    scui_LZ4F_AllocFunction,
    scui_LZ4F_CallocFunction,
    scui_LZ4F_FreeFunction,
    NULL
};

/*@brief 图片资源读取
 *@param image 图片资源信息
 *@param data  图片资源
 */
void scui_image_src_read(scui_image_t *image, void *data)
{
    // 使用image生成==>data
    
    
    
    #if 0   // 原始内存数据,直接使用的
    size_t retval = -1;
    FILE *file = fopen("scui_image_parser.mem", "rb+");
    fseek(file, image->pixel.data_mem, SEEK_SET);
    retval = fread(data, image->pixel.size_mem, 1, file);
    fclose(file);
    
    #elif 1
    /* lz4跨语种平台测试 */
    /* src在python的pc环境下生成,dst在c的pc环境下解析 */
    /* 提取data_raw的数据解压缩完毕应该刚好等于data_mem中数据 */
    
    /* 插眼备注:这里关于内存的使用情况存在争议性 */
    /* 该如何恰当的为此处生成的内存提前预备好内存空间呢 */
    scui_LZ4F_size = 0;
    
    size_t com_size = image->pixel.size_raw;
    void  *com_data = scui_LZ4F_AllocFunction(NULL, com_size);
    
    FILE *file = fopen("scui_image_parser.bin", "rb+");
    fseek(file, image->pixel.data_raw, SEEK_SET);
    fread(com_data, com_size, 1, file);
    fclose(file);
    
    LZ4F_dctx *dctx = LZ4F_createDecompressionContext_advanced(scui_LZ4F_CMem, LZ4F_VERSION);
    SCUI_ASSERT(dctx != NULL);
    size_t dec_size = image->pixel.size_mem;
    size_t dec_ret  = LZ4F_decompress(dctx, data, &dec_size, com_data, &com_size, NULL);
    LZ4F_errorCode_t dec_err = LZ4F_freeDecompressionContext(dctx);
    
    if (dec_ret != 0)
        SCUI_LOG_ERROR("%s", LZ4F_getErrorName(dec_ret));
    if (dec_err != 0)
        SCUI_LOG_ERROR("%s", LZ4F_getErrorName(dec_err));
    
    SCUI_ASSERT(dec_ret == 0);
    SCUI_ASSERT(dec_err == 0);
    
    scui_LZ4F_FreeFunction(NULL, com_data);
    
    if (scui_LZ4F_size_rcd < scui_LZ4F_size) {
        scui_LZ4F_size_rcd = scui_LZ4F_size;
        SCUI_LOG_WARN("lz4 occupy:%d", scui_LZ4F_size);
    }
    
    #else
    SCUI_ASSERT(false);
    #endif
}
