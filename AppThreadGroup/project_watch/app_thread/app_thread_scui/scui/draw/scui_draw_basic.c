/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 行拷贝(可以使用DMA-copy加速优化)
 *@param dst_addr 目标地址
 *@param src_addr 源地址
 *@param len 数据字节长度
 */
void scui_draw_line_copy(void *dst_addr, void *src_addr, uint32_t len)
{
    memcpy(dst_addr, src_addr, len);
}

/*@brief 区域填充像素点(可以使用DMA-fill加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_pixel   像素点
 *@param alpha       像素点透明度
 */
void scui_draw_area_fill(scui_surface_t  *dst_surface, scui_area_t *dst_clip,
                         SCUI_PIXEL_TYPE *src_pixel,   scui_alpha_t alpha)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (alpha == scui_alpha_trans)
        return;
    
    scui_area_t *dst_area = &dst_surface->clip;
    
    scui_area_t draw_area = {0};
    scui_area_inter(&draw_area, dst_area, dst_clip);
    
    SCUI_ASSERT(dst_clip->x + draw_area.w <= scui_disp_get_hor_res());
    SCUI_ASSERT(dst_clip->y + draw_area.h <= scui_disp_get_ver_res());
    
    if (scui_area_empty(&draw_area))
        return;
    
    /* 在src_surface.clip中的draw_area中填满pixel */
    SCUI_PIXEL_TYPE pixel = {0};
    scui_multi_t dst_line = dst_surface->line * SCUI_PIXEL_SIZE;
    scui_multi_t dis_line = draw_area.w * SCUI_PIXEL_SIZE;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * SCUI_PIXEL_SIZE;
    
    if (alpha == 0xFF) {
        /* 先填充第一行像素点 */
        for (scui_multi_t idx = 0; idx < draw_area.w; idx++)
            scui_mem_w(dst_addr + idx * SCUI_PIXEL_SIZE, *src_pixel, SCUI_PIXEL_TYPE);
        /* 后使用第一行像素点填充其他行像素点 */
        for (scui_multi_t idx = 1; idx < draw_area.h; idx++)
            scui_draw_line_copy(dst_addr + idx * dst_line, dst_addr, dis_line);
    } else {
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * SCUI_PIXEL_SIZE;
            SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
            // pixel = scui_pixel_blend_with_alpha(src_pixel, alpha, dst_addr_ofs, dst_surface->alpha);
            pixel = scui_pixel_mix_with_alpha(src_pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
            scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
        }
    }
}

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 */
void scui_draw_area_copy(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                         scui_surface_t *src_surface, scui_area_t *src_clip)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    SCUI_ASSERT(dst_surface->alpha == scui_alpha_cover);
    SCUI_ASSERT(src_surface->alpha == scui_alpha_cover);
    
    scui_area_t *dst_area = &dst_surface->clip;
    scui_area_t *src_area = &src_surface->clip;
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_inter(&dst_clip_v, dst_area, dst_clip);
    scui_area_inter(&src_clip_v, src_area, src_clip);
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    
    SCUI_ASSERT(dst_clip->x + draw_area.w <= scui_disp_get_hor_res());
    SCUI_ASSERT(dst_clip->y + draw_area.h <= scui_disp_get_ver_res());
    
    if (scui_area_empty(&draw_area))
        return;
    
    /* 在dst_surface.clip中的dst_clip_v中拷贝到src_surface.clip中的src_clip_v中 */
    scui_multi_t dst_line = dst_surface->line * SCUI_PIXEL_SIZE;
    scui_multi_t src_line = src_surface->line * SCUI_PIXEL_SIZE;
    scui_multi_t dis_line = draw_area.w * SCUI_PIXEL_SIZE;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * SCUI_PIXEL_SIZE;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * SCUI_PIXEL_SIZE;
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx = 0; idx < draw_area.h; idx++)
        scui_draw_line_copy(dst_addr + idx * dst_line, src_addr + idx * src_line, dis_line);
}

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 */
void scui_draw_area_blend(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                          scui_surface_t *src_surface, scui_area_t *src_clip)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (src_surface->alpha == scui_alpha_trans)
        return;
    /* 全覆盖混合:直接copy */
    if (src_surface->alpha == scui_alpha_cover) {
        scui_draw_area_copy(dst_surface, dst_clip, src_surface, src_clip);
        return;
    }
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t *dst_area = &dst_surface->clip;
    scui_area_t *src_area = &src_surface->clip;
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_inter(&dst_clip_v, dst_area, dst_clip);
    scui_area_inter(&src_clip_v, src_area, src_clip);
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    
    SCUI_ASSERT(dst_clip->x + draw_area.w <= scui_disp_get_hor_res());
    SCUI_ASSERT(dst_clip->y + draw_area.h <= scui_disp_get_ver_res());
    
    if (scui_area_empty(&draw_area))
        return;
    
    SCUI_PIXEL_TYPE pixel = {0};
    /* 在dst_surface.clip中的dst_clip_v中每个像素点混合到src_surface.clip中的src_clip_v中 */
    scui_multi_t dst_line = dst_surface->line * SCUI_PIXEL_SIZE;
    scui_multi_t src_line = src_surface->line * SCUI_PIXEL_SIZE;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * SCUI_PIXEL_SIZE;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * SCUI_PIXEL_SIZE;
    
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * SCUI_PIXEL_SIZE;
        uint8_t *src_ofs = src_addr + idx_line * src_line + idx_item * SCUI_PIXEL_SIZE;
        SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
        SCUI_PIXEL_TYPE *src_addr_ofs = (void *)src_ofs;
        pixel = scui_pixel_mix_with_alpha(src_addr_ofs, src_surface->alpha, dst_addr_ofs, scui_alpha_cover - src_surface->alpha);
        // pixel = scui_pixel_blend_with_alpha(src_addr_ofs, src_surface->alpha, dst_addr_ofs, dst_surface->alpha);
        scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
    }
}
