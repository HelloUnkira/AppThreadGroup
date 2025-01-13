/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
// 二维码生成器库(qrcode):
#include "qrcodegen.h"

/*@brief 自定义控件:插件:二维码生成器
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_qrcode(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event = draw_dsc->event;
    scui_area_t  *clip  = draw_dsc->clip;
    scui_color_t  color = draw_dsc->qrcode.color;
    uint8_t      *data  = draw_dsc->qrcode.data;
    uint32_t      size  = draw_dsc->qrcode.size;
    bool          cover = draw_dsc->qrcode.cover;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    if (cover) {
        scui_color_t bg_color = {.color = color.color_d};
        scui_widget_draw_color(event->object, clip, bg_color);
    }
    
    if (size >= qrcodegen_BUFFER_LEN_MAX) {
        SCUI_LOG_ERROR("error");
        return;
    }
    int qr_version  = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, size);
    int qr_size     = qrcodegen_version2size(qr_version);
    if (qr_version <= 0) {
        SCUI_LOG_ERROR("error");
        return;
    }
    if (qr_size <= 0) {
        SCUI_LOG_ERROR("error");
        return;
    }
    
    int scale  = clip->w / qr_size;
    int remain = clip->w % qr_size;
    
    uint32_t version_extend = remain / (scale << 2);
    if (version_extend != 0 && qr_version < qrcodegen_VERSION_MAX)
        qr_version = scui_min(qr_version + version_extend, qrcodegen_VERSION_MAX);
    
    uint32_t qr_version_len = qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version);
    uint8_t *qrcode = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    uint8_t *data_t = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    memcpy(data_t, data, size);
    
    enum qrcodegen_Ecc  ecc  = qrcodegen_Ecc_MEDIUM;
    enum qrcodegen_Mask mask = qrcodegen_Mask_AUTO;
    if (!qrcodegen_encodeBinary(data_t, size, qrcode, ecc, qr_version, qr_version, mask, true)) {
         SCUI_LOG_ERROR("error");
         SCUI_MEM_FREE(data_t);
         SCUI_MEM_FREE(qrcode);
         return;
    }
    
    qr_size = qrcodegen_getSize(qrcode);
    scale   = clip->w / qr_size;
    int scaled = (qr_size * scale);
    int margin = (clip->w - scaled) / 2;
    
    scui_surface_t *surface   = scui_widget_surface(event->object);
    scui_coord_t pixel_byte   = scui_pixel_bits(surface->format) / 8;
    scui_coord_t pixel_remain = sizeof(scui_color_wt_t) - pixel_byte;
    scui_multi_t pixel_size   = pixel_byte * scaled * scaled + pixel_remain;
    /* 为了加快绘制速度,这里使用SCUI_PIXEL_TYPE格式快速上色 */
    uint8_t *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
    scui_color_wt_t pixel_l = 0;
    scui_color_wt_t pixel_d = 0;
    scui_pixel_by_color(surface->format, &pixel_l, color.color_l);
    scui_pixel_by_color(surface->format, &pixel_d, color.color_d);
    
    for (int y = 0; y < scaled; y++)
    for (int x = 0; x < scaled; x++) {
         bool bit = qrcodegen_getModule(qrcode, x / scale, y / scale);
         scui_pixel_by_cf(surface->format, &pixel[(y * scaled + x) * pixel_byte], bit ? &pixel_l : &pixel_d);
    }
    
    scui_point_t offset = {.x = margin, .y =  margin,};
    scui_area_t dst_clip = {0};
    scui_area_t dst_area = scui_widget_clip(event->object);
    if (scui_area_inter(&dst_clip, &dst_area, clip))
    if (scui_area_limit_offset(&dst_clip, &offset)) {
        
        scui_image_t image_inst = {
            .type           = scui_image_type_mem,
            .format         = surface->format,
            .pixel.width    = scaled,
            .pixel.height   = scaled,
            .pixel.data_bin = pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &image_inst);
        scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
        scui_handle_clear(image);
    }
    
    SCUI_MEM_FREE(pixel);
    SCUI_MEM_FREE(data_t);
    SCUI_MEM_FREE(qrcode);
}
