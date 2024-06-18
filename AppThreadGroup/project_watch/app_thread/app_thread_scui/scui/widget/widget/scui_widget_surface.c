/*实现目标:
 *    控件画布(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件画布创建
 *@param widget  控件实例
 *@param format  画布格式
 *@param hor_res 画布水平尺寸
 *@param ver_res 画布垂直尺寸
 */
void scui_widget_surface_create(scui_widget_t *widget,  scui_pixel_cf_t format,
                                scui_coord_t   hor_res, scui_coord_t    ver_res)
{
    SCUI_ASSERT(widget->surface == NULL && hor_res != 0 && ver_res != 0);
    widget->surface = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
    memset(widget->surface, 0, sizeof(scui_surface_t));
    widget->surface->format  = format;
    widget->surface->hor_res = hor_res;
    widget->surface->ver_res = ver_res;
    widget->surface->alpha   = scui_alpha_cover;
    
    if (widget->surface->format == scui_pixel_cf_def)
        widget->surface->format  = SCUI_PIXEL_CF_DEF;
    
    scui_coord_t src_byte    = scui_pixel_bits(widget->surface->format) / 8;
    scui_coord_t src_remain  = sizeof(scui_color_limit_t) - src_byte;
    scui_multi_t res_surface = hor_res * ver_res * src_byte + src_remain;
    
    widget->surface->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, res_surface);
    /* 添加一个检测机制,如果surface不可创建时,清空image内存重新创建 */
    if (widget->surface->pixel == NULL) {
        SCUI_LOG_WARN("memory deficit was caught");
        scui_image_cache_clear();
        widget->surface->pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, res_surface);
    if (widget->surface->pixel == NULL) {
        scui_image_cache_visit();
        SCUI_ASSERT(false);
    }
    }
}

/*@brief 控件画布销毁
 *@param widget 控件实例
 */
void scui_widget_surface_destroy(scui_widget_t *widget)
{
    SCUI_MEM_FREE(widget->surface->pixel);
    SCUI_MEM_FREE(widget->surface);
    widget->surface = NULL;
}

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget)
{
    if (widget->surface == NULL ||
        widget->surface->pixel == NULL ||
        widget->surface->pixel == scui_surface_fb_draw()->pixel ||
        widget->surface->pixel == scui_surface_fb_refr()->pixel)
        return false;
    
    return true;
}

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->clip_set.clip = widget->clip;
    /* 有独立画布的根控件不记录原点偏移,控件树永远相对独立画布移动 */
    /* 没有独立画布的根控件保留原点偏移,控件树永远相对绘制画布移动 */
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget->clip_set.clip.x = 0;
        widget->clip_set.clip.y = 0;
    }
    
    /* 画布的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        scui_area_t widget_clip = widget->clip_set.clip;
        scui_area_t parent_clip = widget_parent->clip_set.clip;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget_clip, &parent_clip))
            widget->clip_set.clip = clip_inter;
        else
            widget->clip_set.clip = (scui_area_t){0};
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_refr(child, recurse);
    }
}

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    SCUI_ASSERT(widget->surface->pixel != surface->pixel);
    
    /* 直接交换实例最简单 */
    uint8_t *pixel = widget->surface->pixel;
    widget->surface->pixel = surface->pixel;
    surface->pixel = pixel;
}

/*@brief 控件画布重映射
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->surface = surface;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_remap(child, surface);
    }
}

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    SCUI_ASSERT(widget->surface->pixel != surface->pixel);
    
    scui_surface_t *dst_surface = widget->surface;
    scui_surface_t *src_surface = surface;
    scui_area_t dst_clip = {
        .w = dst_surface->hor_res,
        .h = dst_surface->ver_res,
    };
    scui_area_t src_clip = {
        .w = src_surface->hor_res,
        .h = src_surface->ver_res,
    };
    
    scui_draw_area_copy(dst_surface, &dst_clip, src_surface, &src_clip);
}
