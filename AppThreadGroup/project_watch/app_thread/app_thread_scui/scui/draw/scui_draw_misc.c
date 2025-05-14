/*实现目标:
 *    杂项绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// 二维码生成器库(qrcode):
#include "qrcodegen.h"

// 条形码生成器库(barcode):
#include "code128.h"

/*@brief 区域二维码绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_qrcode(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->qrcode.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->qrcode.dst_clip;
    scui_alpha_t    src_alpha   = draw_dsc->qrcode.src_alpha;
    scui_color_t    src_color   = draw_dsc->qrcode.src_color;
    scui_multi_t    src_size    = draw_dsc->qrcode.src_size;
    uint8_t        *src_data    = draw_dsc->qrcode.src_data;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (src_size >= qrcodegen_BUFFER_LEN_MAX) {
        SCUI_LOG_ERROR("error");
        return;
    }
    scui_multi_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, src_size);
    scui_multi_t qr_size    = qrcodegen_version2size(qr_version);
    if (qr_version <= 0 || qr_size <= 0) {
        SCUI_LOG_ERROR("error: <%d, %d>", qr_version, qr_size);
        return;
    }
    
    scui_multi_t scale  = dst_clip->w / qr_size;
    scui_multi_t remain = dst_clip->w % qr_size;
    scui_multi_t extend = remain / (scale << 2);
    if (extend != 0 && qr_version < qrcodegen_VERSION_MAX)
        qr_version = scui_min(qr_version + extend, qrcodegen_VERSION_MAX);
    
    scui_multi_t qr_version_len = qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version);
    uint8_t *qrcode = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    uint8_t *data_t = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    memcpy(data_t, src_data, src_size);
    
    enum qrcodegen_Ecc  ecc  = qrcodegen_Ecc_MEDIUM;
    enum qrcodegen_Mask mask = qrcodegen_Mask_AUTO;
    if (!qrcodegen_encodeBinary(data_t, src_size, qrcode, ecc, qr_version, qr_version, mask, true)) {
         SCUI_LOG_ERROR("error");
         goto over;
    }
    
    qr_size = qrcodegen_getSize(qrcode);
    scale   = dst_clip->w / qr_size;
    scui_multi_t scaled = (qr_size * scale);
    scui_multi_t margin = (dst_clip->w - scaled) / 2;
    
    draw_area.x = margin;
    draw_area.y = margin;
    draw_area.w = scui_min(draw_area.w, scaled);
    draw_area.h = scui_min(draw_area.h, scaled);
    if (scui_area_empty(&draw_area))
        goto over;
    
    scui_color_wt_t pixel_l = 0, pixel_d = 0;
    scui_pixel_by_color(dst_surface->format, &pixel_l, src_color.color_l);
    scui_pixel_by_color(dst_surface->format, &pixel_d, src_color.color_d);
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    dst_addr += draw_area.y * dst_line + draw_area.x * dst_byte;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        bool bit = qrcodegen_getModule(qrcode, idx_item / scale, idx_line / scale);
        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                            dst_surface->format, bit ? &pixel_l : &pixel_d, src_alpha);
    }
    
over:
    SCUI_MEM_FREE(data_t);
    SCUI_MEM_FREE(qrcode);
}

/*@brief 区域条形码绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_barcode(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->barcode.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->barcode.dst_clip;
    scui_alpha_t    src_alpha   = draw_dsc->barcode.src_alpha;
    scui_color_t    src_color   = draw_dsc->barcode.src_color;
    scui_multi_t    src_size    = draw_dsc->barcode.src_size;
    uint8_t        *src_data    = draw_dsc->barcode.src_data;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (src_data == NULL || src_size == 0) {
        SCUI_LOG_ERROR("args invalid");
        return;
    }
    
    size_t len = code128_estimate_len(src_data);
    uint8_t *out_buf = SCUI_MEM_ALLOC(scui_mem_type_mix, len);
    size_t barcode_w = code128_encode_gs1(src_data, out_buf, len);
    
    if (barcode_w > dst_clip->w) {
        SCUI_LOG_ERROR("clip too small:%d - %d", barcode_w , dst_clip->w);
        goto over;
    }
    
    scui_multi_t scale  = dst_clip->w / barcode_w;
    scui_multi_t scaled = (barcode_w * scale);
    scui_multi_t margin = (dst_clip->w - scaled) / 2;
    
    draw_area.x = margin;
    draw_area.y = 0;
    draw_area.w = scui_min(draw_area.w, scaled);
    draw_area.h = draw_area.h;
    if (scui_area_empty(&draw_area))
        goto over;
    
    scui_color_wt_t pixel_l = 0, pixel_d = 0;
    scui_pixel_by_color(dst_surface->format, &pixel_l, src_color.color_l);
    scui_pixel_by_color(dst_surface->format, &pixel_d, src_color.color_d);
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    dst_addr += draw_area.y * dst_line + draw_area.x * dst_byte;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        bool bit = out_buf[scui_map(idx_item, 0, draw_area.w, 0, barcode_w)];
        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                            dst_surface->format, bit ? &pixel_l : &pixel_d, src_alpha);
    }
    
over:
    SCUI_MEM_FREE(out_buf);
}
