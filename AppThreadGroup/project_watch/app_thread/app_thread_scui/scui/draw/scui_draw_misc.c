/*实现目标:
 *    杂项绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 二维码生成器库(qrcode): */
#include "qrcodegen.h"

/* 条形码生成器库(barcode): */
#include "code128.h"

/*@brief 区域二维码绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_qrcode(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(draw_dsc->dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, &draw_dsc->dst_clip))
         return;
    
    if (draw_dsc->src_size >= qrcodegen_BUFFER_LEN_MAX) {
        SCUI_LOG_ERROR("error");
        return;
    }
    scui_multi_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, draw_dsc->src_size);
    scui_multi_t qr_size    = qrcodegen_version2size(qr_version);
    if (qr_version <= 0 || qr_size <= 0) {
        SCUI_LOG_ERROR("error: <%d, %d>", qr_version, qr_size);
        return;
    }
    
    scui_multi_t scale  = draw_dsc->dst_clip.w / qr_size;
    scui_multi_t remain = draw_dsc->dst_clip.w % qr_size;
    scui_multi_t extend = remain / (scale << 2);
    if (extend != 0 && qr_version < qrcodegen_VERSION_MAX)
        qr_version = scui_min(qr_version + extend, qrcodegen_VERSION_MAX);
    
    scui_multi_t qr_version_len = qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version);
    scui_mem_type_t mem_type = qr_version_len > SCUI_MEM_MIX_FRAG_SIZE_LIMIT ? scui_mem_type_graph : scui_mem_type_mix;
    uint8_t *qrcode = SCUI_MEM_ALLOC(mem_type, qr_version_len);
    uint8_t *data_t = SCUI_MEM_ALLOC(mem_type, qr_version_len);
    memcpy(data_t, draw_dsc->src_data, draw_dsc->src_size);
    
    enum qrcodegen_Ecc  ecc  = qrcodegen_Ecc_MEDIUM;
    enum qrcodegen_Mask mask = qrcodegen_Mask_AUTO;
    if (!qrcodegen_encodeBinary(data_t, draw_dsc->src_size, qrcode, ecc, qr_version, qr_version, mask, true)) {
         SCUI_LOG_ERROR("error");
         goto over;
    }
    
    qr_size = qrcodegen_getSize(qrcode);
    scale   = draw_dsc->dst_clip.w / qr_size;
    scui_multi_t scaled = (qr_size * scale);
    scui_multi_t margin = (draw_dsc->dst_clip.w - scaled) / 2;
    
    draw_area.x = scui_max(margin - draw_dsc->src_clip.x, 0);
    draw_area.y = scui_max(margin - draw_dsc->src_clip.y, 0);
    draw_area.w = scui_min(scui_min(draw_area.w, draw_dsc->src_clip.w), scaled);
    draw_area.h = scui_min(scui_min(draw_area.h, draw_dsc->src_clip.h), scaled);
    SCUI_ASSERT(draw_dsc->dst_clip.x + draw_area.w <= draw_dsc->dst_surface->hor_res);
    SCUI_ASSERT(draw_dsc->dst_clip.y + draw_area.h <= draw_dsc->dst_surface->ver_res);
    if (scui_area_empty(&draw_area))
        goto over;
    
    scui_color_wt_t pixel_l = 0, pixel_d = 0;
    scui_pixel_by_color(draw_dsc->dst_surface->format, &pixel_l, draw_dsc->src_color.color_l);
    scui_pixel_by_color(draw_dsc->dst_surface->format, &pixel_d, draw_dsc->src_color.color_d);
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(draw_dsc->dst_surface->format) / 8;
    scui_multi_t dst_line = draw_dsc->dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = draw_dsc->dst_surface->pixel + draw_dsc->dst_clip.y * dst_line + draw_dsc->dst_clip.x * dst_byte;
    dst_addr += draw_area.y * dst_line + draw_area.x * dst_byte;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        scui_coord_t src_ofs_iy = scui_max(draw_dsc->src_clip.y - margin, 0) + idx_line;
        scui_coord_t src_ofs_ix = scui_max(draw_dsc->src_clip.x - margin, 0) + idx_item;
        bool bit = qrcodegen_getModule(qrcode, src_ofs_ix / scale, src_ofs_iy / scale);
        scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - draw_dsc->src_alpha,
                            draw_dsc->dst_surface->format, bit ? &pixel_l : &pixel_d, draw_dsc->src_alpha);
        
        SCUI_ASSERT(dst_ofs < draw_dsc->dst_surface->pixel + dst_line * draw_dsc->dst_surface->ver_res);
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
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(draw_dsc->dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, &draw_dsc->dst_clip))
         return;
    
    draw_area.w = scui_min(draw_area.w, draw_dsc->src_clip.w);
    draw_area.h = scui_min(draw_area.h, draw_dsc->src_clip.h);
    SCUI_ASSERT(draw_dsc->dst_clip.x + draw_area.w <= draw_dsc->dst_surface->hor_res);
    SCUI_ASSERT(draw_dsc->dst_clip.y + draw_area.h <= draw_dsc->dst_surface->ver_res);
    
    if (draw_dsc->src_data == NULL || draw_dsc->src_size == 0) {
        SCUI_LOG_ERROR("args invalid");
        return;
    }
    
    size_t len = code128_estimate_len(draw_dsc->src_data);
    uint8_t *out_buf = SCUI_MEM_ALLOC(scui_mem_type_mix, len);
    size_t barcode_w = code128_encode_gs1(draw_dsc->src_data, out_buf, len);
    
    if (barcode_w > draw_dsc->dst_clip.w) {
        SCUI_LOG_ERROR("clip too small:%d - %d", barcode_w , draw_dsc->dst_clip.w);
        goto over;
    }
    
    scui_multi_t scale  = draw_dsc->dst_clip.w / barcode_w;
    scui_multi_t scaled = (barcode_w * scale);
    scui_multi_t margin = (draw_dsc->dst_clip.w - scaled) / 2;
    
    draw_area.x = margin;
    draw_area.y = 0;
    draw_area.w = scui_min(draw_area.w, scaled);
    draw_area.h = draw_area.h;
    if (scui_area_empty(&draw_area))
        goto over;
    
    scui_color_wt_t pixel_l = 0, pixel_d = 0;
    scui_pixel_by_color(draw_dsc->dst_surface->format, &pixel_l, draw_dsc->src_color.color_l);
    scui_pixel_by_color(draw_dsc->dst_surface->format, &pixel_d, draw_dsc->src_color.color_d);
    
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(draw_dsc->dst_surface->format) / 8;
    scui_multi_t dst_line = draw_dsc->dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = draw_dsc->dst_surface->pixel + draw_dsc->dst_clip.y * dst_line + draw_dsc->dst_clip.x * dst_byte;
    dst_addr += draw_area.y * dst_line + draw_area.x * dst_byte;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        bool bit = out_buf[scui_map(idx_item, 0, draw_area.w, 0, barcode_w)];
        scui_pixel_mix_with(draw_dsc->dst_surface->format, dst_ofs, scui_alpha_cover - draw_dsc->src_alpha,
                            draw_dsc->dst_surface->format, bit ? &pixel_l : &pixel_d, draw_dsc->src_alpha);
    }
    
over:
    SCUI_MEM_FREE(out_buf);
}
