/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// LZ4
#include "lz4.h"
#include "lz4frame_static.h"
#include "lz4frame.h"

// PNG
#include "lodepng_scui.h"

// JPG
#include "tjpgd_scui.h"

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

static size_t scui_LZ4F_size_cur = 0;
static size_t scui_LZ4F_size_top = 0;
static size_t scui_LZ4F_size_rcd = 0;

static void * scui_LZ4F_AllocFunction(void* opaqueState, size_t size)
{
    void *ptr = SCUI_MEM_ALLOC(scui_mem_type_graph, size);
    
    scui_LZ4F_size_cur += scui_mem_size_ptr(ptr);
    if (scui_LZ4F_size_top < scui_LZ4F_size_cur)
        scui_LZ4F_size_top = scui_LZ4F_size_cur;
    
    return ptr;
}
static void * scui_LZ4F_CallocFunction(void* opaqueState, size_t size)
{
    void *ptr = scui_LZ4F_AllocFunction(opaqueState, size);
    memset(ptr, 0, size);
    return ptr;
}
static void   scui_LZ4F_FreeFunction(void* opaqueState, void* address)
{
    scui_LZ4F_size_cur -= scui_mem_size_ptr(address);
    SCUI_MEM_FREE(address);
}

static const  LZ4F_CustomMem scui_LZ4F_CMem = {
    scui_LZ4F_AllocFunction,
    scui_LZ4F_CallocFunction,
    scui_LZ4F_FreeFunction,
    NULL
};

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

static size_t scui_lodepng_size_cur = 0;
static size_t scui_lodepng_size_top = 0;
static size_t scui_lodepng_size_rcd = 0;

void *lodepng_scui_malloc(size_t size)
{
    void *ptr = SCUI_MEM_ALLOC(scui_mem_type_graph, size);
    
    scui_lodepng_size_cur += scui_mem_size_ptr(ptr);
    if (scui_lodepng_size_top < scui_lodepng_size_cur)
        scui_lodepng_size_top = scui_lodepng_size_cur;
    
    return ptr;
}
void lodepng_scui_free(void* ptr)
{
    scui_lodepng_size_cur -= scui_mem_size_ptr(ptr);
    SCUI_MEM_FREE(ptr);
}
void *lodepng_scui_realloc(void* ptr, size_t new_size)
{
    void  *ptr_new = lodepng_scui_malloc(new_size);
    size_t siz_old = scui_mem_size_ptr(ptr);
    memcpy(ptr_new, ptr, scui_min(siz_old, new_size));
    lodepng_scui_free(ptr);
    
    return ptr_new;
}
static void scui_lodepng_cvt_cf(void *png_dec, uint32_t pixel_cnt)
{
    scui_color8888_t *png_argb8888 = png_dec;
    
    for(uint32_t idx = 0; idx < pixel_cnt; idx++) {
        
        scui_color8565_t  png_pixel = {0};
        png_pixel.ch.a = png_argb8888[idx].ch.a;
        png_pixel.ch.r = png_argb8888[idx].ch.b >> 3;
        png_pixel.ch.g = png_argb8888[idx].ch.g >> 2;
        png_pixel.ch.b = png_argb8888[idx].ch.r >> 3;
        
        scui_color8565_t *pixel = (void *)((uint8_t *)png_dec + 3 * idx);
        *pixel = png_pixel;
    }
}

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

// 这个库的API设计的挺有意思的,巧妙
uintptr_t     scui_tjpgd_ptr_ofs = 0;
uint8_t      *scui_tjpgd_dat_out = NULL;
uint8_t      *scui_tjpgd_dat_in  = NULL;
uintptr_t     scui_tjpgd_dat_ins = 0;
scui_image_t *scui_tjpgd_image   = NULL;

size_t scui_tjpgd_in_func_cb(JDEC *jd, uint8_t *buff, size_t nbyte)
{
    if (buff != NULL) {
        SCUI_ASSERT(scui_tjpgd_dat_in != NULL);
        if (scui_tjpgd_ptr_ofs + nbyte >= scui_tjpgd_dat_ins) {
            nbyte = scui_tjpgd_dat_ins - scui_tjpgd_ptr_ofs;
        }
        scui_draw_line_copy(buff, scui_tjpgd_dat_in + scui_tjpgd_ptr_ofs, nbyte);
        scui_tjpgd_ptr_ofs += nbyte;
        return nbyte;
    } else {
        scui_tjpgd_ptr_ofs += nbyte;
        return nbyte;
    }
}
int scui_tjpgd_out_func_cb(JDEC *jd, void *bitmap, JRECT *rect)
{
    SCUI_ASSERT(scui_tjpgd_image != NULL);
    
    // JPG输出格式:
    uint8_t  jpg_bytes  = scui_pixel_bits(scui_pixel_cf_bmp565) / 8;
    uint32_t jpg_data_w = scui_tjpgd_image->pixel.width;
    uint32_t jpg_rect_w = rect->right - rect->left + 1;
    uint32_t jpg_offset = jpg_data_w * rect->top + rect->left;
    jpg_data_w *= jpg_bytes;
    jpg_rect_w *= jpg_bytes;
    
    uint8_t *dst = scui_tjpgd_dat_out + jpg_bytes * jpg_offset;
    uint8_t *src = bitmap;
    
    for (uint32_t line = rect->top; line <= rect->bottom; line++) {
        scui_draw_line_copy(dst, src, jpg_rect_w);
        dst += jpg_data_w;
        src += jpg_rect_w;
    }
    
    return 1;
}

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 图片资源读取
 *@param image 图片资源信息
 *@param data  图片资源
 */
void scui_image_src_read(scui_image_t *image, void *data)
{
    // 使用image生成==>data
    
    if (image->type == scui_image_type_jpg) {
        
        size_t jpg_size = image->pixel.size_bin;
        void  *jpg_data = SCUI_MEM_ALLOC(scui_mem_type_graph, jpg_size);
        
        FILE *file = fopen("scui_image_parser.bin", "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        fread(jpg_data, jpg_size, 1, file);
        fclose(file);
        
        scui_tjpgd_ptr_ofs = 0;
        scui_tjpgd_dat_out = data;
        scui_tjpgd_dat_in  = jpg_data;
        scui_tjpgd_dat_ins = jpg_size;
        scui_tjpgd_image   = image;
        
        JDEC jdec;
        void *work = SCUI_MEM_ALLOC(scui_mem_type_graph, JD_SZBUF * 4);
        JRESULT res = jd_prepare_scui(&jdec, scui_tjpgd_in_func_cb, work, JD_SZBUF * 4, NULL);
        if (res != JDR_OK) {
            SCUI_LOG_ERROR("ret:%d", res);
            SCUI_ASSERT(false);
        }
        
        res = jd_decomp_scui(&jdec, scui_tjpgd_out_func_cb, 0);
        if (res != JDR_OK) {
            SCUI_LOG_ERROR("ret:%d", res);
            SCUI_ASSERT(false);
        }
        
        SCUI_MEM_FREE(work);
        SCUI_MEM_FREE(jpg_data);
        
        return;
    }
    
    if (image->type == scui_image_type_png) {
        
        scui_lodepng_size_cur = 0;
        scui_lodepng_size_top = 0;
        
        size_t png_size = image->pixel.size_bin;
        void  *png_data = lodepng_scui_malloc(png_size);
        
        FILE *file = fopen("scui_image_parser.bin", "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        fread(png_data, png_size, 1, file);
        fclose(file);
        
        /* 这个库只能解出ARGB8888格式 */
        uint8_t *png_data_dec = NULL;
        unsigned png_w_dec = 0;
        unsigned png_h_dec = 0;
        unsigned lib_error = lodepng_scui_decode32(&png_data_dec, &png_w_dec, &png_h_dec, png_data, png_size);
        
        if (lib_error != 0) {
            SCUI_LOG_ERROR("%s", lodepng_scui_error_text(lib_error));
            SCUI_ASSERT(false);
        }
        SCUI_ASSERT(png_w_dec == image->pixel.width);
        SCUI_ASSERT(png_h_dec == image->pixel.height);
        SCUI_ASSERT(png_data_dec != NULL);
        
        /* 这个库只能解出ARGB8888格式,所以还需要流处理 */
        scui_lodepng_cvt_cf(png_data_dec, png_h_dec * png_w_dec);
        
        scui_draw_line_copy(data, png_data_dec, scui_image_size(image));
        lodepng_scui_free(png_data_dec);
        lodepng_scui_free(png_data);
        
        if (scui_lodepng_size_rcd < scui_lodepng_size_top) {
            scui_lodepng_size_rcd = scui_lodepng_size_top;
            SCUI_LOG_WARN("png occupy top:%d", scui_lodepng_size_top);
        }
        
        return;
    }
    
    if (image->type == scui_image_type_lz4) {
        /* lz4跨语种平台测试 */
        /* src在python的pc环境下生成,dst在c的pc环境下解析 */
        /* 提取data_raw的数据解压缩完毕应该刚好等于data_mem中数据 */
        
        /* 插眼备注:这里关于内存的使用情况存在争议性 */
        /* 该如何恰当的为此处生成的内存提前预备好内存空间呢 */
        scui_LZ4F_size_cur = 0;
        scui_LZ4F_size_top = 0;
        
        size_t com_size = image->pixel.size_bin;
        void  *com_data = scui_LZ4F_AllocFunction(NULL, com_size);
        
        FILE *file = fopen("scui_image_parser.bin", "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        fread(com_data, com_size, 1, file);
        fclose(file);
        
        LZ4F_dctx *dctx = LZ4F_createDecompressionContext_advanced(scui_LZ4F_CMem, LZ4F_VERSION);
        SCUI_ASSERT(dctx != NULL);
        size_t dec_size = scui_image_size(image);
        size_t dec_ret  = LZ4F_decompress(dctx, data, &dec_size, com_data, &com_size, NULL);
        LZ4F_errorCode_t dec_err = LZ4F_freeDecompressionContext(dctx);
        scui_LZ4F_FreeFunction(NULL, com_data);
        SCUI_ASSERT(scui_LZ4F_size_cur == 0);
        
        if (dec_ret != 0)
            SCUI_LOG_ERROR("%s", LZ4F_getErrorName(dec_ret));
        if (dec_err != 0)
            SCUI_LOG_ERROR("%s", LZ4F_getErrorName(dec_err));
        
        SCUI_ASSERT(dec_ret == 0);
        SCUI_ASSERT(dec_err == 0);
        
        if (scui_LZ4F_size_rcd < scui_LZ4F_size_top) {
            scui_LZ4F_size_rcd = scui_LZ4F_size_top;
            SCUI_LOG_WARN("lz4 occupy top:%d", scui_LZ4F_size_top);
        }
        
        return;
    }
    
    if (image->type == scui_image_type_bmp) {
        // 原始内存数据,直接使用的
        size_t retval = -1;
        FILE *file = fopen("scui_image_parser.bin", "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        retval = fread(data, image->pixel.size_bin, 1, file);
        fclose(file);
        
        return;
    }
    
    SCUI_ASSERT(false);
}
