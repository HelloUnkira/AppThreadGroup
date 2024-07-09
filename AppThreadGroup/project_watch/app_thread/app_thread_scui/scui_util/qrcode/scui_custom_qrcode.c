/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "qrcodegen_lvgl.h"

/*@brief 自定义控件:插件:二维码生成器
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_lighten,.color_darken,)
 *@param data  url网址链接字符串
 *@param size  字符串长度
 *@param cover 背景覆盖
 */
void scui_custom_draw_qrcode(scui_event_t *event, scui_area_t *clip,
                             scui_color_t  color, bool cover,
                             uint8_t *data, uint32_t size)
{
    SCUI_LOG_DEBUG("");
    
    if (cover) {
        scui_color_t bg_color = {.color = color.color_d};
        scui_widget_draw_color(event->object, clip, bg_color);
    }
    
    if (size >= qrcodegen_lvgl_BUFFER_LEN_MAX) {
        SCUI_LOG_ERROR("error");
        return;
    }
    int qr_version  = qrcodegen_lvgl_getMinFitVersion(qrcodegen_lvgl_Ecc_MEDIUM, size);
    int qr_size     = qrcodegen_lvgl_version2size(qr_version);
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
    if (version_extend != 0 && qr_version < qrcodegen_lvgl_VERSION_MAX)
        qr_version = scui_min(qr_version + version_extend, qrcodegen_lvgl_VERSION_MAX);
    
    uint32_t qr_version_len = qrcodegen_lvgl_BUFFER_LEN_FOR_VERSION(qr_version);
    uint8_t *qrcode = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    uint8_t *data_t = SCUI_MEM_ALLOC(scui_mem_type_mix, qr_version_len);
    memcpy(data_t, data, size);
    
    enum qrcodegen_lvgl_Ecc  ecc  = qrcodegen_lvgl_Ecc_MEDIUM;
    enum qrcodegen_lvgl_Mask mask = qrcodegen_lvgl_Mask_AUTO;
    if (!qrcodegen_lvgl_encodeBinary(data_t, size, qrcode, ecc, qr_version, qr_version, mask, true)) {
         SCUI_LOG_ERROR("error");
         SCUI_MEM_FREE(data_t);
         SCUI_MEM_FREE(qrcode);
         return;
    }
    
    qr_size = qrcodegen_lvgl_getSize(qrcode);
    scale   = clip->w / qr_size;
    int scaled = (qr_size * scale);
    int margin = (clip->w - scaled) / 2;
    
    scui_widget_t  *widget  = scui_handle_get(event->object);
    scui_surface_t *surface = widget->surface;
    uint32_t  pixel_byte = scui_pixel_bits(surface->format) / 8;
    uintptr_t pixel_size = pixel_byte * scaled * scaled;
    /* 为了加快绘制速度,这里使用SCUI_PIXEL_TYPE格式快速上色 */
    uint8_t *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
    scui_color_limit_t pixel_l = 0;
    scui_color_limit_t pixel_d = 0;
    scui_pixel_by_color(surface->format, &pixel_l, color.color_l);
    scui_pixel_by_color(surface->format, &pixel_d, color.color_d);
    
    for (int y = 0; y < scaled; y++)
    for (int x = 0; x < scaled; x++) {
         bool bit = qrcodegen_lvgl_getModule(qrcode, x / scale, y / scale);
         scui_pixel_by_cf(surface->format, &pixel[(y * scaled + x) * pixel_byte], bit ? &pixel_l : &pixel_d);
    }
    
    scui_point_t offset = {.x = margin, .y =  margin,};
    scui_area_t dst_clip = {0};
    scui_area_t dst_area = scui_widget_clip(event->object);
    if (scui_area_inter(&dst_clip, &dst_area, clip))
    if (scui_area_limit_offset(&dst_clip, &offset)) {
        
        scui_image_t image_inst = {
            .status         = scui_image_status_mem,
            .pixel.width    = scaled,
            .pixel.height   = scaled,
            .pixel.data_mem = pixel,
        };
        scui_image_cf_by_pixel_cf(&image_inst.format, &surface->format);
        scui_handle_t image = scui_handle_find();
        scui_handle_set(image, &image_inst);
        scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
        scui_handle_set(image, NULL);
    }
    
    SCUI_MEM_FREE(pixel);
    SCUI_MEM_FREE(data_t);
    SCUI_MEM_FREE(qrcode);
}
