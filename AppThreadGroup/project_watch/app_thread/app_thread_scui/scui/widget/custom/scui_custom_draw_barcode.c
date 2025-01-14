/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
// 条形码生成器库(barcode):
#include "code128.h"

/*@brief 自定义控件:插件:条形码生成器
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_ctx_barcode(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event = draw_dsc->event;
    scui_area_t  *clip  = draw_dsc->clip;
    scui_color_t  color = draw_dsc->barcode.color;
    uint8_t      *data  = draw_dsc->barcode.data;
    uint32_t      size  = draw_dsc->barcode.size;
    bool          cover = draw_dsc->barcode.cover;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    if (cover) {
        scui_color_t bg_color = {.color = color.color_d};
        scui_widget_draw_color(event->object, clip, bg_color);
    }
    
    if (data == NULL || size == 0) {
        SCUI_LOG_ERROR("args invalid");
        return;
    }
    
    size_t len = code128_estimate_len(data);
    uint8_t *out_buf = SCUI_MEM_ALLOC(scui_mem_type_mix, len);
    size_t barcode_w = code128_encode_gs1(data, out_buf, len);
    
    if (barcode_w > clip->w) {
        SCUI_LOG_ERROR("clip too small:%d - %d", barcode_w , clip->w);
        return;
    }
    
    int scale  = clip->w / barcode_w;
    int scaled = (barcode_w * scale);
    int margin = (clip->w - scaled) / 2;
    
    scui_surface_t *surface   = scui_widget_surface(event->object);
    scui_coord_t pixel_byte   = scui_pixel_bits(surface->format) / 8;
    scui_coord_t pixel_remain = sizeof(scui_color_wt_t) - pixel_byte;
    scui_multi_t pixel_size   = pixel_byte * scaled * clip->h + pixel_remain;
    /* 为了加快绘制速度,这里使用SCUI_PIXEL_TYPE格式快速上色 */
    uint8_t *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
    scui_color_wt_t pixel_l = 0;
    scui_color_wt_t pixel_d = 0;
    scui_pixel_by_color(surface->format, &pixel_l, color.color_l);
    scui_pixel_by_color(surface->format, &pixel_d, color.color_d);
    
    for (int i = 0; i < barcode_w; i++) {
         bool bit = out_buf[i];
         SCUI_LOG_DEBUG_RAW("%1d", bit);
         for (int x = 0; x < scale; x++)
              scui_pixel_by_cf(surface->format, &pixel[(i * scale + x) * pixel_byte],
                               bit ? &pixel_l : &pixel_d);
    }
    
    for (int y = 1; y < clip->h; y++) {
        void *dst_addr = &pixel[(y * scaled + 0) * pixel_byte];
        void *src_addr = &pixel[(0 * scaled + 0) * pixel_byte];
        scui_draw_byte_copy(dst_addr, src_addr, pixel_byte * scaled);
    }
    
    scui_point_t offset = {.x = margin,};
    scui_area_t dst_clip = {0};
    scui_area_t dst_area = scui_widget_clip(event->object);
    if (scui_area_inter(&dst_clip, &dst_area, clip))
    if (scui_area_limit_offset(&dst_clip, &offset)) {
        
        scui_image_t image_inst = {
            .type           = scui_image_type_mem,
            .format         = surface->format,
            .pixel.width    = scaled,
            .pixel.height   = clip->h,
            .pixel.data_bin = pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &image_inst);
        scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
        scui_handle_clear(image);
    }
    
    SCUI_MEM_FREE(pixel);
    SCUI_MEM_FREE(out_buf);
}
