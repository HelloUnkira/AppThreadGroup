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
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->qrcode.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->qrcode.dst_clip;
    scui_area_t    *src_area    = &draw_dsc->barcode.src_area;
    scui_area_t    *src_clip    = &draw_dsc->qrcode.src_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->qrcode.src_alpha;
    scui_color_t    src_color   =  draw_dsc->qrcode.src_color;
    scui_multi_t    src_size    =  draw_dsc->qrcode.src_size;
    uint8_t        *src_data    =  draw_dsc->qrcode.src_data;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL);
    
    scui_image_t image = {.format = scui_pixel_cf_alpha1};
    scui_image_make(&image, src_area);
    scui_image_qrcode(&image, src_data, src_size);
    
    scui_draw_image(true, dst_surface, *dst_clip, &image, *src_clip, src_alpha, src_color);
    scui_image_burn(&image);
}

/*@brief 区域条形码绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_barcode(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->barcode.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->barcode.dst_clip;
    scui_area_t    *src_area    = &draw_dsc->barcode.src_area;
    scui_area_t    *src_clip    = &draw_dsc->barcode.src_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->barcode.src_alpha;
    scui_color_t    src_color   =  draw_dsc->barcode.src_color;
    scui_multi_t    src_size    =  draw_dsc->barcode.src_size;
    uint8_t        *src_data    =  draw_dsc->barcode.src_data;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL);
    
    scui_image_t image = {.format = scui_pixel_cf_alpha1};
    scui_image_make(&image, src_area);
    scui_image_barcode(&image, src_data, src_size);
    
    scui_draw_image(true, dst_surface, *dst_clip, &image, *src_clip, src_alpha, src_color);
    scui_image_burn(&image);
}
