/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "barcodegen_lvgl.h"

/*@brief 自定义控件:插件:条形码生成器
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_lighten,.color_darken,)
 *@param data  url网址链接字符串
 *@param size  字符串长度
 *@param cover 背景覆盖
 */
void scui_custom_draw_barcode(scui_event_t *event, scui_area_t *clip,
                              scui_color_t  color, bool cover,
                              uint8_t *data, uint32_t size)
{
    SCUI_LOG_DEBUG("");
    
    if (cover) {
        scui_color_t bg_color = {.color = color.color_d};
        scui_widget_surface_draw_color(event->object, clip, bg_color);
    }
    
    if (size >= barcodegen_lvgl_BUFFER_LEN_MAX) {
        SCUI_LOG_ERROR("error");
        return;
    }
    
    uint8_t *barcode = SCUI_MEM_ALLOC(scui_mem_type_mix, barcodegen_lvgl_BUFFER_LEN_MAX);
    uint8_t *data_t  = SCUI_MEM_ALLOC(scui_mem_type_mix, barcodegen_lvgl_DATA_LEN_MAX);
    memcpy(data_t, data, size);
    
    size_t bitsLen = 0;
    enum barcodegen_lvgl_Type type = barcodegen_lvgl_128C;
    if (!barcodegen_lvgl_encodeBinary(data_t, size, (uint8_t*)barcode, type, &bitsLen)) {
         SCUI_LOG_ERROR("error");
         SCUI_MEM_FREE(data_t);
         SCUI_MEM_FREE(barcode);
         return;
    }
    
    int scale  = clip->w / bitsLen;
    int remain = clip->w % bitsLen;
    int scaled = (bitsLen * scale);
    int margin = (clip->w - scaled) / 2;
    
    scui_widget_t  *widget  = scui_handle_get(event->object);
    scui_surface_t *surface = widget->surface;
    uint32_t  pixel_byte = scui_pixel_bits(surface->format) / 8;
    uintptr_t pixel_size = pixel_byte * scaled * scaled;
    /* 为了加快绘制速度,这里使用SCUI_PIXEL_TYPE格式快速上色 */
    uint8_t *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
    uint32_t pixel_l = 0;
    uint32_t pixel_d = 0;
    scui_pixel_by_color(surface->format, &pixel_l, color.color_l);
    scui_pixel_by_color(surface->format, &pixel_d, color.color_d);
    
    if (pixel == NULL) {
        SCUI_LOG_WARN("memory deficit was caught");
        scui_image_cache_clear();
        pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
    if (pixel == NULL) {
        scui_image_cache_visit();
        SCUI_ASSERT(false);
    }
    }
    
    for (int i = 0; i < bitsLen; i++) {
         bool bit = barcodegen_lvgl_getModule(barcode, i);
        for (int x = 0; x < scale; x++)
            scui_pixel_by_cf(surface->format, &pixel[(0 * scaled + x) * pixel_byte], bit ? &pixel_l : &pixel_d);
    }
    
    for (int y = 1; y < scaled; y++)
        scui_draw_line_copy(&pixel[(y * scaled + 0) * pixel_byte],
                            &pixel[(0 * scaled + 0) * pixel_byte], pixel_byte * scaled);
    
    scui_surface_t pattern = {
        .pixel   = pixel,
        .format  = surface->format,
        .hor_res = scaled,
        .ver_res = scaled,
        .alpha   = scui_widget_alpha_get(event->object),
    };
    
    scui_point_t offset = {.x = margin, .y =  margin,};
    scui_area_t dst_clip = {0};
    scui_area_t dst_area = scui_widget_surface_clip(event->object);
    if (scui_area_inter(&dst_clip, &dst_area, clip))
    if (scui_area_limit_offset(&dst_clip, &offset))
        scui_widget_surface_draw_pattern(event->object, &dst_clip, &pattern, NULL, (scui_color_t){0});
    
    SCUI_MEM_FREE(pixel);
    SCUI_MEM_FREE(data_t);
    SCUI_MEM_FREE(barcode);
}
