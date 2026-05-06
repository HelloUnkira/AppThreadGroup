/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* LZ4 */
#include "lz4.h"
#include "lz4frame_static.h"
#include "lz4frame.h"

/* PNG */
/* 备注:这个组件对瞬时内存的消耗量较大 */
/* 并且它有realloc这个操作,导致峰值内存更多 */
#include "lodepng.h"

/* JPG */
#include "tjpgd.h"

/* 二维码生成器库(qrcode): */
#include "qrcodegen.h"

/* 条形码生成器库(barcode): */
#include "code128.h"

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

void *lodepng_malloc(size_t size)
{
    void *ptr = SCUI_MEM_ALLOC(scui_mem_type_graph, size + sizeof(uintptr_t) * 2);
    
    scui_lodepng_size_cur += scui_mem_size_ptr(ptr);
    if (scui_lodepng_size_top < scui_lodepng_size_cur)
        scui_lodepng_size_top = scui_lodepng_size_cur;
    
    return ptr;
}
void lodepng_free(void* ptr)
{
    scui_lodepng_size_cur -= scui_mem_size_ptr(ptr);
    SCUI_MEM_FREE(ptr);
}
void *lodepng_realloc(void* ptr, size_t new_size)
{
    void  *ptr_new = lodepng_malloc(new_size);
    size_t siz_old = scui_mem_size_ptr(ptr);
    memcpy(ptr_new, ptr, scui_min(siz_old, new_size));
    lodepng_free(ptr);
    
    return ptr_new;
}

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/* 这个库的API设计的挺有意思的,巧妙 */
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
        void *src_addr = scui_tjpgd_dat_in + scui_tjpgd_ptr_ofs;
        scui_draw_byte_copy(true, buff, src_addr, nbyte);
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
    
    /* JPG输出格式: */
    uint8_t  jpg_bytes  = scui_pixel_byte(scui_pixel_cf_bmp565);
    uint32_t jpg_data_w = scui_tjpgd_image->pixel.width;
    uint32_t jpg_rect_w = rect->right - rect->left + 1;
    uint32_t jpg_offset = jpg_data_w * rect->top + rect->left;
    jpg_data_w *= jpg_bytes;
    jpg_rect_w *= jpg_bytes;
    
    uint8_t *dst = scui_tjpgd_dat_out + jpg_bytes * jpg_offset;
    uint8_t *src = bitmap;
    
    for (uint32_t line = rect->top; line <= rect->bottom; line++) {
        scui_draw_byte_copy(true, dst, src, jpg_rect_w);
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
    /* 使用image生成==>data */
    const char *image_bin = "scui_image_parser.bin";
    const char *image_src = image_bin;
    if (image->from != SCUI_HANDLE_INVALID)
        image_src = scui_handle_source(image->from);
    
    
    
    bool image_type_raw = false;
    image_type_raw = image_type_raw || image->type == scui_image_type_bmp;
    image_type_raw = image_type_raw || image->type == scui_image_type_gif;
    image_type_raw = image_type_raw || image->type == scui_image_type_lottie;
    
    /* 原始内存数据,直接使用的 */
    if (image_type_raw) {
        size_t retval = -1;
        FILE *file = fopen(image_src, "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        retval = fread(data, image->pixel.size_bin, 1, file);
        fclose(file);
        
        return;
    }
    
    if (image->type == scui_image_type_jpg) {
        
        size_t jpg_size = image->pixel.size_bin;
        void  *jpg_data = SCUI_MEM_ALLOC(scui_mem_type_graph, jpg_size);
        
        FILE *file = fopen(image_src, "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        fread(jpg_data, jpg_size, 1, file);
        fclose(file);
        
        scui_tjpgd_ptr_ofs = 0;
        scui_tjpgd_dat_out = data;
        scui_tjpgd_dat_in  = jpg_data;
        scui_tjpgd_dat_ins = jpg_size;
        scui_tjpgd_image   = image;
        
        static volatile JDEC jdec = {0};
        void *work = SCUI_MEM_ALLOC(scui_mem_type_graph, JD_SZBUF * 4);
        JRESULT res = jd_prepare(&jdec, scui_tjpgd_in_func_cb, work, JD_SZBUF * 4, NULL);
        if (res != JDR_OK) {
            SCUI_LOG_ERROR("ret:%d", res);
            SCUI_ASSERT(false);
        }
        
        res = jd_decomp(&jdec, scui_tjpgd_out_func_cb, 0);
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
        void  *png_data = lodepng_malloc(png_size);
        
        FILE *file = fopen(image_src, "rb+");
        fseek(file, image->pixel.data_bin, SEEK_SET);
        fread(png_data, png_size, 1, file);
        fclose(file);
        
        /* 这个库只能解出ARGB8888格式 */
        uint8_t *png_data_dec = NULL;
        unsigned png_w_dec = 0;
        unsigned png_h_dec = 0;
        unsigned lib_error = lodepng_decode32(&png_data_dec, &png_w_dec, &png_h_dec, png_data, png_size);
        
        if (lib_error != 0) {
            SCUI_LOG_ERROR("%s", lodepng_error_text(lib_error));
            SCUI_ASSERT(false);
        }
        SCUI_ASSERT(png_w_dec == image->pixel.width);
        SCUI_ASSERT(png_h_dec == image->pixel.height);
        SCUI_ASSERT(png_data_dec != NULL);
        
        /* 这个库只能解出ARGB8888格式,所以还需要流处理 */
        scui_image_t image_t = *image;
        image_t.format = scui_pixel_cf_bmp8888;
        image_t.pixel.data_bin = png_data_dec;
        scui_image_cf_cvt(&image_t, true);
        
        SCUI_ASSERT(image_t.format == image->format);
        scui_draw_byte_copy(true, data, png_data_dec, scui_image_size(image));
        lodepng_free(png_data_dec);
        lodepng_free(png_data);
        
        if (scui_lodepng_size_rcd < scui_lodepng_size_top) {
            scui_lodepng_size_rcd = scui_lodepng_size_top;
            SCUI_LOG_WARN("png occupy top:%d", scui_lodepng_size_top);
        }
        
        /* 检查空间是否释放完毕 */
        SCUI_ASSERT(scui_lodepng_size_cur == 0);
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
        
        FILE *file = fopen(image_src, "rb+");
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
        
        /* 检查空间是否释放完毕 */
        SCUI_ASSERT(scui_LZ4F_size_cur == 0);
        return;
    }
    
    SCUI_ASSERT(false);
}

/*brief 图片文件资源读取
 *param name   包名
 *param offset 偏移量
 *param size   数据长度
 *param data   数据流
 */
void scui_image_bin_read(const char *name, uintptr_t offset, uintptr_t size, uint8_t *data)
{
    size_t retval = -1;
    FILE *file = fopen(name, "rb+");
    fseek(file, offset, SEEK_SET);
    retval = fread(data, size, 1, file);
    fclose(file);
}

/*@brief 生成图片
 *@param image 图片资源信息
 *@param data  url字符串
 *@param size  url字符串长度
 */
void scui_image_qrcode(scui_image_t *image, uint8_t *data, uint32_t size)
{
    /* 不需要渐变灰度,只需要0-1灰度 */
    SCUI_ASSERT(image->format == scui_pixel_cf_alpha1);
    scui_draw_byte_new(true, image->pixel.data_bin, 0x00, image->pixel.size_bin);
    
    /* 画布映射 */
    scui_surface_t image_surface = {
        .pixel   = (void *)image->pixel.data_bin,
        .format  = image->format,
        .hor_res = image->pixel.width,
        .ver_res = image->pixel.height,
    };
    scui_surface_config(&image_surface);
    scui_surface_t *dst_surface = &image_surface;
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    
    #if 1
    SCUI_ASSERT(size < qrcodegen_BUFFER_LEN_MAX);
    scui_multi_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, size);
    scui_multi_t qr_size    = qrcodegen_version2size(qr_version);
    SCUI_ASSERT(qr_version > 0 || qr_size > 0);
    
    scui_multi_t scale  = dst_clip.w / qr_size;
    scui_multi_t remain = dst_clip.w % qr_size;
    scui_multi_t extend = remain / (scale << 2);
    if (extend != 0 && qr_version < qrcodegen_VERSION_MAX)
        qr_version = scui_min(qr_version + extend, qrcodegen_VERSION_MAX);
    
    scui_multi_t qr_version_len = qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version);
    uint8_t *qrcode = SCUI_MEM_ALLOC(scui_mem_type_graph, qr_version_len);
    uint8_t *data_t = SCUI_MEM_ALLOC(scui_mem_type_graph, qr_version_len);
    memcpy(data_t, data, size);
    
    enum qrcodegen_Ecc  ecc  = qrcodegen_Ecc_MEDIUM;
    enum qrcodegen_Mask mask = qrcodegen_Mask_AUTO;
    if (!qrcodegen_encodeBinary(data_t, size, qrcode, ecc, qr_version, qr_version, mask, true)) {
         SCUI_LOG_ERROR("error");
         SCUI_ASSERT(false);
         return;
    }
    
    qr_size = qrcodegen_getSize(qrcode);
    scale   = dst_clip.w / qr_size;
    scui_multi_t scaled = (qr_size * scale);
    scui_multi_t margin = (dst_clip.w - scaled) / 2;
    scui_area_t draw_area = {
        .x = margin, .w = scaled,
        .y = margin, .h = scaled,
    };
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    SCUI_ASSERT(scui_pixel_bits(dst_surface->format) == 1); /* 下述逻辑只支持bpp_1 */
    scui_multi_t dst_ofs_p = scui_surface_point_ofs(dst_surface, draw_area.y, draw_area.x);
    uint8_t *dst_addr = dst_surface->pixel;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint32_t idx_ofs = dst_ofs_p + scui_surface_point_ofs(dst_surface, idx_line, idx_item);
        uint8_t *dst_ofs = dst_addr  + idx_ofs / 8;
        uint32_t pos_ofs = 7 - idx_ofs % 8;
        
        bool bit = qrcodegen_getModule(qrcode, idx_item / scale, idx_line / scale);
        if (bit) scui_bit_ext_set(dst_ofs, pos_ofs, 8);
        else scui_bit_ext_rst(dst_ofs, pos_ofs, 8);
    }
    
    SCUI_MEM_FREE(data_t);
    SCUI_MEM_FREE(qrcode);
    #endif
}

/*@brief 生成图片
 *@param image 图片资源信息
 *@param data  url字符串
 *@param size  url字符串长度
 */
void scui_image_barcode(scui_image_t *image, uint8_t *data, uint32_t size)
{
    /* 不需要渐变灰度,只需要0-1灰度 */
    SCUI_ASSERT(image->format == scui_pixel_cf_alpha1);
    scui_draw_byte_new(true, image->pixel.data_bin, 0x00, image->pixel.size_bin);
    
    /* 画布映射 */
    scui_surface_t image_surface = {
        .pixel   = (void *)image->pixel.data_bin,
        .format  = image->format,
        .hor_res = image->pixel.width,
        .ver_res = image->pixel.height,
    };
    scui_surface_config(&image_surface);
    scui_surface_t *dst_surface = &image_surface;
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    
    #if 1
    size_t len = code128_estimate_len(data);
    uint8_t *out_buf = SCUI_MEM_ALLOC(scui_mem_type_mix, len);
    size_t barcode_w = code128_encode_gs1(data, out_buf, len);
    SCUI_ASSERT(barcode_w <= dst_clip.w);
    
    scui_multi_t scale  = (dst_clip.w / barcode_w);
    scui_multi_t scaled = (barcode_w * scale);
    scui_multi_t margin = (dst_clip.w - scaled) / 2;
    scui_area_t draw_area = {
        .x = margin, .y = 0,
        .w = scaled, .h = dst_clip.h,
    };
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    SCUI_ASSERT(scui_pixel_bits(dst_surface->format) == 1); /* 下述逻辑只支持bpp_1 */
    scui_multi_t dst_ofs_p = scui_surface_point_ofs(dst_surface, draw_area.y, draw_area.x);
    uint8_t *dst_addr = dst_surface->pixel;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint32_t idx_ofs = dst_ofs_p + scui_surface_point_ofs(dst_surface, idx_line, idx_item);
        uint8_t *dst_ofs = dst_addr  + idx_ofs / 8;
        uint32_t pos_ofs = 7 - idx_ofs % 8;
        
        bool bit = out_buf[scui_map(idx_item, 0, draw_area.w, 0, barcode_w)];
        if (bit) scui_bit_ext_set(dst_ofs, pos_ofs, 8);
        else scui_bit_ext_rst(dst_ofs, pos_ofs, 8);
    }
    
    SCUI_MEM_FREE(out_buf);
    #endif
}
