/*实现目标:
 *    图像绘制
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 区域图像绘制(可以使用DMA2D-blend加速优化)
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   数据源
 *@param src_clip     数据源绘制区域
 */
void scui_draw_image(scui_surface_t *dst_surface, scui_area_t *dst_clip, scui_image_unit_t *image_unit, scui_area_t *src_clip)
{
    APP_SYS_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    APP_SYS_ASSERT(image_unit != NULL && image_unit->image != NULL && image_unit->data != NULL);
    
    scui_area_t image_clip = {
        .x = 0, .w = image_unit->image->pixel.width,
        .y = 0, .h = image_unit->image->pixel.height,
    };
    
    scui_surface_t image_surface = {
        .pixel = image_unit->data,
        .clip  = image_clip,
        .alpha = 0xFF,
    };
    
    /* surface copy: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_rgb565) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_rgb888)) {
        scui_draw_area_copy(dst_surface, dst_clip, &image_surface, src_clip);
        APP_SYS_LOG_INFO("image pixel copy");
        return;
    }
    
    scui_area_t *dst_area = &dst_surface->clip;
    scui_area_t *src_area = &image_clip;
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_inter(&dst_clip_v, dst_area, dst_clip);
    scui_area_inter(&src_clip_v, src_area, src_clip);
    
    scui_area_t draw_area = {0};
    draw_area.w = app_sys_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = app_sys_min(dst_clip_v.h, src_clip_v.h);
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_surface_t *src_surface = &image_surface;
    uint8_t *dst_addr = dst_surface->pixel + (dst_clip_v.x) * SCUI_PIXEL_SIZE;
    uint8_t *src_addr = src_surface->pixel + (src_clip_v.x) * SCUI_PIXEL_SIZE;
    scui_coord_t dis_line = draw_area.w * SCUI_PIXEL_SIZE;
    SCUI_PIXEL_TYPE pixel = {0};
    scui_alpha_t alpha = 0;
    
    /* image copy: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_rgb888) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_rgb565)) {
        /* 这个很慢很慢, 除非硬件支持, 建议对齐平台支持 */
        APP_SYS_LOG_WARN("image pixel copy too slow");
        #if 0 /* 不应该支持这种条件,有损性能 */
        if (image_unit->image->format == scui_image_format_rgb888) {
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_addr_ofs = dst_addr + (idx_line + dst_clip_v.y) * dis_line + idx_item;
                uint8_t *src_addr_ofs = src_addr + (idx_line + src_clip_v.y) * dis_line + idx_item;
                app_sys_mem_w(dst_addr_ofs, scui_color_rgb888_to_rgb565(*(scui_color888_t *)src_addr_ofs), SCUI_PIXEL_TYPE);
            }
            return;
        }
        if (image_unit->image->format == scui_image_format_rgb565) {
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_addr_ofs = dst_addr + (idx_line + dst_clip_v.y) * dis_line + idx_item;
                uint8_t *src_addr_ofs = src_addr + (idx_line + src_clip_v.y) * dis_line + idx_item;
                app_sys_mem_w(dst_addr_ofs, scui_color_rgb565_to_rgb888(*(scui_color565_t *)src_addr_ofs), SCUI_PIXEL_TYPE);
            }
            return;
        }
        #endif
    }
    
    /* image blend: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_argb8565) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_argb8888)) {
        if (image_unit->image->format == scui_image_format_argb8565) {
            scui_coord_t src_line = draw_area.w * sizeof(scui_color8565_t);
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_addr_ofs = dst_addr + (idx_line + dst_clip_v.y) * dis_line + idx_item;
                uint8_t *src_addr_ofs = src_addr + (idx_line + src_clip_v.y) * src_line + idx_item;
                pixel = *(SCUI_PIXEL_TYPE *)&((scui_color8565_t *)src_addr_ofs)->ch;
                alpha = (scui_alpha_t)((scui_color8565_t *)src_addr_ofs)->ch.a;
                pixel = scui_pixel_blend_with_alpha((SCUI_PIXEL_TYPE *)&pixel, alpha,
                                                    (SCUI_PIXEL_TYPE *)dst_addr_ofs, 0xFF);
                app_sys_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
            }
            return;
        }
        if (image_unit->image->format == scui_image_format_argb8888) {
            scui_coord_t src_line = draw_area.w * sizeof(scui_color8888_t);
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_addr_ofs = dst_addr + (idx_line + dst_clip_v.y) * dis_line + idx_item;
                uint8_t *src_addr_ofs = src_addr + (idx_line + src_clip_v.y) * src_line + idx_item;
                pixel = *(SCUI_PIXEL_TYPE *)&((scui_color8888_t *)src_addr_ofs)->ch;
                alpha = (scui_alpha_t)((scui_color8888_t *)src_addr_ofs)->ch.a;
                pixel = scui_pixel_blend_with_alpha((SCUI_PIXEL_TYPE *)&pixel, alpha,
                                                    (SCUI_PIXEL_TYPE *)dst_addr_ofs, 0xFF);
                app_sys_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
            }
            return;
        }
    }
    
    /* image blend: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_argb8888) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_argb8565)) {
        /* 这个很慢很慢, 除非硬件支持, 建议对齐平台支持 */
        APP_SYS_LOG_WARN("image pixel copy too slow");
        #if 0 /* 不应该支持这种条件,有损性能 */
        #endif
    }
    
    APP_SYS_LOG_ERROR("image can't support");
    APP_SYS_LOG_ERROR("pixel format:%d", SCUI_PIXEL_FORMAT);
    APP_SYS_LOG_ERROR("image format:%d", image_unit->image->format);
}
